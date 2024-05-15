function switchTab(tabName) {
  var locationContent = document.getElementById("locationContent");
  var remoteContent = document.getElementById("remoteContent");
  var tabButtons = document.getElementsByClassName("tabButton");

  // Hide all content by default
  locationContent.style.display = "none";
  remoteContent.style.display = "none";

  // Remove active class from all tabs
  for (var i = 0; i < tabButtons.length; i++) {
    tabButtons[i].classList.remove("active");
  }

  // Show content and add active class to the selected tab
  if (tabName === "location") {
    locationContent.style.display = "flex";
    tabButtons[0].classList.add("active");
  } else if (tabName === "remote") {
    remoteContent.style.display = "block";
    tabButtons[1].classList.add("active");
  }
}

window.onload = function () {
  // Initialize the location tab as active by default
  switchTab("location");
};
