#ifndef _APP_WEBPAGE_H_
#define _APP_WEBPAGE_H_

#include <ESP8266WiFi.h>
#include <ArduinoWiFiServer.h>
#include "constants.h"
#include "types.h"
#include "helper.h"

ArduinoWiFiServer server(80);

static const char *favicon PROGMEM = R"EOF(
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="#111" stroke="#AAA" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
  <circle cx="12" cy="12" r="10"></circle>
</svg>
)EOF";


static const char *html PROGMEM = R"EOF(
<!DOCTYPE html>
<html>
  <header>
    <title>PegasoRC</title>
    <link rel="icon" href="favicon.svg">
    <link rel="stylesheet" type="text/css" href="style.css">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </header>
  <script src="script.js"></script>
  <body id="body" class="flex background">
    <div class="flex dashboard">
      <div class="flex status">
        <div class="flex status-line">
          <p class="status-key">vehicle:</p>
          <div class="status-light" id="vehicleStatus"></div>
        </div>
        <div class="flex status-line">
          <p class="status-key">gamepad:</p>
          <div class="status-light" id="gamepadStatus"></div>
        </div>
        <div class="flex status-line">
          <p class="status-key">pressed:</p>
          <p class="status-buttons" id="buttonStatus"></p>
        </div>
      </div>
      <div style="font-weight: bold;line-height: 24px;" class="flex messages">
        <div class="flex message-line">
          <p class="message-key">Self-Driving:</p>
          <p class="message" id="message-selfdrive"></p>
        </div>
        <div class="flex message-line">
          <p class="message-key">Leader:</p>
          <p class="message" id="message-leader"></p>
        </div>
        <div class="flex message-line">
          <p class="message-key">Speed:</p>
          <p class="message" id="message-speed"></p>
        </div>
        <div class="flex message-line">
          <p class="message-key">Steering:</p>
          <p class="message" id="message-steering"></p>
        </div>
      </div>
    </div>
    <div class="flex controller">
      <div class="inner-controller">
        <div class="flex buttons left">
          <button id="left" class="flex direction-button"
            onmousedown=onMouseOrTouch(id,true)
            onmouseup=onMouseOrTouch(id,false)
            ontouchstart=onMouseOrTouch(id,true)
            ontouchend=onMouseOrTouch(id,false)
          >
            &#9664;
          </button>
          <button id="right" class="flex direction-button"
            onmousedown=onMouseOrTouch(id,true)
            onmouseup=onMouseOrTouch(id,false)
            ontouchstart=onMouseOrTouch(id,true)
            ontouchend=onMouseOrTouch(id,false)
          >
            &#9654;
          </button>
        </div>
        <div class="flex buttons center">
          <button id="autonomous-switch" class="flex autonomous-switch" onclick=setAutonomousMenu(true)>
            Go Autonomous
          </button>
          <button id="fullscreen" class="fullscreen-button" onclick=toggleFulscreen()>
          </button>
        </div>
        <div class="flex buttons right">
          <button id="up" class="flex direction-button"
            onmousedown=onMouseOrTouch(id,true)
            onmouseup=onMouseOrTouch(id,false)
            ontouchstart=onMouseOrTouch(id,true)
            ontouchend=onMouseOrTouch(id,false)
          >
            &#9650;
          </button>
          <button id="down" class="flex direction-button"
            onmousedown=onMouseOrTouch(id,true)
            onmouseup=onMouseOrTouch(id,false)
            ontouchstart=onMouseOrTouch(id,true)
            ontouchend=onMouseOrTouch(id,false)
          >
            &#9660;
          </button>
        </div>
      </div>
      <div id="autonomous-menu" class="flex autonomous-menu">
        <div class="flex autonomous-left">
          <div class="flex autonomous-upper">
            <button id="emergency" class="flex emergency-button" onclick="autonomousRequest('emergency')">
              <svg xmlns="http://www.w3.org/2000/svg" width="22px" height="22px" viewBox="0 0 24 24" fill="none"
                stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"></path>
                <line x1="12" y1="9" x2="12" y2="13"></line>
                <line x1="12" y1="17" x2="12.01" y2="17"></line>
              </svg>
              <p class="emergency-text">emergency</p>
            </button>
            <button id="stop" class="flex stop-button" onclick="autonomousRequest('stop')">
              stop
            </button>
          </div>
          <div class="flex autonomous-lower">
            <button id="fullscreen2" class="fullscreen-button" onclick=toggleFulscreen()>
            </button>
            <button id="controlled-switch" class="autonomous-switch" onclick=setAutonomousMenu(false)>
              Go Controlled
            </button>
          </div>
        </div>
        <div class="flex autonomous-right">
          <button id="lead" class="flex selfdrive-button" onclick="autonomousRequest('self-drive')">
            self-drive
          </button>
          <p>OR</p>
          <div class="flex autonomous-follow">
            <input id="id-input" type="number" placeholder="leader ID" />
            <button id="follow" class="flex follow-button" onclick="autonomousRequest('follow')">
              follow
            </button>
          </div>
        </div>
      </div>
    </div>
  </body>
</html>
)EOF";



const char *javascript PROGMEM = R"EOF(
const red = "#FF1053";
const green = "lime";
const blue = "royalblue";
const black = "black";
const fullscreenIcons = [
  `<svg xmlns="http://www.w3.org/2000/svg" width="100%" height="100%" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-maximize"><path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3"></path></svg>`,
  `<svg xmlns="http://www.w3.org/2000/svg" width="100%" height="100%" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-minimize"><path d="M8 3v3a2 2 0 0 1-2 2H3m18 0h-3a2 2 0 0 1-2-2V3m0 18v-3a2 2 0 0 1 2-2h3M3 16h3a2 2 0 0 1 2 2v3"></path></svg>`,
]

const input = { keyboard: false, mouse: false, gamepad: false };
const buttons = { up: 0, down: 0, left: 0, right: 0 };
const vehicle = { isSelfDriving: false, speed: 0, steering: 0, leader: 0 };

const socket = {
  vehicle: { address: "ws://" + window.location.hostname + ":81", object: null},
  leader: { address: "", object: null},
}

element = (id) => {
  return document.getElementById(id);
}

updateDirections = () => {
  const {up, down, left, right} =  { ...buttons };
  let status = element("buttonStatus");
  status.innerHTML = "";
  if (left) { status.innerHTML += "&#9664;"; }
  if (up) { status.innerHTML += "&#9650;"; }
  if (down) { status.innerHTML += "&#9660;"; }
  if (right) { status.innerHTML += "&#9654;"; }
  const steering = right? 500 : (left? -500 : 0);
  const period = up? 50 : (down? -50 : 0);
  console.log(steering, period);
  if (vehicle.isSelfDriving || !socket.vehicle || !socket.vehicle.object) return;
  socket.vehicle.object.send(JSON.stringify({steering, period}));
}

calculateSpeed = (period) => {
  if(period === 0) return 0;
  if(period > 0) return Math.floor((2000 - period)/10);
  return Math.floor(-(2000 + period)/10);
}

stop = (connection) => {
  if(!connection || !connection.object) return;
  connection.object.close();
  connection.object = null;
}

follow = (host) => {
  socket.leader.object = null;
  const ip = window.location.hostname.split(".");
  socket.leader.address = "ws://"+ip[0]+"."+ip[1]+"."+ip[2]+"."+host+":81";
  socket.leader.object = new WebSocket(socket.leader.address);

  socket.leader.object.addEventListener("open", () => {
    console.log("connected to", socket.leader.address);
  });
  socket.leader.object.addEventListener("error", () => {
    console.log("socket connection error.");
    stop(socket.leader);
    alert(`Error connecting with leader (ID ${host}).`);
  });
  socket.leader.object.addEventListener("message", ({ data }) => {
    if(!socket.vehicle.object) return;
    try{
      const {action, str, prd} = JSON.parse(data);
      if(action && action == "emergency") return socket.vehicle.object.send(JSON.stringify({action}));
      socket.vehicle.object.send(JSON.stringify({steering: str, period: prd}));
    } catch {
      console.log("received unformatted from leader:", data);
    }
  });
}

updateVehicle = ({iSD, str, prd, ldr}) => {
  vehicle.isSelfDriving = iSD?? false;
  vehicle.steering = str?? 0;
  vehicle.speed = calculateSpeed(prd);
  vehicle.leader = ldr?? 0;
  const driving = vehicle.isSelfDriving;
  element("up").disabled = driving;
  element("down").disabled = driving;
  element("left").disabled = driving;
  element("right").disabled = driving;
  element("lead").disabled = driving;
  element("follow").disabled = driving;
  element("stop").disabled = !driving;
  element("emergency").disabled = !driving;
  element("controlled-switch").disabled = driving;
  element("message-selfdrive").innerText = driving? "Yes" : "No";
  element("message-speed").innerText = vehicle.speed;
  element("message-steering").innerText = vehicle.steering;
  element("message-leader").innerText = vehicle.leader > 0? vehicle.leader : "-"; 
}

setPressed = (id, keydown) => {
  let button = element(id);
  if (!button) return;
  if (!keydown && buttons[id]) {
    buttons[id] = 0;
    button.style.backgroundColor = black;
    updateDirections();
  } else if (keydown && !buttons[id]) {
    buttons[id] = 1;
    button.style.backgroundColor = blue;
    updateDirections();
  }
}

setGamepadStatus = (isConnected) => {
  let status = element("gamepadStatus");
  if (!status) return;
  status.style.backgroundColor = (isConnected) ? green : red;
}

gamepadLoop = () => {
  const gamepad = navigator.getGamepads()[0];
  if (!gamepad) return (input.gamepad = false);
  if (!input.keyboard && !input.mouse) {
    const steering = Math.round(gamepad.axes[0]);
    const up = Math.round(gamepad.buttons[7].value);
    const down = Math.round(gamepad.buttons[6].value);
    const left = (steering === -1);
    const right = (steering === 1);
    console.log(up, down, left, right);
    input.gamepad = up || down || left || right;
    setPressed("up", up);
    setPressed("down", down);
    setPressed("left", left);
    setPressed("right", right);
  }
  window.requestAnimationFrame(gamepadLoop);
}

setAutonomousMenu = (show) => {
  const menu = element("autonomous-menu");
  const size = (show) ? "calc(100% - 40px)" : "0";
  menu.style.width = size;
  menu.style.height = size;
  menu.style.opacity = (show) ? 1 : 0;
}

autonomousACK = (payload) => {
  if(!payload.action) return false;
  const {action, leader} = {...payload};
  switch(action){
    case "emergency":
      if(!vehicle.leader) return alert("emergency detected. Operation halted.");
      alert("emergency detected. Leader will halt.");
    case "stop":
      stop(socket.leader);
    break;
    case "follow":
      if(!leader) return false;
      follow(leader);
    break;
  }
  return true;
}

autonomousRequest = (action) => {
  const sock = socket.vehicle.object;
  const leader = socket.leader.object;
  switch (action) {
    case "stop":
      if(vehicle.isSelfDriving && sock) return sock.send(JSON.stringify({action}));
    break;
    case "emergency":
      if(vehicle.isSelfDriving && sock){
        if(!vehicle.leader) return sock.send(JSON.stringify({action}));
        if(socket.leader.object) return socket.leader.object.send(JSON.stringify({action})); 
      }
    break;
    case "self-drive":
      if (!vehicle.isSelfDriving && sock) sock.send(JSON.stringify({action}));
    break;
    case "follow":
      if (!vehicle.isSelfDriving) {
        const leader = Number.parseInt(element("id-input").value);
        if(!leader || leader < 50 || leader > 254) return alert("invalid ID (must be between 50 and 254).");
        if(sock) sock.send(JSON.stringify({action, leader}));
      }
    break;
    default: alert("not implemented: " + action);
  }
}

vehicleConnect = () => {
  socket.vehicle.object = new WebSocket(socket.vehicle.address);
  
  socket.vehicle.object.addEventListener("open", () => {
    console.log("connected to", socket.vehicle.address);
    element("vehicleStatus").style.backgroundColor = green;
  });
  socket.vehicle.object.addEventListener("close", () => {
    console.log("socket connection has closed.");
    element("vehicleStatus").style.backgroundColor = red;
    vehicleConnect();
  });
  socket.vehicle.object.addEventListener("error", () => {
    console.log("socket connection error.");
    element("vehicleStatus").style.backgroundColor = red;
    vehicleConnect();
  });
  socket.vehicle.object.addEventListener("message", ({ data }) => {
    try{
      const payload = JSON.parse(data);
      if(!autonomousACK(payload)) updateVehicle(payload);
    }
    catch {console.log("received unformatted:", data);}
  });
}

toggleFulscreen = () => {
  if (!document.fullscreenElement) {
    let body = element("body");
    if (body.requestFullscreen) body.requestFullscreen();
    if (body.webkitRequestFullscreen) body.webkitRequestFullscreen();
    if (body.msRequestFullscreen) body.msRequestFullscreen();
    element("fullscreen").innerHTML = fullscreenIcons[1];
    element("fullscreen2").innerHTML = fullscreenIcons[1];
    return;
  }
  if (document.exitFullscreen) document.exitFullscreen();
  if (document.webkitExitFullscreen) document.webkitExitFullscreen();
  if (document.msExitFullscreen) document.msExitFullscreen();
  element("fullscreen").innerHTML = fullscreenIcons[0];
  element("fullscreen2").innerHTML = fullscreenIcons[0];
}

onLoad = () => {
  document.title = window.location.hostname.split(".").at(-1) + " - PegasoRC";
  setGamepadStatus(navigator.getGamepads()[0] != null);
  element("fullscreen").innerHTML = fullscreenIcons[0];
  element("fullscreen2").innerHTML = fullscreenIcons[0];
  updateVehicle({iSD: false, prd: 0, str: 0, ldr: 0});
  vehicleConnect();
}

onKey = (key, keydown) => {
  if (!key.includes("Arrow")) return;
  input.keyboard = keydown;
  input.gamepad = keydown || false;
  key = key.toLowerCase().substring(5);
  setPressed(key, keydown);
}

onMouseOrTouch = (id, keydown) => {
  input.mouse = keydown;
  setPressed(id, keydown);
}

onGamepad = (e) => {
  let connected = (e.type === "gamepadconnected");
  if (connected) {
    console.log(`Connected: ${e.gamepad.id}`);
    setGamepadStatus(connected);
    return gamepadLoop();
  }
  console.log(`Disconnected: ${e.gamepad.id}`);
}

document.addEventListener("DOMContentLoaded", onLoad);
window.addEventListener("gamepadconnected", onGamepad);
window.addEventListener("gamepaddisconnected", onGamepad);
window.addEventListener("keydown", (e) => onKey(e.key, true));
window.addEventListener("keyup", (e) => onKey(e.key, false));
)EOF";



const char *css PROGMEM = R"EOF(
* {
  font-family: "Inter", Avenir, Helvetica, Arial, sans-serif;
  color: #AAA;
  box-sizing: border-box;
  -webkit-tap-highlight-color: transparent;
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
  margin: 0;
  padding: 0;
  font-size: 16px;
  font-weight: bold;
  -webkit-transition: all 0.25s linear;
  -ms-transition: all 0.25s linear;
  transition: all 0.25s linear;
}

input {
  border-radius: 10px 0 0 10px;
  outline: none;
  color: black;
  border: 1px solid #AAA;
  width: 100%;
  max-width: 200px;
  height: 50px;
  text-align: center;
}

input::-webkit-outer-spin-button,
input::-webkit-inner-spin-button {
  -webkit-appearance: none;
  margin: 0;
}

input[type=number] {
  -moz-appearance: textfield;
}

button {
  outline: none;
  border: none;
  cursor: pointer;
  background: black;
  padding: 5px;
  border-radius: 10px;
  border: 1px solid #AAA;
}

button:active {
  background-color: royalblue;
}

button:disabled {
  opacity: 0.25;
}

body {
  width: 100%;
  height: 100%;
  overflow: hidden;
  background: black;
  flex-direction: column;
  justify-content: center;
}

.flex {
  display: flex;
  align-items: center;
}

.dashboard {
  width: 95%;
  max-width: 800px;
  height: 200px;
  padding: 20px;
  border-radius: 20px;
  margin-top: 20px;
  background: #222222;
  justify-content: space-between;
  flex-shrink: 0;
  gap: 20px;
}

.controller {
  flex-shrink: 0;
  width: 95%;
  max-width: 800px;
  height: 260px;
  padding: 20px;
  margin-top: 20px;
  border-radius: 20px;
  background: #222222;
  position: relative;
}

.status {
  width: 220px;
  height: 100%;
  padding: 0;
  border-radius: 15px;
  background: #111;
  flex-direction: column;
  justify-content: space-evenly;
  flex-shrink: 0;
}

.status-line {
  width: 100%;
  height: 25px;
  gap: 10px;
  justify-content: space-between;
  padding: 0 25px;
}

.status-light {
  width: 10px;
  height: 10px;
  border-radius: 5px;
  background: #AAA;
  flex-shrink: 0;
}

.messages {
  width: 100%;
  height: 100%;
  gap: 5px;
  padding-left: 20px;
  border-radius: 15px;
  font-size: 16px;
  background: #AAA;
  justify-content: center;
  flex-direction: column;
  align-items: flex-start;
}

.message-line {
  gap: 10px;
}

.message-key {
  color: #111;
  font-weight: bold;
  white-space: nowrap;
}

.message {
  color: #111;
  font-weight: 400;
}

.inner-controller {
  position: relative;
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  align-items: flex-start;
  justify-content: space-between;
}

.buttons {
  width: 100%;
  height: 220px;
  padding: 0;
  border-radius: 15px;
  background: #111;
  justify-content: space-evenly;
}

.left {
  flex-shrink: 0;
  width: 220px;
}

.center {
  width: 280px;
  height: 200px;
  position: absolute;
  flex-direction: column;
  justify-content: center;
  padding: 20px;
  left: 50%;
  gap: 20px;
  transform: translateX(-50%);
}

.right {
  flex-direction: column;
  flex-shrink: 0;
  width: 220px;
}

.direction-button {
  flex-shrink: 0;
  width: 80px;
  height: 80px;
  color: #AAA;
  background: black;
  justify-content: center;
  font-size: 28px;
}

.fullscreen-button {
  width: 40px;
  height: 40px;
  padding: 10px;
  background: black;
}

.autonomous-switch {
  padding: 10px 20px;
  font-size: 16px;
}

.autonomous-menu {
  overflow: hidden;
  gap: 20px;
  justify-content: space-between;
  width: 0;
  height: 0;
  flex-shrink: 0;
  background: #222;
  border-radius: 10px;
  position: absolute;
  opacity: 0;
}

.autonomous-left {
  flex-direction: column;
  justify-content: space-between;
  align-items: flex-start;
  height: 100%;
}

.autonomous-upper {
  width: 220px;
  height: 70%;
  padding: 0;
  border-radius: 10px;
  background: #111;
  flex-direction: column;
  justify-content: space-evenly;
  flex-shrink: 0;
}

.autonomous-lower {
  width: 100%;
  justify-content: space-between;
}

.autonomous-right {
  background: #111;
  border-radius: 15px;
  height: 100%;
  width: 100%;
  padding: 20px;
  flex-direction: column;
  justify-content: space-evenly;
}

.autonomous-follow {
  width: 100%;
  max-width: 300px;
}

.selfdrive-button {
  width: 100%;
  max-width: 300px;
  height: 50px;
  justify-content: center;
}

.follow-button {
  height: 100%;
  width: 100%;
  max-width: 100px;
  padding: 0 10px;
  border-radius: 0 10px 10px 0;
  justify-content: center;
}

.autonomous-on {
  position: absolute;
  flex-direction: column;
  gap: 20px;
}

.emergency-text {
  color: white;
  white-space: nowrap;
}

.stop-button {
  width: calc(100% - 40px);
  height: 50px;
  justify-content: center;
}

.emergency-button {
  width: calc(100% - 40px);
  height: 50px;
  background-color: #D00;
  color: white;
  justify-content: center;
  gap: 10px;
}

@media(max-width: 800px) {
  .center {
    width: 200px;
    padding: 10px;
  }
}

@media(max-width: 550px) { /*(cellphone portrait) */

  .dashboard {
    gap: 10px;
  }

  .status {
    width: 50%;
  }

  .status-line {
    gap: 0;
    justify-content: space-between;
    padding: 0 15px;
  }

  .messages {
    padding: 0 15px;
    overflow: hidden;
  }

  .controller {
    height: 400px;
  }

  .left {
    width: 55%;
    border-radius: 15px 0 0 0;
  }

  .right {
    width: 45%;
    border-radius: 0 15px 0 0;
  }

  .center {
    left: 0px;
    bottom: 0px;
    width: 100%;
    height: 140px;
    padding: 20px;
    transform: translateX(0);
    border-radius: 0 0 15px 15px;
    align-items: flex-start;
    justify-content: flex-end;
    flex-direction: column-reverse;
  }

  .autonomous-menu {
    gap: 0;
  }

  .autonomous-upper {
    justify-content: space-between;
    gap: 20px;
    width: 160px;
    padding: 30px 0;
    border-radius: 10px 0 0 10px;
  }

  .autonomous-lower {
    align-items: flex-start;
    flex-direction: column;
    gap: 10px;
  }

  .autonomous-right {
    justify-content: space-between;
    border-radius: 0 10px;
    padding: 30px;
  }

  .selfdrive-button,
  .stop-button,
  .emergency-button,
  .follow-button {
    height: 60px;
  }

  .emergency-button {
    gap: 5px;
  }

  .follow-button {
    max-width: 100%;
    border-radius: 10px;
    justify-content: center;
  }

  .autonomous-follow {
    gap: 10px;
    max-width: 100%;
    flex-direction: column;
  }

  .stop-button {
    height: 60px;
  }

  input {
    border-radius: 10px;
    max-width: 100%;
    height: 50px;
  }
}

@media(max-height: 550px) { /*(cellphone landscape)*/
  .dashboard {
    margin-top: 10px;
    height: 60px;
    padding: 10px 20px;
    gap: 10px;
  }

  .status {
    flex: 1;
    flex-direction: row;
  }

  .status-key {
    display: none;
  }

  .status-buttons {
    width: 30px;
    overflow: hidden;
  }

  .status-line {
    height: 25px;
    gap: 10px;
    justify-content: space-between;
    padding: 0 10px;
  }

  .messages {
    width: 100%;
    height: 100%;
    border-radius: 15px;
    font-size: 16px;
    font-weight: 500;
    background: #AAA;
    color: #111;
    flex-direction: row;
    align-items: center;
    justify-content: center;
    padding-left: 0;
    gap: 20px;
    overflow: hidden;
  }


  .controller {
    margin-top: 10px;
  }
}
)EOF";


void sendHTML(WiFiClient web) {
  web.println(F("HTTP/1.1 200 OK"));            
  web.println(F("Content-type: text/html"));
  web.println(F("Connection: close\n\n")); 
  web.print(html);
  web.print(F("\n\n"));
}

void sendCSS(WiFiClient web) {
  web.println(F("HTTP/1.1 200 OK"));            
  web.println(F("Content-type: text/css"));
  web.println(F("Connection: close\n\n")); 
  web.print(css);
  web.print(F("\n\n"));
}

void sendJavascript(WiFiClient web){
  web.println(F("HTTP/1.1 200 OK"));            
  web.println(F("Content-type: text/javascript"));
  web.println(F("Connection: close\n\n")); 
  web.print(javascript);
  web.print(F("\n\n"));
}

void sendIcon(WiFiClient web){
  web.println(F("HTTP/1.1 200 OK"));
  web.println(F("Content-type: image/svg+xml"));
  web.println(F("Connection: close\n\n"));
  web.print(favicon);
  web.print(F("\n\n"));
}


String parse(String m){                                      // gets the first line of the message (until a /n is detected)
  int l = m.length();
  String message = "";
  for(int a=0; a < l; a++){
    message.concat(m[a]);
    if(
      (message.indexOf("HTTP") >= 0) ||
      (message.indexOf("END") >= 0)
    ) return message;
  }
  return "";
}


void processHTTP(WiFiClient client, String request){
  String command = parse(request);
  if(command != NULL){
    if(equals(command, "GET /favicon HTTP") || equals(command, "GET /favicon.ico HTTP")){
      sendIcon(client);
    } else if(equals(command, "GET /style.css HTTP")){
      sendCSS(client);
    } else if(equals(command, "GET /script.js HTTP")){
      sendJavascript(client);
    } else {
      sendHTML(client);
    }
  }
}

#endif