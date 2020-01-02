//common
function CommonDeviceInfo()
{
	this.bridgeMode = false;
	this.featureVPN = false;

	this.featureSharePort = false;
	this.featureDLNA = false;
	this.featureUPNPAV = false;
	this.featureSmartConnect = false;
	this.featureMyDLink = false;
	
	this.supportUSB30Settings = false;
	
	this.helpVer = "";
}

$.getScript("/config/deviceinfo.js", function(){
	DeviceInfo.prototype = new CommonDeviceInfo();
	var currentDevice = new DeviceInfo();
	//alert("test");
	//set device info
	sessionStorage.setItem('currentDevice', $.stringify(currentDevice));
});

