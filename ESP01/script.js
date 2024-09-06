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
  const steering = right? 300 : (left? -300 : 0);
  const period = up? 200 : (down? -200 : 0); 
  console.log(steering, period);
  if (vehicle.isSelfDriving || !socket.vehicle || !socket.vehicle.object) return;
  socket.vehicle.object.send(JSON.stringify({steering, period}));
}

calculateSpeed = (period) => {
  if(period === 0) return 0;
  if(period > 0) return (1000 - period)/10;
  return - (1000 + period)/10;
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
      const {str, prd} = JSON.parse(data);
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
    case "stop":
      stop(socket.leader);
    case "follow":
      if(!leader) return false;
      follow(leader);
    break;
  }
  return true;
}

autonomousRequest = (action) => {
  const sock = socket.vehicle.object;
  switch (action) {
    case "stop":
    case "emergency":
      if(vehicle.isSelfDriving && sock) sock.send(JSON.stringify({action}));
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