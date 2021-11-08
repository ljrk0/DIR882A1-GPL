//custom, these settings will override common settings
function DeviceInfo() {
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
	this.featureVlanAdvanced = true;
	// this.featureWan2 = true;
	// this.etisalatOption = true;
	// this.threeGFailover = true;
	// this.TR069 = true;
	// this.featureExtender = true;
	// this.featureMeshExtenderTopology = true;	

	// countdown
	this.internetCountdown = 25;
	this.internetCountdown_IPv6 = 35;
	this.wifiInterfaceCountdown = 25;
	this.scheuldeCountdown = 30;
}
