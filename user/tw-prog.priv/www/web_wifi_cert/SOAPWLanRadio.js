/**
 * @constructor
 */
 function SOAPSecurityInfo()
 {
 	this.SecurityType = "";
 	this.Encryptions = new Array();
 }

 /**
 * @constructor
 */
 function SOAPSupportedSecurity()
 {
 	var securityInfo = new SOAPSecurityInfo();

 	this.SecurityInfo = $.makeArray(securityInfo);
 }

/**
 * @constructor
 */
function SOAPRadioInfo()
{
	var supportedSecurity = new SOAPSupportedSecurity();

	this.RadioID = "";
	this.Frequency = 2;
	this.SupportedModes = new Array();
	this.Channels = new Array();
	this.SupportedSecurity = $.makeArray(supportedSecurity);
} 

/**
 * @constructor
 */
function SOAPRadioInfos()
{
	var radioInfo = new SOAPRadioInfo();

	this.RadioInfo = $.makeArray(radioInfo);
} 

/**
 * @constructor
 */
function SOAPGetWLanRadiosResponse()
{
	this.RadioInfos = new SOAPRadioInfos();
};

/**
 * @constructor
 */
function SOAPGetWLanRadioSettings()
{
	this.RadioID ="";
}

/**
 * @constructor
 */
function SOAPGetWLanRadioSettingsResponse()
{
	this.Enabled = "";
	this.Mode = "";
	this.MacAddress = "";
	this.SSID = "";
	this.SSIDBroadcast = "";
	this.ChannelWidth = "";
	this.Channel = "";
	this.SecondaryChannel = "";
	this.QoS = "";
	this.ScheduleName = "";
	this.TXPower = "";
	this.RadioEnabled = "";
	this.Coexistence = "";

};

// @prototype
SOAPGetWLanRadioSettingsResponse.prototype =
{
	get ScheduleName(){
		return this._ScheduleName;
	},
	
	set ScheduleName(val){
		if((val == "")||(val.toLowerCase() == 'always'))
		{
			this._ScheduleName = "Always";
		}
		else
		{
			this._ScheduleName = val;
		}
	}
}
/**
 * @constructor
 */
function SOAPGetWLanRadioSecurity()
{
	this.RadioID ="";
}

/**
 * @constructor
 */
function SOAPGetWLanRadioSecurityResponse()
{
	this.Enabled = "";
	this.Type = "";
	this.Encryption = "";
	this.Key = "";
	this.KeyRenewal = "";
	this.RadiusIP1 = "";
	this.RadiusPort1 = "";
	this.RadiusSecret1 = "";
	this.RadiusIP2 = "";
	this.RadiusPort2 = "";
	this.RadiusSecret2 = "";
};

// @prototype
SOAPGetWLanRadioSecurityResponse.prototype =
{	
	get Key(){
		return AES_Decrypt128(this._Key);
	},

	set Key(val){
		this._Key = val;
	},
	
}

/**
 * @constructor
 */
function SOAPSetWLanRadioSettings()
{
	this.RadioID ="";
	this.Enabled = true;
	this.Mode = "";
	this.SSID = "";
	this.SSIDBroadcast = true;
	this.ChannelWidth = "";
	this.Channel = "0";
	this.SecondaryChannel = "0";
	this.QoS = false;
	this.ScheduleName = "";
	this.TXPower = "";
	this.Coexistence = false;

};

/**
 * @constructor
 */
function SOAPSetWLanRadioSecurity()
{
	this.RadioID ="";
	this.Enabled = true;
	this.Type = "";
	this.Encryption = "";
	this.Key = "";
	this.KeyRenewal = "";
	this.RadiusIP1 = "";
	this.RadiusPort1 = "";
	this.RadiusSecret1 = "";
	this.RadiusIP2 = "";
	this.RadiusPort2 = "";
	this.RadiusSecret2 = "";
};

// @prototype
SOAPSetWLanRadioSecurity.prototype =
{	
	get Key(){
		return this._Key;
	},
	
//	get KeyGUI(){
//		return AES_Decrypt128(this._Key);
//	},

	set Key(val){
		this._Key = AES_Encrypt128(val);
	}
}

/**
 * @constructor
 */
function SOAPGetSmartconnectSettingsResponse()
{
	this.Enabled = "";
}

/**
 * @constructor
 */
function SOAPSetSmartconnectSettings()
{
	this.Enabled = false;
}

/**
 * @constructor
 */
function SOAPGetGuestZoneRouterSettingsResponse()
{
	this.InternetAccessOnly = false;
	this.IPAddress = "";
	this.SubnetMask = "";
	this.DHCPServer = "";
	this.DHCPRangeStart = "";
	this.DHCPRangeEnd = "";
	this.DHCPLeaseTime = 0;
}

/**
 * @constructor
 */
function SOAPSetGuestZoneRouterSettings()
{
	this.InternetAccessOnly = false;
	this.IPAddress = "";
	this.SubnetMask = "";
	this.DHCPServer = "";
	this.DHCPRangeStart = "";
	this.DHCPRangeEnd = "";
	this.DHCPLeaseTime = 0;
}

/**
 * @constructor
 */
function SOAPGetWPSSettingsResponse()
{
	this.WPSPBC = false;
	this.WPSPIN = false;
	this.WscConfStatus = 0;
}
/**
 * @constructor
 */
//function SOAPSetTriggerWPS()
//{
//	this.PIN = "";
//	this.ACTION = "";
//}
/**
 * @constructor
 */
function SOAPSetWPSSettings()
{
	this.WPSPBC = false;
	this.WPSPIN = false;
	this.WscConfStatus = 0;
}