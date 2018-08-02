<?php

$action = $_GET["action"];

switch ($action) {
	case 'get':
		getDevice();
		break;
	case 'update':
		updateDevice();
		break;
	case 'humidity':
		getHumidity();
		break;
	case 'battery':
		getBattery();
		break;
	case 'temperature':
		getTemperature();
		break;
	case 'photo':
		getPhoto();
		break;
	default:
		die("Action " . $action . " unkown");
}

function getDevice() {
	$to = time();
	$from = $to - (4 * 60 * 60);
	
	if ($id = $_GET['device']) $whereStatement = "WHERE deviceId = '$id'";
	// $sql = "SELECT Plants.*, AVG(HumidityLog.humidity) AS humidity FROM Plants $whereStatement JOIN HumidityLog ON Plants.code = HumidityLog.plant AND HumidityLog.datetime BETWEEN $from AND $to GROUP BY Plants.code";
	// $sql = "SELECT RedMaple_Device.*, (SELECT AVG(RedMaple_HumidityEntry.humidity) FROM RedMaple_HumidityEntry WHERE RedMaple_HumidityEntry.deviceId = RedMaple_Device.deviceId AND (RedMaple_HumidityEntry.datetime BETWEEN $from AND $to)) AS humidity, (SELECT AVG(RedMaple_VoltageEntry.voltage) FROM RedMaple_VoltageEntry WHERE RedMaple_VoltageEntry.deviceId = RedMaple_Device.deviceId AND (RedMaple_VoltageEntry.datetime BETWEEN $from AND $to)) AS voltage FROM RedMaple_Device $whereStatement";
	$sql = "";
	$sql .= "SELECT RedMaple_Device.*, ";
	$sql .= "(SELECT AVG(RedMaple_HumidityEntry.humidity) FROM RedMaple_HumidityEntry WHERE RedMaple_HumidityEntry.deviceId = RedMaple_Device.deviceId AND (RedMaple_HumidityEntry.datetime BETWEEN $from AND $to)) AS humidity, ";
	$sql .= "(SELECT AVG(RedMaple_VoltageEntry.voltage) FROM RedMaple_VoltageEntry WHERE RedMaple_VoltageEntry.deviceId = RedMaple_Device.deviceId AND (RedMaple_VoltageEntry.datetime BETWEEN $from AND $to)) AS voltage, ";
	$sql .= "(SELECT DATETIME FROM RedMaple_HumidityEntry WHERE RedMaple_HumidityEntry.deviceId = RedMaple_Device.deviceId UNION ALL SELECT DATETIME FROM RedMaple_VoltageEntry WHERE RedMaple_VoltageEntry.deviceId = RedMaple_Device.deviceId ORDER BY DATETIME DESC LIMIT 1) AS heartbeat ";
	$sql .= "FROM RedMaple_Device $whereStatement ORDER BY active DESC, nameDE";
	
	$result = sqlCommand($sql);
	
	$output = array();
	
	while ($devices = $result->fetch_assoc()) {
		$output[] = $devices;
	}
	array_walk_recursive($output, 'encode_items');
	
	header('Content-Type: application/json; charset=utf-8');
	echo json_encode($output);
	// print_r($output);		
	$result->close();
}

function encode_items(&$item, $key)
{
    $item = utf8_encode($item);
}

function updateDevice() {
	if (!$device = $_GET['device']) return;
	
	$sql = "UPDATE RedMaple_Device SET ";
	foreach ($_GET as $key => $value) if (!in_array($key, array("action", "device"))) $set .= "$key = '$value', ";
	
	if ($_POST['name']) {
		$key = $_POST['name'];
		$value = $_POST['value'];
		$set .= "$key = '$value', ";
	}
	$set = trim($set, " ,");
	$sql .= $set;
	$sql .= " WHERE deviceId = '$device'";
	
	if(sqlCommand($sql))
	{
		//header('HTTP 200 OK', true, 200);
		$result['response']['success'] = true;
		$result['response']['message'] = "Updated the $sql";
		echo json_encode($result);
	}
	else {
		header('HTTP 400 Bad Request', true, 400);		
		echo "Error: Unable to update";		

	}
}

function getHumidity() {
	$id = $_GET['device'];
	if (!$days = $_GET['days']) $days = 3;
	
	$curr = time();
	$timeSpan = 60 * 60 * 2;
	$timeInterval = 60 * 60;
	$output = array();
	
	for ($i = 0; $i < 24 * $days; $i++) {
		$from = $curr - $timeSpan;
		$to = $curr + $timeSpan;
		
		$result = sqlCommand("SELECT AVG(humidity) FROM RedMaple_HumidityEntry WHERE deviceId = '$id' AND (datetime BETWEEN $from AND $to)");
		$humidity = ($result->fetch_assoc())['AVG(humidity)'];
		$humidity = $humidity ? round($humidity) : null;
		$entry = array();
		$entry["timestamp"] = $curr;
		$entry["humidity"] = $humidity;
		$output[] = $entry;
		$result->close();
		
		$curr -= $timeInterval;
		// $lastHumidity = $output["$curr"];
	}
	echo json_encode($output);
	// echo json_encode(array_reverse($output));
}

function getBattery() {
	$id = $_GET['device'];
	if (!$days = $_GET['days']) $days = 3;
	
	$curr = time();
	$timeSpan = 60 * 60 * 2;
	$timeInterval = 60 * 60;
	$output = array();
	
	for ($i = 0; $i < 24 * $days; $i++) {
		$from = $curr - $timeSpan;
		$to = $curr + $timeSpan;
		
		$result = sqlCommand("SELECT AVG(voltage) FROM RedMaple_VoltageEntry WHERE deviceId = '$id' AND (datetime BETWEEN $from AND $to)");
		$voltage = ($result->fetch_assoc())['AVG(voltage)']; // 1024 = 100%, 868 = 0%
		// $battery = $voltage ? round(($voltage - 850) / (1010 - 850) * 100) : 100; // 1010 = 100%, 850 = 0%
		$entry = array();
		$entry["timestamp"] = $curr;
		$entry["battery"] = $voltage;
		$output[] = $entry;
		$result->close();
		
		$curr -= $timeInterval;
		// $lastHumidity = $output["$curr"];
	}
	echo json_encode($output);
	// echo json_encode(array_reverse($output));
}

function getTemperature() {
	$id = $_GET['device'];
	if (!$days = $_GET['days']) $days = 3;
	
	$curr = time();
	$timeSpan = 60 * 60 * 2;
	$timeInterval = 60 * 60;
	$output = array();
	
	for ($i = 0; $i < 24 * $days; $i++) {
		$from = $curr - $timeSpan;
		$to = $curr + $timeSpan;
		
		$result = sqlCommand("SELECT AVG(temperature) FROM RedMaple_TemperatureEntry WHERE deviceId = '$id' AND (datetime BETWEEN $from AND $to)");
		$temperature = ($result->fetch_assoc())['AVG(temperature)'];
		$entry = array();
		$entry["timestamp"] = $curr;
		$entry["temperature"] = $temperature;
		$output[] = $entry;
		$result->close();
		
		$curr -= $timeInterval;
	}
	echo json_encode($output);
	// echo json_encode(array_reverse($output));
}

function getPhoto() {
	$id = $_GET['device'];
	if (!$days = $_GET['days']) $days = 3;
	
	$curr = time();
	$timeSpan = 60 * 60 * 2;
	$timeInterval = 60 * 60;
	$output = array();
	
	for ($i = 0; $i < 24 * $days; $i++) {
		$from = $curr - $timeSpan;
		$to = $curr + $timeSpan;
		
		$result = sqlCommand("SELECT AVG(photo) FROM RedMaple_PhotoEntry WHERE deviceId = '$id' AND (datetime BETWEEN $from AND $to)");
		$photo = ($result->fetch_assoc())['AVG(photo)'];
		$entry = array();
		$entry["timestamp"] = $curr;
		$entry["photo"] = $photo;
		$output[] = $entry;
		$result->close();
		
		$curr -= $timeInterval;
	}
	echo json_encode($output);
	// echo json_encode(array_reverse($output));
}

function sqlCommand($sql) {
	
	$servername = "localhost";
	$username = "d028e495";
	$password = "191185";
	$dbname = "d028e495";

	$mysqli = new mysqli($servername, $username, $password, $dbname);
	
	if ($mysqli->connect_error) {
		die("Connection failed: " . $mysqli->connect_error);
	}
	
	if (!$result = $mysqli->query($sql)) {
		// Oh no! The query failed. 
		echo "Sorry, the website is experiencing problems.";

		// Again, do not do this on a public site, but we'll show you how
		// to get the error information
		echo "Error: Our query failed to execute and here is why: \n";
		echo "Query: " . $sql . "\n";
		echo "Errno: " . $mysqli->errno . "\n";
		echo "Error: " . $mysqli->error . "\n";
		exit;
		
	} else return $result;
	
	// if ($result->num_rows === 0) {
}

?>