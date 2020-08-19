<?php

function loadLoopi()
{
	$file = fopen($_SERVER['DOCUMENT_ROOT']."/data/loopi", "r");
	if($file)
	{
		$loopi_array = array();
		while(!feof($file))
		{
			$url = str_replace(array("\r","\n"),'',fgets($file));
			if($url != '')
			{
				$loopi_array[] = $url;
			}
		}
		fclose($file);
		return $loopi_array;
	}
	return false;
}

function saveLoopi($loopi_array)
{
	$file = fopen($_SERVER['DOCUMENT_ROOT']."/data/loopi", "w");
	if($file)
	{
		for($i = 0; $i < count($loopi_array); $i++)
		{
			fwrite($file, $loopi_array[$i]."\n");
		}
		fclose($file);
		return true;
	}
	return false;
}
	
?>
