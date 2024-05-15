function loadScriptAsync(url, integrity, crossorigin, callback) {
  var script = document.createElement("script");
  script.type = "text/javascript";
  script.async = true;
  script.src = url;
  // if (integrity) {
  //   script.integrity = integrity;
  // }
  if (crossorigin) {
    script.crossOrigin = crossorigin;
  }
  script.onload = callback;
  document.head.appendChild(script);
}

function initializeMap() {
  latitude = 23;
  longitude = 75;

  navigator.geolocation.getCurrentPosition((position) => {
    latitude = position.coords.latitude;
    longitude = position.coords.longitude;
    let mapOptions = {
      center: [latitude, longitude],
      zoom: 100,
    };
    let map = new L.map("map", mapOptions);

    let layer = new L.TileLayer(
      "http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
    );
    map.addLayer(layer);

    let marker = null;

    marker = L.marker([latitude, longitude]).addTo(map);
    map.on("click", (event) => {
      if (marker !== null) {
        map.removeLayer(marker);
      }

      marker = L.marker([event.latlng.lat, event.latlng.lng]).addTo(map);

      latitude = event.latlng.lat.toFixed(7);
      longitude = event.latlng.lng.toFixed(7);
      updateCoords(latitude, longitude);
    });
    updateCoords(latitude, longitude);
  });
}

window.onload = function () {
  loadScriptAsync(
    "https://unpkg.com/leaflet@1.9.4/dist/leaflet.js",
    "sha256-20nQCchB9co0qIjJZRGuk2/Z9VM+kNiyxNV1lvTlZBo=",
    "",
    initializeMap
  );
};