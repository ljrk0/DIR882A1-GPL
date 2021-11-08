//These settings will override common settings
function DeviceInfo()
{
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
	this.featureVlanAdvanced = true;
	this.featureServiceProvider = true;
	
	this.helpVer = "";
	
	this.okcount = 25;
	this.wizardcount = 40;	
	this.rebootcount = 120;
}
