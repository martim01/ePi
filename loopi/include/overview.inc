<div class="uk-section uk-section-xsmall uk-section-primary">
	<div class="uk-container uk-container-xlarge" >
		<div class="uk-grid-match uk-child-width-1-3" uk-grid>
			<div>
				<div class="uk-card uk-card-default uk-card-small">
					<div class="uk-card-header">Overview</div>
					<div class="uk-card-body">
						<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
							<div class='uk-width-1-4'><div><span class='uk-text-bold'>System:</span></div></div>
							<div><span class='uk-label' id='lock_0'>Unknown</span></div>
							<div><button id="button_lock" class="uk-button uk-button-default uk-button-small" onclick="lock()">Lock</button></div>
						</div>
						<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
							<div class='uk-width-1-4'><div><span class='uk-text-bold'>Player:</span></div></div>
							<div><span  class='uk-label' id='player'>Unknown</span></div>
							<div id="button_stop" style="visibility: hidden"><button class="uk-button uk-button-danger uk-button-small" onclick="stop()">Stop</button></div>
						</div>
					</div>
				</div>
			</div>
				<div>
					<div class="uk-card uk-card-default uk-card-small" id="card_resource">
						<div class="uk-card-header">Resource</div>
						<div class="uk-card-body">
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Type:</span></div>
								<div><span id='resource_type'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Label:</span></div>
								<div><span id='resource_label'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Status:</span></div>
								<div><span class='loopi_label' id='resource_status'>Unknown</span></div>
							</div>
						</div>
					</div>
				</div>

				<div>
					<div class="uk-card uk-card-default uk-card-small" id="card_playout">
						<div class="uk-card-header">Playout</div>
						<div class="uk-card-body">
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Label:</span></div>
								<div><span id='playout_label'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Started:</span></div>
								<div><span id='playout_started'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Length:</span></div>
								<div><span id='playout_length'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Loop:</span></div>
								<div><span id='playout_loop'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>Time:</span></div>
								<div><span id='playout_time'>Unknown</span></div>
							</div>
							<div class='uk-child-width-expand uk-grid-small uk-text-left' uk-grid>
								<div class='uk-width-1-4'><span class='uk-text-bold'>SRC:</span></div>
								<div><span class="loopi_label" id='playout_src'>Unknown</span></div>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
</div>
