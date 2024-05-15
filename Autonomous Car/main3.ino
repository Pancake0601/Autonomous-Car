#include <TinyGPS++.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <QMC5883LCompass.h>
#include <NewPing.h>

#define GPS_RX_PIN 3
#define GPS_TX_PIN 2

#define TRIGGER_PIN 6     // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 7        // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100  // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;
QMC5883LCompass compass;

String input = "";

// typedef struct
// {
//   double longitude;
//   double latitude;
// } Pair;

int heading_offset = 80;

double targetLatitude = NULL;
double targetLongitude = NULL;
double currentLatitude = NULL;
double currentLongitude = NULL;

float distance = 0;
int bearing = 0;
int heading = 0;
float rotationAngle = 0;

unsigned int collisionDistance; 

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);
  Wire.begin();

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  compass.init();  // Initialize compass (ensure it's properly calibrated)
  //calibrateCompass();
}

void loop() {
  if (targetLatitude != NULL && targetLongitude != NULL) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid()) {

          currentLatitude = gps.location.lat();
          currentLongitude = gps.location.lng();
          // Serial.print("Current Coordinates: ");
          // Serial.print(currentLatitude, 6);
          // Serial.print(", ");
          // Serial.println(currentLongitude, 6);

          // Serial.print("Target Coordinates: ");
          // Serial.print(targetLatitude, 6);
          // Serial.print(", ");
          // Serial.println(targetLongitude, 6);

          distance = TinyGPSPlus::distanceBetween(currentLatitude, currentLongitude, targetLatitude, targetLongitude);
          // Serial.print("Distance: ");
          // Serial.println(distance);

          bearing = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), targetLatitude, targetLongitude);
          // Serial.print("Bearing: ");
          // Serial.println(bearing);

          heading = getHeading();
          // Serial.print("Compass: ");
          // Serial.println(heading);

          rotationAngle = abs(bearing - heading);
          // Serial.print("Rotation Angel: ");
          // Serial.println(rotationAngle);


          //Adjust control logic based on accurate sensor readings and navigation requirements
          if (distance < 2.5) {
            targetLatitude = NULL;
            targetLongitude = NULL;
            stopCar();
          } else if (rotationAngle <= 10) {

            forward();
          } else
            adjustCourse(bearing, heading);  // Adjust course based on GPS and compass
        }
      }
    }
    return;
  }

  if (Serial.available() > 0) {
    input = Serial.readStringUntil('\n');
    //delay(1000);
    input.trim();  // Remove leading/trailing whitespace characters
    //Serial.println(input);
    if (input.startsWith("[") && input.endsWith("]")) {
      // Remove square brackets
      input = input.substring(1, input.length() - 1);

      // Find the comma separating latitude and longitude
      int commaIndex = input.indexOf(',');

      // Extract latitude and longitude substrings
      String latitudeStr = input.substring(0, commaIndex);
      String longitudeStr = input.substring(commaIndex + 2);

      // Convert latitude and longitude strings to double
      targetLatitude = latitudeStr.toDouble();
      targetLongitude = longitudeStr.toDouble();


    } else if (input == "1")  // If "1" is received, FORWARD
      forward();
    else if (input == "2")  // If "2" is received, TURN RIGHT
      turnRight();
    else if (input == "3")  // If "3" is received, REVERSE
      reverse();
    else if (input == "4")  // If "4" is received, TURN LEFT
      turnLeft();

  } else
    stopCar();
}

// Get current heading from compass
int getHeading() {
  compass.read();
  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();
  float heading = atan2(y, x) * 180.0 / PI;
  heading -= heading_offset;
  if (heading < 0)
    heading += 360.0;  // Normalize to [0, 360) range
  if (heading >= 360.0)
    heading -= 360.0;  // Normalize to [0, 360) range
  return heading;
}

// Adjust vehicle course based on GPS course and compass heading
void adjustCourse(float GPS_Course, float heading) {
  int x = (GPS_Course - 360);
  int y = (heading - x);
  int z = (y - 360);
  if ((z <= 180) && (z >= 0))
    turnLeft();  // Turn left
  else
    turnRight();  // Turn right
}



// Control functions for motor operations
void forward() {
  collisionDistance = sonar.ping_cm();  // Send ping, get distance in cm and store it in 'distance'.
  if (collisionDistance >= 30 || collisionDistance == 0) {
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    delay(100);  // Go forward if aligned with target direction
  } else stopCar();
}

void stopCar() {
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

void turnLeft() {
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  delay(100);
}

void turnRight() {
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  delay(100);
}

void reverse() {
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  delay(100);
}
