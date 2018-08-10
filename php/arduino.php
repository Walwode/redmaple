<?php
$mysqli = getSqlInterface();

$action = $_GET["action"];
switch ($action) {
	case 'sensor':
		logSensorData();
		break;
}

function logSensorData() {
	// http://api.walterheger.de/redmaple/arduino.php?action=sensor&access=d548434a-2c80-11e8-8c45-00248c77bce5&device=2a.1a.df.7d.99&type=1&value=264
	$type	= toValidSqlValue($_GET['type']);
	switch($type) {
		case 0: logVoltage(); break;
		case 1: logHumidity(); break;
		case 2: logTemperature(); break;
		case 3: logPhoto(); break;
		case 4: logGas(); break;
		case 5: logHumidityAir(); break;
		die("Sensor type " . $type . " unkown.");
	}
}

function logVoltage() {

	$accessToken	= toValidSqlValue($_GET['access']);
	$deviceId		= toValidSqlValue($_GET['device']);
	$voltage		= toValidSqlValue($_GET['value']);
	$datetime		= toValidSqlValue(time());
	
	if ($userId = getUserFromAccessToken($accessToken)) {
		if (sqlCommand("INSERT INTO RedMaple_VoltageEntry (datetime, userId, deviceId, voltage) VALUES ('$datetime', '$userId','$deviceId', '$voltage')")) echo "Entry created";
	} else {
		die("Token " . $accessToken . " is not permitted");
	}
}

function logHumidity() {

	$accessToken	= toValidSqlValue($_GET['access']);
	$deviceId		= toValidSqlValue($_GET['device']);
	$humidity		= 1024 - toValidSqlValue($_GET['value']);
	$datetime		= toValidSqlValue(time());
	
	if ($userId = getUserFromAccessToken($accessToken)) {
		if (sqlCommand("INSERT INTO RedMaple_HumidityEntry (datetime, userId, deviceId, humidity) VALUES ('$datetime', '$userId','$deviceId', '$humidity')")) echo "Entry created";
	} else {
		die("Token " . $accessToken . " is not permitted");
	}
}

function logTemperature() {

	$accessToken	= toValidSqlValue($_GET['access']);
	$deviceId		= toValidSqlValue($_GET['device']);
	$temperature	= toValidSqlValue($_GET['value']);
	$datetime		= toValidSqlValue(time());
	
	if ($userId = getUserFromAccessToken($accessToken)) {
		if (sqlCommand("INSERT INTO RedMaple_TemperatureEntry (datetime, userId, deviceId, temperature) VALUES ('$datetime', '$userId','$deviceId', '$temperature')")) echo "Entry created";
	} else {
		die("Token " . $accessToken . " is not permitted");
	}
}

function logPhoto() {

	$accessToken	= toValidSqlValue($_GET['access']);
	$deviceId		= toValidSqlValue($_GET['device']);
	$photo			= toValidSqlValue($_GET['value']);
	$datetime		= toValidSqlValue(time());
	
	if ($userId = getUserFromAccessToken($accessToken)) {
		if (sqlCommand("INSERT INTO RedMaple_PhotoEntry (datetime, userId, deviceId, photo) VALUES ('$datetime', '$userId','$deviceId', '$photo')")) echo "Entry created";
	} else {
		die("Token " . $accessToken . " is not permitted");
	}
}

function logGas() {

	$accessToken	= toValidSqlValue($_GET['access']);
	$deviceId		= toValidSqlValue($_GET['device']);
	$gas			= toValidSqlValue($_GET['value']);
	$datetime		= toValidSqlValue(time());
	
	if ($userId = getUserFromAccessToken($accessToken)) {
		if (sqlCommand("INSERT INTO RedMaple_GasEntry (datetime, userId, deviceId, photo) VALUES ('$datetime', '$userId','$deviceId', '$gas')")) echo "Entry created";
	} else {
		die("Token " . $accessToken . " is not permitted");
	}
}

function logHumidityAir() {

	$accessToken	= toValidSqlValue($_GET['access']);
	$deviceId		= toValidSqlValue($_GET['device']);
	$humidity		= toValidSqlValue($_GET['value']);
	$datetime		= toValidSqlValue(time());
	
	if ($userId = getUserFromAccessToken($accessToken)) {
		if (sqlCommand("INSERT INTO RedMaple_HumidityAirEntry (datetime, userId, deviceId, photo) VALUES ('$datetime', '$userId','$deviceId', '$humidity')")) echo "Entry created";
	} else {
		die("Token " . $accessToken . " is not permitted");
	}
}

function getUserFromAccessToken($accessToken) {
	
	if ($result = sqlCommand("SELECT * FROM RedMaple_User WHERE `accessToken` = '$accessToken'")) {
		if ($user = $result->fetch_assoc()) return $user["id"];
	}
}

// -------------------------------------------------- SQL ------------------------------------------------------

function getSqlInterface() {
	global $mysqli;
	
	$servername = "localhost";
	$username = "d028e495";
	$password = "191185";
	$dbname = "d028e495";

	$_mysqli = new mysqli($servername, $username, $password, $dbname);
	
	if ($_mysqli->connect_error) die("Connection failed: " . $_mysqli->connect_error);
	
	return $_mysqli;
}

function sqlCommand($cmd) {
	global $mysqli;
	
	if (!$result = $mysqli->query($cmd)) {
		echo "Sorry, the website is experiencing problems.";
		echo "Error: Our query failed to execute and here is why: \n";
		echo "Query: " . $cmd . "\n";
		echo "Errno: " . $mysqli->errno . "\n";
		echo "Error: " . $mysqli->error . "\n";
		exit;
		
	} else return $result;
}

function toValidSqlValue($value) {
	global $mysqli;
	
	return $mysqli->real_escape_string(checkSqlValue($value));
}

function checkSqlValue($value) {
	return $value;
}

?>