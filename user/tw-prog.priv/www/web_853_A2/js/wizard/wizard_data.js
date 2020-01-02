var setWanSettings = new SOAPSetWanSettings();
var setWLanRadioSettings_24g = new SOAPSetWLanRadioSettings();
var setWLanRadioSecurity_24g = new SOAPSetWLanRadioSecurity();
var setWLanRadioSettings_5g = new SOAPSetWLanRadioSettings();
var setWLanRadioSecurity_5g = new SOAPSetWLanRadioSecurity();


function set_wlanSettings(settingObj,securityObj, RadioID)
{
	settingObj.RadioID = RadioID;
	settingObj.Enabled = true;
	settingObj.SSID = "";
	settingObj.SSIDBroadcast = true;
	
	settingObj.Channel = "0";
	settingObj.SecondaryChannel = "0";
	settingObj.Qos = false;
	settingObj.ScheduleName = "";
	settingObj.TXPower = "100";
	settingObj.Coexistence = true;

	securityObj.RadioID = RadioID;
	securityObj.Enabled = true;
	securityObj.Type = "WPAORWPA2-PSK";
	securityObj.Encryption = "TKIPORAES";
	securityObj.Key = "";
	securityObj.KeyRenewal = "3600";
	securityObj.RadiusIP1 = "";
	securityObj.RadiusPort1 = "";
	securityObj.RadiusSecret1 = "";
	securityObj.RadiusIP2 = "";
	securityObj.RadiusPort2 = "";
	securityObj.RadiusSecret2 = "";

	if(RadioID == "RADIO_2.4GHz")
	{
		settingObj.Mode = "802.11bgn";
		settingObj.ChannelWidth = "0";
	}
	else	//5GHz
	{
		settingObj.Mode = "802.11anac";
		settingObj.ChannelWidth = "1";
	}
}


set_wlanSettings(setWLanRadioSettings_24g, setWLanRadioSecurity_24g, "RADIO_2.4GHz");
set_wlanSettings(setWLanRadioSettings_5g, setWLanRadioSecurity_5g, "RADIO_5GHz");




function SOAPSetTriggerWPS()
{
	this.ACTION = "PBC";
	this.PIN = "";
	this.Duration = "";
}

/**
 * @constructor
 */
function SOAPSetTriggerWPSResponse()
{
	this.WaitTime = "";
}


/**
 * @constructor
 */
function SOAPWPSStatus()
{
	this.RadioID = "";
	this.Status = "";
}

/**
 * @constructor
 */
function SOAPWPSStatusLists()
{
	var list = new SOAPWPSStatus();

	this.WPSStatus = $.makeArray(list);
}

/**
 * @constructor
 */
function SOAPGetWPSStatusResponse()
{
	this.WPSStatusLists = new SOAPWPSStatusLists();
}


/*
function SOAPSetDeviceSettings()
{
	this.DeviceName = "";
	this.AdminPassword = "";
	this.PresentationURL = "";
	this.CAPTCHA = false;
	this.ChangePassword = false;
}
*/
/*
function SOAPSetMultipleActionsResponse()
{
	this.CountDown= 25;
}
*/

