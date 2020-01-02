//These settings will override common settings
function DeviceInfo()
{
	this.bridgeMode = true;
	this.featureVPN = true;
	this.featureDLNA = true;
	//this.featureSmartConnect = false;  // default: false
	this.featureMyDLink = false;
	this.featureFTPServer = true;
	this.featureSystemLog = true;
	this.featureInternetVlan = true;
	this.featureWPS = true;
	this.featureWEP = false;
	
	this.featureQoSMgmtType = true;
	this.featureQoSSpeed = true;
	this.featureQosManageByApp = false;
	this.featureUsbPort = true;
	//this.featureSharePort = false;  // default: false
	this.featureUserDevice = true;
	this.featureTimeMachine = true;
	//this.featureAutoRebootCfg = true; // default: true
    this.featureCovrWIFI = true;
  
	this.helpVer = "";
}
