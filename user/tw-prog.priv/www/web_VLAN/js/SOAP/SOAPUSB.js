/**
 * @constructor
 */
function SOAPGetUSB30SettingsResponse()
{
	this.Enabled = "";
};


/**
 * @constructor
 */
function SOAPGetDLNAResponse()
{
	this.Enabled = "";
	this.DeviceName = "";
};

/**
 * @constructor
 */
function SOAPSetDLNA()
{
	this.Enabled = "";
	this.DeviceName = "";
};

/**
 * @constructor
 */
function SOAStorageUser()
{
	this.UserName = "";
	this.Password = "";
	this.AccessPath = "";
	this.Permission = false;

}

/**
 * @constructor
 */
function SOAPGetUSBStorageSettingsResponse()
{
	var storageUser = new SOAStorageUser();

	this.Enabled = "";
	this.RemoteHttp = false;
	this.RemoteHttpPort = 0;
	this.RemoteHttps = false;
	this.RemoteHttpsPort = 0;
	this.StorageUserInfoLists = $.makeArray(storageUser);
};


/**
 * @constructor
 */
function SOAPSetUSBStorageSettings()
{
	this.Enabled = "";
	this.RemoteHttp = false;
	this.RemoteHttpPort = 0;
	this.RemoteHttps = false;
	this.RemoteHttpsPort = 0;
};

/**
 * @constructor
 */
function SOAPUSBDevice()
{
	this.Port = "";
	this.Model = "";
	this.VID = "";
	this.PID = "";
	this.TotalSpace = "";
	this.FreeSpace = "";
	this.SmartStatus = "";
	this.PartitionInfoLists = new SOAPPartitionInfoLists();
}

/**
 * @constructor
 */
function SOAPUSBDeviceInfoLists()
{
	var usbDevice = new SOAPUSBDevice();

	this.USBDevice = $.makeArray(usbDevice);
}

/**
 * @constructor
 */
function SOAPGetUSBDeviceResponse()
{
	this.USBDeviceInfoLists = new SOAPUSBDeviceInfoLists();
}

/**
 * @constructor
 */
function SOAPGetSMBStatusResponse()
{
	this.Enabled = false;
};

/**
 * @constructor
 */
function SOAPSetSMBStatus()
{
	this.Enabled = false;
};

/**
 * @constructor
 */
function SOAPGetFTPSettings()
{
	this.Enabled = false;
	this.RemoteFTP = false;
	this.FTPPort = "";
	this.IdleTime = "";
};

/**
 * @constructor
 */
function SOAPSetFTPSettings()
{
	this.Enabled = false;
	this.RemoteFTP = false;
	this.FTPPort = "";
	this.IdleTime = "";
};

/**
 * @constructor
 */
function SOAPPartitionInfoLists()
{
	var partitionInfo = new SOAPPartitionInfo();

	this.PartitionInfo = $.makeArray(partitionInfo);
};

/**
 * @constructor
 */
function SOAPPartitionInfo()
{
	this.PartitionName = "";
	this.PartitionLabel = "";
	this.TotalSpace = "";
	this.FreeSpace = "";
};



