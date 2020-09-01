<?php
OutputCronLine("Seconds", 's',60,0);
OutputCronLine("Minutes", 'm',60,0);
OutputCronLine("Hours", 'h',24,0);
OutputCronLine("DoM", 'd',24,1);
?>
	
	
	<div class="uk-grid-collapse" style="margin-top: 8px" uk-grid>
		<div style="width: 70px" class="uk-text-bold">Month </div>
		<div class="uk-width-expand">
			<div class="uk-grid-collapse" uk-grid>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_01" onclick="cron(event)">January</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_02" onclick="cron(event)">Febuary</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_03" onclick="cron(event)">March</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_04" onclick="cron(event)">April</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_05" onclick="cron(event)">May</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_06" onclick="cron(event)">June</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_07" onclick="cron(event)">July</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_08" onclick="cron(event)">August</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_09" onclick="cron(event)">September</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_10" onclick="cron(event)">October</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_11" onclick="cron(event)">November</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="M_12" onclick="cron(event)">December</button></div>
				<div><button class="uk-button uk-button-primary uk-button-small" id="M_all" onclick="cronAll(event)">All</button></div>
			</div>
		</div>
	</div>
	
	<div class="uk-grid-collapse" style="margin-top: 8px" uk-grid>
		<div style="width: 70px" class="uk-text-bold">DoW</div>
		<div class="uk-width-expand">
			<div class="uk-grid-collapse" uk-grid>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_06" onclick="cron(event)">Saturday</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_00" onclick="cron(event)">Sunday</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_01" onclick="cron(event)">Monday</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_02" onclick="cron(event)">Tuesday</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_03" onclick="cron(event)">Wednesday</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_04" onclick="cron(event)">Thursday</button></div>
				<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="w_05" onclick="cron(event)">Friday</button></div>
				<div><button class="uk-button uk-button-primary uk-button-small" id="w_all" onclick="croAll(event)">All</button></div>
			</div>
		</div>
	</div>


<?php
function OutputCronLine($text, $id, $count, $start)
{
?>
	<div class="uk-grid-collapse" style="margin-top: 8px" uk-grid>
	<div style="width: 70px" class="uk-text-bold"><?php echo $text  ?></div>
	<div class="uk-width-expand">
	<div class="uk-grid-collapse" uk-grid>

<?php
	for($i = 0; $i < $count; $i++)
	{
		$key = $start+$i;
		if($key < 10)
		{
			$key = '0'.$key;
		}
		echo '<div><button class="uk-button uk-button-default uk-button-small loopi_cron_deselected" id="'.$id.'_'.$key.'" onclick="cron(event)">'.$key.'</button></div>';
	}
	echo '<div><button class="uk-button uk-button-primary uk-button-small" id="'.$id.'_all" onclick="cronAll(event)">All</button></div>';
?>
	</div>
	</div>
	</div>
<?php
}
?>

