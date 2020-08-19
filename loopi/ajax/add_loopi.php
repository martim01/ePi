<?php

ini_set('display_errors', 1);
error_reporting(E_ALL);


require_once("../functions/file.php");
require_once("../functions/helper.php");


CheckPost('loopi');

$loopi_array = loadLoopi();

if($loopi_array === false)
{
	$loopi_array = array();
}

$loopi_array[] = $_POST['loopi'];


if(saveLoopi($loopi_array) === false)
{
	$json = array("error" => "Could not save loopi file");
	echo json_encode($json);
	exit;
}

$json = array("loopi" => $_POST['loopi']);

echo json_encode($json);
exit;
