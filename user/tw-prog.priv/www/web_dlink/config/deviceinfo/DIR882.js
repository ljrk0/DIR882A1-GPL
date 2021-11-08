//custom, these settings will override common settings
function DeviceInfo() {
	//Router Mode
	this.featureOperationMode = true;
	this.bridgeMode = true;
	this.featureVPN = true;
	this.featureDLNA = true;
	this.featureSmartConnect = true;
	this.featureMyDLink = false;
	this.featureFTPServer = true;
	this.featureSystemLog = true;
	this.featureInternetVlan = true;
	this.featureWPS = true;
	this.featureWEP = false;
	this.featureQoSSpeed = true;
	this.featureQosManageByApp = false;
	this.featureUsbPort = true;
	this.featureSharePort = false;
	this.featureUserDevice = true;
	this.featureTimeMachine = true;
	this.featureAutoRebootCfg = true;
	this.featureVlanAdvanced = true;
	this.helpVer = "";
	this.wlanRadioWMMEnable = true;

	//Extender Mode
	// this.featureSmartConnect_e = true;
	// this.featureWPS_e = true;
	// this.featureWPSPIN_e = true;
	// this.featureOpwifimode_e = true;

	this.internetCountdown_IPv6 = 35;
	this.scheuldeCountdown = 30;
}
