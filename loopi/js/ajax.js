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
	
	setFileDetails(jsonObj)
	{
		var i = this.findFile(jsonObj["uid"]);
		if(i != -1)
		{
			this.files[i].details = jsonObj;
			
		}
	}
	
	setPlaylistDetails(jsonObj)
	{
		var i = this.findPlaylist(jsonObj["uid"]);
		if(i != -1)
		{
			this.playlists[i].details = jsonObj;
			
		}
	}
	
	setScheduleDetails(jsonObj)
	{
		var i = this.findSchedule(jsonObj["uid"]);
		if(i != -1)
		{
			this.schedules[i].details = jsonObj;
			
		}
	}
	
	
	findFile(uid)
	{
		for(var i = 0; i < this.files.length; i++)
		{
			if(this.files[i]['uid'] == uid)
			{
				return i;
			}
		}
		return -1;
	}
	
	findPlaylist(uid)
	{
		for(var i = 0; i < this.playlists.length; i++)
		{
			if(this.playlists[i]['uid'] == uid)
			{
				return i;
			}
		}
		return -1;
	}
	
	findSchedule(uid)
	{
		for(var i = 0; i < this.schedules.length; i++)
		{
			if(this.schedules[i]['uid'] == uid)
			{
				return i;
			}
		}
		return -1;
	}
	
	removeFile(uid)
	{
		var i = this.findFile(uid);
		if(i > -1)
		{
			this.files.splice(i,1);
		}		
	}
	removePlaylist(uid)
	{
		var i = this.findPlaylist(uid);
		if(i > -1)
		{
			this.playlists.splice(i,1);
		}		
	}
	removeSchedule(uid)
	{
		var i = this.findSchedule(uid);
		if(i > -1)
		{
			this.schedules.splice(i,1);
		}		
	}
}


var g_loopi_array = new Array();
var g_ajax = new XMLHttpRequest();
	

const CLR_PLAYING = "#92d14f";
const CLR_IDLE = "#8db4e2";
const CLR_ERROR =  "#ff7777";
const CLR_ORPHANED =  "#ffa000";
const CLR_NO_FILE = "#a0a0a0"
const CLR_UNKNOWN = "#c8c8c8"
const CLR_WARNING = "#ffa000"
const CLR_CONNECTING = "#ffff00";
const CLR_SYNC = "#008000";



function dashboard()
{
	var loopi = 0;
			
	getLoopiConfig_Dashboard(loopi);
	
}

function getLoopiConfig_Dashboard(loopi)
{
	
	if(g_loopi_array[loopi].connected == false)
	{
		getConfig(loopi, handleConfig_Dashboard)
	}
	else
	{
		doNextLoopi_Dashboard(loopi);
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
		doNextLoopi_Dashboard(loopi);
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
	ws_connect(loopi, updateStatus_Dashboard, null,null);
	doNextLoopi_Dashboard(loopi);
}


function ws_connect(loopi, statusCallback, systemCallback, resourceCallback)
{
	g_loopi_array[loopi].setWebsocket(new WebSocket('ws://' + g_loopi_array[loopi].url));
	g_loopi_array[loopi].ws.index = loopi;			
	g_loopi_array[loopi].ws.timeout = true;
	g_loopi_array[loopi].ws.onopen = function(ev)  { this.tm = setTimeout(loopiOffline, 4000, this.index) };
	g_loopi_array[loopi].ws.onerror = function(ev) { loopiOffline(this.index); };
	g_loopi_array[loopi].ws.onclose = function(ev) { loopiOffline(this.index); };
	
	
	g_loopi_array[loopi].ws.onmessage = function(ev) 
	{
		clearTimeout(this.tm);
		if(this.timeout)
		{
			this.tm = setTimeout(loopiOffline, 4000, this.index);
		}
		
		var jsonObj = JSON.parse(ev.data);
		
		if(jsonObj["player"] !== undefined)
		{
			if(statusCallback !== null)
			{
				statusCallback(this.index, jsonObj);
			}
		}
		else if(jsonObj["application"] !== undefined)
		{
			if(systemCallback !== null)
			{
				systemCallback(this.index, jsonObj);
			}
		}
		else
		{
			if(Array.isArray(jsonObj) && resourceCallback !== null)
			{
				resourceCallback(this.index, jsonObj);
			}
		}
	}	
}



function doNextLoopi_Dashboard(loopi)
{
	loopi++;
	if(loopi < g_loopi_array.length)
	{
		getLoopiConfig_Dashboard(loopi);
	}
	else
	{
		setTimeout(dashboard, 2000);
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
	
	if(jsonObj["player"] === undefined || jsonObj["player"] == "Stopped" || (jsonObj["status"] == undefined && jsonObj["player"] != "Orphaned"))
	{
		document.getElementById('status_'+loopi).innerHTML = 'Idle';
		document.getElementById('label_'+loopi).innerHTML = '';		
		
		document.getElementById('loopi_'+loopi).style.backgroundColor = CLR_IDLE;		
		document.getElementById('playout_'+loopi).innerHTML = "";
		document.getElementById('playout_time_'+loopi).innerHTML = "";
		
	}
	else if(jsonObj["player"] == "Orphaned")
	{
		document.getElementById('status_'+loopi).innerHTML = 'Orphaned';
		document.getElementById('label_'+loopi).innerHTML = 'Unknown';		
		
		document.getElementById('loopi_'+loopi).style.backgroundColor = CLR_ORPHANED;		
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
				loopiOffline(loopi);
				callback(loopi, JSON.parse(this.responseText));
			}
		}
	}
	ajax.onerror = function(e)
	{
		loopiOffline(loopi);
		callback(loopi, null);
	}
	ajax.ontimeout = function(e)
	{
		loopiOffline(loopi);
		callback(loopi, null);
	}
	ajax.open("GET", "http://"+g_loopi_array[loopi].url+endpoint, true);
	ajax.send();
}



function loopiOffline(loopi)
{
	document.getElementById('loopi_'+loopi).style.backgroundColor = CLR_ERROR;
	document.getElementById("lock_"+loopi).innerHTML = "OFFLINE";
	document.getElementById("lock_"+loopi).classList.add("loopi_label_danger");
	document.getElementById("lock_"+loopi).classList.remove("loopi_label_warning");
	document.getElementById("lock_"+loopi).classList.remove("loopi_label_running");
	
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
		clearCron();
		getConfig(0, handleConfig_Details)
	}
	setTimeout(getLoopiConfig_Details, 2000);
}

function handleConfig_Details(loopi, jsonObj)
{
	if(jsonObj !== null)
	{
		document.getElementById('loopi_'+loopi).style.backgroundColor = "#ffffff";
		
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
	
	updateResourceSelects(loopi);
	
	listResource_Details(jsonObj, "files", getFileDetails);
	
	getPlaylists(loopi, storePlaylists_Details);
}


function storePlaylists_Details(loopi, jsonObj)
{
	g_loopi_array[loopi].setPlaylists(jsonObj);
	
	updateResourceSelects(loopi);
	
	listResource_Details(jsonObj, "playlists", getPlaylistDetails);
	
	getSchedules(loopi, storeSchedules_Details);
}

function storeSchedules_Details(loopi, jsonObj)
{
	g_loopi_array[loopi].setSchedules(jsonObj);
	
	listResource_Details(jsonObj, "schedules", getScheduleDetails);
	
	ws_connect(loopi, updateStatus_Details, updateSystem_Details, updateResources_Details);
	
}


function updateResourceSelects(loopi)
{
	var playlistSelect =  null;
	var scheduleSelect =  null;
	
	playlistSelect = document.getElementById('select_files');
	if(playlistSelect)
	{
		clearElement(playlistSelect);
	}
		
	scheduleSelect = document.getElementById('schedule_select_files');
	if(scheduleSelect)
	{
		clearElement(scheduleSelect);
	}
	
	var fileObj = g_loopi_array[loopi].files;
	fileObj.sort(function(a,b) { 
								var x = a.label.toLowerCase();
								var y = b.label.toLowerCase();
								if(x < y) return -1;
								if(y < x) return 1;
								return 0;
							});
	
	for(var i = 0; i < fileObj.length; i++)
	{
		if(playlistSelect !== null)
		{
			var opt = document.createElement('option');
			opt.value = fileObj[i]["uid"];
			opt.innerHTML = fileObj[i]["label"];
			playlistSelect.appendChild(opt);
			
			scheduleSelect.appendChild(opt.cloneNode(true));		
		}	
	}
	
	var playlistObj = g_loopi_array[loopi].playlists;
	playlistObj.sort(function(a,b) { 
								var x = a.label.toLowerCase();
								var y = b.label.toLowerCase();
								if(x < y) return -1;
								if(y < x) return 1;
								return 0;
							});
	
	for(var i = 0; i < playlistObj.length; i++)
	{
		if(scheduleSelect !== null)
		{
			var opt = document.createElement('option');
			opt.value = playlistObj[i]["uid"];
			opt.innerHTML = '&equiv; '+playlistObj[i]["label"];
			scheduleSelect.appendChild(opt);
		}	
	}
}

function listResource_Details(jsonObj, type, clickFunction)
{
	var ul = document.getElementById('resource_'+type);
	clearElement(ul);
	
	
	var resObj = jsonObj;
	resObj.sort(function(a,b) { 
								var x = a.label.toLowerCase();
								var y = b.label.toLowerCase();
								if(x < y) return -1;
								if(y < x) return 1;
								return 0;
							});
	
	
	for(var i = 0; i < resObj.length; i++)
	{
		createResourceLi(ul, resObj[i], type, clickFunction);
			
	}
}

function createResourceLi(ul, resObj, type, clickFunction)
{
	var li = document.createElement('li');
	li.classList.add('uk-margin-small-top');
	li.style.padding = '4px 6px';
	li.id = "li_"+resObj['uid'];
		
	var a = document.createElement('a');
	a.className = 'uk-accordion-title';
	a.id = resObj['uid'];
	a.addEventListener('click', clickFunction, false);
	a.href='#';
		
	var div_label = document.createElement('div');
	div_label.style.float = 'left';
	div_label.style.width = '60%';
		
	var span = document.createElement('span');
	span.innerHTML = resObj['label'];
	span.id="span_"+resObj['uid'];
	div_label.appendChild(span);
		
	var div_action = document.createElement('div');
	div_action.style.float = 'left';
	div_action.style.width = '20%';
		
	var buttonOnce = document.createElement('button');
	buttonOnce.className = "uk-button";
	buttonOnce.classList.add("uk-button-primary");
	buttonOnce.classList.add("uk-button-small");
	buttonOnce.id = "play_"+type+"_"+resObj["uid"];
	buttonOnce.innerHTML = 'Play';
	buttonOnce.addEventListener('click',play, false);
	div_action.appendChild(buttonOnce);
			
	
	var buttonDelete = document.createElement('button');
	buttonDelete.className = "uk-button";
	buttonDelete.classList.add("uk-button-danger");
	buttonDelete.classList.add("uk-button-small");
	buttonDelete.style.marginLeft = '5px';
	buttonDelete.id = "delete_"+type+"_"+resObj["uid"];
	buttonDelete.innerHTML = 'Delete';
	buttonDelete.addEventListener('click',deleteResource, false);
	div_action.appendChild(buttonDelete);
	
		
	a.appendChild(div_label);
	a.appendChild(div_action);
		
	li.appendChild(a);
		
			
	var div_content = document.createElement('div');
	div_content.className = "uk-accordion-content";
	
		
	var tableResource = document.createElement('table');
	tableResource.className = 'uk-table';
	tableResource.classList.add('uk-table-small');
		
	var tbodyResourceDetails = document.createElement('tbody');
	tbodyResourceDetails.id = 'details_'+resObj["uid"];
		
	tableResource.appendChild(tbodyResourceDetails);
	div_content.appendChild(tableResource);
		
	var tableSpecific = document.createElement('table');
	tableSpecific.className = 'uk-table';
	tableSpecific.classList.add('uk-table-small');
	if(type != 'files')
	{
		tableSpecific.classList.add('uk-table-divider');
	}
		
	var theadspecific = document.createElement('thead');
	theadspecific.id = 'specifichead_'+resObj["uid"];
		
	var tbodySpecificDetails = document.createElement('tbody');
	tbodySpecificDetails.id = 'specific_'+resObj["uid"];
		
	tableSpecific.appendChild(theadspecific);
	tableSpecific.appendChild(tbodySpecificDetails);
	div_content.appendChild(tableSpecific);

	var buttonMod = document.createElement('button');
	buttonMod.className = "uk-button";
	buttonMod.classList.add("uk-button-primary");
	buttonMod.classList.add("uk-button-small");
	buttonMod.style.marginLeft = '5px';
	buttonMod.id = "modify_"+type+"_"+resObj["uid"];
	buttonMod.addEventListener('click',modifyResource, false);
	div_content.appendChild(buttonMod);
	
	if(type != 'files')
	{
		buttonMod.innerHTML = 'Modify';
	}
	else
	{
		buttonMod.innerHTML = 'Relabel';
		var buttonReplace = buttonMod.cloneNode();
		buttonReplace.style.marginLeft = '5px';
		buttonReplace.innerHTML = 'Replace Audio';
		buttonReplace.id = "replace_"+type+"_"+resObj["uid"];
		buttonReplace.addEventListener('click',modifyResource, false);
		div_content.appendChild(buttonReplace);
	}
		
	li.appendChild(div_content);
		
	ul.appendChild(li);

}


function getFileDetails(e)
{
	var target = e.target;
	while(target.tagName != 'A' && target.tagName != 'a')
	{
		target = target.parentElement;
	}
	
	var tbody = document.getElementById('details_'+target.id);
	if(!tbody.firstChild)
	{
		getFile(0, target.id, createFileDetails);
	}
}

function getPlaylistDetails(e)
{
	var target = e.target;
	while(target.tagName != 'A' && target.tagName != 'a')
	{
		target = target.parentElement;
	}
	var tbody = document.getElementById('details_'+target.id);
	if(!tbody.firstChild)
	{
		getPlaylist(0, target.id, createPlaylistDetails);
	}
}

function getScheduleDetails(e)
{
	var target = e.target;
	while(target.tagName != 'A' && target.tagName != 'a')
	{
		target = target.parentElement;
	}
	var tbody = document.getElementById('details_'+target.id);
	if(!tbody.firstChild)
	{
		getSchedule(0, target.id, createScheduleDetails);
	}
}


function createDetailsRow(key, value)
{
	var tr = document.createElement('tr');
	var td_key = document.createElement('td');
	td_key.style.padding = '3px 6px';
	
	var span = document.createElement('span');
	span.className = 'uk-text-bold';
	span.innerHTML = key;
	
	td_key.appendChild(span);
	
	
	var td_value = document.createElement('td');
	td_value.innerHTML = value;
	td_value.style.padding = '3px 6px';
	
	tr.appendChild(td_key);
	tr.appendChild(td_value);
	
	return tr;
}

function createResourceDetails(jsonObj)
{
	var tbody = document.getElementById('details_'+jsonObj["uid"]);
	if(tbody)
	{
		clearElement(tbody);
		
		tbody.appendChild(createDetailsRow('UID', jsonObj['uid']));
		tbody.appendChild(createDetailsRow('Description', jsonObj['description']));
		tbody.appendChild(createDetailsRow('Created', jsonObj['created']));
		if(jsonObj['modified'] !== undefined)
		{
			tbody.appendChild(createDetailsRow('Modified', jsonObj['modified']));
		}
		
		if(jsonObj['file_modified'] !== undefined)
		{
			tbody.appendChild(createDetailsRow('File Modified', jsonObj['file_modified']));
		}
		tbody.appendChild(createDetailsRow('Locked', jsonObj['locked']));
	}
	return tbody;
}

function createFileDetails(loopi, jsonObj)
{
	g_loopi_array[loopi].setFileDetails(jsonObj);
	
	createResourceDetails(jsonObj);
	var tbody = document.getElementById('specific_'+jsonObj["uid"]);
	if(tbody)
	{
		clearElement(tbody);
		tbody.appendChild(createFormatRow('Channels', jsonObj['channels'], 'Sample Rate', jsonObj['samplerate']));
		tbody.appendChild(createFormatRow('Type', jsonObj["format"]['type'], "Format", jsonObj["format"]['subtype']));
		tbody.appendChild(createDetailsRow('Length', millisecondsToTime(jsonObj['length'], '&nsbp;', '&nsbp;')));	
	}
}


function createFormatRow(key1, value1, key2, value2)
{
	var tr = document.createElement('tr');
	var td_key1 = document.createElement('td');
	td_key1.style.padding = '3px 6px';
	
	var span1 = document.createElement('span');
	span1.className = 'uk-text-bold';
	span1.innerHTML = key1;
	
	td_key1.appendChild(span1);
	
	
	var td_value1 = document.createElement('td');
	td_value1.innerHTML = value1;
	td_value1.style.padding = '3px 6px';
	
	var td_key2 = document.createElement('td');
	td_key2.style.padding = '3px 6px';
	
	var span2 = document.createElement('span');
	span2.className = 'uk-text-bold';
	span2.innerHTML = key2;
	
	td_key2.appendChild(span2);
	
	
	var td_value2 = document.createElement('td');
	td_value2.innerHTML = value2;
	td_value2.style.padding = '3px 6px';
	
	
	
	tr.appendChild(td_key1);
	tr.appendChild(td_value1);
	tr.appendChild(td_key2);
	tr.appendChild(td_value2);
	
	return tr;
}

function createPlaylistDetails(loopi, jsonObj)
{
	g_loopi_array[loopi].setPlaylistDetails(jsonObj);
	
	createResourceDetails(jsonObj);
	
	var thead = document.getElementById('specifichead_'+jsonObj["uid"]);
	clearElement(thead);
	
	var thFile = document.createElement('th');
	thFile.innerHTML = 'File';
	
	var thTime = document.createElement('th');
	thTime.innerHTML = 'Times To Play';
	thead.appendChild(thFile);
	thead.appendChild(thTime);
	
	
	var tbody = document.getElementById('specific_'+jsonObj["uid"]);
	if(tbody)
	{
		clearElement(tbody);
		for(var i = 0; i < jsonObj['files'].length; i++)
		{
			var file = g_loopi_array[loopi].findFile(jsonObj['files'][i]['uid']);
			
			var loop = '&infin;';
			if(jsonObj['files'][i]['times_to_play'] >= 0)
			{
				loop = jsonObj['files'][i]['times_to_play'];
			}
			
			if(file > -1)
			{
				tbody.appendChild(createDetailsRow(g_loopi_array[loopi].files[file]['label'], loop));
			}
			else
			{
				tbody.appendChild(createDetailsRow(jsonObj['files'][i]['uid'], loop));
			}
		}
	}
}

function createScheduleDetails(loopi, jsonObj)
{
	
	g_loopi_array[loopi].setScheduleDetails(jsonObj);
	
	createResourceDetails(jsonObj);
	
	var thead = document.getElementById('specifichead_'+jsonObj["uid"]);
	clearElement(thead);
	
	var thFile = document.createElement('th');
	thFile.innerHTML = 'Label';
	
	var thTime = document.createElement('th');
	thTime.innerHTML = 'Times To Play';
	
	var thShuffle = document.createElement('th');
	thShuffle.innerHTML = 'Shuffle';
	
	
	var thS = document.createElement('th');
	thS.innerHTML = 'Schedule To Play';
	
	thead.appendChild(thFile);
	thead.appendChild(thTime);
	thead.appendChild(thShuffle);
	thead.appendChild(thS);
	
	
	var tbody = document.getElementById('specific_'+jsonObj["uid"]);
	if(tbody)
	{
		clearElement(tbody);
		createScheduleResourceDetails(tbody, 'file',jsonObj);
		createScheduleResourceDetails(tbody, 'playlist',jsonObj);
		
	}
}

function createScheduleResourceDetails(tbody,type, jsonObj)
{
	var res = type+'s';
	for(var i = 0; i < jsonObj[res].length; i++)
	{
		var file = -1;
		var label = jsonObj[res][i]['uid'];
		var shuffle;
		
		if(type == 'file')
		{
			file = g_loopi_array[0].findFile(jsonObj[res][i]['uid']);
			if(file > -1)
			{
				label = g_loopi_array[0].files[file]['label'];
			}
			shuffle = "-";
		}
		else
		{
			file = g_loopi_array[0].findPlaylist(jsonObj[res][i]['uid']);
			if(file > -1)
			{
				label = g_loopi_array[0].playlists[file]['label'];
			}
			if(jsonObj[res][i]['shuffle'] == true)
			{
				shuffle = 'Y';
			}
			else
			{
				shuffle = 'N';
			}
		}
		
		
		tbody.appendChild(createScheduleDetailsRow(type, label, jsonObj[res][i]['times_to_play'], shuffle, jsonObj[res][i]['cron']));
	}
}

function createScheduleDetailsRow(type, label, loop, shuffle, cron)
{

	var tr = document.createElement('tr');
	
	
		
	var td_label = document.createElement('td');
	if(type == 'file')
	{
		td_label.innerHTML = label;
	}
	else
	{
		td_label.innerHTML = '&equiv; '+label;
	}
	
	td_label.style.padding = '3px 6px';
	
	var td_loop = document.createElement('td');
	if(loop >= 0)
	{
		td_loop.innerHTML = loop;
	}
	else
	{
		td_loop.innerHTML = '&infin;';
	}
	
	
	td_loop.style.padding = '3px 6px';
	
	var td_shuffle = document.createElement('td');
	td_shuffle.innerHTML = shuffle;
	td_shuffle.style.padding = '3px 6px';
	
	tr.appendChild(td_label);
	tr.appendChild(td_loop);
	tr.appendChild(td_shuffle);
	
	var td_cron = document.createElement('td');
	td_cron.style.padding = '3px 6px';
	td_cron.innerHTML = cronstrue.toString(cron, { use24HourTimeFormat: true});
	
	tr.appendChild(td_cron);
			
	
	return tr;
}


function updateConfig_Details(loopi, jsonObj)
{
	var title = document.getElementById('loopi_0');
	
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setConfig(jsonObj);
			
		if(title)
		{
			title.innerHTML = jsonObj["server"]["hostname"];
		}	
		window.document.title = jsonObj["server"]["hostname"];
	}
	
}

function showPlayButtons(mode)
{
	var playButtons = document.querySelectorAll('button[id^="play_"]');
	for(var i = 0; i < playButtons.length; i++)
	{
		playButtons[i].style.visibility = mode;
	}
}

function updateOverallStatus(loopi, jsonObj)
{
	if(jsonObj["locked"] !== undefined)
	{
		if(jsonObj["locked"] == false)
		{
			document.getElementById("lock_0").innerHTML = "Unlocked";
			document.getElementById("lock_0").classList.remove("loopi_label_danger");
			document.getElementById("lock_0").classList.remove("loopi_label_warning");
			document.getElementById("lock_0").classList.add("loopi_label_running");
			document.getElementById("button_lock").innerHTML = "Lock";
			
		}
		else if(jsonObj["locked"] == true)
		{
			document.getElementById("lock_0").innerHTML = "Locked";
			document.getElementById("lock_0").classList.remove("loopi_label_running");
			document.getElementById("lock_0").classList.remove("loopi_label_danger");
			document.getElementById("lock_0").classList.add("loopi_label_warning");
			document.getElementById("button_lock").innerHTML = "Unlock";
			
			document.getElementById("button_stop").style.visibility = "hidden";
		}
	}

	if(jsonObj["current_time"] !== undefined)
	{
		document.getElementById('current_time').innerHTML = jsonObj["current_time"];
	}
}

function updatePlayerStatus(loopi, jsonObj)
{
	if(jsonObj["player"] === undefined)
	{
		document.getElementById("player").innerHTML = "Unknown";
		document.getElementById("player").classList.remove("loopi_label_running");
		document.getElementById("player").classList.remove("loopi_label_warning");
		document.getElementById("player").classList.add("loopi_label_danger");
		
		document.getElementById("button_stop").style.visibility = "hidden";
		
		
	}
	else if(jsonObj["player"] === "Stopped")
	{
		if(document.getElementById("player").innerHTML != "Stopped")
		{
			document.getElementById("player").innerHTML = "Stopped";
			document.getElementById("player").classList.remove("loopi_label_running");
			document.getElementById("player").classList.remove("loopi_label_warning");
			document.getElementById("player").classList.remove("loopi_label_danger");
			
			
			document.getElementById("card_resource").style.visibility = "hidden";
			document.getElementById("card_playout").style.visibility = "hidden";
			
			document.getElementById("button_stop").style.visibility = "hidden";
			
			
			showPlayButtons("visible");
		}
	}
	else if(jsonObj["player"] === "Running")
	{
		if(document.getElementById("player").innerHTML != "Running")
		{
			document.getElementById("player").innerHTML = "Running";
			document.getElementById("player").classList.add("loopi_label_running");
			document.getElementById("player").classList.remove("loopi_label_-warning");
			document.getElementById("player").classList.remove("loopi_label_-danger");
			showPlayButtons("hidden");
				
		
			document.getElementById("card_resource").style.visibility = "visible";
			
			document.getElementById('resource_type').innerHTML = jsonObj["resource"]["type"];
			document.getElementById('resource_label').innerHTML = jsonObj["resource"]["label"];
		}
		showStopButton(loopi,'stop');
		if(jsonObj["status"] !== undefined)
		{
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
				document.getElementById("playout_time").innerHTML = 
millisecondsToTime(jsonObj["status"]["playing"]["time"])+" (-"+millisecondsToTime((jsonObj["status"]["playing"]["length"]-jsonObj["status"]["playing"]["time"]))+")";
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
	}
	else if(jsonObj["player"] === "Orphaned")
	{
		document.getElementById("player").innerHTML = "Orphaned";
		document.getElementById("player").classList.remove("uk-label-success");
		document.getElementById("player").classList.add("uk-label-warning");
		document.getElementById("player").classList.remove("uk-label-danger");
		
		document.getElementById("card_resource").style.visibility = "hidden";
		document.getElementById("card_playout").style.visibility = "hidden";
		
		showStopButton(loopi, 'kill');
		showPlayButtons("hidden");
	}
}

function updateStatus_Details(loopi, jsonObj)
{
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setStatus(jsonObj);
		
		updateOverallStatus(loopi, jsonObj);
		updatePlayerStatus(loopi, jsonObj);
	}
	
}


function updateResources_Details(loopi, jsonObj)
{
	for(var i = 0; i < jsonObj.length; i++)
	{
		updateResource_Details(loopi, jsonObj[i]);
	}
}


function updateResource_Details(loopi, jsonObj)
{
	if(jsonObj["modification"] == "modified")
	{
		if(jsonObj["type"] == "file")
		{
			getFile(loopi, jsonObj['uid'], updateFile_Details);
		}
		else if(jsonObj["type"] == "playlist")
		{
			getPlaylist(loopi, jsonObj['uid'], updatePlaylist_Details);
		}
		else if(jsonObj["type"] == "schedule")
		{
			getSchedule(loopi, jsonObj['uid'], updateSchedule_Details);
		}
	}
	else if(jsonObj["modification"] == "deleted")
	{
		//delete the specific elements
		var li = document.getElementById('li_'+jsonObj['uid']);
		if(li !== undefined)
		{
			li.parentNode.removeChild(li);
			
			if(jsonObj["type"] == "file")
			{
				g_loopi_array[loopi].removeFile(jsonObj['uid']);
			}
			else if(jsonObj["type"] == "playlist")
			{
				g_loopi_array[loopi].removePlaylist(jsonObj['uid']);
			}
			else if(jsonObj["type"] == "schedule")
			{
				g_loopi_array[loopi].removeSchedule(jsonObj['uid']);
			}
		}
	}
	else if(jsonObj["modification"] == "added")
	{
		if(jsonObj["type"] == "file")
		{
			getFile(loopi, jsonObj['uid'], addFile_Details);
		}
		else if(jsonObj["type"] == "playlist")
		{
			getPlaylist(loopi, jsonObj['uid'], addPlaylist_Details);
		}
		else if(jsonObj["type"] == "schedule")
		{
			getSchedule(loopi, jsonObj['uid'], addSchedule_Details);
		}
	}
}


function updateFile_Details(loopi, jsonObj)
{
	var i = g_loopi_array[loopi].findFile(jsonObj["uid"]);
	if(i > -1)
	{
		g_loopi_array[loopi].files[i]["label"] = jsonObj["label"];
	}
	
	var span = document.getElementById('span_'+jsonObj["uid"]);
	if(span)
	{
		span.innerHTML = jsonObj["label"];
	}
	createFileDetails(loopi, jsonObj);
}

function updatePlaylist_Details(loopi, jsonObj)
{
	var i = g_loopi_array[loopi].findPlaylist(jsonObj["uid"]);
	if(i > -1)
	{
		g_loopi_array[loopi].playlists[i]["label"] = jsonObj["label"];
	}
	
	var span = document.getElementById('span_'+jsonObj["uid"]);
	if(span)
	{
		span.innerHTML = jsonObj["label"];
	}
	createPlaylistDetails(loop, jsonObj);
}

function updateSchedule_Details(loopi, jsonObj)
{
	var i = g_loopi_array[loopi].findSchedule(jsonObj["uid"]);
	if(i > -1)
	{
		g_loopi_array[loopi].schedules[i]["label"] = jsonObj["label"];
	}
	
	var span = document.getElementById('span_'+jsonObj["uid"]);
	if(span)
	{
		span.innerHTML = jsonObj["label"];
	}
	createScheduleDetails(loopi, jsonObj);
}


function addFile_Details(loopi, jsonObj)
{
	var obj = { "label": jsonObj["label"], "uid" : jsonObj["uid"]};
	g_loopi_array[loopi].files.push(obj);
		
	createResourceLi(document.getElementById("resource_files"), obj, "files", getFileDetails);
	
	createFileDetails(loopi, jsonObj);
}

function addPlaylist_Details(loopi, jsonObj)
{
	var obj = { "label": jsonObj["label"], "uid" : jsonObj["uid"]};
	g_loopi_array[loopi].playlists.push(obj);
		
	createResourceLi(document.getElementById("resource_playlists"), obj, "playlists", getPlaylistDetails);
	
	createPlaylistDetails(loopi, jsonObj);
}

function addSchedule_Details(loopi, jsonObj)
{
	var obj = { "label": jsonObj["label"], "uid" : jsonObj["uid"]};
	g_loopi_array[loopi].schedules.push(obj);
		
	createResourceLi(document.getElementById("resource_schedules"), obj, "schedules", getScheduleDetails);
	
	createScheduleDetails(loopi, jsonObj);
}



function updatePlaylist_Details(loopi, jsonObj)
{
	var i = g_loopi_array[loopi].findPlaylist(jsonObj["uid"]);
	if(i > -1)
	{
		g_loopi_array[loopi].playlists[i]["label"] = jsonObj["label"];
	}
	
	var span = document.getElementById('span_'+jsonObj["uid"]);
	if(span)
	{
		span.innerHTML = jsonObj["label"];
	}
	createPlaylistDetails(loopi, jsonObj);
}

function updateSchedule_Details(loopi, jsonObj)
{
	var i = g_loopi_array[loopi].findSchedule(jsonObj["uid"]);
	if(i > -1)
	{
		g_loopi_array[loopi].schedules[i]["label"] = jsonObj["label"];
	}
	
	var span = document.getElementById('span_'+jsonObj["uid"]);
	if(span)
	{
		span.innerHTML = jsonObj["label"];
	}
	createScheduleDetails(loopi, jsonObj);
}


function showStopButton(loopi, action)
{
	if(g_loopi_array[loopi].status.locked == false)
	{
		document.getElementById("button_stop").style.visibility = "visible";
		document.getElementById("button_stop").setAttribute('action', action);
	}
	else
	{
		document.getElementById("button_stop").style.visibility = "hidden";
	}
}



function createPlaylistEntry()
{
	var sel =  document.getElementById('select_files');
	doCreatePlaylistEntry(sel.options[sel.selectedIndex].text, sel.options[sel.selectedIndex].value, document.getElementById('playlist_file_loop').value); 
}


function doCreatePlaylistEntry(label, uid, loop)
{
	var row = document.getElementById('playlist_entry');
	
	var tr  = document.createElement('tr');
	var td_file =  document.createElement('td');
	var td_loop =  document.createElement('td');
	var td_action =  document.createElement('td');
	
	td_file.style.padding = '3px 6px';
	td_loop.style.padding = '3px 6px';
	td_action.style.padding = '3px 6px';
	
	td_file.className = "uk-background-muted";
	td_loop.className = "uk-background-muted";
	td_action.className = "uk-background-muted";
	
	
	var index = document.getElementById('playlist_count').value;
	document.getElementById('playlist_count').value = index+1;
	
	tr.id = "playlistEntry_"+index;
	
	var span_label = document.createElement('span');
	span_label.id = 'playlistEntryLabel_'+index;
	span_label.innerHTML = label;
	td_file.appendChild(span_label);
	
	
	var input_file = document.createElement('input');
	input_file.type = "hidden";
	input_file.id = "playlistEntryUid_"+index;
	input_file.value = uid;
	td_file.appendChild(input_file);


	var span_loop = document.createElement('span');
	span_loop.id = 'playlistEntryLoopLabel_'+index;
	if(loop >= 0)
	{
		span_loop.innerHTML = loop;
	}
	else
	{
		span_loop.innerHTML = "&infin;";
	}
	td_loop.appendChild(span_loop);
		
	var input_loop = document.createElement('input');
	input_loop.type = "hidden";
	input_loop.id = "playlistEntryLoop_"+index;
	input_loop.value = loop;
	td_loop.appendChild(input_loop);
	
	var buttonMod = document.createElement('button');
	buttonMod.className = "uk-button";
	buttonMod.classList.add("uk-button-small");
	buttonMod.style.background = '#ffffff';
	buttonMod.id = "playlistModify_"+index;
	buttonMod.innerHTML = 'Modify';
	buttonMod.addEventListener('click',modifyPlaylistEntry, false);
	td_action.appendChild(buttonMod);
	
	var button = document.createElement('button');
	button.className = "uk-button";
	button.classList.add("uk-button-small");
	button.style.background = '#ffffff';
	button.style.marginLeft = '5px';
	button.id = "playlistRemove_"+index;
	button.innerHTML = 'Remove';
	button.addEventListener('click',removePlaylistEntry, false);
	td_action.appendChild(button);
	
	tr.appendChild(td_file);
	tr.appendChild(td_loop);
	tr.appendChild(td_action);
	
	row.parentNode.insertBefore(tr, row);
}

function removePlaylistEntry(e)
{
	var target = e.target.id.split('_')[1];
	var row = document.getElementById('playlistEntry_'+target);
	row.parentNode.removeChild(row);
}


function modifyPlaylistEntry(e)
{
	cancelPlaylistEntry();
	
	var index = e.target.id.split('_')[1];
	
	document.getElementById('playlist_edit').value = index;
	
	var row = document.getElementById('playlistEntry_'+index);
	row.style.display = 'none';
	
	var tr  = document.createElement('tr');
	tr.id = 'mod_row';
	
	var td_file =  document.createElement('td');
	var td_loop =  document.createElement('td');
	var td_action =  document.createElement('td');
	
	td_file.style.padding = '3px 6px';
	td_loop.style.padding = '3px 6px';
	td_action.style.padding = '3px 6px';
	
	td_file.className = "loopi_edit";
	td_loop.className = "loopi_edit";
	td_action.className = "loopi_edit";
	
	
		
	var sel =  document.getElementById('select_files');
	
	var selMod = sel.cloneNode(true);
	selMod.value = document.getElementById('playlistEntryUid_'+index).value;
	//set the value
	
	selMod.id = 'playlist_mod_sel';
	td_file.appendChild(selMod);
	
	var loopMod = document.getElementById('playlist_file_loop').cloneNode();
	loopMod.id = 'playlist_mod_loop';
	loopMod.value = document.getElementById('playlistEntryLoop_'+index).value;
	//set the value
	td_loop.appendChild(loopMod);
	
	
	var buttonMod = document.createElement('button');
	buttonMod.className = "uk-button";
	buttonMod.classList.add("uk-button-small");
	buttonMod.style.background = '#ffffff';
	buttonMod.innerHTML = 'Update';
	buttonMod.addEventListener('click',updatePlaylistEntry, false);
	td_action.appendChild(buttonMod);
	
	var button = document.createElement('button');
	button.className = "uk-button";
	button.classList.add("uk-button-small");
	button.style.background = '#ffffff';
	button.style.marginLeft = '5px';
	button.innerHTML = 'Cancel';
	button.addEventListener('click',cancelPlaylistEntry, false);
	td_action.appendChild(button);
	
	tr.appendChild(td_file);	
	tr.appendChild(td_loop);	
	tr.appendChild(td_action);
		
	row.parentNode.insertBefore(tr, row);	
	
}

function updatePlaylistEntry()
{
	var index = document.getElementById('playlist_edit').value;
	
	var sel = document.getElementById('playlist_mod_sel');
	var uid = sel.options[sel.selectedIndex].value;
	var label = sel.options[sel.selectedIndex].text;
	
	document.getElementById("playlistEntryUid_"+index).value = uid;
	document.getElementById('playlistEntryLabel_'+index).innerHTML = label;
	
	document.getElementById('playlistEntryLoop_'+index).value = document.getElementById('playlist_mod_loop').value;
	if(document.getElementById('playlist_mod_loop').value >= 0)
	{
		document.getElementById('playlistEntryLoopLabel_'+index).innerHTML = document.getElementById('playlist_mod_loop').value;
	}
	else
	{
		document.getElementById('playlistEntryLoopLabel_'+index).innerHTML = "&infin;";
	}
	
	
	cancelPlaylistEntry();
	
	
}


function cancelPlaylistEntry()
{
	var mod =document.getElementById('mod_row');
	if(mod !== null)
	{
		var index = document.getElementById('playlist_edit').value;
	
		var row = document.getElementById('playlistEntry_'+index);
		row.style.display = '';
	
		mod.parentNode.removeChild(document.getElementById('mod_row'));
	
		document.getElementById('playlist_edit').value = -1;
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
			ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handleStatusLockPatch);
		}
	});
}

function stop()
{	
	
	var play = { "command" : document.getElementById('button_stop').getAttribute('action')};
	ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handlePatchDefault);
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
	
	ajax.open("PATCH", "http://"+g_loopi_array[0].url+endpoint, true);
	ajax.setRequestHeader("Content-type", "application/json");
	ajax.send(jsonData);
}


function ajaxPut(loopi, endpoint, jsonData, callback)
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
	ajax.open("PUT", "http://"+g_loopi_array[0].url+endpoint, true);
	ajax.setRequestHeader("Content-type", "application/json");
	ajax.send(jsonData);
}

function ajaxDelete(loopi, endpoint, callback)
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
	
	ajax.open("DELETE", "http://"+g_loopi_array[0].url+endpoint, true);
	ajax.send(null);
}

function handleDelete(status, jsonObj)
{
	if(status != 200)
	{
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 4000})
	}
}

function handlePatchDefault(status, jsonObj)
{
	if(status != 200)
	{
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 4000})
	}
	//else
	//{
	//	getStatus(0, handleStatus_Details);
	//}
}

function handleStatusLockPatch(status, jsonObj)
{
	if(status != 200)
	{
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 4000})
	}
	else
	{
		getStatus(0, handleStatus_Details);
	}
}


function fileChosen()
{
	if(document.getElementById('upload_uid').value == '')
	{
		document.getElementById('upload_label').value = document.getElementById('upload_file').files[0].name;
	}
}


function uploadFile()
{
	if(document.getElementById('upload_label').value == '')
	{
		UIkit.notification({message: "You must enter a label for the file", status: 'danger', timeout: 2000})
		return;
	}
	
	if(document.getElementById('upload_description').value == '')
	{
		UIkit.notification({message: "You must enter a description for the file", status: 'danger', timeout: 2000})
		return;
	}
	
	var fd = new FormData(document.getElementById('upload_form'));
	


		
	g_ajax.upload.addEventListener('progress', function(e) {
		var percent_complete = (e.loaded/e.total)*100;
		document.getElementById('progress').value = percent_complete;
		});


	g_ajax.onreadystatechange = function()
	{
		
		if(this.readyState == 4)
		{
			UIkit.modal(document.getElementById('progress_modal')).hide();
			document.getElementById('uploading_label').innerHTML = '';
			document.getElementById('progress').value = 0;
			g_loopi_array[0].ws.timeout = true;
			
			if(this.status == 0)
			{
				UIkit.notification({message: "Upload canceled", status: 'danger', timeout: 2000})
			}
			else if(this.status != 201)
			{
				
				var jsonObj = JSON.parse(this.responseText);
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
			}
		}
		else if(this.readyState == 0 && this.status == 0)
		{			
			UIkit.modal(document.getElementById('progress_modal')).hide();
			document.getElementById('uploading_label').innerHTML = '';
			document.getElementById('progress').value = 0;
			
			g_loopi_array[0].ws.timeout = true;
		}
		
	}

	UIkit.modal(document.getElementById('upload_modal')).hide();

	UIkit.modal(document.getElementById('progress_modal')).show();
	document.getElementById('uploading_label').innerHTML = document.getElementById('upload_label').value;

	//stop the websocket timeout
	clearTimeout(g_loopi_array[0].ws.tm);
	g_loopi_array[0].ws.timeout = false;
	
	var uid = document.getElementById('upload_uid').value;
	if(uid == '')
	{
		g_ajax.open('POST',"http://"+g_loopi_array[0].url+"/x-epi/files");
	}
	else
	{
		g_ajax.open('PUT',"http://"+g_loopi_array[0].url+"/x-epi/files/"+uid);
	}
	g_ajax.send(fd);
	
}

function cancelUpload()
{
	g_ajax.abort();
	
}

function createPlaylist()
{
	if(document.getElementById('playlist_label').value == '')
	{
		UIkit.notification({message: "You must enter a label for the playlist", status: 'danger', timeout: 2000})
		return;
	}
	if(document.getElementById('playlist_description').value == '')
	{
		UIkit.notification({message: "You must enter a description for the playlist", status: 'danger', timeout: 2000})
		return;
	}
		
	var tbody = document.getElementById('playlist_entries');
	if(tbody.childElementCount == 1)		
	{
		UIkit.notification({message: "You must enter some files for the playlist", status: 'danger', timeout: 2000})
		return;
	}
				
	var jsonPost = {"label" : document.getElementById('playlist_label').value, "description" : document.getElementById('playlist_description').value, "files" : []};
	
	var row = tbody.childNodes;
	for(var i = 0; i < tbody.childElementCount; i++)
	{
		if(row[i].id !== undefined)
		{
			var sp = row[i].id.split('_');
			if(sp[0] == 'playlistEntry')
			{
				var uid = document.getElementById('playlistEntryUid_'+sp[1]).value;
				var jsonRow = {"uid" : uid, "times_to_play" : parseInt(document.getElementById('playlistEntryLoop_'+sp[1]).value,10)};
				jsonPost.files.push(jsonRow);
			}
		}
	}
		
	
				
	var ajax = new XMLHttpRequest();
	ajax.onreadystatechange = function()
	{
		if(this.readyState == 4)
		{
			var jsonObj = JSON.parse(this.responseText);
			if(this.status != 201 && this.status != 200)
			{
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
			}
		}
	}		
	
	closePlaylist();
	
	var uid = document.getElementById('playlist_uid').value;
	if(uid == "")
	{
		ajax.open('POST',"http://"+g_loopi_array[0].url+"/x-epi/playlists");
	}
	else
	{
		ajax.open('PUT',"http://"+g_loopi_array[0].url+"/x-epi/playlists/"+uid);
	}
	ajax.setRequestHeader("Content-type", "application/json");
	ajax.send(JSON.stringify(jsonPost));
}

function closePlaylist()
{
	document.getElementById('playlist_label').value = '';
	document.getElementById('playlist_description').value = '';

	var tbody = document.getElementById('playlist_entries');
	while(tbody.childElementCount > 1)
	{
		tbody.removeChild(tbody.firstChild);
	}
	
	UIkit.modal(document.getElementById('playlist_modal')).hide();
}

function play(e)
{

	
	e.stopPropagation();
	
	var split = e.target.id.split('_');
	var type = split[1];
	var uid = split[2];
				
	type = type.substring(0, type.length-1);	//remove the s
	
	document.getElementById('play_label').innerHTML = document.getElementById('span_'+uid).innerHTML;
	
	if(type == 'schedule')
	{
		document.getElementById('play_options').style.display = 'none';
	}
	else
	{
		document.getElementById('play_options').style.display = 'block';
	
		if(type == 'playlist')
		{
			document.getElementById('play_shuffle').style.display = 'block';
		}
		else
		{
			document.getElementById('play_shuffle').style.display = 'none';
		}
	}
	document.getElementById('play_shuffle').checked = false;
	
	document.getElementById('play_uid').value = uid;
	document.getElementById('play_type').value = type;
	
	document.getElementsByName('play_radio')[0].checked = true;
	
	UIkit.modal(document.getElementById('play_modal')).show();
}

function doPlay()
{
	
	var uid = document.getElementById('play_uid').value;
	var type = document.getElementById('play_type').value;
	
	var radios = document.getElementsByName('play_radio');
	var i = 0;
	for(; i < radios.length; i++)
	{
		if(radios[i].checked)
		{
			break;
		}
	}
	
	var loop = 0;
	switch(i)
	{
		case 0:
			loop = 1;
			break;
		case 1:
			loop = -1;
			break;
		case 2:
			loop = parseInt(document.getElementById("play_x").value,10);
			break;
	}
	var shuffle = document.getElementById('check_play_shuffle').checked;
	
	var play = { "command" : "play", "type" : type, "uid" : uid, "shuffle" : shuffle, "times_to_play" : loop};
	
	ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handlePatchDefault);
	
	UIkit.modal(document.getElementById('play_modal')).hide();
			
}

function deleteResource(e)
{
	e.stopPropagation();
	var split = e.target.id.split('_');
	var type = split[1];
	var uid = split[2];
			
		
	UIkit.modal.confirm("Delete '"+document.getElementById('span_'+uid).innerHTML+"'. Are you sure?").then(function() 
	{
		ajaxDelete(0, "/x-epi/"+type+"/"+uid, handleDelete);
	}, function () {
	});
}

function modifyResource(e)
{
	e.stopPropagation();
	var split = e.target.id.split('_');
	var type = split[1];
	var uid = split[2];
			
	if(type == "playlists")
	{
		modifyPlaylist(uid);
	}
	else if(type == "schedules")
	{
		modifySchedule(uid);
	}
	else if(type == "files")
	{
		if(split[0] == 'replace')
		{
			showReplaceModal(uid);
		}
		else
		{
			showRelabelModal(uid);
		}
	}
}

	
function playLoop(e)
{
	e.stopPropagation();
	UIkit.modal.prompt('Enter number of times to play', '1').then(function (times_to_play) 
	{
		if(times_to_play !== null && times_to_play != "")
		{
	
			var split = e.target.id.split('_');
			var type = split[1];
			var uid = split[2];
				
			type = type.substring(0, type.length-1);	//remove the s
				
			var play = { "command" : "play", "type" : type, "uid" : uid, "times_to_play" : parseInt(times_to_play,10), "shuffle" : false};
		
			ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handlePatchDefault);
		}
	})
			
}


function clearElement(elm)
{
	while(elm.firstChild)
	{
		elm.removeChild(elm.lastChild);
	}
}



function getLoopiConfig_System()
{
	if(g_loopi_array[0].connected == false)
	{
		getConfig(0, handleConfig_System);
	}
	setTimeout(getLoopiConfig_System, 2000);
	
}



function handleConfig_System(loopi, jsonObj)
{
	if(jsonObj !== null)
	{
		document.getElementById('loopi_'+loopi).style.backgroundColor = "#ffffff";
		updateConfig_Details(loopi, jsonObj);
		getStatus(loopi, handleStatus_System);
	}
}

function handleStatus_System(loopi, jsonObj)
{
	updateStatus_System(loopi, jsonObj);
	
	getUpdate(loopi, handleUpdate_System);
}

function handleUpdate_System(loopi, jsonObj)
{
	showVersion(loopi,jsonObj);
	getFiles(loopi, storeFiles_System);
}

function storeFiles_System(loopi, jsonObj)
{
	g_loopi_array[loopi].setFiles(jsonObj);
	ws_connect(loopi, updateStatus_System, updateInfo_System, null);
}

function showVersion(loopi, jsonObj)
{
	if(jsonObj != null)
	{
		document.getElementById('server-version').innerHTML = jsonObj.server.version;
		document.getElementById('server-date').innerHTML = jsonObj.server.date;
		
		document.getElementById('player3-version').innerHTML = jsonObj.player3.version;
		document.getElementById('player3-date').innerHTML = jsonObj.player3.date;
		
		if(jsonObj.player67 !== undefined)
		{
			document.getElementById('player67-version').innerHTML = jsonObj.player67.version;
			document.getElementById('player67-date').innerHTML = jsonObj.player67.date;
		}
	}
}

	
function updateStatus_System(loopi, jsonObj)
{
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setStatus(jsonObj);
		
		updateOverallStatus(loopi, jsonObj);
		updatePlayerStatus(loopi, jsonObj);
	}
}

function updateInfo_System(loopi, jsonObj)
{
	document.getElementById('application-start_time').innerHTML = jsonObj.application.start_time;
	document.getElementById('application-up_time').innerHTML = millisecondsToTime(jsonObj.application.up_time*1000);
	
	document.getElementById('system-uptime').innerHTML = millisecondsToTime(jsonObj.system.uptime*1000);
	document.getElementById('system-procs').innerHTML = jsonObj.system.procs;
	document.getElementById('temperature-cpu').innerHTML = jsonObj.temperature.cpu;
	
	document.getElementById('cpu-cpu').innerHTML = jsonObj.cpu.cpu;
	document.getElementById('cpu-cpu0').innerHTML = jsonObj.cpu.cpu0;
	document.getElementById('cpu-cpu1').innerHTML = jsonObj.cpu.cpu1;
	document.getElementById('cpu-cpu2').innerHTML = jsonObj.cpu.cpu2;
	document.getElementById('cpu-cpu3').innerHTML = jsonObj.cpu.cpu3;
	
	document.getElementById('disk-bytes-total').innerHTML = Math.round(jsonObj.disk.bytes.total/1073741824);
	document.getElementById('disk-bytes-free').innerHTML = Math.round(jsonObj.disk.bytes.free/1073741824);
	document.getElementById('disk-bytes-available').innerHTML = Math.round(jsonObj.disk.bytes.available/1073741824);
	
	document.getElementById('disk-inodes-total').innerHTML = jsonObj.disk.inodes.total;
	document.getElementById('disk-inodes-free').innerHTML = jsonObj.disk.inodes.free;
	document.getElementById('disk-inodes-available').innerHTML = jsonObj.disk.inodes.available;
	
	document.getElementById('system-loads-1').innerHTML = Math.round(jsonObj.system.loads["1"]*100)/100;
	document.getElementById('system-loads-5').innerHTML = Math.round(jsonObj.system.loads["5"]*100)/100;
	document.getElementById('system-loads-15').innerHTML = Math.round(jsonObj.system.loads["15"]*100)/100;
	
	document.getElementById('system-ram-total').innerHTML = Math.round(jsonObj.system.ram.total/1048576);
	document.getElementById('system-ram-buffered').innerHTML = Math.round(jsonObj.system.ram.buffered/1048576);
	document.getElementById('system-ram-shared').innerHTML = Math.round(jsonObj.system.ram.shared/1048576);
	document.getElementById('system-ram-free').innerHTML = Math.round(jsonObj.system.ram.free/1048576);	
	
	if(jsonObj.process !== undefined)
	{
		document.getElementById('process-rs').innerHTML = Math.round(jsonObj.process.rs/1024);
		document.getElementById('process-vm').innerHTML = Math.round(jsonObj.process.vm/1024);
	}
	
	
	if(jsonObj.ntp !== undefined)
	{
		if(jsonObj.ntp.error !== undefined)
		{
			document.getElementById("ntp-sync").innerHTML = jsonObj.ntp.error;
			document.getElementById("current_time").style.color = CLR_ERROR;
		}
		else
		{
			if(jsonObj.ntp.synchronised == true)
			{
				document.getElementById("current_time").style.color = CLR_SYNC;
			}
			else
			{
				document.getElementById("current_time").style.color = CLR_WARNING;
			}
			document.getElementById("ntp-sync").innerHTML = jsonObj.ntp.synchronised;
			document.getElementById("ntp-clock").innerHTML = jsonObj.ntp.source;
			document.getElementById("ntp-refid").innerHTML = jsonObj.ntp.refid;
			document.getElementById("ntp-stratum").innerHTML = jsonObj.ntp.stratum;
			document.getElementById("ntp-precision").innerHTML = Math.round(Math.pow(2, jsonObj.ntp.precision)*1000000)+"us";
			document.getElementById("ntp-offset").innerHTML = jsonObj.ntp.offset+"s";
			document.getElementById("ntp-jitter").innerHTML = jsonObj.ntp.sys_jitter+"s";
			document.getElementById("ntp-poll").innerHTML = Math.pow(2, jsonObj.ntp.tc)+"s";
		}
	}
	else
	{
		document.getElementById("current_time").style.color = CLR_UNKNOWN;
	}
}

function updateSystem_Details(loopi, jsonObj)
{
	if(jsonObj.ntp !== undefined)
	{
		if(jsonObj.ntp.error !== undefined)
		{
			document.getElementById("current_time").style.color = CLR_ERROR;
		}
		else
		{
			if(jsonObj.ntp.synchronised == true)
			{
				document.getElementById("current_time").style.color = CLR_SYNC;
			}
			else
			{
				document.getElementById("current_time").style.color = CLR_WARNING;
			}
		}
	}
	else
	{
		document.getElementById("current_time").style.color = CLR_UNKNOWN;
	}
}

function restart(command)
{
	UIkit.modal.confirm('Are you sure?').then(function() 
	{
		var play = { "command" : command};
		ajaxPut(0, "/x-epi/power", JSON.stringify(play), handleRestartPut);
	}, function () {
	});

	
}


function handleRestartPut(status, jsonObj)
{
	if(status != 200)
	{
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 3000})
	}

}


function showUpdate(application)
{
	document.getElementById('update_application_title').innerHTML = application;
	document.getElementById('update_application').value = application;
	UIkit.modal(document.getElementById('update_modal')).show();
}

function updateApp()
{
	if(document.getElementById('filename').value != document.getElementById('update_application').value)
	{
		UIkit.modal.confirm('File chosen has different name to application. Continue?').then(function() 
		{
			doUpdate();
		}, function () {
		});
	}
	else
	{
		doUpdate();
	}
}

function doUpdate()
{
	var fd = new FormData(document.getElementById('update_form'));
	
	var ajax = new XMLHttpRequest();

		
	ajax.onreadystatechange = function()
	{
		
		if(this.readyState == 4)
		{
			var jsonObj = JSON.parse(this.responseText);
			
			
			if(this.status != 200)
			{
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 4000});
			}
			else if(jsonObj["restart"] == true)
			{
				UIkit.notification({message: document.getElementById('update_application').value+" has been updated but needs restarting.", status: 'danger', timeout: 3000});
				
				getUpdate(0, showVersion);
			}
			else
			{
				UIkit.notification({message: document.getElementById('update_application').value+" has been updated", status: 'danger', timeout: 3000});
				getUpdate(0, showVersion);
			}
		}
	}

	UIkit.modal(document.getElementById('update_modal')).hide();
	
	
	ajax.open('PUT',"http://"+g_loopi_array[0].url+"/x-epi/update");
	ajax.send(fd);
}



var g_cron = {'s': {}, 'm': {}, 'h': {}, 'd': {}, 'M': {}, 'w': {}};

function cron(event)
{
	var sp = event.target.id.split('_');
	
	if(event.target.classList.contains('uk-button-default'))
	{
		event.target.classList.remove('uk-button-default');
		event.target.classList.add('uk-button-primary');
	
		g_cron[sp[0]][sp[1]] = true;
	}
	else
	{
		event.target.classList.add('uk-button-default');
		event.target.classList.remove('uk-button-primary');
		g_cron[sp[0]][sp[1]] = false;
	}
	
	var all = true;
	for(key in g_cron[sp[0]])
	{
		if(g_cron[sp[0]][key])
		{
			all = false;
			break;
		}
	}
	
	if(all)
	{
		document.getElementById(sp[0]+'_all').classList.remove('uk-button-default');
		document.getElementById(sp[0]+'_all').classList.add('uk-button-primary');
	}
	else
	{
		document.getElementById(sp[0]+'_all').classList.add('uk-button-default');
		document.getElementById(sp[0]+'_all').classList.remove('uk-button-primary');
	}
	
	document.getElementById('cron_text').innerHTML = createReadableCron(convertgCronToString());	
	var mod = document.getElementById('schedule_mod_cron');
	if(mod)
	{
		mod.innerHTML = document.getElementById('cron_text').innerHTML;
	}
		
}

function cronAll(event)
{
	var sp = event.target.id.split('_');
	for(key in g_cron[sp[0]])
	{
		if(g_cron[sp[0]][key] == true)
		{
			g_cron[sp[0]][key] = false;
			document.getElementById(sp[0]+'_'+key).classList.add('uk-button-default');
			document.getElementById(sp[0]+'_'+key).classList.remove('uk-button-primary');
		}
	}
	
	document.getElementById(sp[0]+'_all').classList.remove('uk-button-default');
	document.getElementById(sp[0]+'_all').classList.add('uk-button-primary');
	
	document.getElementById('cron_text').innerHTML = createReadableCron(convertgCronToString());	
	var mod = document.getElementById('schedule_mod_cron');
	if(mod)
	{
		mod.innerHTML = document.getElementById('cron_text').innerHTML;
	}
}

function convertgCronToString()
{
	var cron = "";
	
	for(cronSection in g_cron)
	{
		var str = "";
		
	
		for(time in g_cron[cronSection])
		{
			if(g_cron[cronSection][time])
			{
				if(str != "")
				{
					str += ",";
				}
				str += time;
			}
		}
		
    
		
		if(str == "")
		{
			str = "*";
		}
		
		if(cron != "")
		{
			cron += " ";
		}
		cron += str;
	}
	return cron;
}

function createReadableCron(cron)
{
	return cronstrue.toString(cron, { use24HourTimeFormat: true});
}


function createScheduleEntry()
{
	var sel =  document.getElementById('schedule_select_files');
	doCreateScheduleEntry(sel.options[sel.selectedIndex].text, sel.options[sel.selectedIndex].value, 
	document.getElementById('schedule_file_loop').value,document.getElementById('schedule_file_shuffle').checked,
	convertgCronToString(),
	document.getElementById('cron_text').innerHTML);
}

function doCreateScheduleEntry(label, uid, loop, shuffle,cron, cronLabel)
{
	var row = document.getElementById('schedule_entry');
	
	var index = document.getElementById('schedule_count').value;
	document.getElementById('schedule_count').value = index+1;
	
	var tr  = document.createElement('tr');
	tr.id = 'scheduleEntry_'+index;
	
	var input_file = document.createElement('input');
	input_file.type = "hidden";
	input_file.id = "scheduleEntryUid_"+index;
	input_file.value = uid;
	
	var input_cron = document.createElement('input');
	input_cron.type = 'hidden';
	input_cron.id="scheduleEntryCron_"+index;
	input_cron.value = cron;
	
	var input_type = document.createElement('input');
	input_type.type = 'hidden';
	input_type.id="scheduleEntryType_"+index;
	
	var input_loop = document.createElement('input');
	input_loop.type = 'hidden';
	input_loop.id="scheduleEntryLoop_"+index;
	input_loop.value = loop;
	
	tr.appendChild(input_file);
	tr.append(input_cron);
	tr.append(input_type);
	tr.append(input_loop);
	
	var td_file =  document.createElement('td');
	td_file.style.padding = '3px 6px';
	td_file.className = "uk-background-muted";
	td_file.id = 'scheduleEntryLabel_'+index;
	td_file.innerHTML = label;
		
	
	var td_loop =  td_file.cloneNode();
	td_loop.id = "scheduleEntryLoopLabel_"+index;
	
	var td_shuffle =  td_file.cloneNode();
	td_shuffle.id = "scheduleEntryShuffle_"+index;
	
	var td_cron =  td_file.cloneNode();
	td_cron.id = "scheduleEntryCronReadable_"+index;
	
	
	var td_action =  td_file.cloneNode();
	
	
		
	if(loop >= 0)
	{
		td_loop.innerHTML = loop;
	}
	else
	{
		td_loop.innerHTML = "&infin;";
	}
	
	
	
	
	if(isPlaylist(label))
	{
		input_type.value = "playlist";
	}
	
	td_shuffle.innerHTML = shuffle;
	td_cron.innerHTML = cronLabel;
	
	
	
	var buttonMod = document.createElement('button');
	buttonMod.className = "uk-button";
	buttonMod.classList.add("uk-button-small");
	buttonMod.style.background = '#ffffff';
	buttonMod.id = "scheduleEntryModify_"+index;
	buttonMod.innerHTML = 'Modify';
	buttonMod.addEventListener('click',modifyScheduleEntry, false);
	td_action.appendChild(buttonMod);
	
	
	var button = document.createElement('button');
	button.className = "uk-button";
	button.classList.add("uk-button-small");
	button.style.background = '#ffffff';
	button.style.marginLeft = '5px';
	button.id = "scheduleEntryRemove_"+index;
	button.innerHTML = 'Remove';
	button.addEventListener('click',removeScheduleEntry, false);
	td_action.appendChild(button);
	
	
	
	tr.appendChild(td_file);
	tr.appendChild(td_loop);
	tr.appendChild(td_shuffle);
	tr.appendChild(td_cron);
	tr.appendChild(td_action);
	
	row.parentNode.insertBefore(tr, row);
}

function removeScheduleEntry(e)
{
	if(document.getElementById('schedule_edit').value != -1)
	{
		cancelScheduleEntry();
	}
	else
	{
		var target = e.target.id.split('_')[1];
		var row = document.getElementById('scheduleEntry_'+target);
		row.parentNode.removeChild(row);
	}
}


function scheduleSelectChange()
{
	var sel =  document.getElementById('schedule_select_files');
	

	if(isPlaylist(sel.options[sel.selectedIndex].innerHTML))
	{
		document.getElementById('schedule_file_shuffle').style.visibility = 'visible';
	}
	else
	{
		document.getElementById('schedule_file_shuffle').style.visibility = 'hidden';
		document.getElementById('schedule_file_shuffle').checked = false;
	}
}

function isPlaylist(label)
{
	var playlist = document.createElement('div');
	playlist.innerHTML = "&equiv;";
	return (label.startsWith(playlist.innerHTML));
}

function closeSchedule()
{
	document.getElementById('schedule_label').value = '';
	document.getElementById('schedule_description').value = '';
	
	
	var tbody = document.getElementById('schedule_entries');
	while(tbody.childElementCount > 1)
	{
		tbody.removeChild(tbody.firstChild);
	}
	
	clearCron();
	
	
	UIkit.modal(document.getElementById('schedule_modal')).hide();
}

function clearCron()
{
	//clear
	for(cronSection in g_cron)
	{
		for(time in g_cron[cronSection])
		{
			if(g_cron[cronSection][time])
			{
				document.getElementById(cronSection+'_'+time).classList.add('loopi_cron_deselected');
				document.getElementById(cronSection+'_'+time).classList.remove('uk-button-primary');
			}
		}
		
		document.getElementById(cronSection+'_all').classList.remove('loopi_cron_deselected');
		document.getElementById(cronSection+'_all').classList.add('uk-button-primary');
    }
	
	g_cron = {'s': {'00' : true}, 'm': {}, 'h': {}, 'd': {}, 'M': {}, 'w': {}};
	
	setCron();
	
}

function setCron()
{
	for(cronSection in g_cron)
	{
		for(time in g_cron[cronSection])
		{
			if(g_cron[cronSection][time])
			{
				if(document.getElementById(cronSection+'_'+time))
				{
					document.getElementById(cronSection+'_'+time).classList.remove('loopi_cron_deselected');
					document.getElementById(cronSection+'_'+time).classList.add('uk-button-primary');
				}
				else
				{
					console.log(cronSection+'_'+time);
				}
				
				document.getElementById(cronSection+'_all').classList.add('loopi_cron_deselected');
				document.getElementById(cronSection+'_all').classList.remove('uk-button-primary');
			}
		}
    }
    document.getElementById('cron_text').innerHTML = createReadableCron(convertgCronToString())
}

function convertToCron(cronStr)
{
	var split = cronStr.split(' ');
	
	convertSingleCron(split[0],'s');
	convertSingleCron(split[1],'m');
	convertSingleCron(split[2],'h');
	convertSingleCron(split[3],'d');
	convertSingleCron(split[4],'M');
	convertSingleCron(split[5],'w');
	
}

function convertSingleCron(cronStr, key)
{
	if(cronStr == '*' || cronStr == '?')
	{
		g_cron[key] = {};
	}
	else
	{
		var split = cronStr.split(',');
		
		for(var i = 0; i < split.length; i++)
		{
			var range = split[i].split('-');
			if(range.length == 1)
			{
				if(range[0].length < 2)
				{
					range[0] = '0'+range[0];
				}
				g_cron[key][range[0]] = true;
			}
			else
			{
				for(var i = range[0]; i <= range[1]; i++)
				{
					var value = i;
					if(value < 10)
					{
						value = '0'+value;
					}
				
					g_cron[key][value] = true;
				
				}
			}
		}
	}		
}


function createSchedule()
{
	if(document.getElementById('schedule_label').value == '')
	{
		UIkit.notification({message: "You must enter a label for the schedule", status: 'danger', timeout: 2000})
		return;
	}
	if(document.getElementById('schedule_description').value == '')
	{
		UIkit.notification({message: "You must enter a description for the schedule", status: 'danger', timeout: 2000})
		return;
	}
		
	var tbody = document.getElementById('schedule_entries');
	if(tbody.childElementCount == 1)		
	{
		UIkit.notification({message: "You must enter some resources for the schedule", status: 'danger', timeout: 2000})
		return;
	}
				
	var jsonPost = {"label" : document.getElementById('schedule_label').value, "description" : document.getElementById('schedule_description').value, "files" : [], "playlists" : []};
	
	var row = tbody.childNodes;
	for(var i = 0; i < tbody.childElementCount; i++)
	{
		if(row[i].id !== undefined)
		{
			var sp = row[i].id.split('_');
			if(sp[0] == 'scheduleEntry')
			{
				var loop = parseInt(document.getElementById('scheduleEntryLoop_'+sp[1]).value,10);
				var aCron = document.getElementById('scheduleEntryCron_'+sp[1]).value+' *';		//add the year in
				var uid = document.getElementById('scheduleEntryUid_'+sp[1]).value;
				
				if(document.getElementById('scheduleEntryType_'+sp[1]).value == "playlist")
				{
					var shuffle = (document.getElementById('scheduleEntryShuffle_'+sp[1]).value == "true");
					
					
					
					var jsonRow = {"uid" : uid, "times_to_play" : loop, "cron" : aCron, "shuffle" : shuffle};
					jsonPost.playlists.push(jsonRow);
				}
				else
				{
					var jsonRow = {"uid" : uid, "times_to_play" : loop, "cron" : aCron};
					jsonPost.files.push(jsonRow);
				}
			}
		}
	}
		
	
	
	
				
	var ajax = new XMLHttpRequest();
	ajax.onreadystatechange = function()
	{
		if(this.readyState == 4)
		{
			var jsonObj = JSON.parse(this.responseText);
			if(this.status != 201 && this.status != 200)
			{
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
			}
			else
			{
				closeSchedule();
			}
		}
	}		

	var uid = document.getElementById('schedule_uid').value;
	if(uid == '')
	{
		ajax.open('POST',"http://"+g_loopi_array[0].url+"/x-epi/schedules");
	}
	else
	{
		ajax.open('PUT',"http://"+g_loopi_array[0].url+"/x-epi/schedules/"+uid);
	}
	ajax.setRequestHeader("Content-type", "application/json");
	ajax.send(JSON.stringify(jsonPost));
}


function showPlaylistModal()
{
	document.getElementById('create_playlist_title').innerHTML = "Create New Playlist";
	document.getElementById('playlist_label').value = '';
	document.getElementById('playlist_description').value = '';
	document.getElementById('playlist_uid').value = '';
	document.getElementById('create_playlist_button').innerHTML = 'Create';

	var tbody = document.getElementById('playlist_entries');
	while(tbody.childElementCount > 1)
	{
		tbody.removeChild(tbody.firstChild);
	}
	
	UIkit.modal(document.getElementById('playlist_modal')).show();
}

function modifyPlaylist(uid)
{
	var index = g_loopi_array[0].findPlaylist(uid);
	if(index != -1)
	{
		document.getElementById('playlist_uid').value = uid;
		document.getElementById('create_playlist_title').innerHTML = "Modify Playlist";
		document.getElementById('playlist_label').value = g_loopi_array[0].playlists[index].details.label;
		document.getElementById('playlist_description').value = g_loopi_array[0].playlists[index].details.description;
		
		document.getElementById('create_playlist_button').innerHTML = 'Save';
		
		for(var i = 0; i < g_loopi_array[0].playlists[index].details.files.length; i++)
		{
			var file = g_loopi_array[0].findFile(g_loopi_array[0].playlists[index].details.files[i].uid);
			if(file != -1)
			{
				doCreatePlaylistEntry(g_loopi_array[0].files[file].label, g_loopi_array[0].playlists[index].details.files[i].uid, g_loopi_array[0].playlists[index].details.files[i].times_to_play);
			}
		}
		
		UIkit.modal(document.getElementById('playlist_modal')).show();
	}
	else
	{
		console.log("playlist not found");
	}
}


function modifyScheduleEntry(e)
{
		
	var index = e.target.id.split('_')[1];
	
	if(index == document.getElementById('schedule_edit').value)
	{
		updateScheduleEntry();
	}
	else
	{
		cancelScheduleEntry();
		
		document.getElementById('schedule_entry').style.display = 'none';
				
		
		document.getElementById('schedule_edit').value = index;
		
		var row = document.getElementById('scheduleEntry_'+index);
		row.style.display = 'none';
	
		//
		var tr  = document.createElement('tr');
		tr.id = 'mod_row';
	
		var td_file =  document.createElement('td');
		var td_loop =  document.createElement('td');
		var td_shuffle =  document.createElement('td');
		var td_cron =  document.createElement('td');
		var td_action =  document.createElement('td');
	
		td_file.style.padding = '3px 6px';
		td_loop.style.padding = '3px 6px';
		td_shuffle.style.padding = '3px 6px';
		td_cron.style.padding = '3px 6px';
		td_action.style.padding = '3px 6px';
		
		td_file.className = "loopi_edit";
		td_loop.className = "loopi_edit";
		td_shuffle.className = "loopi_edit";
		td_cron.className = "loopi_edit";
		td_action.className = "loopi_edit";
	
		var sel =  document.getElementById('select_files');
		var selMod = sel.cloneNode(true);
		selMod.value = document.getElementById('scheduleEntryUid_'+index).value;
		selMod.id = 'schedule_mod_sel';
		td_file.appendChild(selMod);
	
		var loopMod = document.getElementById('schedule_file_loop').cloneNode();
		loopMod.id = 'schedule_mod_loop';
		loopMod.value = document.getElementById('scheduleEntryLoop_'+index).value;
		td_loop.appendChild(loopMod);
	
		
		var shuffleMod = document.getElementById('schedule_file_shuffle').cloneNode();
		shuffleMod.id = 'schedule_mod_shuffle';

	
		if(isPlaylist(document.getElementById('scheduleEntryLabel_'+index).innerHTML))
		{
			shuffleMod.style.visibility = 'visible';
			shuffleInput.checked = (document.getElementById('scheduleEntryShuffle_'+index).innerHTML == 'true');
		}
		else
		{
			shuffleMod.style.visibility = 'hidden';
		}
		td_shuffle.appendChild(shuffleMod);
				
		td_cron.id = 'schedule_mod_cron';
		td_cron.innerHTML = document.getElementById('scheduleEntryCronReadable_'+index).innerHTML;
		
	
	
		var buttonMod = document.createElement('button');
		buttonMod.className = "uk-button";
		buttonMod.classList.add("uk-button-small");
		buttonMod.style.background = '#ffffff';
		buttonMod.innerHTML = 'Update';
		buttonMod.addEventListener('click',updateScheduleEntry, false);
		td_action.appendChild(buttonMod);
	
		var button = document.createElement('button');
		button.className = "uk-button";
		button.classList.add("uk-button-small");
		button.style.background = '#ffffff';
		button.style.marginLeft = '5px';
		button.innerHTML = 'Cancel';
		button.addEventListener('click',cancelScheduleEntry, false);
		td_action.appendChild(button);
	
		tr.appendChild(td_file);	
		tr.appendChild(td_loop);	
		tr.appendChild(td_shuffle);	
		tr.appendChild(td_cron);	
		tr.appendChild(td_action);
		
		row.parentNode.insertBefore(tr, row);	
	
		convertToCron(document.getElementById('scheduleEntryCron_'+index).value);
		setCron();
		
		document.getElementById('add_schedule_entry').style.visibility = 'hidden';
		
		document.getElementById('schedule_cron_div').classList.add('loopi_edit');
	}
}


function cancelScheduleEntry()
{
	var index = document.getElementById('schedule_edit').value;
	if(index != -1)
	{
		document.getElementById('scheduleEntry_'+index).style.display = '';
		document.getElementById('schedule_entry').style.display = '';
		
		
		document.getElementById('scheduleEntryModify_'+index).innerHTML = "Modify";
		document.getElementById('scheduleEntryRemove_'+index).innerHTML = "Remove";
		
		var row = document.getElementById('scheduleEntry_'+index);
		for(var i = 0; i < row.cells.length; i++)
		{
			row.cells[i].className = "uk-background-muted";
		}
		
		document.getElementById('schedule_edit').value = -1;
		
		document.getElementById('add_schedule_entry').style.visibility = 'visible';
		
		var mod =document.getElementById('mod_row');
		if(mod !== null)
		{
			mod.parentNode.removeChild(document.getElementById('mod_row'));
		}
		
		document.getElementById('schedule_cron_div').classList.remove('loopi_edit');
	}
}


function updateScheduleEntry()
{
	var index = document.getElementById('schedule_edit').value;

	
	var sel =  document.getElementById('schedule_mod_sel');
	var label = sel.options[sel.selectedIndex].text;
	var uid = sel.options[sel.selectedIndex].value;
	var loop =  document.getElementById('schedule_mod_loop').value;
	var shuffle = document.getElementById('schedule_mod_shuffle').checked;
	var cron_text = document.getElementById('schedule_mod_cron').innerHTML;
	
	
	var input_file = document.getElementById('scheduleEntryUid_'+index);
	var input_cron = document.getElementById("scheduleEntryCron_"+index);
	var input_loop = document.getElementById("scheduleEntryLoop_"+index);
	var input_type = document.getElementById("scheduleEntryType_"+index);
	var td_file =  document.getElementById('scheduleEntryLabel_'+index);
	var td_shuffle = document.getElementById("scheduleEntryShuffle_"+index);
	var td_cron =  document.getElementById("scheduleEntryCronReadable_"+index);
	var td_loop =  document.getElementById("scheduleEntryLoopLabel_"+index);
	
	input_file.value = uid;
	
	input_cron.value = convertgCronToString();
	input_loop.value = loop;
	
	
	td_file.innerHTML = label;
	if(isPlaylist(label))
	{
		input_type.value = "playlist";
	}	
	
		
	if(loop >= 0)
	{
		td_loop.innerHTML = loop;
	}
	else
	{
		td_loop.innerHTML = "&infin;";
	}
	
	td_shuffle.innerHTML = shuffle;
	td_cron.innerHTML = cron_text;
	
	
	cancelScheduleEntry();
}


function showRelabelModal(uid)
{
	var index = g_loopi_array[0].findFile(uid);
	if(index != -1)
	{
		document.getElementById('relabel_label').value = g_loopi_array[0].files[index].label;
		document.getElementById('relabel_description').value = g_loopi_array[0].files[index].details.description;
		document.getElementById('relabel_uid').value = uid;
		UIkit.modal(document.getElementById('relabel_modal')).show();	
		document.getElementById('relabel_label').focus();
	}
}

function relabelFile()
{
	var patch = { "label" : document.getElementById('relabel_label').value, "description" : document.getElementById('relabel_description').value};
	ajaxPatch(0, "/x-epi/files/"+document.getElementById('relabel_uid').value, JSON.stringify(patch), handleRelabelPatch);
}

function handleRelabelPatch(status, jsonObj)
{
	if(status != 200)
	{
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 4000})
	}
	else
	{
		UIkit.modal(document.getElementById('relabel_modal')).hide();	
	}
}

function showReplaceModal(uid)
{
	var index = g_loopi_array[0].findFile(uid);
	if(index != -1)
	{
		document.getElementById('upload_title').innerHTML = 'Replace Audio';
		document.getElementById('upload_label').value = g_loopi_array[0].files[index].label;
		document.getElementById('upload_description').value = g_loopi_array[0].files[index].details.description;
		document.getElementById('upload_uid').value = uid;
		UIkit.modal(document.getElementById('upload_modal')).show();	
	}
}

function showUploadModal()
{
	document.getElementById('upload_title').innerHTML = 'Upload New Audio';
	document.getElementById('upload_label').value = "";
	document.getElementById('upload_description').value = "";
	document.getElementById('upload_uid').value = "";
	document.getElementById('upload_file').value = "";
	UIkit.modal(document.getElementById('upload_modal')).show();	
}


function modifySchedule(uid)
{
	var index = g_loopi_array[0].findSchedule(uid);
	if(index != -1)
	{
		document.getElementById('schedule_uid').value = uid;
		document.getElementById('create_schedule_title').innerHTML = "Modify Schedule";
		document.getElementById('schedule_label').value = g_loopi_array[0].schedules[index].details.label;
		document.getElementById('schedule_description').value = g_loopi_array[0].schedules[index].details.description;
		
		document.getElementById('create_schedule_button').innerHTML = 'Save';
		
		for(var i = 0; i < g_loopi_array[0].schedules[index].details.files.length; i++)
		{
			var file = g_loopi_array[0].findFile(g_loopi_array[0].schedules[index].details.files[i].uid);
			if(file != -1)
			{
				//label, uid, loop, shuffle, cron
				doCreateScheduleEntry(g_loopi_array[0].files[file].label, 
								      g_loopi_array[0].schedules[index].details.files[i].uid, 
								      g_loopi_array[0].schedules[index].details.files[i].times_to_play,
								      false, 
								      g_loopi_array[0].schedules[index].details.files[i].cron,
								      createReadableCron(g_loopi_array[0].schedules[index].details.files[i].cron));
			}
		}
		
		for(var i = 0; i < g_loopi_array[0].schedules[index].details.playlists.length; i++)
		{
			var playlist = g_loopi_array[0].findPlaylist(g_loopi_array[0].schedules[index].details.playlists[i].uid);
			if(playlist != -1)
			{
				//label, uid, loop, shuffle, cron
				doCreateScheduleEntry(g_loopi_array[0].playlists[playlist].label, 
								      g_loopi_array[0].schedules[index].details.playlists[i].uid, 
								      g_loopi_array[0].schedules[index].details.playlists[i].times_to_play,
								      g_loopi_array[0].schedules[index].details.playlists[i].shuffle, 
								      g_loopi_array[0].schedules[index].details.playlists[i].cron,
								      createReadableCron(g_loopi_array[0].schedules[index].details.playlists[i].cron));
			}
		}
		
		UIkit.modal(document.getElementById('schedule_modal')).show();
	}
	else
	{
		console.log("schedule not found");
	}
}

function showScheduleModal()
{
	document.getElementById('create_schedule_title').innerHTML = "Create New Schedule";
	document.getElementById('schedule_label').value = '';
	document.getElementById('schedule_description').value = '';
	document.getElementById('schedule_uid').value = '';
	document.getElementById('create_schedule_button').innerHTML = 'Create';

	var tbody = document.getElementById('schedule_entries');
	while(tbody.childElementCount > 1)
	{
		tbody.removeChild(tbody.firstChild);
	}
	clearCron();
	
	UIkit.modal(document.getElementById('schedule_modal')).show();
}
