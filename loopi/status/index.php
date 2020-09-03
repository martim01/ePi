<?php
ini_set('display_errors', 1);
error_reporting(E_ALL);

include("../include/header.inc");
include("../include/navbar.inc");

OutputHeader("getLoopiConfig_Details()");
OutputNavBar("Status", $_GET['loopi']);

include ("../include/overview.inc");

?>

	

	
	<div class="uk-section uk-section-xsmall uk-section-muted uk-margin-medium" id="section_resources" >
		<div class="uk-container uk-container-xlarge">
			<ul uk-tab class="uk-subnav uk-subnav-pill">
			<li><a href="#">Files</a></li>
			<li><a href="#">Playlists</a></li>
			<li><a href="#">Schedules</a></li>
			</ul>			
			<ul class="uk-switcher uk-margin">
				<li>
					<div class="uk-card uk-card-default uk-card-small uk-card-body">
						<ul uk-accordion id="resource_files" class="uk-list  uk-list-striped uk-list-collapse">
						</ul>
						<a class="uk-button uk-button-default"  style="background: #c8efc8" onclick="showUploadModal()">Upload New Audio</a>
					</div>
				</li>
				<li>
					<div class="uk-card uk-card-default uk-card-small uk-card-body">
						<ul uk-accordion id="resource_playlists" class="uk-list  uk-list-striped uk-list-collapse">>
						</ul>
						<a class="uk-button uk-button-default"  style="background: #c8efc8" onclick="showPlaylistModal()">Create New Playlist</a>
					</div>
				</li>
				<li>
					<div class="uk-card uk-card-default uk-card-small uk-card-body">
						<ul uk-accordion id="resource_schedules" class="uk-list  uk-list-striped uk-list-collapse">>
						</ul>
						<a class="uk-button uk-button-default"  style="background: #c8efc8" onclick="showScheduleModal()">Create New Schedule</a>
					</div>
				</li>
			</ul>
		</div>
	</div>
	
<!-- modal dialogs -->
	<div id="play_modal" uk-modal>
		<div class="uk-modal-dialog">
			<button class="uk-modal-close-default" type="button" uk-close></button>
			<div class="uk-modal-header">
				<h3 class="uk-modal-title">Play '<span id="play_label"></span>'</h3>
			</div>
			<div class="uk-modal-body">
				<div id="play_options">
					<div class="uk-form-controls">
						  <label><input class="uk-radio" type="radio" name="play_radio" checked> Play Once</label>
					</div>
					<div class="uk-form-controls">
						  <label><input class="uk-radio" type="radio" name="play_radio"> Play Forever</label>
					 </div>
					 <div class="uk-form-controls">
						  <label><input class="uk-radio" type="radio" name="play_radio"> Play <input type="number" class="uk-input uk-form-width-xsmall" id="play_x"> times</label>
					 </div>
					 <div class="uk-form-controls" id="play_shuffle" style="margin-top: 10px; display: none">
						  <label><input class="uk-checkbox" type="checkbox" id="check_play_shuffle"> Shuffle</label>
						  <input id="play_uid" type="hidden"><input id="play_type" type="hidden">
					</div>
				</div>
			</div>
			<div class="uk-modal-footer uk-text-right">
			<button class="uk-button uk-button-primary" type="button" onclick="doPlay()">Play</button>
			</div>
		</div>
	</div>


	<div id="upload_modal" uk-modal="{'bg-close': false, 'stack': true}">
		<div class="uk-modal-dialog">
			<button class="uk-modal-close-default" type="button" uk-close></button>
			<div class="uk-modal-header">
				<h3 class="uk-modal-title" id="upload_title">Upload New Audio</h3>
			</div>
			<div class="uk-modal-body">
				<form class="uk-form-horizontal uk-margin-small" id="upload_form">
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="upload_file">File</label>
						<div class="uk-form-controls">
							<div uk-form-custom="target: true">
								<input type="file" id="upload_file" name="file" onchange="fileChosen()">
								<input class="uk-input uk-form-width-medium" type="text" placeholder="Select file" disabled>
								<input class="uk-input" type="hidden" name="uid" id="upload_uid">
							</div>
						</div>
					</div>
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="upload_label">Label</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="label" id="upload_label">
						</div>
					</div>
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="upload_description">Description</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="description" id="upload_description">
						</div>
					</div>
				</form>
			</div>
			<div class="uk-modal-footer uk-text-right">
			<button class="uk-button uk-button-default uk-modal-close" type="button">Cancel</button>
			<button class="uk-button uk-button-primary" type="button" onclick="uploadFile()">Upload</button>
			</div>
		</div>
	</div>
	
	<div id="progress_modal" uk-modal='bg-close: false'>
		<div class="uk-modal-dialog uk-margin-auto-vertical">
			<div class="uk-modal-header">
				<h3 class="uk-modal-title">Uploading <span id="uploading_label"></span></h3>
			</div>
			<div class="uk-modal-body">
				<progress class="uk-progress" value="0" max="100" id="progress"></progress>
			</div>
			<div class="uk-modal-footer uk-text-right">
				<button class="uk-button uk-button-default" type="button" onclick="cancelUpload()">Cancel</button>
			</div>
		</div>
	</div>

	<div id="playlist_modal" uk-modal='{"bg-close" : false, "stack" : true}'>
		<div class="uk-modal-dialog" style="width: 800px">
			<button class="uk-modal-close-default" type="button" uk-close></button>
			<div class="uk-modal-header">
				<h3 class="uk-modal-title" id="create_playlist_title">Create New Playlist</h3>
				<input type="hidden" id="playlist_count" value="0">
				<input type="hidden" id="playlist_edit" value="-1">
				<input type="hidden" id="playlist_uid" value="">
			</div>
			<div class="uk-modal-body">
				<form class="uk-form-horizontal uk-margin-small" id="playlist_form">
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="playlist_label">Label</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="playlist_label" id="playlist_label">
						</div>
					</div>
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="playlist_description">Description</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="playlist_description" id="playlist_description">
						</div>
					</div>
				</form>
				<table class="uk-table uk-table-small uk-table-divider uk-table-justify">
					<caption>Entries</caption>
					<thead>
						<th class="uk-table-expand">File</th>
						<th class="uk-table-shrink">Play</th>
						<td class="uk-table-expand">&nbsp;</td>
					</thead>
					<tbody id="playlist_entries">
						<tr id="playlist_entry">
							<td class="loopi_entry_td"><select class="uk-select" id="select_files"></select></td>
							<td class="loopi_entry_td"><input class="uk-input uk-form-width-xsmall" type="number" value="1" id="playlist_file_loop"></td>
							<td class="loopi_entry_td"><button class="uk-button uk-button-default loopi_button_add" onclick="createPlaylistEntry()">Add</button></td>
						</tr>
					</tbody>
				</table>			
			</div>
			<div class="uk-modal-footer uk-text-right">
				<button class="uk-button uk-button-default" type="button" onclick="closePlaylist()">Cancel</button>
			<button class="uk-button uk-button-primary" type="button" onclick="createPlaylist()" id="create_playlist_button">Create</button>
			</div>
		</div>
	</div>
	<div id="schedule_modal" uk-modal="{'bg-close': false, 'stack': true}">
		<div class="uk-modal-dialog" style="width: 1100px">
			<button class="uk-modal-close-default" type="button" uk-close></button>
			<div class="uk-modal-header">
				<h3 class="uk-modal-title" id="create_schedule_title">Create New Schedule</h3>
				<input type="hidden" id="schedule_count" value="0">
				<input type="hidden" id="schedule_edit" value="-1">
				<input type="hidden" id="schedule_uid" value="">
			</div>
			<div class="uk-modal-body">
				<form class="uk-form-horizontal uk-margin-small" id="schedule_form">
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="schedule_label">Label</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="schedule_label" id="schedule_label">
						</div>
					</div>
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="schedule_description">Description</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="schedule_description" id="schedule_description">
						</div>
					</div>
				</form>
				<table class="uk-table uk-table-small uk-table-divider uk-table-justify">
					<caption>Entries</caption>
					<thead>
						<th class="uk-table-expand">Resource</th>
						<th style="width: 1%">Play</th>
						<th class="uk-table-shrink">Shuffle</th>
						<th class="uk-table-expand">Schedule To Play</th>
						<td style="width: 200px">&nbsp;</td>
					</thead>
					<tbody id="schedule_entries">
						<tr id="schedule_entry">
							<td class="loopi_entry_td"><select class="uk-select" id="schedule_select_files" onchange="scheduleSelectChange()"></select></td>
							<td class="loopi_entry_td"><input class="uk-input uk-form-width-xsmall" type="number" value="1" id="schedule_file_loop"></td>
							<td class="loopi_entry_td"><input class="uk-checkbox" type="checkbox" id="schedule_file_shuffle" style="visibility: hidden"></td>
							<td class="loopi_entry_td"><span id="cron_text"></span></td>
							<td class="loopi_entry_td">&nbsp;</td>
						</tr>
					</tbody>
				</table>
				<div class="uk-section uk-section-xsmall" id="schedule_cron_div">
					<div>
					<span class="uk-text-muted loopi_subheading">SCHEDULE TO PLAY</span> 
					<button class="uk-align-right uk-button uk-button-small uk-button-default" onclick="clearCron()">Reset</button>
					</div>
				<?php
				include("../include/cron.inc");
				?>
				<div class="uk-margin-small">
					<button class="uk-button uk-align-center loopi_button_add" id="add_schedule_entry" onclick="createScheduleEntry()">Add</button>
					
				</div>
				</div>
			</div>
			<div class="uk-modal-footer uk-text-right">
				<button class="uk-button uk-button-default" type="button" onclick="closeSchedule()">Cancel</button>
				<button class="uk-button uk-button-primary" type="button" id="create_schedule_button" onclick="createSchedule()">Create</button>
			</div>
		</div>
	</div>
	
	<div id="relabel_modal" uk-modal="{'bg-close': false, 'stack': true}">
		<div class="uk-modal-dialog">
			<button class="uk-modal-close-default" type="button" uk-close></button>
			<div class="uk-modal-header">
				<h3 class="uk-modal-title">Relabel Audio</h3>
			</div>
			<div class="uk-modal-body">
				<form class="uk-form-horizontal uk-margin-small" id="relabel_form">
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="relabel_label">Label</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="label" id="relabel_label">
						</div>
					</div>
					<div class="uk-margin uk-margin-small">
						<label class="uk-form-label" for="relabel_description">Description</label>
						<div class="uk-form-controls">
							<input class="uk-input" type="text" name="description" id="relabel_description">
							<input class="uk-input" type="hidden" name="uid" id="relabel_uid">
						</div>
					</div>
				</form>
			</div>
			<div class="uk-modal-footer uk-text-right">
			<button class="uk-button uk-button-default uk-modal-close" type="button">Cancel</button>
			<button class="uk-button uk-button-primary" type="button" onclick="relabelFile()">Save</button>
			</div>
		</div>
	</div>
	
	</body>
</html>
