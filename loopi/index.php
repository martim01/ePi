<?php

require_once("functions/file.php");

$loopi_array = loadloopi();



?>
<!DOCTYPE html>
<html>
    <head>
        <title>loopi Dashboard</title>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="stylesheet" href="css/uikit.min.css" />
        <script src="js/uikit.min.js"></script>
        <script src="js/uikit-icons.min.js"></script>
        <script src="js/ajax.js"></script>
        <script type="text/javascript">
        
            var loaded_array=<?php echo json_encode($loopi_array);?>;

            for(var i = 0; i < loaded_array.length; i++)
            {
                g_loopi_array.push(new loopi(i, loaded_array[i]));
            }
        </script>
    </head>
    <body onload="dashboard()">
        <nav class="uk-navbar-container" uk-navbar style="background: #ffffff; border-bottom: 1px solid #a8a8d8;">
		<div class="uk-navbar-left">
			<ul class="uk-navbar-nav">
				<li class="uk-parent"><a href="../" class="uk-logo"><img src="images/loopi.png"></a></a></li>
				<li class="uk-active"><a href=""><span class="uk-h1" id="hostname">loopi Dashboard</span></a></li>
			</ul>
		</div>
		<div class="uk-navbar-center">
			<ul class="uk-navbar-nav">
				<li><span class="uk-h3" id="current_time"></span></li>
			</ul>
		</div>
		<div class="uk-navbar-right">
		</div>
        </nav>
        <div class="uk-section uk-section-xsmall uk-section-muted">
            <div class="uk-grid-column-small uk-grid-row-small uk-grid-match uk-text-center" uk-grid id="loopi_grid">
        <?php
        for($i = 0; $i < count($loopi_array); $i++)
        {
            echo "<div>\n";
            echo "<a class='uk-link-reset uk-display-block uk-width-large@l uk-width-medium@m uk-card uk-card-default uk-card-body uk-card-hover uk-card-small' id='loopi_$i'  href='status/?loopi=".$loopi_array[$i]."'>\n";
            echo "<div class='uk-card-header'>\n";
            echo "<h3 class='uk-card-title'><span id='host_$i'>".$loopi_array[$i]."</span></h3>\n";
            echo "</div>\n";
            echo "<div class='uk-card-body'>\n";
            echo "<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>\n";
            echo "<div class='uk-width-1-4'><div><span class='uk-text-bold'>Status:</span></div></div>\n";
            echo "<div id='status_$i'>?</div>\n";
            echo "</div>\n";
            
            echo "<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>\n";
            echo "<div class='uk-width-1-4'><div><span class='uk-text-bold'>Resource:</span></div></div>\n";
            echo "<div id='label_$i'>?</div>\n";
            echo "</div>\n";
            
            echo "<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>\n";
            echo "<div class='uk-width-1-4'><div><span class='uk-text-bold'>Playout:</span></div></div>\n";
            echo "<div id='playout_$i'>?</div>\n";
            echo "</div>\n";
            
            echo "<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>\n";
            echo "<div class='uk-width-1-4'><div><span class='uk-text-bold'>Time:</span></div></div>\n";
            echo "<div id='playout_time_$i'>?</div>\n";
            echo "</div>\n";
            
            echo "</div>\n";
            echo "<div class='uk-card-footer'><div class='uk-label' id='lock_$i'>&nbsp;</div></div>\n";
            echo "</a>\n";
            echo "</div>\n";
        
        }
        ?>
                <div  id="loopi_add">
                <div class="uk-width-large@l uk-width-medium@m uk-card uk-card-default uk-card-body uk-card-hover uk-card-small" onclick="addLoopi()">
                   <div class='uk-card-header'><h3 class="uk-card-title">Add loopi</h3></div>
                   <div class='uk-card-body uk-background-norepeat uk-background-center-center' style="background-image: url(images/add.png)"><br/><br/><br/><br/><br/><br/></div>
                   <div class='uk-card-footer'></div>
                </div>
                </div>
          </div>
        </div>
    </body>
</html>
