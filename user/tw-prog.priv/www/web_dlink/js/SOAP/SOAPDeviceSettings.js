/**
 * @constructor
 */
function SOAPSetDeviceSettings() {
	this.DeviceName = "";
	this.AdminPassword = "";
	this.PresentationURL = "";
	this.CAPTCHA = false;
	this.ChangePassword = false;
};

// @prototype
SOAPSetDeviceSettings.prototype = {
	get AdminPassword(){
		return this._AdminPassword;
	},

	set AdminPassword(val){
		this._AdminPassword = AES_Encrypt128(val);
	}
}



/**
 * @constructor
 */
function SOAPGetDeviceSettingsResponse() {
	this.Type = "";
	this.DeviceName = "";
	this.VendorName = "";
	this.ModelDescription = "";
	this.ModelName = "";
	this.FirmwareVersion = "";
	this.FirmwareRegion = "";
	this.LatestFirmwareVersion = "";
	this.HardwareVersion = "";
	this.PresentationURL = "";
	this.CAPTCHA = false;
	this.SharePortStatus = true;
	this.DCS = "";
	this.BackOff = "";
};

// @prototype
SOAPGetDeviceSettingsResponse.prototype = {

}

/**
 * @constructor
 */ 
function SOAPSetLEDStatus() {
	this.Enabled = true;
}

/**
 * @constructor
 */ 
function SOAPGetLEDStatusResponse() {
	this.Enabled = true;
} 