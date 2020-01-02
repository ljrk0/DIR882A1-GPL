var getDeviceSettings = new SOAPGetDeviceSettingsResponse();

//model information object
function ModelInfo(modelName, hwVer, fwVer)
{
	this.modelName = modelName;
	this.hwVer = hwVer;
	this.fwVer = fwVer;
}

function getDeviceInfo()
{
	var deferred = $.Deferred();
	var soapAction = new SOAPAction();
	
	soapAction.sendSOAPAction("GetDeviceSettings", null, getDeviceSettings)
	.done(function(obj)
	{
		var newFW = obj.LatestFirmwareVersion;
		if (newFW == "" || newFW == ".")
		{	
			newFW = 0;
		}

		var modelInfo = new ModelInfo(obj.ModelName, obj.HardwareVersion, obj.FirmwareVersion);
		sessionStorage.setItem('modelInfomation', JSON.stringify(modelInfo));
		sessionStorage.setItem('currentFWVersion', obj.FirmwareVersion);
		sessionStorage.setItem('newFWVersion', newFW);
		
		deferred.resolve();
	})
	.fail(function()
	{
		deferred.reject();
	});
	
	return deferred.promise();
}

function setLang()
{
	var LangList = new Array("en-us", "zh-tw", "zh-cn", "ko-kr", "fr-fr", "pt-br", "es-es", "it-it", "de-de", "ru-ru");
	
	try
	{
		var language = localStorage.getItem('language');
		if (language == null)
		{
			language = "en-us";
			//get language from browser
			var browserLanguage = null;
			if (navigator.appName == 'Netscape')
			{	
				browserLanguage = navigator.language;			
			}
			else
			{
				browserLanguage = navigator.browserLanguage;	
			}
			browserLanguage = browserLanguage.toLowerCase();
			//check if we supported the browser language
			for (var i = 0; i < LangList.length; i ++)
			{
				if (browserLanguage == LangList[i])
				{
					language = browserLanguage;
				}
			}
			localStorage.setItem('language', language);
		}
		
		InitLANG(language);
	}
	catch (error)
	{
		if (error.code === DOMException.QUOTA_EXCEEDED_ERR && localStorage.length === 0)
		{
			alert(I18N("j", "The Private Browsing feature of Safari is incompatible with this device's interface. You will need to disable Private Browsing to log in."));
		}
		throw error;
	}
}

function detectBrowserVersion()
{
	var browser = $.client.browser;
	var version = $.client.version;
	if(browser == "Mozilla" && version == "11"){
		browser = "Explorer"
	}
	var bool = true;
	//console.log("OS: " + $.client.os);
	//console.log("Browser: " + browser + " / Version: " + version);
	//alert("Browser: " + browser + " / Version: " + version);
	switch(browser) {
		case "Chrome":
			if(version < 28) {
				bool = false;
			}
			break;
		case "Explorer":
			if(version < 9) {
				bool = false;
			}
			break;
		case "Edge":
			if(version < 12) {
				bool = false;
			}
			break;
		case "Safari":
			if(version < 6) {
				bool = false;
			}
			break;
		case "Firefox":
			if(version < 28) {
				bool = false;
			}
	}
	return bool;
}

function initEnv()
{
	var deferred = $.Deferred();
	
	var detectBrowser = detectBrowserVersion();
	if (!detectBrowser){
		alert (I18N("j", "The system doesn't support your browser."));
		return deferred.reject();
	}
	
	//async
	var result1 = getDeviceInfo();
	
	$.when(result1)
	.done(function()
	{
		deferred.resolve();
	})
	.fail(function()
	{
		deferred.reject();
	});

	//sync
	setLang();

	return deferred.promise();
}