<?php
ini_set('display_errors', 1);
error_reporting(E_ALL);

include("../include/header.inc");
include("../include/navbar.inc");

OutputHeader("getLoopiConfig_System()");
OutputNavBar("Status", $_GET['loopi']);

include ("../include/overview.inc");

?>

	<div class="uk-section uk-section-xsmall uk-section-muted">
		<div class="uk-container uk-container-expand" >
			<div class="uk-grid-small uk-child-width-1-3 uk-text-center uk-grid-match" uk-grid>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">episerver</h3></div>
						<div class="uk-card-body">
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div><span class='uk-text-bold'>Start Time:</span></div>
								<div><span id="application-start_time"></span></div>
								<div><span class='uk-text-bold'>Up Time:</span></div>
								<div><span id="application-up_time"></span></div>
							</div>
						</div>
						<div class="uk-card-footer">
							<button class="uk-button loopi_button_warning" onclick="restart('restart server')">Restart Server</button>
						</div>
					</div>
				</div>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">System</h3></div>
						<div class="uk-card-body">
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div><span class='uk-text-bold'>Up Time:</span></div>
								<div><span id="system-uptime"></span></div>
								<div><span class='uk-text-bold'>Procs:</span></div>
								<div><span id="system-procs"></span></div>
								<div><span class='uk-text-bold'>Temperature:</span></div>
								<div><span id="temperature-cpu"></span>&#176; C</div>
							</div>
						</div>
						<div class="uk-card-footer">
							<button class="uk-button loopi_button_warning" onclick="restart('restart os')">Restart OS</button>
							<button class="uk-button uk-button-danger"onclick="restart('shutdown')">Shutdown OS</button>
						</div>
					</div>
				</div>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">CPU <span id="cpu-cpu"></span>%</h3>
						</div>
						<div class="uk-card-body">
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div class="uk-width-1-4"><span class='uk-text-bold'>CPU1:</span></div>
								<div class="uk-width-1-4"> <span id="cpu-cpu0"></span>%</div>
								<div class="uk-width-1-4"><span class='uk-text-bold'>CPU2:</span></div>
								<div class="uk-width-1-4"> <span id="cpu-cpu1"></span>%</div>
							</div>
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div class="uk-width-1-4"><span class='uk-text-bold'>CPU3:</span></div>
								<div class="uk-width-1-4"> <span id="cpu-cpu2"></span>%</div>
								<div class="uk-width-1-4"><span class='uk-text-bold'>CPU4:</span></div>
								<div class="uk-width-1-4"> <span id="cpu-cpu3"></span>%</div>
							</div>
						</div>
					</div>
				</div>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">Disk</h3></div>
						<div class="uk-card-body  uk-text-left">
							<span class="uk-text-muted loopi_subheading">BYTES</span>
							<div class="uk-grid-small uk-text-left uk-child-width-1-4" uk-grid>
								<div><span class='uk-text-bold'>Total:</span></div>
								<div><span id="disk-bytes-total"></span>GB</div>
								<div><span class='uk-text-bold'>Free:</span></div>
								<div><span id="disk-bytes-free"></span>GB</div>
								<div><span class='uk-text-bold'>Available:</span></div>
								<div><span id="disk-bytes-available"></span>GB</div>
							</div>
							<hr>
							<span class="loopi_subheading uk-text-muted">INODES</span>
							<div class="uk-grid-small uk-text-left uk-child-width-1-4" uk-grid>
								<div><span class='uk-text-bold'>Total:</span></div>
								<div><span id="disk-inodes-total"></span></div>
								<div><span class='uk-text-bold'>Free:</span></div>
								<div><span id="disk-inodes-free"></span></div>
								<div><span class='uk-text-bold'>Available:</span></div>
								<div><span id="disk-inodes-available"></span></div>
							</div>
						</div>
					</div>
				</div>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">Loads</h3></div>
						<div class="uk-card-body">
							<div class="uk-grid-small uk-text-left uk-child-width-1-4" uk-grid>
								<div><span class='uk-text-bold'>1 Minute:</span></div>
								<div><span id="system-loads-1"></span>%</div>
								<div><span class='uk-text-bold'>5 Minute:</span></div>
								<div><span id="system-loads-5"></span>%</div>
								<div><span class='uk-text-bold'>15 Minute:</span></div>
								<div><span id="system-loads-15"></span>%</div>
							</div>
						</div>
					</div>
				</div>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">Memory</h3></div>
						<div class="uk-card-body">
							<div class="uk-grid-small uk-text-left uk-child-width-1-6" uk-grid>
								<div><span class='uk-text-bold'>Total:</span></div>
								<div><span id="system-ram-total"></span>MB</div>
								<div><span class='uk-text-bold'>Buffered:</span></div>
								<div><span id="system-ram-buffered"></span>MB</div>
								<div><span class='uk-text-bold'>Shared:</span></div>
								<div><span id="system-ram-shared"></span>MB</div>
								<div><span class='uk-text-bold'>Free:</span></div>
								<div><span id="system-ram-free"></span>MB</div>
							</div>
						</div>
					</div>
				</div>
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">NTP</h3></div>
						<div class="uk-card-body uk-text-left">
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Synchronised:</span></div>
								<div class="uk-width-expand"><span id="ntp-sync"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Clock Type:</span></div>
								<div class="uk-width-expand"><span id="ntp-clock"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Clock Id:</span></div>
								<div class="uk-width-expand"><span id="ntp-refid"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Stratum:</span></div>
								<div class="uk-width-expand"><span id="ntp-stratum"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Precision:</span></div>
								<div class="uk-width-expand"><span id="ntp-precision"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Offset:</span></div>
								<div class="uk-width-expand"><span id="ntp-offset"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Jitter:</span></div>
								<div class="uk-width-expand"><span id="ntp-jitter"></span></div>
								<div class="uk-width-1-2"><span class='uk-text-bold'>Poll:</span></div>
								<div class="uk-width-expand"><span id="ntp-poll"></span></div>
							</div>
							
						</div>
						<div class="uk-card-footer">
							<button class="uk-button uk-button-small loopi_button_update" onclick="showUpdate('episerver')">Update episerver</button>
							<button class="uk-button uk-button-small loopi_button_update" onclick="showUpdate('player3')">Update player3</button>
							<button class="uk-button uk-button-small loopi_button_update" onclick="showUpdate('player67')">Update player67</button>
						</div>
					</div>
				</div>
				
				<div>
					<div class="uk-card uk-card-default">
						<div class="uk-card-header"> <h3 class="uk-card-title">Versions</h3></div>
						<div class="uk-card-body uk-text-left">
							<span class="loopi_subheading uk-text-muted">EPISERVER</span>
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div class="uk-width-1-6"><span class='uk-text-bold'>Version:</span></div>
								<div class="uk-width-1-6"><span id="server-version"></span></div>
								<div class="uk-width-1-6"><span class='uk-text-bold'>Created:</span></div>
								<div class="uk-width-expand"><span id="server-date"></span></div>
							</div>
							<span class="loopi_subheading uk-text-muted">PLAYER3</span>
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div class="uk-width-1-6"><span class='uk-text-bold'>Version:</span></div>
								<div class="uk-width-1-6"><span id="player3-version"></span></div>
								<div class="uk-width-1-6"><span class='uk-text-bold'>Created:</span></div>
								<div class="uk-width-expand"><span id="player3-date"></span></div>
							</div>
							<span class="loopi_subheading uk-text-muted">PLAYER67</span>
							<div class="uk-grid-small uk-text-left" uk-grid>
								<div class="uk-width-1-6"><span class='uk-text-bold'>Version:</span></div>
								<div class="uk-width-1-6"><span id="player67-version"></span></div>
								<div class="uk-width-1-6"><span class='uk-text-bold'>Created:</span></div>
								<div class="uk-width-expand"><span id="player67-date"></span></div>
							</div>
						</div>
						<div class="uk-card-footer">
							<button class="uk-button uk-button-small loopi_button_update" onclick="showUpdate('episerver')">Update episerver</button>
							<button class="uk-button uk-button-small loopi_button_update" onclick="showUpdate('player3')">Update player3</button>
							<button class="uk-button uk-button-small loopi_button_update" onclick="showUpdate('player67')">Update player67</button>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
	<div id="update_modal" uk-modal>
		<div class="uk-modal-dialog">
			<button class="uk-modal-close-default" type="button" uk-close></button>
			<div class="uk-modal-header">
				<h3 class="uk-modal-title">Update <span id="update_application_title"></span></h3>
			</div>
			<div class="uk-modal-body">
				<form class="uk-form-horizontal uk-margin-small" id="update_form" enctype="multipart/form-data" onsubmit="return false;">
					<div class="uk-form-controls">
						<div uk-form-custom="target: true">
							<input type="file" id="upload_file" name="file">
							<input class="uk-input uk-form-width-medium" type="text" id="filename" placeholder="Select file" disabled>
							<input type="hidden" id="update_application" name="application">
						</div>
					</div>
				</form>
			</div>
			<div class="uk-modal-footer uk-text-right">
				<button class="uk-button uk-button-default uk-modal-close" type="button">Cancel</button>
			<button class="uk-button uk-button-primary" type="button" onclick="updateApp()">Update</button>
			</div>
		</div>
	</div>
</body>
</html>
