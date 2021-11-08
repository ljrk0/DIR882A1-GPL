/**
 * @constructor
 */
function SOAPGetUplinkInterfaceResponse()
{
	this.UplinkInterface = "";
	this.NetworkLayerStatus = "";
}

/**
 * @constructor
 */
function SOAPGetAPLanSettingsResponse()
{
	this.Mode = "";
	this.IPAddr = "";
	this.SubnetMask = "";
	this.Gateway = "";
	this.PrimaryDNS = "";
	this.SecondaryDNS = "";
	this.DeviceName = "";
	this.LocalDomainName = "";

};

// @prototype
SOAPGetAPLanSettingsResponse.prototype =
{
	
}

 /**
 * @constructor
 */
 function SOAPSetAPLanSettings()
 {
	this.Mode = "";
	this.IPAddr = "";
	this.SubnetMask = "";
	this.Gateway = "";
	this.PrimaryDNS = "";
	this.SecondaryDNS = "";
	this.DeviceName = "";
	this.LocalDomainName = "";
 }

/**
 * @constructor
 */
 function SOAPGetHostIPv6StatusResponse()
 {
 	this.IPv6_ConnectionType = "";
 	this.IPv6_Address = "";
 	this.IPv6_DefaultGateway = "";
 	this.IPv6_PrimaryDNS = "";
 	this.IPv6_SecondaryDNS = "";
 	this.IPv6_LinkLocalAddress = "";
 }



/**
 * @constructor
 */
function SOAPGetHostIPv6SettingsResponse()
{
	this.IPv6Mode = "";

};

// @prototype
SOAPGetHostIPv6SettingsResponse.prototype =
{
	
}

/**
 * @constructor
 */
function SOAPSetHostIPv6Settings()
{
	this.IPv6Mode = "";

};

/**
 * @constructor
 */
function SOAPGetHostIPv6StaticSettingsResponse()
{
	this.StaticAddress = "";
	this.StaticPrefixLength = "";
	this.StaticDefaultGateway = "";
	this.StaticDNS1 = "";
	this.StaticDNS2 = "";

};

// @prototype
SOAPGetHostIPv6StaticSettingsResponse.prototype =
{
	
}

function SOAPSetHostIPv6StaticSettings()
{
	this.StaticAddress = "";
	this.StaticPrefixLength = "";
	this.StaticDefaultGateway = "";
	this.StaticDNS1 = "";
	this.StaticDNS2 = "";

};

/**
 * @constructor
 */
function SOAPGetHostIPv6AutoConfigurationSettingsResponse()
{
	this.ObtainDNS = "";
	this.AutoDNS1 = "";
	this.AutoDNS2 = "";

};

// @prototype
SOAPGetHostIPv6AutoConfigurationSettingsResponse.prototype =
{
	
}

function SOAPSetHostIPv6AutoConfigurationSettings()
{
	this.ObtainDNS = "";
	this.AutoDNS1 = "";
	this.AutoDNS2 = "";

};


function SOAPGetPLCInfoResponse()
{
	this.PLCInterface = "";
	this.PLCMode = "";
	this.PLCMAC = "";
	this.DAK = "";
	this.PLCStationNumber = "";
}

SOAPGetPLCInfoResponse.prototype = {
	get DAK() {
		return AES_Decrypt128(this._DAK);
	},
	set DAK(val) {
		this._DAK = val;
	}
}

