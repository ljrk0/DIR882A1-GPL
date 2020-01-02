//common
function CommonDeviceInfo()
{
	this.bridgeMode = false;
	this.featureVPN = false;

	this.featureSharePort = false;  // true: SharePort, false: USB Sharing
	this.featureDLNA = false;
	this.featureUPNPAV = true;
	this.featureSmartConnect = false;
	this.featureMyDLink = true;
	this.featureWPS = false;
	this.featureWEP = true;  // for wireless security mode

	this.featureAutoRebootCfg = true;

	this.supportUSB30Settings = false;

	this.featureFTPServer = false;
	this.featureSystemLog = false;
	this.featureInternetVlan = false;
	
	this.featureQoSMgmtType = false;
	this.featureQoSSpeed = true;
	this.featureQosManageByApp = false;

	this.featureUsbPort = false;
	this.featureUserDevice = false;
	this.featureTimeMachine = false;

	this.feature2nd5GHz = false;  // for 2nd 5GHz wireless

	// for covr wifi. 
	// featureSmartConnect & featureCovrWIFI can't be true at the same time
	this.featureCovrWIFI = false; 


	this.helpVer = "";
}

$.getScript("/config/deviceinfo.js", function(){
	DeviceInfo.prototype = new CommonDeviceInfo();
	var currentDevice = new DeviceInfo();
	
	//set device info
	sessionStorage.setItem('currentDevice', $.stringify(currentDevice));
});

