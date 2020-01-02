/**
 * @constructor
 */
function SOAPGetNetworkSettingsResponse()
{
	this.IPAddress = "";
	this.SubnetMask = "";
	this.DeviceName = "";
	this.LocalDomainName = "";
	this.IPRangeStart = "";
	this.IPRangeEnd = "";
	this.LeaseTime = "";
	this.Boradcast = "";
	this.DNSRelay = "";

};

// @prototype
SOAPGetNetworkSettingsResponse.prototype =
{
	
}

/**
 * @constructor
 */
function SOAPGetRouterLanSettingsResponse()
{
	this.RouterIPAddress = "";
	this.RouterSubnetMask = "";
	this.DHCPServerEnabled = "";
	this.RouterMACAddress = "";
};

/**
 * @constructor
 */
function SOAPGetAdvNetworkSettingsResponse()
{
	this.UPNP = "";
	this.MulticastIPv4 = "";
	this.MulticastIPv6 = "";
	this.WANPortSpeed = "";
	
};

// @prototype
SOAPGetAdvNetworkSettingsResponse.prototype =
{
	
}

/**
 * @constructor
 */
function SOAPSetNetworkSettings()
{
	this.IPAddress = "";
	this.SubnetMask = "";
	this.DeviceName = "";
	this.LocalDomainName = "";
	this.IPRangeStart = "";
	this.IPRangeEnd = "";
	this.LeaseTime = "";
	this.Boradcast = "";
	this.DNSRelay = "";
	
};

/**
 * @constructor
 */
function SOAPSetAdvNetworkSettings()
{
	this.UPNP = "";
	this.MulticastIPv4 = "";
	this.MulticastIPv6 = "";
	this.WANPortSpeed = "";
	
};

/**
 * @constructor
 */
function SOAPSetRouterLanSettings()
{
	this.RouterIPAddress = "";
	this.RouterSubnetMask = "";
	this.DHCPServerEnabled = "";
	this.RouterMACAddress = "";
};