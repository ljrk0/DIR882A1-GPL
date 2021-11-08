define([], function(){

function WizDeviceInfo(){

	this.opMode = "";
	
	this.apcSSID = "";
	this.apcPWD = "";

	this.apSSID = "";
	this.apPWD = "";
	this.password = "";

	//data from server
	this.changePassword = false;
	this.deviceSettings = null;
	this.apcSettings = null;
	this.wifi_datalist = new Array();
	this.uplink = "";
	this.hasInternet = false;
	this.availableOPMode = null;

	this.wps = false;
}


function setWizardStatus(enabled){
	var dtd = $.Deferred();
	var setSetupWizardStatus = new SOAPSetSetupWizardStatus();

	setSetupWizardStatus.Enabled = enabled;
	setSetupWizardStatus.Duration = enabled? "180": "0";

	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("SetSetupWizardStatus", setSetupWizardStatus, null).done(function(obj){
		dtd.resolve();
	}).fail(function(){
		dtd.reject();
	});
	
	return dtd.promise();
}

function getDeviceInfo()
{
	var deferred = $.Deferred();
	var soapAction = new SOAPAction();
	
	soapAction.sendSOAPAction("GetDeviceSettings", null, getDeviceSettings)
	.done(function(obj){	
		deferred.resolve(obj);
	})
	.fail(function(){
		deferred.reject();
	});
	
	return deferred.promise();
}

function getWizDeviceInfo(deviceInfo){
	var dtd = $.Deferred();

	//opmode
	var opmodeResult = getOPmode().then(function(obj){
		deviceInfo.availableOPMode = obj.OperationModeList[0].OperationModeInfo.AvailableOPMode;		
	});

	//wifi
	var wirelessResult = get_wireless("RADIO_2.4GHz").then(function(obj){
		deviceInfo.apSSID = obj.setting.SSID;
		deviceInfo.apPWD = obj.security.Key;
		deviceInfo.wifi_datalist.push(obj);
	});

	//ip
	var uplinkResult = getUplinkInterface().then(function(obj){
		deviceInfo.uplink = obj.UplinkInterface;
		deviceInfo.hasInternet = COMM_ToBOOL(obj.NetworkLayerStatus);
	});

	//devicesettings
	var deviceResult = getDeviceInfo().then(function(obj){
		deviceInfo.deviceSettings = obj;
	});

	$.when(opmodeResult, wirelessResult, uplinkResult, deviceResult).always(function(){
		dtd.resolve(deviceInfo);
	});

	return dtd.promise();
}


function createWizWlanSettings(band, deviceInfo)
{
	var setting = new SOAPSetWLanRadioSettings();
	var radioData = getWiFiData(deviceInfo.wifi_datalist, band);

	setting.RadioID = "RADIO_2.4GHz";
	setting.Enabled = true;
	
	setting.Mode = "802.11bgn";
	setting.SSID = XMLEncode(deviceInfo.apSSID);
	setting.Coexistence = true;

	setting.ChannelWidth = 0;
	setting.Channel = 0;

	setting.SSIDBroadcast = true;
	setting.SecondaryChannel = radioData.setting.SecondaryChannel;
	setting.QoS = radioData.setting.QoS;
	setting.ScheduleName = radioData.setting.ScheduleName;
	setting.TXPower = radioData.setting.TXPower;

	return setting;
}

function createWizWlanSecurity(band, deviceInfo)
{
	var security = new SOAPSetWLanRadioSecurity();

	security.RadioID = "RADIO_2.4GHz";	
	
	security.Enabled = true;		
	security.Type = "WPAORWPA2-PSK";
	security.Encryption = "TKIPORAES";
	security.Key = deviceInfo.apPWD;
	security.KeyRenewal = 3600;

	return security;
}

function saveDeviceSetting(soapAction, deviceInfo)
{
	var DeviceSettings = new SOAPSetDeviceSettings();
	var getDevice = deviceInfo.deviceSettings;
	
	DeviceSettings.DeviceName = getDevice.DeviceName;
	DeviceSettings.PresentationURL = getDevice.PresentationURL;								
	DeviceSettings.AdminPassword = deviceInfo.password;
	DeviceSettings.ChangePassword = true;
	DeviceSettings.CAPTCHA = getDevice.CAPTCHA;
				
	soapAction.SetMultipleSOAP("SetDeviceSettings", DeviceSettings, null);
}

function storeWizDeviceInfo(deviceInfo){
	var dtd = $.Deferred();
	var soapAction = new SOAPAction();
	var setting;
	var security;

	if(!currentDevice.featureCovrWIFI_e){
		if(deviceInfo.wps == false){
			//OP mode
			for(var i in RadioIDList){
				var radioID = RadioIDList[i];
				setting = createOPmode(radioID, deviceInfo.opMode);
				soapAction.SetMultipleSOAP("SetOperationMode", setting, null);
			}

			if(deviceInfo.opMode != "WirelessAp")
			{
				setting = createAPCSettings(deviceInfo.apcSSID, deviceInfo.apcPWD);
				soapAction.SetMultipleSOAP("SetAPClientSettings", setting, null);
			}
		}
	}

	//smart connect
	if(currentDevice.featureSmartConnect_e){
		setSmartConnectStatus(soapAction, true);
	}

	//covr wifi
	if(currentDevice.featureCovrWIFI_e){
		setCovrWifiStatus(soapAction, true, false);
	}

	var radioID = "RADIO_2.4GHz";
	var radioIDStr = RadioIDToBand(radioID);

	setting = createWizWlanSettings(radioIDStr, deviceInfo);
	soapAction.SetMultipleSOAP("SetWLanRadioSettings", setting, null);

	security = createWizWlanSecurity(radioIDStr, deviceInfo);
	soapAction.SetMultipleSOAP("SetWLanRadioSecurity", security, null);

	//password
	saveDeviceSetting(soapAction, deviceInfo);

	setWizardStatus(false).then(function(){
		return soapAction.SendMultipleSOAPAction("SetMultipleActions");
	}).done(function(){
		dtd.resolve();
	}).fail(function(){
		dtd.reject();
	});

	return dtd.promise();
}

return {
	WizDeviceInfo: WizDeviceInfo,
	setWizardStatus: setWizardStatus,
	getWizDeviceInfo: getWizDeviceInfo,
	storeWizDeviceInfo: storeWizDeviceInfo
}

});