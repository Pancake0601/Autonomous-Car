#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WebServer.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial gpsSerial(2, 4);

struct CoordinatePair {
  float longitude;
  float latitude;
};
CoordinatePair coordinatePairs[100];

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

int current_waypoint = 0;
int waypoint_count = 0;

String receivedMessage = "No message received yet";  // Global variable to store received message

TinyGPSPlus gps;

ESP8266WebServer server(80);
String targetLat = "0";
String targetLon = "0";

double currentLat = 0;
double currentLon = 0;
float distance = 0;

String payload;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);  // Initialize serial communication for GPS module

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Handle root URL '/'
  server.on("/", HTTP_GET, handleRoot);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        currentLat = gps.location.lat();
        currentLon = gps.location.lng();
        // Serial.println(currentLat);
        // Serial.println(currentLon);
      }
    }
  }
  distance = TinyGPSPlus::distanceBetween(currentLat, currentLon, coordinatePairs[current_waypoint].latitude, coordinatePairs[current_waypoint].longitude);
  if (distance < 3) {
    current_waypoint++;
    // Serial.print("Longitude: ");
    // Serial.print(coordinatePairs[current_waypoint].latitude, 6);
    // Serial.print(", Latitude: ");
    // Serial.println(coordinatePairs[current_waypoint].longitude, 6);
  } else if (current_waypoint < waypoint_count) {
    Serial.print("[");
    Serial.print(coordinatePairs[current_waypoint].latitude, 6);
    Serial.print(", ");
    Serial.print(coordinatePairs[current_waypoint].longitude, 6);
    Serial.println("]");
    delay(150);
  } else {
    current_waypoint = 0;
    waypoint_count = 0;
  }
}

void handleRoot() {
  if (server.hasArg("message")) {  // Check if a message is sent
    message();
  } else if (server.hasArg("latitude") && server.hasArg("longitude")) {

    targetLat = server.arg("latitude");   // Get the latitude from the request
    targetLon = server.arg("longitude");  // Get the longitude from the request

    if (server.hasArg("route")) {
      //Serial.print("Routing to: ");
      apiRequest();
    } else {
      coordinatePairs[waypoint_count].longitude = targetLon.toFloat();
      coordinatePairs[waypoint_count].latitude = targetLat.toFloat();
      waypoint_count++;
      server.send(200, "text/plain", "Received message: " + receivedMessage + "\n Going to coordinates");  // Send response to the client
    }
  }
}
void message() {
  receivedMessage = server.arg("message");                                 // Get the message from the request
  Serial.println(receivedMessage);                                         // Print the received message
  server.send(200, "text/plain", "Received message: " + receivedMessage);  // Send response to the client
}

void apiRequest() {
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

  // Ignore SSL certificate validation
  client->setInsecure();


  //WiFiClient client;  // Create a WiFiClient object
  HTTPClient http;
  String apiUrl = "https://api.openrouteservice.org/v2/directions/driving-car";
  apiUrl += "?api_key=YOUR_API_KEY";

  char temp[50];
  sprintf(temp, "&start=%.7f,%.7f", currentLon, currentLat);
  apiUrl += temp;

  apiUrl += "&end=" + targetLon + "," + targetLat;  // Replace with your desired destination

  if (http.begin(*client, apiUrl)) {
    // Specify the request destination
    int httpCode = http.GET();  // Send the GET request

    if (httpCode > 0) {
      payload = http.getString();
    } else {
      // Serial.println("Error on HTTP request");
    }
  }

  http.end();  //Close connection

  // Create a JSON document
  StaticJsonDocument<2000> doc;

  // Deserialize the JSON string
  DeserializationError error = deserializeJson(doc, payload);

  // Check if parsing succeeds
  if (error) {
    Serial.print("deserializeJson() failed: ");
    server.send(200, "text/plain", "deserializeJson() failed:");
    Serial.println(error.c_str());
    return;
  }

  // Extract the coordinates array from features.0.geometry.coordinates
  JsonArray coordinates = doc["features"][0]["geometry"]["coordinates"];

  // Store coordinates in pairs
  for (JsonArray::iterator it = coordinates.begin(); it != coordinates.end(); ++it) {
    JsonArray point = *it;
    double lon = point[0];
    double lat = point[1];
    coordinatePairs[waypoint_count].longitude = lon;
    coordinatePairs[waypoint_count].latitude = lat;
    waypoint_count++;
  }
  server.send(200, "text/plain", payload);  // Send response to the client
}
