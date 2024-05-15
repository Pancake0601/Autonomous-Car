const forwardBtn = document.getElementById("forwardBtn");
const backwardBtn = document.getElementById("backwardBtn");
const leftBtn = document.getElementById("leftBtn");
const rightBtn = document.getElementById("rightBtn");

function setupEvents(element, signal) {
  element.addEventListener("mousedown", function () {
    handleButtonPress(signal);
  });
  element.addEventListener("touchstart", function (event) {
    event.preventDefault(); // Prevent default touch behavior (like scrolling)
    handleButtonPress(signal);
  });

  element.addEventListener("touchend", handleButtonRelease);
  element.addEventListener("mouseup", handleButtonRelease);
}

setupEvents(forwardBtn, "1");
setupEvents(backwardBtn, "3");
setupEvents(leftBtn, "4");
setupEvents(rightBtn, "2");