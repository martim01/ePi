<?php

function OutputNavBar($page, $loopiIp)
{
	//$nav_array = array("Overview" => "../status", "Files" => "../files", "Schedules" => "../schedules", "Playlists" => "../playlists", "System" => "../system");
	$nav_array = array("Overview" => "../status", "System" => "../system");
?>
	<nav class="uk-navbar-container" uk-navbar style="background: #ffffff; border-bottom: 1px solid #a8a8d8">
			<div class="uk-navbar-left">
				<ul class="uk-navbar-nav">
					<li class="uk-parent"><a href="../" class="uk-logo"><img src="../images/loopi.png" title="dashboard"></a></a></li>
					<li class="uk-active"><a href="" id="a_status"><span class="uk-h1" id="loopi_0"></span></a></li>
				</ul>
			</div>
			<div class="uk-navbar-center">
				<ul class="uk-navbar-nav">
					<li><span class="uk-h3" id="current_time"></span></li>
				</ul>
			</div>
			<div class="uk-navbar-right">
				<ul class="uk-navbar-nav">
					<li class="uk-parent"><a href="../">Dashboard</a></a></li>
<?php
			foreach($nav_array as $key => $value)
			{
				echo '<li ';
				if($key == $page)
				{
					echo 'class="uk-active"';
				}
				echo '><a href="'.$value.'/?loopi='.$loopiIp.'">'.$key.'</a></li>';
			}
?>
				</ul>
			</div>
		</nav>
<?php
}
?>
