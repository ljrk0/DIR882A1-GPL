document.write('<script type="text/javascript" charset="utf-8" src="/js/SOAP/SOAPFirmware.js"></script>');


function GetFirmwareSettings()
{
	var deferred = $.Deferred();
	var getFirmwareSettingsResponse = new SOAPGetFirmwareSettingsResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetFirmwareSettings", null, getFirmwareSettingsResponse).done(function(obj)
	{
		deferred.resolve(obj);
	}).fail(function(){
		deferred.reject();
	});
	return deferred.promise();
}

function GetFirmwareValidation()
{
	var deferred = $.Deferred();
	var getFirmwareValidationResponse = new SOAPGetFirmwareValidationResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetFirmwareValidation", null, getFirmwareValidationResponse).done(function(obj)
	{
		if(obj.IsValid == "true")
		{
			deferred.resolve(parseInt(obj.CountDown));
		}
		else
		{
			deferred.reject();
		}

	}).fail(function(){
		deferred.reject();
	});		
	return deferred.promise();
}

function StartFirmwareDownload()
{
	var deferred = $.Deferred();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("StartFirmwareDownload", null, null).done(function(obj)
	{
		if(obj.StartFirmwareDownloadResult == "OK")
		{
			GetPollingFirmwareDownloadStatus(3000).progress(function(val){
				deferred.notify(val);
			}).done(function(){
				deferred.resolve();
			}).fail(function(){
				deferred.reject();
			});
		}
		else
		{
			deferred.reject();
		}
	}).fail(function(){
		deferred.reject();
	});
	return deferred.promise();
}

function GetPollingFirmwareDownloadStatus(interval)
{
	var deferred = $.Deferred();
	var pollingFirmwareDownloadResponse = new SOAPPollingFirmwareDownloadResponse();
	var soapAction = new SOAPAction();
	var timer = null;
	var failTimer = null;

	if(interval > 0){
		timer = setInterval(function(){
			soapAction.sendSOAPAction("PollingFirmwareDownload", null, pollingFirmwareDownloadResponse).done(function(obj)
			{
				var progress = parseInt(obj.DownloadPercentage);
				if(progress < 100)
				{
					deferred.notify(progress);
				}
				else
				{
					clearInterval(timer);
					clearInterval(failTimer);
					deferred.resolve();
				}	
			});

		}, interval);

		failTimer = setTimeout(function(){
			clearInterval(timer);
			deferred.reject();
		},180000);
	}
	else
	{
		clearInterval(timer);
		clearInterval(failTimer);
		deferred.resolve();
	}
	return deferred.promise();
}

function GetFirmwareStatus()
{
	var deferred = $.Deferred();
	var getFirmwareStatusResponse = new SOAPGetFirmwareStatusResponse();
	var soapAction = new SOAPAction();

	soapAction.sendSOAPAction("GetFirmwareStatus", null, getFirmwareStatusResponse).done(function(obj)
	{
		deferred.resolve(obj);
	}).fail(function(){
		deferred.reject();
	});
	return deferred.promise();
}

function GetConfigFileValidation()
{
	var deferred = $.Deferred();
	var getConfigFileValidationResponse = new SOAPGetConfigFileValidationResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetConfigFileValidation", null, getConfigFileValidationResponse).done(function(obj)
	{
		if(obj.IsValid == "true")
		{
			deferred.resolve(parseInt(obj.CountDown));
		}
		else
		{
			deferred.reject();
		}

	}).fail(function(){
		deferred.reject();
	});		
	return deferred.promise();
}