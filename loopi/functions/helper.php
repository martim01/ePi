<?php


function CheckGet($item)
{
	if(!isset($_GET[$item]))
	{
		echo '{"error": "'.$item.' not set"}';
		exit;
	}
	else
	{
		$_GET[$item] = urldecode($_GET[$item]);
	}
}

function CheckPost($item)
{
	if(!isset($_POST[$item]))
	{
		echo '{"error": "'.$item.' not set"}';
		exit;
	}
	else
	{
		$_POST[$item] = urldecode($_POST[$item]);
	}
}

?>
