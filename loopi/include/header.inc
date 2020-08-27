<?php

function OutputHeader($script)
{
?>
<!DOCTYPE html>
<html>
    <head>
        <title>Title</title>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="stylesheet" href="../css/uikit.min.css" />
        <link rel="stylesheet" href="../css/loopi.css" />
        <script src="../js/uikit.min.js"></script>
        <script src="../js/uikit-icons.min.js"></script>
        <script src="../js/ajax.js"></script>
	<script src="../js/cronstrue.min.js"></script>
        <script type="text/javascript">        
		var url = new URL(window.location.href);
		var endpoint = url.searchParams.get("loopi");			
		g_loopi_array.push(new loopi(0, endpoint));
						
		function status()
		{
			document.getElementById('loopi_0').innerHTML = endpoint;
			<?php echo "$script;\n"; ?>
		}
	</script>
	</head>
    <body onload="status()">
<?php
}
?>
