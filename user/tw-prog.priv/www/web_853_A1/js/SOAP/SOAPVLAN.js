/**
 * @constructor
 */
function SOAPGetVLANSettings()
{
	this.Enabled = false;
	this.PriorityEnabled = false;
	this.VLANIDInfoLists = new SOAPVLANIDInfoLists();
	this.VLANRuleInfoLists = new SOAPVLANRuleInfoLists();
};

/**
 * @constructor
 */
function SOAPSetVLANSettings()
{
	this.Enabled = false;
	this.PriorityEnabled = false;
	this.VLANIDInfoLists = new SOAPVLANIDInfoLists();
	this.VLANRuleInfoLists = new SOAPVLANRuleInfoLists();
};

/**
 * @constructor
 */
function SOAPVLANID()
{
	this.Service = "";
	this.VID = 3;
	this.Priority = 0;
};

/**
 * @constructor
 */
function SOAPVLANIDInfoLists()
{
	var vlanId = new SOAPVLANID();

	this.VLANID = $.makeArray(vlanId);
};

/**
 * @constructor
 */
function SOAPVLANRule()
{
	this.Interface = "";
	this.ServiceMapping = "";
};

/**
 * @constructor
 */
function SOAPVLANRuleInfoLists()
{
	var vlanRule = new SOAPVLANRule();

	this.VLANRule = $.makeArray(vlanRule);
};

