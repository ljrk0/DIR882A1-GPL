
function DeviceInfo() {
	this.opMode = "";
	this.uplink = "";
	this.apcSettings = "";
	this.wifi_datalist = "";
	this.lanSettings = "";
	this.ipv6settings = "";

	this.hasInternet = false;
	this.smartconnect = false;

	this.covrwifi = null;
	this.WiFiSONRole = "";
	this.WiFiSONList = null;
	this.plcinfo = "";
}

function getNetworkSettings()
{
	var dtd = $.Deferred();
	var lanSettings = new SOAPGetAPLanSettingsResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetAPLanSettings", null, lanSettings).done(function(obj){
		dtd.resolve(lanSettings);
	});
	
	return dtd.promise();
}

function getIPv6Settings(){
	var dtd = $.Deferred();
	var v6status = new SOAPGetHostIPv6StatusResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetHostIPv6Status", null, v6status).done(function(obj){
		dtd.resolve(v6status);
	});
	
	return dtd.promise();
}

function getUplinkInterface(){
	var dtd = $.Deferred();
	var iface = new SOAPGetUplinkInterfaceResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetUplinkInterface", null, iface).done(function(obj){
		dtd.resolve(iface);
	});
	
	return dtd.promise();
}

function getPLCInfo(){
	var dtd = $.Deferred();
	var getPLCInfo = new SOAPGetPLCInfoResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetPLCInfo", null, getPLCInfo).done(function(obj){
		dtd.resolve(getPLCInfo);
	});

	return dtd.promise();
}



function getDeviceInfo(currentDeviceInfo){
	var dtd = $.Deferred();
	var init = false;
	var opmodeResult = null;
	var smartResult = null;
	var wirelessResult = null;
	var covrResult = null;
	var opmodeResult = null;
	var plcinfoResult = null;
	var deviceInfo = currentDeviceInfo;


	if(deviceInfo == null){
		deviceInfo = new DeviceInfo();
		init = true;
	}

	//opmode
	if(init){
		opmodeResult = getOPmode().then(function(obj){
			deviceInfo.opMode = obj.OperationModeList[0].OperationModeInfo.CurrentOPMode;
		});
	}


	//wifi
	if(init){
		if(currentDevice.featureSmartConnect_e == true){
			smartResult = getSmartConnectStatus().then(function(obj){
				deviceInfo.smartconnect = COMM_ToBOOL(obj.Enabled);
			});
		}

		wirelessResult = GetWirelessXML().then(function(obj){
			deviceInfo.wifi_datalist = obj;	//from wireless.js

			return getAPCinfo(RadioIDList);
		}).always(function(obj){
			deviceInfo.apcSettings = obj;
		});
	}
	else
	{
		getAPCinfo(RadioIDList).then(function(obj){
			deviceInfo.apcSettings = obj;
		});
	}


	//covrwifi
	if(currentDevice.featureCovrWIFI_e == true){
		covrResult = getSmartCovrStatus().then(function(obj){
			deviceInfo.covrwifi = COMM_ToBOOL(obj.WiFiSONEnabled);
			deviceInfo.WiFiSONRole = obj.WiFiSONRole;
			deviceInfo.WiFiSONList = obj.WiFiSONList;
		});
	}

	//ip
	var uplinkResult = getUplinkInterface().then(function(obj){
		deviceInfo.uplink = obj;
		deviceInfo.hasInternet = COMM_ToBOOL(obj.NetworkLayerStatus);
	});

	var networkResult = getNetworkSettings().then(function(obj){
		deviceInfo.lanSettings = obj;
	});

	var v6Result = getIPv6Settings().then(function(obj){
		deviceInfo.ipv6settings = obj;
	});

	//plc
	if(currentDevice.featurePLC_e){
		plcinfoResult = getPLCInfo().then(function(obj){
			deviceInfo.plcinfo = obj;
		});
	}

	$.when(opmodeResult, smartResult, wirelessResult, covrResult, uplinkResult, networkResult, v6Result, plcinfoResult).always(function(){
		dtd.resolve(deviceInfo);
	});

	return dtd.promise();
}