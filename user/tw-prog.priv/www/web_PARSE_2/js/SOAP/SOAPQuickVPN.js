/**
 * @constructor
 */
function SOAPGetQuickVPNSettings()
{
	this.Enabled = false;
	this.Username = "vpn";
  this.Password = "vpn";
	this.PSK = "";
	this.AuthProtocol = "MSCHAPv2";
  this.MPPE = "none";
};

/**
 * @constructor
 */
function SOAPSetQuickVPNSettings()
{
	this.Enabled = false;
	this.Username = "vpn";
  this.Password = "vpn";
	this.PSK = "";
	this.AuthProtocol = "MSCHAPv2";
  this.MPPE = "none";
};

/**
 * @constructor
 */
function SOAPGetQuickVPNProfileLink()
{
	this.ProfileLink = "";
};

/**
 * @constructor
 */
function SOAPSetQuickVPNProfileLink()
{
		this.ProfileLink = "";
};
