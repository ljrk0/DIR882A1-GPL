/**
 * @constructor
 */
function SOAPGetIPv6StatusResponse()
{
	this.IPv6_ConnectionType = "";
	this.IPv6_Network_Status = "";
	this.IPv6_ConnectionTime = 0;
	this.IPv6_WanAddress = new Array();
	this.IPv6_WanLinkLocalAddress = "";
	this.IPv6_DefaultGateway = "";
	this.IPv6_LanAddress = new Array();
	this.IPv6_LanLinkLocalAddress = "";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	this.IPv6_DhcpPd = "";
	this.IPv6_DhcpPdPrefix = new Array();
	this.IPv6_GuestZoneAddress = new Array();
	this.IPv6_GuestZoneLinkLocalAddress = "";

};

// @prototype
SOAPGetIPv6StatusResponse.prototype = 
{

}

/**
 * @constructor
 */
function SOAPRenewIPv6WanConnection()
{
	this.Action = "";
}

function SOAPSetIPv6LinkLocalOnlySettings()
{
	this.IPv6_ConnectionType = "IPv6_LinkLocalOnly";
	this.IPv6_LanUniqueLocalAddressStatus = "Enable";
	this.IPv6_LanUniqueLocalAddressDefaultPrefix = "Enable";
	this.IPv6_LanUniqueLocalAddressPrefix = "";
};

function SOAPSetIPv6AutoDetectionSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_AutoDetection";
	this.IPv6_ObtainDNS = "";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}


function SOAPSetIPv6StaticSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_Static";
	this.IPv6_UseLinkLocalAddress = "Disable";
	this.IPv6_Address = "";
	this.IPv6_SubnetPrefixLength = 64;
	this.IPv6_DefaultGateway = "";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPSetIPv6AutoConfigurationSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_AutoConfiguration";
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPSetIPv6PppoeSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_PPPoE";
	this.IPv6_PppoeNewSession = "SharedWithIPv4";
	this.IPv6_PppoeType = "Dynamic";
	this.IPv6_PppoeStaticIp = "";
	this.IPv6_PppoeUsername = "";
	this.IPv6_PppoePassword = "";
	this.IPv6_PppoeReconnectMode = "AlwaysOn";
	this.IPv6_PppoeMaxIdleTime = "5";
	this.IPv6_PppoeMTU = "1492";
	this.IPv6_PppoeServiceName = "";
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

// @prototype
SOAPSetIPv6PppoeSettings.prototype = 
{
	get IPv6_PppoePassword(){
		return this._IPv6_PppoePassword;
	},

	set IPv6_PppoePassword(val){
		this._IPv6_PppoePassword = AES_Encrypt128(val);
	}
}

function SOAPSetIPv66in4TunnelSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_IPv6InIPv4Tunnel";
	this.IPv6_6In4RemoteIPv4Address = "";
	this.IPv6_6In4RemoteIPv6Address = "";
	this.IPv6_6In4LocalIPv6Address = "";
	this.IPv6_6In4SubnetPrefixLength = "";
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";

	// WAN
//this.IPv6_6In4LocalIPv4Address = "";
}

function SOAPSetIPv66to4TunnelSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_6To4";
	this.IPv6_6To4Relay = "192.88.99.1";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_6to4LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";

	// WAN
//this.IPv6_6to4Address = "";
}

function SOAPSetIPv66rdTunnelSettings()
{
	// WAN
	this.IPv6_ConnectionType = "IPv6_6RD";
	this.IPv6_6rdHubSpokeMode = "Enable";
	this.IPv6_6Rd_Configuration = "DHCPv4_Option";
	this.IPv6_6Rd_IPv6Prefix = "";
	this.IPv6_6Rd_IPv6PrefixLength = "";
	this.IPv6_6Rd_MaskLength = "";
	this.IPv6_6Rd_BorderRelayIPv4Address = "";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";

//this.IPv6_LanAddress = "";
//this.IPv6_AssignIPv6Prefix = "";
}

function SOAPGetIPv6LinkLocalOnlySettingsResponse()
{
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_LanUniqueLocalAddressStatus = "Enable";
	this.IPv6_LanUniqueLocalAddressDefaultPrefix = "Enable";
	this.IPv6_LanUniqueLocalAddressPrefix = "";
	this.IPv6_LanUniqueLocalAddressPrefixLength = 64;
	this.IPv6_LanUniqueLocalAddress = "";
}

function SOAPGetIPv6AutoDetectionSettingsResponse()
{
	// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanLinkLocalAddress = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPGetIPv6StaticSettingsResponse()
{
// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_UseLinkLocalAddress = "Disable";
	this.IPv6_Address = "";
	this.IPv6_SubnetPrefixLength = "";
	this.IPv6_DefaultGateway = "";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
// LAN
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanLinkLocalAddress = "";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPGetIPv6AutoConfigurationSettingsResponse()
{
	// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
	// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanLinkLocalAddress = "";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPGetIPv6PppoeSettingsResponse()
{
// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_PppoeNewSession = "SharedWithIPv4";
	this.IPv6_PppoeType = "Dynamic";
	this.IPv6_PppoeStaticIp = "";
	this.IPv6_PppoeUsername = "";
	this.IPv6_PppoePassword = "";
	this.IPv6_PppoeReconnectMode = "AlwaysOn";
	this.IPv6_PppoeMaxIdleTime = "5";
	this.IPv6_PppoeMTU = "1492";
	this.IPv6_PppoeServiceName = "";
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanLinkLocalAddress = "";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPGetIPv66in4TunnelSettingsResponse()
{
// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_6In4RemoteIPv4Address = "";
	this.IPv6_6In4RemoteIPv6Address = "";
	this.IPv6_6In4LocalIPv6Address = "";
	this.IPv6_6In4LocalIPv4Address = "";
	this.IPv6_6In4SubnetPrefixLength = "";
	this.IPv6_ObtainDNS = "Automatic";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
// LAN
	this.IPv6_DhcpPd = "Enable";
	this.IPv6_LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutomaticDhcpPd = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanLinkLocalAddress = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPGetIPv66to4TunnelSettingsResponse()
{
// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_6to4Address = "";
	this.IPv6_6To4Relay = "192.88.99.1";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
// LAN
	this.IPv6_6to4LanAddress = "";
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}

function SOAPGetIPv66rdTunnelSettingsResponse()
{
// WAN
	this.IPv6_IsCurrentConnectionType = false;
	this.IPv6_AssignIPv6Prefix = "";
	this.IPv6_6rdHubSpokeMode = "Enable";
	this.IPv6_6Rd_Configuration = "DHCPv4_Option";
	this.IPv6_6Rd_IPv6Prefix = "";
	this.IPv6_6Rd_IPv6PrefixLength = "";
	this.IPv6_6Rd_MaskLength = "";
	this.IPv6_6Rd_BorderRelayIPv4Address = "";
	this.IPv6_PrimaryDNS = "";
	this.IPv6_SecondaryDNS = "";
// LAN
	this.IPv6_LanIPv6AddressAutoAssignment = "Enable";
	this.IPv6_LanAutoConfigurationType = "SLAAC_StatelessDhcp";
	this.IPv6_LanIPv6AddressRangeStart = "";
	this.IPv6_LanIPv6AddressRangeEnd = "";
	this.IPv6_LanDhcpLifeTime = "10080";
	this.IPv6_LanRouterAdvertisementLifeTime = "30";
}
