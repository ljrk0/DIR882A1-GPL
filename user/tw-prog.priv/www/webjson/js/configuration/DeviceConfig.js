/*	===================================================	*/
/*				Device Default Configuration			*/
/*	Here you can define default setting for this device	*/
/*	===================================================	*/
var GUIVersion;
var WEPMode_24GHz;
var WEPMode_5GHz;
var WPAMode_24GHz;
var WPAMode_5GHz;
var IsSupport5GHz;
var IsSupport11ac_5GHz;
var IsSupportGuestZone;
var IsSupportAPClient;
var IsSupportRepeater;
var DebugMode;
var DemoMode;

/*	===================================================	*/
/*	Debug Mode & Demo Mode								*/
/*	Enabled: [1]	Disabled: [0]						*/
DebugMode = 0;
DemoMode = 0;
DemoZWave = 0;
/*	=================================================== */
/*	===================================================	*/
/*	GUI Version											*/
GUIVersion	=	"2013/12/30 03:24PM";
/*	===================================================	*/

/*	===================================================	*/
/*	Support Wireless Mode								*/
/*	Support / Enabled: [1]	Not support / Disabled: [0]	*/
IsSupportGuestZone =	1;
IsSupportAPClient =	1;
IsSupportRepeater =	0;
/*	===================================================	*/

/*	===================================================	*/
/*	Support Wireless 5GHz								*/
/*	Support / Enabled: [1]	Not support / Disabled: [0]	*/
IsSupport5GHz	=	1;
/*	===================================================	*/

/*	===================================================	*/
/*	2.4GHz SupportMode: [b],[g],[n],[bg],[gn],[bgn]		*/
/*	5GHz SupportMode:	[a],[n],[an]					*/
/*	Separated by commas, for example: "b,g,n"			*/
WEPMode_24GHz	=	"gn,bgn";
WEPMode_5GHz	=	"n,an";
WPAMode_24GHz	=	"n,gn,bgn";
WPAMode_5GHz	=	"a,n,an";
/*	===================================================	*/

/*	===================================================	*/
/*	If support 11ac, value = 1, if not, value = 0.		*/
/*	Support Mode:	[ac],[acn],[acna]					*/
/*	Support Cahnnel Width:	[80],[160]					*/
IsSupport11ac_5GHz		=	1;
Support11acMode_WPA		=	"ac,acn,acna";
Support11acMode_WEP		=	"acna";
Support11acChannelWidth	=	"80";
/*	===================================================	*/