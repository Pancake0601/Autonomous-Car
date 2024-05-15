let latitude = 0;
let longitude = 0;

document
  .getElementById("toggleLocation")
  .addEventListener("change", function () {
    let mapElement = document.getElementById("map");
    if (this.checked) {
      mapElement.style.opacity = "0";
      navigator.geolocation.getCurrentPosition((position) => {
        latitude = position.coords.latitude;
        longitude = position.coords.longitude;

        updateCoords(latitude, longitude);
      });
    } else {
      mapElement.style.opacity = "1";
    }
  });

document
  .getElementById("routeButton")
  .addEventListener("click", () => route());

document.getElementById("sendLocation").addEventListener("click",()=>sendLocation())

function updateCoords(latitude, longitude) {
  let latitudeElem = document.getElementById("latitude");
  let longitudeElem = document.getElementById("longitude");

  latitudeElem.innerHTML = latitude;
  longitudeElem.innerHTML = longitude;
}
