class loopi
{
	constructor(index, url)
	{
		this.index = index;
		this.url = url;
		this.config = [];
		this.status = [];
		this.files = [];
		this.playlists = [];
		this.schedules = [];
		this.connected = false;
	}
	
	setWebsocket(ws)
	{
		this.ws = ws;
	}
	
	setConfig(jsonObj)
	{
		this.config = jsonObj;
	}
	
	setStatus(jsonObj)
	{
		this.status = jsonObj;
	}
	
	setFiles(jsonObj)
	{
		if(jsonObj !== null)
		{
			this.files = jsonObj;
		}
	}
	
	setPlaylists(jsonObj)
	{
		if(jsonObj !== null)
		{
			this.playlists = jsonObj;
		}
	}
	
	setSchedules(jsonObj)
	{
		if(jsonObj !== null)
		{
			this.schedules = jsonObj;
		}
	}
	
	
	findFile(uid)
	{
		console.log(this.files);
		for(var i = 0; i < this.files.length; i++)
		{
			if(this.files[i]['uid'] == uid)
			{
				return i;
			}
		}
		return -1;
	}
	
}


var g_loopi_array = new Array();

const CLR_PLAYING = "#92d14f";
const CLR_IDLE = "#8db4e2";
const CLR_ERROR =  "#c3c3c3";
const CLR_NO_FILE = "#a0a0a0"
const CLR_CONNECTING = "#ffff00";



function dashboard()
{
	var loopi = 0;
			
	getLoopiConfig_Dashboard(loopi);
	
}

function getLoopiConfig_Dashboard(loopi)
{
	
	if(g_loopi_array[loopi].connected == false)
	{
		console.log("Checking loopi #"+loopi);
		getConfig(loopi, handleConfig_Dashboard)
	}
	else
	{
		doNextLoopi(loopi);
	}
}

function handleConfig_Dashboard(loopi, jsonObj)
{
	if(jsonObj !== null)
	{
		updateConfig_Dashboard(loopi, jsonObj);
		getStatus(loopi, handleStatus_Dashboard);
	}
	else
	{
		doNextLoopi(loopi);
	}
}

function handleStatus_Dashboard(loopi, jsonObj)
{
	updateStatus_Dashboard(loopi, jsonObj);
	
	getFiles(loopi, storeFiles_Dashboard);

}




function storeFiles_Dashboard(loopi, jsonObj)
{
	g_loopi_array[loopi].setFiles(jsonObj);
	ws_connect(loopi, updateStatus_Dashboard);
	doNextDashboardLoopi(loopi);
}


function ws_connect(loopi, statusCallback)
{
	g_loopi_array[loopi].setWebsocket(new WebSocket('ws://' + g_loopi_array[loopi].url));
	g_loopi_array[loopi].ws.index = loopi;			
	
	g_loopi_array[loopi].ws.onopen = function(ev)  { this.tm = setTimeout(loopiOffline, 4000, this.index) };
	g_loopi_array[loopi].ws.onerror = function(ev) { loopiOffline(this.index); };
	g_loopi_array[loopi].ws.onclose = function(ev) { loopiOffline(this.index); };
	
	
	g_loopi_array[loopi].ws.onmessage = function(ev) 
	{
		clearTimeout(this.tm);
		this.tm = setTimeout(loopiOffline, 4000, this.index);
		
		var jsonObj = JSON.parse(ev.data);
		if(jsonObj["player"] !== undefined)
		{
			statusCallback(this.index, jsonObj);
		}							
	}	
}



function doNextDashboardLoopi(loopi)
{
	loopi++;
	if(loopi < g_loopi_array.length)
	{
		getLoopiConfig_Dashboard(loopi);
	}
	else
	{
		setTimeout(dashboard, 5000);
	}
}


	
function updateConfig_Dashboard(loopi, jsonObj)
{
	var title = document.getElementById('host_'+loopi);
	
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setConfig(jsonObj);
			
		if(title)
		{
			title.innerHTML = jsonObj["server"]["hostname"];
		}	
	}
	else if(title)
	{
		title.innerHTML = "??";
	}
}


function updateStatus_Dashboard(loopi, jsonObj)
{
	if(jsonObj != null)
	{
		
		g_loopi_array[loopi].setStatus(jsonObj);
		
		if(jsonObj["locked"] !== undefined)
		{
			if(jsonObj["locked"] == false)
			{
				document.getElementById("lock_"+loopi).innerHTML = "Unlocked";
				document.getElementById("lock_"+loopi).classList.remove("uk-label-danger");
				document.getElementById("lock_"+loopi).classList.remove("uk-label-warning");
				document.getElementById("lock_"+loopi).classList.add("uk-label-success");
				
			}
			else if(jsonObj["locked"] == true)
			{
				document.getElementById("lock_"+loopi).innerHTML = "Locked";
				document.getElementById("lock_"+loopi).classList.remove("uk-label-success");
				document.getElementById("lock_"+loopi).classList.remove("uk-label-danger");
				document.getElementById("lock_"+loopi).classList.add("uk-label-warning");
			}
		}
	}
	
	if(jsonObj["player"] === undefined || jsonObj["player"] == "Stopped" || jsonObj["status"] == undefined)
	{
		document.getElementById('status_'+loopi).innerHTML = 'Idle';
		document.getElementById('label_'+loopi).innerHTML = '';		
		
		document.getElementById('loopi_'+loopi).style.backgroundColor = CLR_IDLE;		
		document.getElementById('playout_'+loopi).innerHTML = "";
		document.getElementById('playout_time_'+loopi).innerHTML = "";
		
	}
	else
	{
		document.getElementById('status_'+loopi).innerHTML = jsonObj['status']['action']+': '+jsonObj['resource']['type'];
		document.getElementById('label_'+loopi).innerHTML = jsonObj['resource']['label'];				
		
		document.getElementById('loopi_'+loopi).style.backgroundColor = CLR_PLAYING;
		
		if(jsonObj["status"]["playing"] === undefined)
		{
			document.getElementById('playout_'+loopi).innerHTML = '';
			document.getElementById('playout_time_'+loopi).innerHTML = '';
		}
		else
		{
			var fileIndex = g_loopi_array[loopi].findFile(jsonObj["status"]["playing"]["uid"]);
			if(fileIndex > -1)
			{

				document.getElementById('playout_'+loopi).innerHTML = g_loopi_array[loopi].files[fileIndex]["label"];
			}
			else
			{
				document.getElementById('playout_'+loopi).innerHTML = jsonObj["status"]["playing"]["uid"];
			}
			
			document.getElementById('playout_time_'+loopi).innerHTML = millisecondsToTime(jsonObj["status"]["playing"]["time"]);
		}
			
	}
	
}





function getConfig(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/config",callback);
}


function getStatus(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/status",callback);
}

function getFiles(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/files",callback);
}

function getFile(loopi, uid, callback)
{
	ajaxGet(loopi, "/x-epi/files/"+uid,callback);
}

function getSchedules(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/schedules",callback);
}

function getSchedule(loopi, uid, callback)
{
	ajaxGet(loopi, "/x-epi/schedules/"+uid,callback);
}

function getPlaylists(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/playlists",callback);
}

function getPlaylist(loopi, uid, callback)
{
	ajaxGet(loopi, "/x-epi/playlists/"+uid,callback);
}

function getInfo(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/info",callback);
}

function getPower(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/power",callback);
}

function getUpdate(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/update",callback);
}

function getOutputs(loopi, callback)
{
	ajaxGet(loopi, "/x-epi/outputs",callback);
}

function ajaxGet(loopi, endpoint, callback)
{
	var ajax = new XMLHttpRequest();
	ajax.timeout = 2000;
	
	ajax.onload = function() 
	{
		if(this.readyState == 4)
		{
			if(this.status === 200)
			{
				g_loopi_array[loopi].connected = true;
				callback(loopi, JSON.parse(this.responseText));
			}
			else
			{
				console.log("ajax status="+this.status);
				loopiOffline(loopi);
				callback(loopi, JSON.parse(this.responseText));
			}
		}
	}
	ajax.onerror = function(e)
	{
		console.log("ajax error");
		loopiOffline(loopi);
		callback(loopi, null);
	}
	ajax.ontimeout = function(e)
	{
		console.log("ajax timeout");
		loopiOffline(loopi);
		callback(loopi, null);
	}
	console.log("http://"+g_loopi_array[loopi].url+endpoint);
	ajax.open("GET", "http://"+g_loopi_array[loopi].url+endpoint, true);
	ajax.send();
}


function loopiOffline(loopi)
{
	console.log("loopi "+loopi+" offline");
	document.getElementById('loopi_'+loopi).style.backgroundColor = CLR_ERROR;
	document.getElementById("lock_"+loopi).innerHTML = "OFFLINE";
	document.getElementById("lock_"+loopi).classList.add("uk-label-danger");
	document.getElementById("lock_"+loopi).classList.remove("uk-label-warning");
	document.getElementById("lock_"+loopi).classList.remove("uk-label-success");
	
	g_loopi_array[loopi].connected = false;
	
}




function millisecondsToTime(milliseconds)
{
	var seconds = Math.floor(milliseconds/1000)%60;
	var minutes = Math.floor(milliseconds/(1000*60))%60;
	var hours = Math.floor(milliseconds/(1000*3600));
	
	return toString(hours)+":"+toString(minutes)+":"+toString(seconds);
}

function toString(value)
{
	if(value == 0)
	{
		return '00';
	}
	else if(value < 10)
	{
		return '0'+value;
	}
	return value;
}



function addLoopi()
{
	//UIkit.modal(document.getElementById('add_loopi')).show();
	UIkit.modal.prompt('Add loopi: {ipaddress:port}', '').then(function (name) 
	{
		if(name !== null && name != "")
		{
			var ajax = new XMLHttpRequest();
			ajax.onreadystatechange = function()
			{
				if(this.readyState == 4)
				{
					console.log(this.responseText);
					if(this.status === 200)
					{
						var jsonObj = JSON.parse(this.responseText);
						if(jsonObj.error !== undefined)
						{
							alert(jsonObj.error);
						}
						else
						{
							createLoopi(jsonObj.loopi);
						}
					}
				}
			}
			ajax.open("POST", "ajax/add_loopi.php", true);
			ajax.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			ajax.send("loopi="+name);
		}
	});
}



function createLoopi(url)
{
	var grid = document.getElementById('loopi_grid');
	
	var div = document.createElement('div');
	
	var card = document.createElement('a');
	card.classList.add('uk-width-large');
	card.classList.add('uk-card');
	card.classList.add('uk-card-default');
	card.classList.add('uk-card-body');
	card.classList.add('uk-card-hover');
	card.classList.add('uk-card-small');
	card.id = 'loopi_'+g_loopi_array.length;
	card.href = 'status/?loopi='+url;
	
	var header = document.createElement('div');
	header.className= 'uk-card-header';
	
	var h3 = document.createElement('h3');
	h3.className = 'uk-card-title';
	h3.innerHTML = url;
	
	
	var host = document.createElement('span');
	host.id = 'host_'+g_loopi_array.length;;
	
	h3.appendChild(host);
	header.appendChild(h3);
	card.appendChild(header);
	
	
	var body = document.createElement('div');
	body.className= 'uk-card-body';

	body.appendChild(createBodyGrid('Status', 'status'));
	body.appendChild(createBodyGrid('Resource','label'));
	body.appendChild(createBodyGrid('Playout','playout'));
	body.appendChild(createBodyGrid('Time','playout_time'));

    card.appendChild(body);
            
	var footer = document.createElement('div');
	footer.className= 'uk-card-footer';
            
    var footer_label = document.createElement('div');
    footer_label.className = 'uk-label';
    footer_label.id = 'lock_'+g_loopi_array.length;
    footer.appendChild(footer_label);
    card.appendChild(footer);
    
    grid.insertBefore(card, document.getElementById('loopi_add'));
            
    //Now create the loopi itself
	g_loopi_array.push(new loopi(g_loopi_array.length, url));
}


function createBodyGrid(name, id)
{
	var body_grid = document.createElement('div');
	body_grid.classList.add('uk-child-width-expand');
	body_grid.classList.add('uk-grid-small');
	body_grid.classList.add('uk-text-left');
	body_grid.classList.add('uk-link-reset');
	body_grid.classList.add('uk-display-block');
	body_grid.setAttribute('uk-grid',true);
	
	
	
	var div_title = document.createElement('div');
	div_title.className = 'uk-width-1-4';
	
	var span = document.createElement('span');
	span.className = 'uk-text-bold';
	span.innerHTML = name+':';
	
	div_title.appendChild(span);
	body_grid.appendChild(div_title);
	
	var div_value = document.createElement('div');
	div_value.id = id+'_'+g_loopi_array.length;
	div_value.innerHTML = '?';
	
	body_grid.appendChild(div_value);
	
	return body_grid;
}
	
	
	
function getLoopiConfig_Details()
{
	
	if(g_loopi_array[0].connected == false)
	{
		getConfig(0, handleConfig_Details)
	}
}

function handleConfig_Details(loopi, jsonObj)
{
	if(jsonObj !== null)
	{
		updateConfig_Details(loopi, jsonObj);
		getStatus(loopi, handleStatus_Details);
	}
}

function handleStatus_Details(loopi, jsonObj)
{
	updateStatus_Details(loopi, jsonObj);
	
	getFiles(loopi, storeFiles_Details);

}


function storeFiles_Details(loopi, jsonObj)
{
	g_loopi_array[loopi].setFiles(jsonObj);
	ws_connect(loopi, updateStatus_Details);
	
}

function updateConfig_Details(loopi, jsonObj)
{
	var title = document.getElementById('hostname');
	
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setConfig(jsonObj);
			
		if(title)
		{
			title.innerHTML = jsonObj["server"]["hostname"];
		}	
	}
}

function updateStatus_Details(loopi, jsonObj)
{
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setStatus(jsonObj);
		
		if(jsonObj["locked"] !== undefined)
		{
			if(jsonObj["locked"] == false)
			{
				document.getElementById("lock").innerHTML = "Unlocked";
				document.getElementById("lock").classList.remove("uk-label-danger");
				document.getElementById("lock").classList.remove("uk-label-warning");
				document.getElementById("lock").classList.add("uk-label-success");
				document.getElementById("button_lock").innerHTML = "Lock";
				
			}
			else if(jsonObj["locked"] == true)
			{
				document.getElementById("lock").innerHTML = "Locked";
				document.getElementById("lock").classList.remove("uk-label-success");
				document.getElementById("lock").classList.remove("uk-label-danger");
				document.getElementById("lock").classList.add("uk-label-warning");
				document.getElementById("button_lock").innerHTML = "Unlock";
				
				document.getElementById("button_stop").style.visibility = "hidden";
			}
		}
	}
	
	if(jsonObj["player"] === undefined)
	{
		document.getElementById("player").innerHTML = "Unknown";
		document.getElementById("player").classList.remove("uk-label-success");
		document.getElementById("player").classList.remove("uk-label-warning");
		document.getElementById("player").classList.add("uk-label-danger");
		document.getElementById("section_player").style.display = "none";
		
		document.getElementById("button_stop").style.visibility = "hidden";
		
		
	}
	else if(jsonObj["player"] === "Stopped")
	{
		document.getElementById("player").innerHTML = "Stopped";
		document.getElementById("player").classList.remove("uk-label-success");
		document.getElementById("player").classList.remove("uk-label-warning");
		document.getElementById("player").classList.remove("uk-label-danger");
		
		document.getElementById("section_player").style.display = "none";
		document.getElementById("card_resource").style.visibility = "hidden";
		document.getElementById("card_playout").style.visibility = "hidden";
		
		document.getElementById("button_stop").style.visibility = "hidden";
	}
	else if(jsonObj["player"] === "Running")
	{
		document.getElementById("player").innerHTML = "Running";
		document.getElementById("player").classList.add("uk-label-success");
		document.getElementById("player").classList.remove("uk-label-warning");
		document.getElementById("player").classList.remove("uk-label-danger");
		
		showStopButton(loopi);
		
		document.getElementById("section_player").style.display = "block";
		document.getElementById("card_resource").style.visibility = "visible";
		
		document.getElementById('resource_type').innerHTML = jsonObj["resource"]["type"];
		document.getElementById('resource_label').innerHTML = jsonObj["resource"]["label"];
		
		document.getElementById('resource_status').innerHTML = jsonObj["status"]["action"];
		if(jsonObj["status"]["action"] == "Idle")
		{
			document.getElementById('resource_status').classList.remove("loopi_label_running");
			document.getElementById('resource_status').classList.add("loopi_label_idle");
		}
		else
		{
			document.getElementById('resource_status').classList.add("loopi_label_running");
			document.getElementById('resource_status').classList.remove("loopi_label_idle");
		}
		
		if(jsonObj["status"]["playing"] !== undefined)
		{
			document.getElementById("card_playout").style.visibility = "visible";
			
			var fileIndex = g_loopi_array[loopi].findFile(jsonObj["status"]["playing"]["uid"]);
			if(fileIndex > -1)
			{

				document.getElementById('playout_label').innerHTML = g_loopi_array[loopi].files[fileIndex]["label"];
			}
			else
			{
				document.getElementById('playout_label').innerHTML = jsonObj["status"]["playing"]["uid"];
			}
			
			document.getElementById("playout_started").innerHTML = jsonObj["status"]["playing"]["started_at"];
			document.getElementById("playout_length").innerHTML = millisecondsToTime(jsonObj["status"]["playing"]["length"]);
			document.getElementById("playout_time").innerHTML = millisecondsToTime(jsonObj["status"]["playing"]["time"]);
			document.getElementById("playout_loop").innerHTML = jsonObj["status"]["playing"]["loop"]
			if(jsonObj["status"]["playing"]["resampler"] == true)
			{
				document.getElementById("playout_src").classList.remove("loopi_label_idle");
				document.getElementById("playout_src").classList.add("loopi_label_warning");
				document.getElementById("playout_src").innerHTML = "On";
			}
			else
			{
				document.getElementById("playout_src").classList.add("loopi_label_idle");
				document.getElementById("playout_src").classList.remove("loopi_label_warning");
				document.getElementById("playout_src").innerHTML = "Off";
			}
		}
		else
		{
			document.getElementById("card_playout").style.visibility = "hidden";
		}
		
	}
	else if(jsonObj["player"] === "Orphaned")
	{
		document.getElementById("player").innerHTML = "Orphaned";
		document.getElementById("player").classList.remove("uk-label-success");
		document.getElementById("player").classList.add("uk-label-warning");
		document.getElementById("player").classList.remove("uk-label-danger");
		
		document.getElementById("section_player").style.display = "none";
		document.getElementById("card_resource").style.visibility = "hidden";
		document.getElementById("card_playout").style.visibility = "hidden";
		
		showStopButton(loopi);
	}
}


function showStopButton(loopi)
{
	if(g_loopi_array[loopi].status.locked == false)
	{
		document.getElementById("button_stop").style.visibility = "visible";
	}
	else
	{
		document.getElementById("button_stop").style.visibility = "hidden";
	}
}



function lock()
{
	var lock = false;
	var message = "Unlock loopi: Enter password";
	if(document.getElementById('button_lock').innerHTML == "Lock")
	{
		lock = true;
		message = "Lock loopi: Enter password";
	}
				
	UIkit.modal.prompt(message, '').then(function (password) 
	{
		if(password !== null && password != "")
		{
			var play = { "command" : "lock", "lock" : lock, "password" : password};
			ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handleStatusPatch);
		}
	});
}

function stop()
{	
	var play = { "command" : "stop"};
	ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handleStatusPatch);
}


function ajaxPatch(loopi, endpoint, jsonData, callback)
{
	var ajax = new XMLHttpRequest();
	ajax.onreadystatechange = function()
	{
		
		if(this.readyState == 4)
		{
			var jsonObj = JSON.parse(this.responseText);
			callback(this.status, jsonObj);
		}
	}
	console.log(jsonData);
	ajax.open("PATCH", "http://"+g_loopi_array[0].url+endpoint, true);
	ajax.setRequestHeader("Content-type", "application/json");
	ajax.send(jsonData);
}

function handleStatusPatch(status, jsonObj)
{
	if(status != 200)
	{
		alert(jsonObj["reason"]);
	}		
}
