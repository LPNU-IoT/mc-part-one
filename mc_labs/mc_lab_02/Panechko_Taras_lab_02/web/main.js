const button1 = document.getElementById("algo1");
const button2 = document.getElementById("algo2");

const holdTime = 500;
let timer1, wasHeldEnough1, timer2, wasHeldEnough2;

button1.addEventListener('mousedown', handleHold);
button1.addEventListener('mouseup', handleRelease);
button1.addEventListener('touchstart', handleHold);
button1.addEventListener('touchend', handleRelease);

button2.addEventListener('mousedown', handleStart);
button2.addEventListener('mouseup', handleStop);
button2.addEventListener('touchstart', handleStart);
button2.addEventListener('touchend', handleStop);

function handleHold() {
  wasHeldEnough1 = false;
  timer1 = setTimeout(() => {
    wasHeldEnough1 = true;
    fetch('/hold').then();
  }, holdTime);
}

function handleRelease() {
  if (wasHeldEnough1) {
    fetch('/release').then();
  }
  clearTimeout(timer1);
  wasHeldEnough1 = false;
}

function handleStart() {
  wasHeldEnough2 = false;
  timer2 = setTimeout(() => {
    wasHeldEnough2 = true;
    fetch('/start').then();
  }, holdTime);
}

function handleStop() {

  if (wasHeldEnough2) {
    fetch('/stop').then();
  }
  clearTimeout(timer2);
  wasHeldEnough2 = false;
}

const socket = new WebSocket("ws://192.168.4.1/ws");

socket.onmessage = (event) => {
  let data = event.data;
  document.querySelectorAll('.led').forEach(led => led.classList.remove('active'));

  if (data === "red") document.getElementById("red").classList.add("active");
  if (data === "yellow") document.getElementById("yellow").classList.add("active");
  if (data === "green") document.getElementById("green").classList.add("active");
}
