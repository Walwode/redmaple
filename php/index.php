<html>

<head>
	<link href="//netdna.bootstrapcdn.com/bootstrap/3.0.0/css/bootstrap.min.css" rel="stylesheet">
	<script src="//code.jquery.com/jquery-2.0.3.min.js"></script> 
	<script src="//netdna.bootstrapcdn.com/bootstrap/3.0.0/js/bootstrap.min.js"></script>

	<link href="//cdnjs.cloudflare.com/ajax/libs/x-editable/1.5.0/bootstrap3-editable/css/bootstrap-editable.css" rel="stylesheet"/>
	<script src="//cdnjs.cloudflare.com/ajax/libs/x-editable/1.5.0/bootstrap3-editable/js/bootstrap-editable.min.js"></script>
	
	<script src="//cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.1/Chart.bundle.min.js"></script>
	
	<style type="text/css">
		#container {
			margin: 0 auto;
			padding-top: 5px;
			padding-left: 20px;
			padding-right: 20px;
			width: 1200px;
		}
		a {
			color: #000;
		}
	</style>
</head>

<body>

<div id="container">

<div style="float: left; width: 100%;">
<h4>Devices <a href="#" onclick="getDevices();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4>
<table id="table-devices" class="table">
  <thead>
    <tr>
      <th>Device ID</th>
      <th>Name (DE)</th>
      <th>Name (EN)</th>
      <th>Active</th>
      <th>Chart Color</th>
      <th>Heartbeat</th>
      <th>Humidity</th>
      <th>Battery</th>
    </tr>
  </thead>
  <tbody>
  </tbody>
</table>
</div>

<!-- <h4>Humidity <a href="#" onclick="createHumidityChart();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4>-->
<canvas id="humidityChart" width="1170" height="400"></canvas>

<!-- <h4>Battery <a href="#" onclick="createBatteryChart();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4> -->
<canvas id="batteryChart" width="1170" height="200"></canvas>

<!-- <h4>Battery <a href="#" onclick="createBatteryChart();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4> -->
<canvas id="temperatureChart" width="1170" height="200"></canvas>

<!-- <h4>Battery <a href="#" onclick="createBatteryChart();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4> -->
<canvas id="photoChart" width="1170" height="200"></canvas>

<!-- <h4>Battery <a href="#" onclick="createBatteryChart();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4> -->
<canvas id="gasChart" width="1170" height="200"></canvas>

<!-- <h4>Battery <a href="#" onclick="createBatteryChart();  return false;" class="btn btn-primary btn-sm">Refresh</a></h4> -->
<canvas id="humidityAirChart" width="1170" height="200"></canvas>

</div> <!-- container -->
<script>
// $.fn.editable.defaults.mode = 'inline';

$(document).ready(function () {
	getDevices();
	createHumidityChart();
	createBatteryChart();
	createTemperatureChart();
	createPhotoChart();
	createGasChart();
	createHumidityAirChart();
});

function getDevices() {
	$('#table-devices tbody').empty();
	$.getJSON('ajax.php', { action: 'get' }, function(data) {
		$.each(data, function(index, element) {
			
			var deviceId = $('<td>' + element.deviceId + '</td>');
			var nameDE = $('<td><a href="#" data-type="text">' + element.nameDE + '</a></td>');
			ajaxUpdateText(nameDE, "ajax.php", "Device Name DE", element.deviceId, "nameDE");
			var nameEN = $('<td><a href="#" data-type="text">' + element.nameEN + '</a></td>');
			ajaxUpdateText(nameEN, "ajax.php", "Device Name EN", element.deviceId, "nameEN");
			var active = $('<td><a href="#" data-type="select">' + getTextForValue(element.active, boolSelector) + '</a></td>');
			ajaxUpdateSelector(active, "ajax.php", "On/Off", element.deviceId, "active", boolSelector, createHumidityChart);
			if (element.heartbeat) var heartbeat = $('<td>' + timeConverter(element.heartbeat) + '</td>');
			else var heartbeat = $('<td>-</td>');
			var chartColor = $('<td><a href="#" data-type="text">' + element.chartColor + '</a></td>');
			ajaxUpdateText(chartColor, "ajax.php", "Chart Color", element.deviceId, "chartColor", createHumidityChart);
			if (element.humidity) var humidity = $('<td>' + Math.round(element.humidity) + '</td>');
			else var humidity = $('<td>-</td>');
			if (element.voltage) var voltage = $('<td>' + voltageToBattery(element.voltage) + '% </td>');
			else var voltage = $('<td>-</td>');
			
			var htmlDevices = $('<tr style="background-color: ' + element.chartColor + '"></tr>');
			htmlDevices.append(deviceId);
			htmlDevices.append(nameDE);
			htmlDevices.append(nameEN);
			htmlDevices.append(active);
			htmlDevices.append(chartColor);
			htmlDevices.append(heartbeat);
			htmlDevices.append(humidity);
			htmlDevices.append(voltage);
			
			$('#table-devices tbody').append(htmlDevices);
		});
		$('#table-devices').parent().css("height", $('#table-devices').height() + 20 + 50);
		
	}).fail(function( jqxhr, textStatus, error ) {
		var err = textStatus + ", " + error;
		console.log( "Request Failed: " + err );
	});
}

function ajaxUpdateText(tdControl, url, title, device, fieldName, callbackFunction) {
	link = tdControl.find('a');
	link.editable({
        url: url + '?action=update&device=' + device,
        title: title,
		name: fieldName,
		pk: 1, // avoid undefined error
		value: link.text(),
		success: function(response, newValue) {
			// alert(response);
			if (response.status == 'error') return response.msg; //msg will be shown in editable form
			if (callbackFunction) callbackFunction();
		}
	});
}

function ajaxUpdateSelector(tdControl, url, title, device, fieldName, sourceSelector, callbackFunction) {
	link = tdControl.find('a');
	link.editable({
        url: url + '?action=update&device=' + device,
        title: title,
		name: fieldName,
		pk: 0, // avoid undefined error
		source: sourceSelector,
		value: getValueForText(link.text(), sourceSelector),
		success: function(response, newValue) {
			// alert(response);
			if (response.status == 'error') return response.msg; //msg will be shown in editable form
			if (callbackFunction) callbackFunction();
		}
	});
}

var boolSelector = [
	{value: 0, text: 'off'},
	{value: 1, text: 'on'},
];

function timeConverter(UNIX_timestamp){
	var a = new Date(UNIX_timestamp * 1000);
	var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
	var year = a.getFullYear();
	// var month = months[a.getMonth()];
	var month = ("0" + (a.getMonth() + 1)).slice(-2);
	var date = ("0" + a.getDate()).slice(-2);
	var hour = ("0" + a.getHours()).slice(-2);
	var min = ("0" + a.getMinutes()).slice(-2);
	var sec = ("0" + a.getSeconds()).slice(-2);
	var time = date + '.' + month + '.' + year + ' ' + hour + ':' + min + ':' + sec ;
	return time;
}

function getTextForValue(value, selector) {
	for (var index = 0; index < selector.length; ++index) {
		if (value == selector[index].value) return selector[index].text;
	}	
}

function getValueForText(text, selector) {
	for (var index = 0; index < selector.length; ++index) {
		if (text == selector[index].text) return selector[index].value;
	}
}

function createHumidityChart() {
	
	var ctx = document.getElementById("humidityChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
			labels: getDateTimeLabel(),
			datasets: getHumidityChartDatasets(),
		},
		options: {
			title:{
				text: "Chart.js Time Scale"
			},
			scales: {
				xAxes: [{
					type: "time",
					time: {
						tooltipFormat: 'll HH:mm'
					},
					scaleLabel: {
						display: true,
						labelString: 'Last 3 days'
					}
				}, ],
				yAxes: [{
					scaleLabel: {
						display: true,
						labelString: 'Humidity'
					},
					ticks: {
						suggestedMin: 200,    // minimum will be 0, unless there is a lower value.
						suggestedMax: 600,
						// beginAtZero: true   // minimum value will be 0.
					}
				}]
			},
		}
	});
}

function getHumidity(deviceId) {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'humidity', device: deviceId },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push({
					x : new Date(element.timestamp * 1000),
					y : element.humidity
				});
				// output.push(element.humidity);
			});
		}
	});
	return output;
}

function getDateTimeLabel() {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'humidity' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push(new Date(element.timestamp * 1000));
				// output.push(index);
			});
		}
	});
	return output;
}

function getHumidityChartDatasets() {
	var datasets = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'get' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				if (element.active != 0) {
					datasets.push({
						label: element.nameEN,
						data: getHumidity(element.deviceId),
						backgroundColor: element.chartColor,
						borderColor: element.chartColor,
						borderWidth: 2,
						fill: false,
					});
				}
			});
		}
	});
	return datasets;
}

function createBatteryChart() {
	
	var ctx = document.getElementById("batteryChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
			labels: getDateTimeLabel(),
			datasets: getBatteryChartDatasets(),
		},
		options: {
			legend: {
				display: false
			},
			title:{
				text: "Chart.js Time Scale"
			},
			scales: {
				xAxes: [{
					type: "time",
					time: {
						tooltipFormat: 'll HH:mm'
					},
					scaleLabel: {
						display: false,
						labelString: 'Last 3 days'
					}
				}, ],
				yAxes: [{
					scaleLabel: {
						display: true,
						labelString: 'Battery'
					},
					ticks: {
						suggestedMin: 20,    // minimum will be 0, unless there is a lower value.
						suggestedMax: 100,
						// beginAtZero: true   // minimum value will be 0.
					}
				}]
			},
		}
	});
}

function getBattery(deviceId) {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'battery', device: deviceId },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push({
					x : new Date(element.timestamp * 1000),
					y : voltageToBattery(element.battery)
				});
				// output.push(element.humidity);
			});
		}
	});
	return output;
}

function getBatteryChartDatasets() {
	var datasets = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'get' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				if (element.active != 0) {
					datasets.push({
						label: element.nameEN,
						data: getBattery(element.deviceId),
						backgroundColor: element.chartColor,
						borderColor: element.chartColor,
						borderWidth: 2,
						fill: false,
					});
				}
			});
		}
	});
	return datasets;
}

function createTemperatureChart() {
	
	var ctx = document.getElementById("temperatureChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
			labels: getDateTimeLabel(),
			datasets: getTemperatureChartDatasets(),
		},
		options: {
			legend: {
				display: false
			},
			title:{
				text: "Chart.js Time Scale"
			},
			scales: {
				xAxes: [{
					type: "time",
					time: {
						tooltipFormat: 'll HH:mm'
					},
					scaleLabel: {
						display: false,
						labelString: 'Last 3 days'
					}
				}, ],
				yAxes: [{
					scaleLabel: {
						display: true,
						labelString: 'Temperature'
					},
					ticks: {
						suggestedMin: 20,    // minimum will be 0, unless there is a lower value.
						suggestedMax: 100,
						// beginAtZero: true   // minimum value will be 0.
					}
				}]
			},
		}
	});
}

function getTemperature(deviceId) {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'temperature', device: deviceId },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push({
					x : new Date(element.timestamp * 1000),
					y : element.temperature
				});
				// output.push(element.humidity);
			});
		}
	});
	return output;
}

function getTemperatureChartDatasets() {
	var datasets = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'get' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				if (element.active != 0) {
					datasets.push({
						label: element.nameEN,
						data: getTemperature(element.deviceId),
						backgroundColor: element.chartColor,
						borderColor: element.chartColor,
						borderWidth: 2,
						fill: false,
					});
				}
			});
		}
	});
	return datasets;
}

function createPhotoChart() {
	
	var ctx = document.getElementById("photoChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
			labels: getDateTimeLabel(),
			datasets: getPhotoChartDatasets(),
		},
		options: {
			legend: {
				display: false
			},
			tooltips: {
				callbacks: {
				   label: function(tooltipItem) {
						  return tooltipItem.yLabel;
				   }
				}
			},
			title:{
				text: "Chart.js Time Scale"
			},
			scales: {
				xAxes: [{
					type: "time",
					time: {
						tooltipFormat: 'll HH:mm'
					},
					scaleLabel: {
						display: false,
						labelString: 'Last 3 days'
					}
				}, ],
				yAxes: [{
					scaleLabel: {
						display: true,
						labelString: 'Photo'
					},
					ticks: {
						suggestedMin: 50,    // minimum will be 0, unless there is a lower value.
						suggestedMax: 800,
						// beginAtZero: true   // minimum value will be 0.
					}
				}]
			},
		}
	});
}

function getPhoto(deviceId) {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'photo', device: deviceId },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push({
					x : new Date(element.timestamp * 1000),
					y : element.photo
				});
				// output.push(element.humidity);
			});
		}
	});
	return output;
}

function getPhotoChartDatasets() {
	var datasets = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'get' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				if (element.active != 0) {
					datasets.push({
						label: element.nameEN,
						data: getPhoto(element.deviceId),
						backgroundColor: element.chartColor,
						borderColor: element.chartColor,
						borderWidth: 2,
						fill: false,
					});
				}
			});
		}
	});
	return datasets;
}

function createGasChart() {
	
	var ctx = document.getElementById("gasChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
			labels: getDateTimeLabel(),
			datasets: getGasChartDatasets(),
		},
		options: {
			legend: {
				display: false
			},
			tooltips: {
				callbacks: {
				   label: function(tooltipItem) {
						  return tooltipItem.yLabel;
				   }
				}
			},
			title:{
				text: "Chart.js Time Scale"
			},
			scales: {
				xAxes: [{
					type: "time",
					time: {
						tooltipFormat: 'll HH:mm'
					},
					scaleLabel: {
						display: false,
						labelString: 'Last 3 days'
					}
				}, ],
				yAxes: [{
					scaleLabel: {
						display: true,
						labelString: 'Gas'
					},
					ticks: {
						suggestedMin: 50,    // minimum will be 0, unless there is a lower value.
						suggestedMax: 800,
						// beginAtZero: true   // minimum value will be 0.
					}
				}]
			},
		}
	});
}

function getGas(deviceId) {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'gas', device: deviceId },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push({
					x : new Date(element.timestamp * 1000),
					y : element.gas
				});
				// output.push(element.humidity);
			});
		}
	});
	return output;
}

function getGasChartDatasets() {
	var datasets = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'get' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				if (element.active != 0) {
					datasets.push({
						label: element.nameEN,
						data: getGas(element.deviceId),
						backgroundColor: element.chartColor,
						borderColor: element.chartColor,
						borderWidth: 2,
						fill: false,
					});
				}
			});
		}
	});
	return datasets;
}

function createHumidityAirChart() {
	
	var ctx = document.getElementById("humidityAirChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
			labels: getDateTimeLabel(),
			datasets: getHumidityAirChartDatasets(),
		},
		options: {
			legend: {
				display: false
			},
			tooltips: {
				callbacks: {
				   label: function(tooltipItem) {
						  return tooltipItem.yLabel;
				   }
				}
			},
			title:{
				text: "Chart.js Time Scale"
			},
			scales: {
				xAxes: [{
					type: "time",
					time: {
						tooltipFormat: 'll HH:mm'
					},
					scaleLabel: {
						display: false,
						labelString: 'Last 3 days'
					}
				}, ],
				yAxes: [{
					scaleLabel: {
						display: true,
						labelString: 'Humidity Air'
					},
					ticks: {
						suggestedMin: 50,    // minimum will be 0, unless there is a lower value.
						suggestedMax: 800,
						// beginAtZero: true   // minimum value will be 0.
					}
				}]
			},
		}
	});
}

function getHumidityAir(deviceId) {
	var output = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'humidityAir', device: deviceId },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				output.push({
					x : new Date(element.timestamp * 1000),
					y : element.humidity
				});
				// output.push(element.humidity);
			});
		}
	});
	return output;
}

function getHumidityAirChartDatasets() {
	var datasets = new Array();
	$.ajax({
		type: 'GET',
		url: 'ajax.php',
		data: { action: 'get' },
		dataType: 'json',
		async: false,
		success: function(data) {
			$.each(data, function(index, element) {
				if (element.active != 0) {
					datasets.push({
						label: element.nameEN,
						data: getHumidityAir(element.deviceId),
						backgroundColor: element.chartColor,
						borderColor: element.chartColor,
						borderWidth: 2,
						fill: false,
					});
				}
			});
		}
	});
	return datasets;
}

function getDate(days) {
	var date = new Date();
	date.setDate(date.getDate() + days);
	return date;
}

function voltageToBattery(voltage) {
	console.log(voltage);
	if (voltage) return Math.round((voltage - 850) / (1010 - 850) * 100);
	return null;
	// return 100;
}
</script>

</body>
</html>