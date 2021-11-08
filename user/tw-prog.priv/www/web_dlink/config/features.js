//common
function CommonDeviceInfo() {
	//Router Mode
	this.featureOperationMode = false;
	this.bridgeMode = false;
	this.featureVPN = false;
	this.featureSharePort = false;  //true SharePort, false: USB Sharing
	this.featureDLNA = false;
	this.featureUPNPAV = true;
	this.featureSmartConnect = false;
	this.featureMyDLink = true;
	this.featureWPS = false;
	this.featureWEP = true;  //for wireless security mode
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
	this.feature2nd5GHz = false;  //for 2nd 5GHz wireless
	this.featureCovrWIFI = false;  //for covr wifi, featureSmartConnect & featureCovrWIFI can't be true at the same time
	this.helpVer = "";
	this.featureVlanAdvanced = false;
	this.featureWan2 = false;
	this.etisalatOption = false;
	this.threeGFailover = false;
	this.TR069 = false;
	this.featureServiceProvider = false;
	this.featureLanPort = 4;
	this.featureMultiFirmware = false;
	this.featureExtender = false;
	this.featureQuickVPN = true;
	this.featureRunningWizardNoCloseBtn = false;
	this.featureCovrBundle = false;
	this.featureTraffic = false;
	this.featureMeshExtenderTopology = false;
	this.wlanRadioWMMEnable = false;
	this.featureNewTime = false;
	this.featureSpeedTest = false;
	this.featureQuickVPNProfile = false;
	this.featureAutoUpgrade = false;
	this.featureIntelWireless = false;
	this.featureMcAfee = false;
	this.featureCloseVLANWireless = false;
	this.featureLEDControl = false;
	this.featureCloseWPSPIN = false;
	this.featureCovrForWan = false;
	this.featurePPPoEValid = false;
	this.featureRequireLevel = false;
	this.featureCloseCaptcha = false;
	this.feature5GBW160MHz = false;

	//Extender Mode
	this.uiAdminAdvBlock_e = false;
	this.supportRemoteManagement_e = false; 
	this.featurePLC_e = false;
	this.featureCovrWIFI_e = false;
	this.featureWPS_e = false;  //if featureWPS_e not true, featureWPSPIN_e can't true
	this.featureWPSPIN_e = false;
	this.featureSmartConnect_e = false;
	this.featureBundled_e = false;
	this.featureQoSMgmtType_e = false;
	this.featureQosManageByApp_e = false;
	this.featureQoSSpeed_e = false;
	this.featureSystemLog_e = false;
	this.featureRebootSchedule_e = false;	
	this.featureOpwifimode_e = false;
	this.featureParentControl_e = false;

	// countdown
	this.okcount = 3;
	this.wizardcount = 40;
	this.rebootcount = 80;
	this.internetCountdown = 25;
	this.internetCountdown_IPv6 = 30;  //add
	this.internetCountdown_VLAN = 25;  //add
	this.wifiInterfaceCountdown = 25;
	this.timeCountdown = 25;  //add
	this.scheuldeCountdown = 25;  //add
	this.adminCountdown = 25;  //add
	this.usbSharingCountdown = 25;  //add
	this.systemCountdown = 80;
}

// $.getScript("/config/deviceinfo.js", function(){
// 	DeviceInfo.prototype = new CommonDeviceInfo();
// 	var currentDevice = new DeviceInfo();
	
// 	//set device info
// 	sessionStorage.setItem('currentDevice', $.stringify(currentDevice));
// });

$(function() {
	DeviceInfo.prototype = new CommonDeviceInfo();
	var currentDevice = new DeviceInfo();

	//set device info
	sessionStorage.setItem('currentDevice', $.stringify(currentDevice));
});