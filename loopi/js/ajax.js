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
	removeSchedue(uid)
	{
		var i = this.findSchedule(uid);
		if(i > -1)
		{
			this.schedules.splice(i,1);
		}		
	}
}


var g_loopi_array = new Array();

const CLR_PLAYING = "#92d14f";
const CLR_IDLE = "#8db4e2";
const CLR_ERROR =  "#ff7777";
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
			console.log(jsonObj);
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
	
	listResource_Details(jsonObj, "files", getFileDetails);
	
	getPlaylists(loopi, storePlaylists_Details);
}


function storePlaylists_Details(loopi, jsonObj)
{
	g_loopi_array[loopi].setPlaylists(jsonObj);
	
	listResource_Details(jsonObj, "playlists", getPlaylistDetails);
	
	getSchedules(loopi, storeSchedules_Details);
}

function storeSchedules_Details(loopi, jsonObj)
{
	g_loopi_array[loopi].setSchedules(jsonObj);
	
	listResource_Details(jsonObj, "schedules", getScheduleDetails);
	
	ws_connect(loopi, updateStatus_Details, null, updateResources_Details);
	
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
	
	var select =  null;
	if(type == "files")
	{
		select = document.getElementById('select_files');
		clearElement(select);
	}
	
	for(var i = 0; i < resObj.length; i++)
	{
		createResourceLi(ul, resObj[i], type, clickFunction);
			
		if(select !== null)
		{
			var opt = document.createElement('option');
			opt.value = resObj[i]["uid"];
			opt.innerHTML = resObj[i]["label"];
			select.appendChild(opt);
		}
			
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
		
	if(type != 'schedules')
	{
		var buttonMulti = document.createElement('button');
		buttonMulti.className = "uk-button";
		buttonMulti.classList.add("uk-button-primary");
		buttonMulti.classList.add("uk-button-small");
		buttonMulti.style.marginLeft = '5px';
		buttonMulti.id = "play_"+type+"_"+resObj["uid"];
		buttonMulti.innerHTML = 'Play Loop';
		buttonMulti.addEventListener('click',playLoop, false);
		div_action.appendChild(buttonMulti);
	}
		
		
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
			if(file > -1)
			{
				tbody.appendChild(createDetailsRow(g_loopi_array[loopi].files[file]['label'], jsonObj['files'][i]['times_to_play']));
			}
			else
			{
				tbody.appendChild(createDetailsRow(jsonObj['files'][i]['uid'], jsonObj['files'][i]['times_to_play']));
			}
		}
	}
}

function createScheduleDetails(loopi, jsonObj)
{
	createResourceDetails(jsonObj);
	
	var thead = document.getElementById('specifichead_'+jsonObj["uid"]);
	clearElement(thead);
	
	var thType = document.createElement('th');
	thType.innerHTML = 'Type';
	
	var thFile = document.createElement('th');
	thFile.innerHTML = 'Label';
	
	var thTime = document.createElement('th');
	thTime.innerHTML = 'Times To Play';
	
	var thS = document.createElement('th');
	thS.innerHTML = 'Sec';
	var thM = document.createElement('th');
	thM.innerHTML = 'Min';
	var thH = document.createElement('th');
	thH.innerHTML = 'Hr';
	var thD = document.createElement('th');
	thD.innerHTML = 'DoM';
	var thMo = document.createElement('th');
	thMo.innerHTML = 'Mon';
	var thDoW = document.createElement('th');
	thDoW.innerHTML = 'DoW';
	var thY = document.createElement('th');
	thY.innerHTML = 'Year';
	
	thead.appendChild(thType);
	thead.appendChild(thFile);
	thead.appendChild(thTime);
	thead.appendChild(thS);
	thead.appendChild(thM);
	thead.appendChild(thH);
	thead.appendChild(thD);
	thead.appendChild(thMo);
	thead.appendChild(thDoW);
	thead.appendChild(thY);
	
	
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
		
		if(type == 'file')
		{
			file = g_loopi_array[0].findFile(jsonObj[res][i]['uid']);
			if(file > -1)
			{
				label = g_loopi_array[0].files[file]['label'];
			}
		}
		else
		{
			file = g_loopi_array[0].findPlaylist(jsonObj[res][i]['uid']);
			if(file > -1)
			{
				label = g_loopi_array[0].playlists[file]['label'];
			}
		}
		
		
		tbody.appendChild(createScheduleDetailsRow(type, label, jsonObj[res][i]['times_to_play'], jsonObj[res][i]['cron']));
	}
}

function createScheduleDetailsRow(type, label, loop, cron)
{

	var tr = document.createElement('tr');
	
	var td_type= document.createElement('td');
	td_type.style.padding = '3px 6px';
	td_type.innerHTML = type;
	
		
	var td_label = document.createElement('td');
	td_label.innerHTML = label;
	td_label.style.padding = '3px 6px';
	
	var td_loop = document.createElement('td');
	td_loop.innerHTML = loop;
	td_loop.style.padding = '3px 6px';
	
	tr.appendChild(td_type);
	tr.appendChild(td_label);
	tr.appendChild(td_loop);
	
	var split = cron.split(' ');
	for(var i = 0; i < split.length; i++)
	{
		if(split[i] != '')
		{
			var td_cron = document.createElement('td');
			td_cron.innerHTML = split[i];
			td_cron.style.padding = '3px 6px';
			tr.appendChild(td_cron);
		}
	}
	
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


function updateOverallStatus(loopi, jsonObj)
{
	if(jsonObj["locked"] !== undefined)
	{
		if(jsonObj["locked"] == false)
		{
			document.getElementById("lock_0").innerHTML = "Unlocked";
			document.getElementById("lock_0").classList.remove("uk-label-danger");
			document.getElementById("lock_0").classList.remove("uk-label-warning");
			document.getElementById("lock_0").classList.add("uk-label-success");
			document.getElementById("button_lock").innerHTML = "Lock";
			
		}
		else if(jsonObj["locked"] == true)
		{
			document.getElementById("lock_0").innerHTML = "Locked";
			document.getElementById("lock_0").classList.remove("uk-label-success");
			document.getElementById("lock_0").classList.remove("uk-label-danger");
			document.getElementById("lock_0").classList.add("uk-label-warning");
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
		document.getElementById("player").classList.remove("uk-label-success");
		document.getElementById("player").classList.remove("uk-label-warning");
		document.getElementById("player").classList.add("uk-label-danger");
		document.getElementById("section_player").style.display = "none";
		document.getElementById("section_resources").style.display = "block";
		
		document.getElementById("button_stop").style.visibility = "hidden";
		
		
	}
	else if(jsonObj["player"] === "Stopped")
	{
		document.getElementById("player").innerHTML = "Stopped";
		document.getElementById("player").classList.remove("uk-label-success");
		document.getElementById("player").classList.remove("uk-label-warning");
		document.getElementById("player").classList.remove("uk-label-danger");
		
		document.getElementById("section_player").style.display = "none";
		document.getElementById("section_resources").style.display = "block";
		
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
		document.getElementById("section_resources").style.display = "none";
		
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
	console.log("updateResources_Details");
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
	createFileDetails(loop, jsonObj);
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


function createPlaylistEntry()
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
	
	
	var sel =  document.getElementById('select_files');
	
	tr.id = "playlistEntry_"+sel.options[sel.selectedIndex].value;
	td_loop.id = "playlistEntryLoop_"+sel.options[sel.selectedIndex].value;
				
	td_file.innerHTML = sel.options[sel.selectedIndex].text;
	td_loop.innerHTML = document.getElementById('playlist_file_loop').value;
	
	var button = document.createElement('button');
	button.className = "uk-button";
	button.classList.add("uk-button-small");
	button.style.background = '#ffffff';
	button.id = "playlistRemove_"+sel.options[sel.selectedIndex].value;
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
	console.log(target);
	var row = document.getElementById('playlistEntry_'+target);
	row.parentNode.removeChild(row);
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

function handleStatusPatch(status, jsonObj)
{
	if(status != 200)
	{
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
	}
	else
	{
		getStatus(0, handleStatus_Details);
	}
}


function fileChosen()
{
	var label = document.getElementById('upload_label');
	label.value = document.getElementById('upload_file').files[0].name;
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
	
	var ajax = new XMLHttpRequest();

		
	ajax.upload.addEventListener('progress', function(e) {
		var percent_complete = (e.loaded/e.total)*100;
		document.getElementById('progress').value = percent_complete;
		});


	ajax.onreadystatechange = function()
	{
		
		if(this.readyState == 4)
		{
			var jsonObj = JSON.parse(this.responseText);
			
			UIkit.modal(document.getElementById('progress_modal')).hide();
			document.getElementById('uploading_label').innerHTML = '';
			document.getElementById('progress').value = 0;
			
			if(this.status != 201)
			{
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
			}
		}
	}

	UIkit.modal(document.getElementById('upload_modal')).hide();
	UIkit.modal(document.getElementById('progress_modal')).show();
	document.getElementById('uploading_label').innerHTML = document.getElementById('upload_label').value;
	
	
	ajax.open('POST',"http://"+g_loopi_array[0].url+"/x-epi/files");
	ajax.send(fd);
	
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
		if(row[i].id !== undefined && row[i].id.split('_')[0] == 'playlistEntry')
		{
			var jsonRow = {"uid" : row[i].id.split('_')[1], "times_to_play" : document.getElementById('playlistEntryLoop_'+row[i].id.split('_')[1]).innerHTML};
			jsonPost.files.push(jsonRow);
		}
	}
		
	
				
	var ajax = new XMLHttpRequest();
	ajax.onreadystatechange = function()
	{
		if(this.readyState == 4)
		{
			var jsonObj = JSON.parse(this.responseText);
			if(this.status != 201)
			{
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
			}
		}
	}		
	
	closePlaylist();

	ajax.open('POST',"http://"+g_loopi_array[0].url+"/x-epi/playlists");
	ajax.setRequestHeader("Content-type", "application/json");
	ajax.send(JSON.stringify(jsonPost));
}

function closePlaylist()
{
	document.getElementById('playlist_label').value = '';
	document.getElementById('playlist_description').value = '';
	
	clearElement(document.getElementById('playlist_entries'));
	UIkit.modal(document.getElementById('playlist_modal')).hide();
}

function play(e)
{
	e.stopPropagation();
	
	var split = e.target.id.split('_');
	var type = split[1];
	var uid = split[2];
				
	type = type.substring(0, type.length-1);	//remove the s
				
	var play = { "command" : "play", "type" : type, "uid" : uid, "times_to_play" : 1, "shuffle" : false};
	
	ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handleStatusPatch);
				
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
		
			ajaxPatch(0, "/x-epi/status", JSON.stringify(play), handleStatusPatch);
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


function setLinks()
{
	document.getElementById("a_system").href = "../system/?loopi="+endpoint;
	document.getElementById("a_playlists").href = "../playlists/?loopi="+endpoint;
	document.getElementById("a_schedules").href = "../schedules/?loopi="+endpoint;
	document.getElementById("a_files").href = "../files/?loopi="+endpoint;
	document.getElementById("a_control").href = "../status/?loopi="+endpoint;
	document.getElementById("a_status").href = "../status/?loopi="+endpoint;
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
	ws_connect(loopi, updateStatus_System, updateInfo_System, null);
}

	
function updateStatus_System(loopi, jsonObj)
{
	if(jsonObj != null)
	{
		g_loopi_array[loopi].setStatus(jsonObj);
		
		updateOverallStatus(loopi, jsonObj);
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
		UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000})
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
			
			UIkit.modal(document.getElementById('progress_modal')).hide();
			document.getElementById('uploading_label').innerHTML = '';
			document.getElementById('progress').value = 0;
			
			if(this.status != 200)
			{
				UIkit.notification({message: jsonObj["reason"], status: 'danger', timeout: 2000});
			}
			else if(jsonObj["restart"] == true)
			{
				UIkit.notification({message: document.getElementById('update_application').value+" has been updated but needs restarting.", status: 'danger', timeout: 2000});
			}
			else
			{
				UIkit.notification({message: document.getElementById('update_application').value+" has been updated", status: 'danger', timeout: 2000});
			}
		}
	}

	UIkit.modal(document.getElementById('update_modal')).hide();
	
	
	ajax.open('PUT',"http://"+g_loopi_array[0].url+"/x-epi/update");
	ajax.send(fd);
}

