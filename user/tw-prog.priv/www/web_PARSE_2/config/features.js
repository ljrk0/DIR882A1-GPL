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

	this.featurevlanadvanced = false;

	//extender mode
	this.uiAdminAdvBlock_e = false;
	this.supportRemoteManagement_e = false; 
	this.featurePLC_e = false;
	this.featureCovrWIFI_e = true;
	this.featureWPS_e = false;
	this.featureSmartConnect_e = false;
	this.featureBundled_e = false;
	this.featureQoSMgmtType_e = false;
	this.featureQosManageByApp_e = false;
	this.featureQoSSpeed_e = false;
	this.featureSystemLog_e = false;
	this.featureRebootSchedule_e = false;	

}

$.getScript("/config/deviceinfo.js", function(){
	DeviceInfo.prototype = new CommonDeviceInfo();
	var currentDevice = new DeviceInfo();
	
	//set device info
	sessionStorage.setItem('currentDevice', $.stringify(currentDevice));
});

