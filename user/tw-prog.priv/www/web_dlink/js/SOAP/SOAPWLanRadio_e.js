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
	this.RadioID = "";
	this.Frequency = 2;
	this.SupportedModes = new Array();
	this.Channels = new Array();
	this.SupportedSecurity = new SOAPSupportedSecurity();
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
		if(this._Key == "")
		{
			return "";
		}
		return AES_Decrypt128(this._Key);
	},
	get RadiusSecret1(){
		if(this._RadiusSecret1 == "")
		{
			return "";
		}
		return AES_Decrypt128(this._RadiusSecret1);
	},
	get RadiusSecret2(){
		if(this._RadiusSecret2 == "")
		{
			return "";
		}
		return AES_Decrypt128(this._RadiusSecret2);
	},

	set Key(val){
		this._Key = val;
	},
	set RadiusSecret1(val){
		this._RadiusSecret1 = val;
	},
	set RadiusSecret2(val){
		this._RadiusSecret2 = val;
	}
	
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
		if(this.Enabled == false){
			return "";
		}
		return this._Key;
	},
	
	get RadiusSecret1(){
		if((this.Enabled == false)||(this.RadiusIP1 == "")){
			return "";
		}
		return this._RadiusSecret1;
	},

	get RadiusSecret2(){
		if((this.Enabled == false)||(this.RadiusIP2 == "")){
			return "";
		}
		return this._RadiusSecret2;
	},

	set Key(val){
		this._Key = AES_Encrypt128(val);
	},

	set RadiusSecret1(val){
		this._RadiusSecret1 = AES_Encrypt128(val);
	},

	set RadiusSecret2(val){
		this._RadiusSecret2 = AES_Encrypt128(val);
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
}

/**
 * @constructor
 */
function SOAPSetWPSSettings()
{
	this.WPSPBC = false;
	this.WPSPIN = false;
}

/**
 * @constructor
 */
function SOAPOperationModeInfo()
{
	this.RadioID = "";
	this.CurrentOPMode = "";
	this.AvailableOPMode = new Array();
}

/**
 * @constructor
 */
function SOAPOperationModeList()
{
	this.OperationModeInfo = new SOAPOperationModeInfo();
}

/**
 * @constructor
 */
function SOAPGetOperationModeResponse()
{
	var operationModeList = new SOAPOperationModeList();

	this.OperationModeList = $.makeArray(operationModeList);
}

/**
 * @constructor
 */
function SOAPGetAPClientSettings()
{
	this.RadioID ="";
}
/**
 * @constructor
 */
function SOAPGetAPClientSettingsResponse()
{
	this.Enabled = "";
	this.SSID = "";
	this.MacAddress = "";
	this.ChannelWidth = "";
	this.SupportedSecurity = new SOAPSupportedSecurity();
	this.Key = "";
};

// @prototype
SOAPGetAPClientSettingsResponse.prototype =
{	
	get Key(){
		if(this._Key != "") {
			return AES_Decrypt128(this._Key);
		} else {
			return this._Key;
		}
	},

	set Key(val){
		this._Key = val;
	},
	
}

/**
 * @constructor
 */
function SOAPSetAPClientSettings()
{
	this.RadioID = "";
	this.Enabled = true;
	this.SSID = "";
	this.MacAddress = "";
	this.ChannelWidth = 0;
	this.SupportedSecurity = new SOAPSupportedSecurity();
	this.Key = "";
};

// @prototype
SOAPSetAPClientSettings.prototype =
{	
	get Key(){
		return this._Key;
	},

	set Key(val){
		this._Key = AES_Encrypt128(val);
	}
}

/**
 * @constructor
 */
function SOAPSetOperationMode()
{
	this.RadioID = "";
	this.CurrentOPMode = "";
}

/**
 * @constructor
 */
function SOAPSetTriggerWPS()
{
	this.ACTION = "PBC";
	this.PIN = "";
	this.Duration = "";
}

/**
 * @constructor
 */
function SOAPSetTriggerWPSResponse()
{
	this.WaitTime = "";
}


/**
 * @constructor
 */
function SOAPWPSStatus()
{
	this.RadioID = "";
	this.Status = "";
}

/**
 * @constructor
 */
function SOAPWPSStatusLists()
{
	var list = new SOAPWPSStatus();

	this.WPSStatus = $.makeArray(list);
}

/**
 * @constructor
 */
function SOAPGetWPSStatusResponse()
{
	this.WPSStatusLists = new SOAPWPSStatusLists();
}

/**
 * @constructor
 */
function SOAPSetTriggerWirelessSiteSurvey()
{
	this.RadioID = "";
}

/**
 * @constructor
 */
function SOAPSetTriggerWirelessSiteSurveyResponse()
{
	this.WaitTime = "";
}

/**
 * @constructor
 */
function SOAPAPStatInfo()
{
	this.SSID = "";
	this.MacAddress = "";
	this.SupportedSecurity = new SOAPSupportedSecurity();
	this.Channel = "";
	this.SignalStrength = "";
}

/**
 * @constructor
 */
function SOAPAPStatInfoLists()
{
	var apstatInfo = new SOAPAPStatInfo();

	this.APStatInfo = $.makeArray(apstatInfo);
}

/**
 * @constructor
 */
function SOAPGetSiteSurvey()
{
	this.RadioID = "";
}

/**
 * @constructor
 */
function SOAPGetSiteSurveyResponse()
{
	this.APStatInfoLists = new SOAPAPStatInfoLists();
}


function SOAPGetWiFiSONSettingsResponse()
{
	this.WiFiSONStatus = false;
	this.WiFiSONRole = "";
	this.WiFiSONList = new Array();
    this.WiFiSONEnabled = false;
    this.MUMIMOEnabled = true;
    this.BandSteeringEnabled = true;
    this.AirTimeFairnessEnabled = true;
}


function SOAPSetWiFiSONSettings()
{
    this.WiFiSONEnabled = false;
    this.MUMIMOEnabled = true;
    this.BandSteeringEnabled = true;
    this.AirTimeFairnessEnabled = true;
}
