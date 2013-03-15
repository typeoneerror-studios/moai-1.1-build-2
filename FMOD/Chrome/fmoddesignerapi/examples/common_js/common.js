var fmodSampleModule = null; 
var pageStatusText = '';
var moduleStatusText = '';
var storageAcquired = false;
var pageLoaded = false;

function sendMessageToModule(msg)
{
	fmodSampleModule.postMessage(msg);
} 

function attemptLoadModule(isLoadCallback) 
{
	pageLoaded |= isLoadCallback;
	if (storageAcquired && pageLoaded) 
	{
		var holder = document.createElement('div');
		holder.setAttribute('id', 'moduleHolder');		
		holder.addEventListener('load', moduleLoaded, true);
		holder.addEventListener('progress', moduleLoading, true);
		
		var embed = document.createElement('embed');
		embed.setAttribute('id', 'sampleModule');
		embed.setAttribute('width', 0);
		embed.setAttribute('height', 0);
		embed.setAttribute('src', moduleUrl);
		embed.setAttribute('type', 'application/x-nacl');
		holder.appendChild(embed);
		
		document.body.appendChild(holder);
	}
}

var kBytesNeeded = 100*1024*1024;
var storageInfo = window.webkitStorageInfo;
storageInfo.queryUsageAndQuota(storageInfo.PERSISTENT,
	{ 
		handleEvent: function(currentUsageInBytes, currentQuotaInBytes) 
		{
			if (currentQuotaInBytes < kBytesNeeded) 
			{
				storageInfo.requestQuota(storageInfo.PERSISTENT, kBytesNeeded,
					{ 
						handleEvent: function(granted_bytes) 
						{
							if (granted_bytes >= kBytesNeeded) 
							{
								storageAcquired = true;
								attemptLoadModule(false);
							}
							else 
							{
								displayError("Can't store game data."); 
							}
						}
					});
			} 
			else 
			{
				console.log("Already have quota of " + currentQuotaInBytes);
				storageAcquired = true;
				attemptLoadModule(false);
			}
		}
	});

function handleMessage(message_event)
{	
	if (message_event.data.indexOf('status') == 0)
	{
		var elements = message_event.data.split(":");
		updateModuleStatus(elements[1]);
	}
	else
	{
		if (extraCallback)
		{
			extraCallback(message_event);
		}
	}
}

function moduleLoaded() 
{
	fmodSampleModule = document.getElementById('sampleModule');
	fmodSampleModule.addEventListener('message', handleMessage, false);
	updatePageStatus('Native client module loaded');
}

var progress = "";
function moduleLoading() 
{	
	progress += "."
	updatePageStatus('Native client module loading: ' + progress);
}
  

function updatePageStatus(opt_message)
{
	if (opt_message)
	{
		pageStatusText = opt_message;
	}
	var statusField = document.getElementById('pageStatusField');
	if (statusField) 
	{
		statusField.innerHTML = pageStatusText;
	}
}

function updateModuleStatus(opt_message)
{
	if (opt_message)
	{
		moduleStatusText = opt_message;
	}
	var statusField = document.getElementById('moduleStatusField');
	if (statusField) 
	{
		statusField.innerHTML = moduleStatusText;
	}
}