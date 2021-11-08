/*old for C1200*/
function SOAPGetWiFiSONSettings() {
    this.WiFiSONEnabled = false;
    this.MUMIMOEnabled = true;
    this.BandSteeringEnabled = true;
    this.AirTimeFairnessEnabled = true;
}
function SOAPSetWiFiSONSettings() {
    this.WiFiSONEnabled = false;
    this.MUMIMOEnabled = true;
    this.BandSteeringEnabled = true;
    this.AirTimeFairnessEnabled = true;
}


/*new*/
function SOAPGetWiFiSONSettingsResponse() {
	this.WiFiSONStatus = false;
	this.WiFiSONList = new SOAPWiFiSONListResponse();
	this.WiFiSONEnabled = false;
}

function SOAPWiFiSONListResponse() {
	var wiFiSON = new SOAPWiFiSONResponse();

	this.WiFiSON = $.makeArray(wiFiSON);
}

function SOAPWiFiSONResponse() {
	this.MacAddress = "";
	this.WiFiSONRole = "";
	this.HOP = "";
	this.UplinkMacAddress = "";
	this.UplinkType = "";
	this.ErrorCode = "";
	this.VarInfo = "";
}
