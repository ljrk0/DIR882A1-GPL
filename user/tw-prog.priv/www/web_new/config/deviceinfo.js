/* svn info: $Revision: 1259 $ $Date: 2019-01-14 16:03:31 +0800 (Mon, 14 Jan 2019) $ */
//custom, these settings will override common settings
function DeviceInfo() {
	this.bridgeMode = true;
	this.featureVPN = true;
	this.featureDLNA = true;
	this.featureMyDLink = false;
	this.featureFTPServer = true;
	this.featureSystemLog = true;
	this.featureInternetVlan = true;
	this.featureWPS = true;
	this.featureWEP = false;
	this.featureQoSMgmtType = true;
	this.featureQoSSpeed = true;
	this.featureQosManageByApp = false;
    this.featureCovrWIFI = true;
	this.helpVer = "";
	this.featureLanPort = 1;
	this.featureMultiFirmware = true;
	this.featureExtender = true;
	this.featureQuickVPN = false;
	this.featureRunningWizardNoCloseBtn = true;
	this.featureCovrBundle = true;
	this.featureNewTime = true;
	this.featureAutoUpgrade = true;
	this.featureLEDControl = true;
	this.featureCloseWPSPIN = true;
	this.wlanRadioWMMEnable = true;

	// countdown
	this.okcount = 25;	
	this.internetCountdown = 55;
	this.wifiInterfaceCountdown = 60;
	this.internetCountdown_IPv6 = 55;
	this.internetCountdown_VLAN = 55;
	this.systemCountdown = 140;
}
