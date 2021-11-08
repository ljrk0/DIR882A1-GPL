/* svn info: $Revision: 1131 $ $Date: 2018-10-03 14:06:50 +0800 (Wed, 03 Oct 2018) $ */
//common
function CommonDeviceInfo() {
	this.featuresvn=1135;
	
	//Router Mode
	this.featureOperationMode = false;
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
	this.featurePPPoEValid = false;
	this.featureRequireLevel = false;
	this.featureCloseCaptcha = false;
	this.feature5GBW160MHz = false;
	this.featureCloseAdminHTTPS = false;
	this.featureAddHNAP1Cgi = false;
	this.featureSingleUpdateForMultiFirmware = false;
	this.feature8080MHz = false;
	this.featureAlwaysOnWanWizard = false;
	this.featureSingleUpdateForSimpleFirmware = false;
	this.featureWizardHNAPChannelWidth = 1;
	this.featureCloseChannelWidth = false;
	this.hide2ForceInternetOnlyAccess = false;
	this.featureOperationMode_APnamedBridge = false;
	this.featureEnableOnlineUpgrade = false;
	this.featureEnableLinkAggregation = false;
	this.featureVLANVID0Support = false;

	//Extender Mode
	this.parentalControl_e=false;
	this.scheduleAndWiFi_e=false;
	
	// countdown
	this.okcount = 3;
	this.rebootcount = 80;
	this.internetCountdown = 25;
	this.internetCountdown_IPv6 = 30;  //add
	this.internetCountdown_VLAN = 25;  //add
	this.wifiInterfaceCountdown = 25;
	this.timeCountdown = 25;  //add
	this.usbSharingCountdown = 25;  //add
	this.systemCountdown = 80;

	// not use
	this.adminCountdown = 25;
	this.wizardcount = 40;
	this.scheuldeCountdown = 25;
	this.featureVPN = false;
	this.helpVer = "";
	this.bridgeMode = false;
	this.featureCloseWizExtendedWifi_e=false;
	this.featureWizardWPS_e=false;
	this.featureSmartConnect_e = false;
}