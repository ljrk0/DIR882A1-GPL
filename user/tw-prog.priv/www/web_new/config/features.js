/* svn info: $Revision: 1615 $ $Date: 2019-09-03 11:55:01 +0800 (Tue, 03 Sep 2019) $ */
//common
function CommonDeviceInfo() {
	this.featuresvn=1627;
	
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
	this.hide2ForceInternetOnlyAccess = false;
	this.featureOperationMode_APnamedBridge = false;
	this.featureEnableOnlineUpgrade = false;
	this.featureEnableLinkAggregation = false;
	this.featureVLANVID0Support = false;
	this.no10mbpsWAN = false; // Network WANPortSpeed10Mbps
	this.featureWANSpeed2point5 = false;
	this.featureWANSpeed5 = false;
	this.featureWANSpeed10 = false;
	this.featureSuperMeshSwitch = false;
	this.featureSwitchNameOfWiFiSONEnabled = "Mesh";
	this.featureIpv6HtmlLegacyWanStatus = true;
	this.featureIdenticalVID = false;
	this.checkWirelessStatus = true;
	this.featureVLANwizard = false;
	this.featureWirelessSchedule = false;
	this.featureInboundFilter = false;
	this.featureScheduleRuleSlotInterval = 60;
	this.featureBitMapSchedule = false;
	this.featureWizardDFSDefaultByCountry = false;
	this.feature5GHzNonAutoBandwidthByCountry = false;
	this.featureSCChannelWidthOptShowByCountry = false;
	this.featureChannelWidthByCountry = false;
	this.featureLegacyExtender = false;
	this.featureFivePortRange = false;
	this.featureUserPasswordValidation = false;
	this.featureWizardWanLinkDownButton = false;
	this.feature6in4Enable = false;
	this.feature6to4Enable = false;
	this.featureWizardWPA2WPA3 = false;
	this.featureIPv4AsService = false;
	this.featureCgiAuth = false;
	this.featureSecurityOption = false;

	// bridge mode
	this.parentalControl_e = false;
	this.scheduleAndWiFi_e = false;
	this.feature_WirelessAp_WPS = false;
	
	// extender mode
	this.parentalControl_ea = false;
	this.scheduleAndWiFi_ea = false;
	this.featureWizardWPS_ea = false;
	this.featureCloseWizExtendedWifi_ea = false;
	this.featureEXTSSIDCheckBox_ea = false;

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
	this.operationModeCountdown = 140;
	this.wizardCountdown = 25;

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
	this.wlanRadioWMMEnable = false;
	this.featureCloseChannelWidth = false;
	this.featureCgiMethodGET = false;
}