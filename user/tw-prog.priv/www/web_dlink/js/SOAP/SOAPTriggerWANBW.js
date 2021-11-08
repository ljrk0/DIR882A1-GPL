function SOAPSetTriggerWANBW() {
	this.MacAddress = "";
	this.TriggerWANBW = "";
};

function SOAPSetTriggerWANBWResponse() {
	this.InterfaceBandwidthList = new SOAPInterfaceBandwidthList();
};

function SOAPInterfaceBandwidthList() {
	var bandwidthRecord = new SOAPBandwidthRecord();
	this.BandwidthRecord = $.makeArray(bandwidthRecord);
}

function SOAPBandwidthRecord() {
	this.MacAddress = "";
	this.RemoteServerName = "";
	this.RemoteServerIP = "";
	this.LocallSPName = "";
	this.LocallIP = "";
	this.UploadBandwidth = "";
	this.DownloadBandwidth = "";
	this.Latency = "";
	this.DetectDateTime = "";
	this.ErrorCode = "";
	this.ErrorMsg = "";
}