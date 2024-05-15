const ipAddress = "ESP8266_IP_address"; // Change this to your ESP8266 IP address
// Function to send location
function route() {
  const url = `http://${ipAddress}/?latitude=${latitude}&longitude=${longitude}&route=1`;
  console.log(url);
  apiRequest(url);
}

function sendLocation() {
  const url = `http://${ipAddress}/?latitude=${latitude}&longitude=${longitude}`;
  console.log(url);
  apiRequest(url);
}

function apiRequest(url){
  fetch(url)
  .then((response) => {
    if (!response.ok) {
      throw new Error("Network response was not ok");
    }
  })
  .catch((error) => {
    console.error("Error sending location:", error);
  });
}

let intervalId; // To store the interval ID for continuous signal sending
let currentSignal = null; // To store the current signal being sent

// Function to send signal continuously
function sendContinuousSignal(signal) {
  const url = `http://${ipAddress}/?message=${signal}`;
  intervalId = setInterval(() => {
    console.log(url);
    document.getElementById("testDiv").innerHTML = signal;
    apiRequest(url)
  }, 100); // Adjust interval as needed (e.g., 100ms for smoother control)
}

// Function to handle button press
function handleButtonPress(signal) {
  if (currentSignal === null) {
    currentSignal = signal;
    sendContinuousSignal(signal);
  }
}

// Function to handle button release
function handleButtonRelease() {
  clearInterval(intervalId);
  document.getElementById("testDiv").innerHTML = "";
  currentSignal = null;
}
