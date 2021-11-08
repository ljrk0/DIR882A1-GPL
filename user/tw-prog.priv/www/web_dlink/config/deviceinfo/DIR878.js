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
	this.featureUsbPort = false;
	this.featureSharePort = false;
	this.featureUserDevice = true;
	this.featureTimeMachine = true;
	this.featureAutoRebootCfg = true;
	this.helpVer = "";
	this.wlanRadioWMMEnable = true;

	this.internetCountdown_IPv6 = 35;
	this.scheuldeCountdown = 30;
}
