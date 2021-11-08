function SOAPTimeInfo() {
	this.TimeHourValue = "";
	this.TimeMinuteValue = "";
	this.TimeMidDateValue = false;
}

function SOAPSetFirmwareAutoUpdate() {
	this.TimeToUpdate = new SOAPTimeInfo();
	this.AutoQuery = true;
	this.AutoDownload = false;
	this.AutoUpdate = false;
	this.BetaFirmware = false;
}

function SOAPGetFirmwareAutoUpdateResponse() {
	this.TimeToUpdate = new SOAPTimeInfo();
	this.AutoQuery = true;
	this.AutoDownload = false;
	this.AutoUpdate = false;
	this.BetaFirmware = false;
}

function SOAPSetEventNotification() {
	this.Enabled = false;
	this.AutoFirmwareUpgrade = false;
}

function SOAPGetEventNotificationResponse() {
	this.Enabled = false;
	this.AutoFirmwareUpgrade = false;
}