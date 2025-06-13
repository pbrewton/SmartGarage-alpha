// html.h
#pragma once

const char HOME_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Garage Door Opener</title>
  <link rel="icon" type="image/png" href="https://icons.iconarchive.com/icons/pictogrammers/material/48/garage-icon.png">
  <link rel="apple-touch-icon" href="https://icons.iconarchive.com/icons/pictogrammers/material/128/garage-icon.png">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <style>
    body {
      font-family: 'Segoe UI', sans-serif;
      margin: 0;
      padding: 0;
      background: #f0f2f5;
      color: #333;
      text-align: center;
    }
    h1 {
      background: #2c3e50;
      color: white;
      padding: 1rem 0;
      margin: 0;
      font-size: 1.8rem;
    }
    .container {
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      padding: 1rem;
      gap: 1rem;
    }
    .door-card {
      background: white;
      border-radius: 10px;
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
      padding: 1rem;
      width: 100%;
      max-width: 300px;
    }
    .door-icon {
      width: 100px;
      height: 100px;
      margin-bottom: 0.5rem;
      cursor: pointer;
      transition: transform 0.2s;
    }
    .door-icon:active {
      transform: scale(0.95);
    }
    .door-title {
      font-size: 1.2rem;
      font-weight: bold;
    }
    .status {
      margin-top: 0.25rem;
      font-size: 0.95rem;
      color: #555;
    }
    .obstruction {
      margin-top: 0.25rem;
      font-size: 0.85rem;
      color: #e74c3c;
      height: 1.2em;
      /* Reserve space */
      visibility: hidden;
      /* Initially hidden */
    }
  </style>
</head>
<body>
  <h1>Garage Door Opener</h1>
  <div class="container">
    <div class="door-card" id="door1">
      <img class="door-icon" id="icon1" src="https://cdn.jsdelivr.net/gh/templarian/MaterialDesign@master/svg/garage.svg" alt="Garage Door 1">
      <div class="door-title" id="title1">DOOR #1</div>
      <div class="status" id="status1">closed</div>
      <div class="obstruction" id="obstruction1"></div>
    </div>
    <div class="door-card" id="door2">
      <img class="door-icon" id="icon2" src="https://cdn.jsdelivr.net/gh/templarian/MaterialDesign@master/svg/garage-variant.svg" alt="Garage Door 2">
      <div class="door-title" id="title2">DOOR #2</div>
      <div class="status" id="status2">closed</div>
      <div class="obstruction" id="obstruction2"></div>
    </div>
  </div>
  <script>
    const states = [{
        status: "opening...",
        icon1: "garage-open.svg",
        icon2: "garage-open-variant.svg"
      },
      {
        status: "open",
        icon1: "garage-open.svg",
        icon2: "garage-open-variant.svg"
      },
      {
        status: "closing...",
        icon1: "garage.svg",
        icon2: "garage-variant.svg"
      },
      {
        status: "open - failed to close",
        icon1: "garage-alert.svg",
        icon2: "garage-alert-variant.svg",
        obstruction: true
      },
      {
        status: "closing...",
        icon1: "garage.svg",
        icon2: "garage-variant.svg"
      },
      {
        status: "closed",
        icon1: "garage.svg",
        icon2: "garage-variant.svg"
      }
    ];
    let door1State = 5;
    let door2State = 5;
    function updateDoor(doorNum) {
      let stateIndex;
      if (doorNum === 1) {
        door1State = (door1State + 1) % states.length;
        stateIndex = door1State;
      } else {
        door2State = (door2State + 1) % states.length;
        stateIndex = door2State;
      }
      const state = states[stateIndex];
      const icon = document.getElementById(`icon${doorNum}`);
      const status = document.getElementById(`status${doorNum}`);
      const obstruction = document.getElementById(`obstruction${doorNum}`);
      icon.src = `https://cdn.jsdelivr.net/gh/templarian/MaterialDesign@master/svg/${doorNum === 1 ? state.icon1 : state.icon2}`;
      status.textContent = state.status;
      if (state.obstruction) {
        obstruction.textContent = "OBSTRUCTION DETECTED";
        obstruction.style.visibility = "visible";
      } else {
        obstruction.textContent = "NO OBSTRUCTION"; // Keep same text for spacing
        obstruction.style.visibility = "hidden";
      }
    }
    document.getElementById("icon1").addEventListener("click", () => updateDoor(1));
    document.getElementById("icon2").addEventListener("click", () => updateDoor(2));
  </script>
</body>
</html>
)rawliteral";

/////////////////////////////
////////////////////////////

const char STATUS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>Hookii Info</title>
<!--<link rel="icon" href="data:,">-->
<link rel="icon" type="image/png" href="https://icons.iconarchive.com/icons/icons8/ios7/48/Data-Combo-Chart-icon.png">
<link rel="apple-touch-icon" href="https://icons.iconarchive.com/icons/icons8/ios7/128/Data-Combo-Chart-icon.png">
<meta name="mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<script>
window.onload = function () {
  var charts = [];
  var toolTip = {
    shared: true
  },
  legend = {
    cursor: "pointer",
    itemclick: function (e) {
      if (typeof (e.dataSeries.visible) === "undefined" || e.dataSeries.visible) {
        e.dataSeries.visible = false;
      } else {
        e.dataSeries.visible = true;
      }
      e.chart.render();
    }
  };  

  var systemDps = [], userDps=[], waitDps = [], buffersDps = [], cacheDps = [], usedDps=[], inboundDps = [], outboundDps = [], writeDps = [], readDps = [];

  var cpuChartOptions = {
    animationEnabled: true,
    theme: "light2", // "light1", "light2", "dark1", "dark2"
    title:{
      text: "Temperatures"
    },
    toolTip: toolTip,
    axisY: {
      valueFormatString: "#0.#%",
    },
    legend: legend,
    data: [{
      type: "splineArea", 
      showInLegend: "true",
      name: "battery",
      yValueFormatString: "#0.#%",
      color: "#64b5f6",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      legendMarkerType: "square",
      dataPoints: userDps
    },{
      type: "splineArea", 
      showInLegend: "true",
      name: "leftDriveMotor",
      yValueFormatString: "#0.#%",
      color: "#2196f3",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      legendMarkerType: "square",
      dataPoints: systemDps
    },{
      type: "splineArea", 
      showInLegend: "true",
      name: "rightDriveMotor",
      yValueFormatString: "#0.#%",
      color: "#1976d2",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      legendMarkerType: "square",
      dataPoints: waitDps
    }]
  };
  var memoryChartOptions = {
    animationEnabled: true,
    theme: "light2",
    title:{
      text: "Battery / Charging"
    },
    axisY: {
      suffix: " GB"
    },
    toolTip: toolTip,
    legend: legend,
    data: [{
      type: "splineArea", 
      showInLegend: "true",
      name: "battery %",
      color: "#e57373",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## GB",
      legendMarkerType: "square",
      dataPoints: cacheDps
    },{
      type: "splineArea", 
      showInLegend: "true",
      name: "voltage",
      color: "#f44336",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## GB",
      legendMarkerType: "square",
      dataPoints: buffersDps
    },{
      type: "splineArea", 
      showInLegend: "true",
      name: "dischargeCurrent",
      color: "#d32f2f",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## GB",
      legendMarkerType: "square",
      dataPoints: usedDps
    }]
  }
  var networkChartOptions = {
    animationEnabled: true,
    theme: "light2",
    title:{
      text: "Other Shit"
    },
    axisY: {
      suffix: " Kb/s"
    },
    toolTip: toolTip,
    legend: legend,
    data: [{
      type: "splineArea", 
      showInLegend: "true",
      name: "mowingCoverageRate",
      color: "#81c784",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## Kb/s",
      legendMarkerType: "square",
      dataPoints: outboundDps
    },{
      type: "splineArea", 
      showInLegend: "true",
      name: "wifiSignal",
      color: "#388e3c",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## Kb/s",
      legendMarkerType: "square",
      dataPoints: inboundDps
    }]
  }
  var diskChartOptions = {
    animationEnabled: true,
    theme: "light2",
    title:{
      text: "Even More Shit"
    },
    axisY: {},
    toolTip: toolTip,
    legend: legend,
    data: [{
      type: "splineArea", 
      showInLegend: "true",
      name: "My Time Wasted",
      color: "#ffb74d",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## ops/second",
      legendMarkerType: "square",
      dataPoints: writeDps
    },{
      type: "splineArea", 
      showInLegend: "true",
      name: "knifeDiscMotorSpeed (RPM)",
      color: "#f57c00",
      xValueType: "dateTime",
      xValueFormatString: "DD MMM YY HH:mm",
      yValueFormatString: "#.## ops/second",
      legendMarkerType: "square",
      dataPoints: readDps
    }]
  }

  charts.push(new CanvasJS.Chart("chartContainer1", cpuChartOptions));
  charts.push(new CanvasJS.Chart("chartContainer2", memoryChartOptions));
  charts.push(new CanvasJS.Chart("chartContainer3", networkChartOptions));
  charts.push(new CanvasJS.Chart("chartContainer4", diskChartOptions));

  $.get("https://canvasjs.com/data/gallery/javascript/server-matrics.json", function(data) {
    for (var i = 1; i < data.length; i++) {
      systemDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].system)});
      userDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].user)});
      waitDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].wait)});
      buffersDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].buffers)});
      cacheDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].cache)});
      usedDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].used)});
      inboundDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].inbound)});
      outboundDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].outbound)});
      writeDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].write)});
      readDps.push({x: parseInt(data[i].time), y: parseFloat(data[i].read)});
    }
    for( var i = 0; i < charts.length; i++){
      charts[i].options.axisX = {
        labelAngle: 0,
        crosshair: {
          enabled: true,
          snapToDataPoint: true,
          valueFormatString: "HH:mm"
        }
      }
    }

    syncCharts(charts, true, true, true); // syncCharts(charts, syncToolTip, syncCrosshair, syncAxisXRange)

    for( var i = 0; i < charts.length; i++){
      charts[i].render();
    }

  });  

  function syncCharts(charts, syncToolTip, syncCrosshair, syncAxisXRange) {

    if(!this.onToolTipUpdated){
      this.onToolTipUpdated = function(e) {
        for (var j = 0; j < charts.length; j++) {
          if (charts[j] != e.chart)
            charts[j].toolTip.showAtX(e.entries[0].xValue);
        }
      }
    }

    if(!this.onToolTipHidden){
      this.onToolTipHidden = function(e) {
        for( var j = 0; j < charts.length; j++){
          if(charts[j] != e.chart)
            charts[j].toolTip.hide();
        }
      }
    }

    if(!this.onCrosshairUpdated){
      this.onCrosshairUpdated = function(e) {
        for(var j = 0; j < charts.length; j++){
          if(charts[j] != e.chart)
            charts[j].axisX[0].crosshair.showAt(e.value);
        }
      }
    }

    if(!this.onCrosshairHidden){
      this.onCrosshairHidden =  function(e) {
        for( var j = 0; j < charts.length; j++){
          if(charts[j] != e.chart)
            charts[j].axisX[0].crosshair.hide();
        }
      }
    }

    if(!this.onRangeChanged){
      this.onRangeChanged = function(e) {
        for (var j = 0; j < charts.length; j++) {
          if (e.trigger === "reset") {
            charts[j].options.axisX.viewportMinimum = charts[j].options.axisX.viewportMaximum = null;
            charts[j].options.axisY.viewportMinimum = charts[j].options.axisY.viewportMaximum = null;
            charts[j].render();
          } else if (charts[j] !== e.chart) {
            charts[j].options.axisX.viewportMinimum = e.axisX[0].viewportMinimum;
            charts[j].options.axisX.viewportMaximum = e.axisX[0].viewportMaximum;
            charts[j].render();
          }
        }
      }
    }

    for(var i = 0; i < charts.length; i++) { 

      //Sync ToolTip
      if(syncToolTip) {
        if(!charts[i].options.toolTip)
          charts[i].options.toolTip = {};

        charts[i].options.toolTip.updated = this.onToolTipUpdated;
        charts[i].options.toolTip.hidden = this.onToolTipHidden;
      }

      //Sync Crosshair
      if(syncCrosshair) {
        if(!charts[i].options.axisX)
          charts[i].options.axisX = { crosshair: { enabled: true }};
		
        charts[i].options.axisX.crosshair.updated = this.onCrosshairUpdated; 
        charts[i].options.axisX.crosshair.hidden = this.onCrosshairHidden; 
      }

      //Sync Zoom / Pan
      if(syncAxisXRange) {
        charts[i].options.zoomEnabled = true;
        charts[i].options.rangeChanged = this.onRangeChanged;
      }
    }
  }  

}
</script>
<style>
  .row:after {
    content: "";
    display: table;
    clear: both;
  }
  .col {
    float: left;
    width: 50%;
    height: 270px;
  }
</style>
</head>
<body>
<div class="row">
  <div class="col"id="chartContainer1"></div>
  <div class="col" id="chartContainer2"></div>
</div>
<div class="row">
  <div class="col" id="chartContainer3"></div>
  <div class="col" id="chartContainer4"></div>
</div>
<script type="text/javascript" src="https://canvasjs.com/assets/script/jquery-1.11.1.min.js"></script>
<script src="https://cdn.canvasjs.com/canvasjs.min.js"></script>
</body>
</html>
)rawliteral";

/////////////////////////////
////////////////////////////

const char UPDATE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>SmartGarage Firmware</title>
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <style>
    html,body { margin:0; padding:15px; line-height:2; box-sizing:border-box; background:#222; color:#fff; font-family:sans-serif; }
    h2, label, #log { width:100%; box-sizing:border-box; }
    h2 { margin:15px 0 15px 0; font-size:1.3em; color:orange; }
    label { margin-bottom:8px; }
    #progressBar { width:100%; height:40px; }
    #log { margin-top:1em; background:#111; padding:1em; border-radius:6px; min-height:300px; font-family:monospace; }
    .btn { padding:8px 20px; margin:20px; font-size:2em; }
  </style>
</head>
<body>
  <h2>SmartGarage Firmware Update</h2>
  <label style="white-space:nowrap;">
  1) Select Firmware File: <input type="file" id="firmware" style="display:inline-block; max-width:65vw;"></label><br>
  2) <button id="goBtn" class="btn">Go!</button>
  <progress id="progressBar" value="0" max="100"></progress>
  <div id="log">Waiting for firmware file...<br></div>
  <script>
    const fileInput = document.getElementById('firmware');
    const goBtn = document.getElementById('goBtn');
    const progressBar = document.getElementById('progressBar');
    const logDiv = document.getElementById('log');

    function log(msg) {
      logDiv.innerHTML += msg + "<br>";
      logDiv.scrollTop = logDiv.scrollHeight;
    }

    goBtn.onclick = function() {
      if (!fileInput.files.length) {
        log("No file selected!");
        return;
      }
      fileInput.disabled = true;
      goBtn.disabled = true;
      progressBar.value = 0;
      log("Uploading firmware...");
      var file = fileInput.files[0];
      var formData = new FormData();
      formData.append("update", file);

      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/upload", true);

      xhr.upload.onprogress = function(event) {
        if (event.lengthComputable) {
          var percent = (event.loaded / event.total) * 100;
          progressBar.value = percent;
        }
      };
      xhr.onerror = function() {
        log("Network error!");
      };
      xhr.onload = function() {
        if (xhr.status == 200) {
          log("Upload finished. Applying...");
          setTimeout(function() {log(xhr.responseText)}, 2000);
        } else {
          log("Upload failed: HTTP " + xhr.status);
        }
      };
      xhr.send(formData);
    };
  </script>
</body>
</html>
)rawliteral";
