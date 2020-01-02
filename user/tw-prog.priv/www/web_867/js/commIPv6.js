
function COMM_IPv6ADDR2INT(addr)
{
	//init
	var v6Array = new Array(8);
	for(var i = 0; i < v6Array.length; i++)
	{
		v6Array[i] = 0;
	}

	//split ::
	var addrArray = addr.split("::");
	
	//split :
	var upperAddr = addrArray[0];
	var upperAddrArray = upperAddr.split(":");
	for( var i = 0; i < upperAddrArray.length; i++)
	{
		v6Array[i] = parseInt(upperAddrArray[i], 16);
	}
	
	if(addrArray.length > 1)
	{
		var lowerAddr = addrArray[1];
		var lowerAddrArray = lowerAddr.split(":");
		for( var i = 0; i < lowerAddrArray.length; i++)
		{
			v6Array[v6Array.length -1 - i] = parseInt(lowerAddrArray[i], 16);
		}
	}

	return v6Array;
}

/* COMM_IPv4NETWORK("192.168.1.1", 24) -> "192.168.1.0" */
function COMM_IPv6NETWORK(addr, prefixlen)
{
	var v6Array = COMM_IPv6ADDR2INT(addr);
	
	// prefix length
	var quotient = Math.floor(prefixlen/16);
	var remainder = prefixlen%16;
	
	if(remainder > 0)
	{
		//assume prefix length must be product of 4
		var mask = Math.pow(2, 16) - Math.pow(2, (16 - remainder));
		
		v6Array[quotient] &= mask;
		quotient++;
	}
	
	// set host domain to zero
	for(var i = quotient; i < v6Array.length; i++)
	{
		v6Array[i] = 0;
	}

	return v6Array;
}

/* Check ipv6 address format. */
function COMM_ValidV6Format(ipstr, check_addr)
{
	var vals = ipstr.split(":");
	
	for(var i = 0; i<vals.length; i++)
	{
		vals[i] = parseInt(vals[i], 16);
	}
	
	// block all ip and loopback ip
	if(vals[0] == 0)
	{
		return false;
	}
	
	// block multicast
	if((vals[0] >= 0xff00)&&(vals[0] <= 0xff0f))
	{
		return false;
	}
	
	if(check_addr)
	{
		// block link local and 6to4
		if((vals[0] == 0xfe80)||(vals[0] == 0x2002))
		{
			return false;
		}
	}
	
	return true;
}

function COMM_V6Format(value)
{
	var ValidIpAddressRegex = /^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})))))(%.+)?\s*$/;
	var returnVal = true;
	// grammar check
	if (ValidIpAddressRegex.test(value) == false)
	{
		//alert("grammar check fail");
		return false;
	}
	return returnVal;
}

/* Check ipv6 address format. */
function COMM_ValidV6Prefix(ipstr, check_llr)
{
	var vals = ipstr.split(":");
	
	for(var i = 0; i<vals.length; i++)
	{
		vals[i] = parseInt(vals[i], 16);
	}
	
	// block all ip and loopback ip
	if(vals[0] == 0)
	{
		return false;
	}
	
	// block multicast
	if((vals[0] >= 0xff00)&&(vals[0] <= 0xff0f))
	{
		return false;
	}
	
	if(check_llr)
	{
		// block link local and 6to4
		if((vals[0] == 0xfe80)||(vals[0] == 0x2002))
		{
			return false;
		}
	}
	
	return true;
}

function BoolToEnable(b){
	if(b)
	{
		return "Enable";
	}
	return "Disable";
}

function getPrefix(addr, prefixlen)
{
	var v6Array = COMM_IPv6ADDR2INT(addr);
	var prefixArray = new Array(8);
	var p = prefixlen;
	var prefixP = 0;
	var outputStr = ":";

	for(var i = 0; i < prefixArray.length; i++)
	{
		prefixArray[i] = 0;
	}

	while(p > 16)
	{
		prefixArray[prefixP] = v6Array[prefixP];
		p -= 16;
		prefixP++;
	}
	if(p > 0)
	{
		prefixArray[prefixP] = (v6Array[prefixP] >> (16-p)) << (16-p);
	}

	var zero = true;
	for(var i = prefixArray.length-1; i >= 0; i--)
	{
		if((prefixArray[i] == 0)&&(zero))
		{
			continue;
		}
		else
		{
			zero = false;
			outputStr = prefixArray[i].toString(16)+":"+outputStr;
		}
	}
	return outputStr;
}

function get6to4addr(v4addr){
	var addrarray = v4addr.split(".");
	var v4array = new Array(4);

	if(addrarray.length != 4)
		return "";

	for(var i = 0; i < addrarray.length; i++)
	{
		v4array[i] = parseInt(addrarray[i], 10);
	}

	var v6addr = "2002:" + v4array[0].toString(16) + v4array[1].toString(16) + ":" + v4array[2].toString(16)+v4array[3].toString(16) + "::1";
	return v6addr;
}