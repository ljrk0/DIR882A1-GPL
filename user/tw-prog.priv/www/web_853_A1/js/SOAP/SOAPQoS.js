/**
 * @constructor
 */
function SOAPGetQoSSettings()
{
	this.QoSInfoLists = new SOAPQoSInfoLists();
	this.UploadBandwidth = -1;
	//this.UploadBandwidthUnit = "";
	this.DownloadBandwidth = -1;
	//this.DownloadBandwidthUnit = "";
};

/**
 * @constructor
 */
function SOAPSetQoSSettings()
{
	this.QoSInfoData = new SOAPQoSInfoLists();
	this.UploadBandwidth = 0;
	//this.UploadBandwidthUnit = "";
	this.DownloadBandwidth = 0;
	//this.DownloadBandwidthUnit = "";
};

/**
 * @constructor
 */
function SOAPQoSInfo()
{
	this.Hostname= "";
	this.IPAddress = "";
	this.MACAddress = "";
	this.Priority = 0;
	this.Type = "";
};

/**
 * @constructor
 */
function SOAPQoSInfoLists()
{
	var qosInfo = new SOAPQoSInfo();

	this.QoSInfo = $.makeArray(qosInfo);
};

/**
 * @constructor
 */
function SOAPGetQoSManagementType()
{
	this.QoSManagementType = false;
};

/**
 * @constructor
 */
function SOAPSetQoSManagementType()
{
	this.QoSManagementType = false;
};
