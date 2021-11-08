

//data list
function Datalist()
{
	this.list = new Array();
	this.maxrowid = 0;

	//Add tag
	var newdata = new Data("", "", "", "New", "", "", "","","");
	this.push(newdata);
}

Datalist.prototype =
{
	get length(){
		return this.list.length;
	},
	
	set length(val){
		if(parseInt(val, 10) >= 0)
			this.list.length = parseInt(val,10);
	},
	get extenderLength(){
		var extenderCount = 0;
		for(var i in this.list)
		{
			if(this.list[i].Type.indexOf("Extender") > 0 || this.list[i].Type.indexOf("AP") > 0 || this.list[i].Type.indexOf("PowerLine") > 0)
			{
				extenderCount++;
			}
		}
		return extenderCount;
	},

	get connectedLength(){
		var macfilterCount = 0;
		var extenderCount = 0;
		var offline = 1;	//include add tag
		for(var i in this.list)
		{
			if(this.list[i].Type == "Blocked")
			{
				macfilterCount++;
			}
			if(this.list[i].Type.toLowerCase() == "offline")
			{
				offline++;
			}

			if(this.list[i].Type.indexOf("Extender") > 0 || this.list[i].Type.indexOf("AP") > 0 || this.list[i].Type.indexOf("PowerLine") > 0)
			{
				extenderCount++;
			}
		}
		return this.list.length - macfilterCount - extenderCount - offline;
	}

}

Datalist.prototype.prepareUpdate = function(){
	for(var num in this.list)
	{
		this.list[num].Live = false;
	}
}

Datalist.prototype.getData = function(rowid){
	var i;
	var data;
	
	for(i = 0; i < this.list.length; i++)
	{
		data = this.list[i];
		if(data.rowid == rowid)
		{
			break;
		}
	}
	
	//assume data exist
	return data;
}

Datalist.prototype.getDataRowNumByMac = function(mac){
	var i;
	var data = null;
	var rowNum = null;
	var macLower = mac.toLowerCase();
	
	for(i = 0; i < this.list.length; i++)
	{
		data = this.list[i];
		if(data.MacAddress.toLowerCase() == macLower)
		{
			rowNum = i;
			break;
		}
	}
	
	//assume data exist
	return rowNum;
}

Datalist.prototype.getRowNum = function(rowid){
	var rowNum = 0;
	for(rowNum = 0; rowNum < this.list.length; rowNum++)
	{
		if(rowid == this.list[rowNum].rowid)
		{
			break;
		}
	}
	return rowNum;
}

Datalist.prototype.editData = function(id, newdata){
	var obj = this.list[this.getRowNum(id)];

	obj.IPv4Address = newdata.IPv4Address;
	obj.IPv6Address = newdata.IPv6Address;
	obj.Type = newdata.Type;
	obj.DeviceName = newdata.DeviceName;
	obj.NickName = newdata.NickName;
	obj.ReserveIP = newdata.ReserveIP;
	obj.Vendor = newdata.Vendor;
	obj.ScheduleName = newdata.ScheduleName;
	obj.MacfilterEnabled = newdata.MacfilterEnabled;
	obj.Status = newdata.Status;
	obj.isAllowList = newdata.isAllowList;
	obj.isMacfilterEnabled = newdata.isMacfilterEnabled;

	obj.Live = true;

	return true;
}

Datalist.prototype.deleteData = function(id){
	var rowNum = this.getRowNum(id);
	this.list.splice(rowNum, 1);
}

Datalist.prototype.push = function(data){
	data.setRowid(this.maxrowid);
	this.list.push(data);
	
	this.maxrowid++;

	//sort
	this.list.sort(function(a,b){

		function getPriority(type)
		{
			if(type == "LAN")
			{
				return 0;
			}
			else if(type.toUpperCase().indexOf("WIFI_") >= 0)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		};

		return getPriority(a.Type) - getPriority(b.Type);
	});

	return true;
}

Datalist.prototype.clean = function(){
	for(var num in this.list)
	{
		obj = this.list[num];
		if(obj.Live == false)
		{
			//obj.removeFromHTML();
			this.list.splice(num, 1);
		}
	}
}

//constructor
function Data(macAddress, ipv4Address, ipv6Address, type, deviceName, nickName, reserveIP, extenderMacAddress, signalStrength){
	this.MacAddress = macAddress;
	this.IPv4Address = ipv4Address;
	this.IPv6Address = ipv6Address;
	this.Type = type;
	this.DeviceName = deviceName;
	this.NickName = nickName;
	this.ReserveIP = reserveIP;
	
	this.Vendor = getVendor(macAddress);
	
	//mac filter
	this.ScheduleName = "Always";
	this.MacfilterEnabled = false;
	this.isAllowList = false;
	this.isMacfilterEnabled = true;
	this.Status = false;

	//extender
	this.ExtenderMacAddress = extenderMacAddress;
	this.SignalStrength = signalStrength;
	this.Live = true;
}


Data.prototype = 
{
	//method
	get NickName(){
		if(this._NickName == "")
			return this.DeviceName;
		
		return this._NickName;
	},
	
	set NickName(val){
		this._NickName = val;
		if(val == this.DeviceName)
			this._NickName = "";
	},
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
	},
	
	setRowid : function(rowid)
	{
		this.rowid = rowid;
	},

	showExtenderInfo: function()
	{
		var self = this;
		var name = self.NickName;
		var macFilterString = "";
		var fontColor;
		var cssBackGround = "link_IconE_Allow";	//default type = LAN
		var uplink = "uplink_IconAP";
		var signal = "signal_IconHigh";
		var nameCSS = "client_Name";
		var ipv4Address = self.IPv4Address;
		var vendor = self.Vendor;

		if (typeof(self.MacAddress) == "undefined" || self.MacAddress.length == 0)
		{
			return "";
		}
		if (name == "")
		{
			name = "Unknown";
		}

		var objSignal = parseInt(self.SignalStrength);
		if (objSignal > 85)
			signal = "signal_IconMax";
		else if (objSignal > 50 && objSignal <= 85)
			signal = "signal_IconHigh";
		else if (objSignal > 20 && objSignal <= 50)
			signal = "signal_IconLow";
		else
            signal = "signal_IconMin";

        var objType = self.Type.toUpperCase();
		if (objType == "WiFi_5G_Extender")
		{
			uplink = "uplink_Icon5G";
			cssBackGround = "link_IconW5G";
		} else if (objType == "WiFi_2.4G_Extender") {
            uplink = "uplink_Icon";
            cssBackGround = "link_IconW";
		} else if (objType == "PowerLine") {
            uplink = "uplink_IconPowerLine";
            cssBackGround = "link_IconPowerLine";
		} else if (objType == "WiFi_2.4G_AP" || objType == "WiFi_5G_AP") {
            uplink = "uplink_IconAP";
            cssBackGround = "link_IconExtender";
		}

		if(vendor.length > 22)
		{
			vendor = vendor.substr(0, 19) + "...";
		}

		var outputString;
        if (objType == "WiFi_5G_Extender" || objType == "WiFi_2.4G_Extender") {
            outputString = "<td>"
                + '<div class ="client_Tag" style ="cursor:default">'
                + '<div class ="' + cssBackGround + '"></div>'
                + '<div class ="client_Info">'
                + '<div class ="' + nameCSS + '">' + name + '</div>'
                + '<div class ="client_Vender" title="' + vendor + '" style="overflow:hidden;text-overflow:ellipsis;width:100px;white-space:nowrap;">' + vendor + '</div>'
                + '<div class ="client_IPv4Address">' + ipv4Address + '</div>'
                + '<div class ="client_IPv6Address" id="IPv6Address_Client_' + self.rowid + '" title="' + self.IPv6Address + '">' + self.IPv6Address + '</div>'
                + '<div class ="' + uplink + '"></div>'
                + '<div class ="' + signal + '"></div>'
                + "</div>"
                + "</div>"
                + "</td>";
        } else {
            outputString = "<td>"
                + '<div class ="client_Tag" style ="cursor:default">'
                + '<div class ="' + cssBackGround + '"></div>'
                + '<div class ="client_Info">'
                + '<div class ="client_IPv4Address">' + ipv4Address + '</div>'
                + '<div class ="client_IPv6Address" id="IPv6Address_Client_' + self.rowid + '" title="' + self.IPv6Address + '">' + self.IPv6Address + '</div>'
                + '<div class ="' + uplink + '"></div>'
                + '<div class ="' + signal + '"></div>'
                + "</div>"
                + "</div>"
                + "</td>";
		}
		return outputString;
	},
	
	showClientInfo: function()
	{
		var self = this;
		var name = self.NickName;
		var macFilterString = "";
		var fontColor;
		var cssBackGround = "link_IconE_";	//default type = LAN
		var nameCSS = "client_Name";
		var ipv4Address = self.IPv4Address;
		var vendor = self.Vendor;

		//Add tag
		if(self.Type == "New")
		{
			var outputString = "<td>"
				   + '<div class ="client_add_Tag" onclick ="addData()" style ="cursor:pointer">'
				   + "</div>"
				   +"</td>";			
			return outputString;
		}
			
		if (typeof(self.MacAddress) == "undefined" || self.MacAddress.length == 0)
		{
			return "";
		}
		if (name == "")
		{
			name = "Unknown";
		}
			
		var objType = self.Type.toUpperCase();
		
		if (objType == "WIFI_2.4G" || objType == "WIFI_2.4G_GUEST")
		{
			cssBackGround = "link_IconW_";
		} else if (objType == "WIFI_5G" || objType == "WIFI_5G_GUEST"){
			cssBackGround = "link_IconW5G_";
			
		}

		else if(objType == "OFFLINE")
		{
			cssBackGround = "link_Offline_";
			nameCSS += " client_Name_Offline";
			ipv4Address = self.ReserveIP;
		}
		
		if(self.MacfilterEnabled)
		{
			macFilterString = I18N("j", "Enabled");
			fontColor = "#FF0000";
			if((self.Type == "Blocked")||(self.Status))
			{
				cssBackGround += "Block";
			}
			else
			{
				cssBackGround += "Allow";
			}
		}
		else
		{
			macFilterString = I18N("j", "Disabled");
			fontColor = "#000000";
			cssBackGround += "Allow";
		}

		/*
		if(vendor.length > 22)
		{
			vendor = vendor.substr(0, 19) + "...";
		}
		*/

		var outputString = "<td>"
			   + '<div class ="client_Tag" style ="cursor:default">'
			   + '<div class ="' + cssBackGround + '"></div>'
			   + '<div class ="client_Info">'
			   + '<div class ="' + nameCSS + '">' + HTMLEncode(name) + '</div>'
			   + '<div class ="client_EditImage" onclick ="editData(' + self.rowid + ')" style ="cursor:pointer"></div>'
			   + '<div class ="client_Vender" title="'+vendor+'" style="overflow:hidden;text-overflow:ellipsis;width:100px;white-space:nowrap;">' + vendor + '</div>'
			   + '<div class ="client_IPv4Address">' + ipv4Address + '</div>'
			   + '<div class ="client_IPv6Address" id="IPv6Address_Client_' + self.rowid + '" title="'+ self.IPv6Address +'">' + self.IPv6Address + '</div>'
			   + '<div class ="client_Access">' + I18N("j", "Parental Control") + ': <font color ="' + fontColor + '">' +macFilterString + '</font></div>'
			   + "</div>"
			   + "</div>"
			   +"</td>";
			
		return outputString;
	}

}

var tmpXML_ScheduleCnt;
var Client_Type = "Host";

var datalist = new Datalist();
var macList = "";
var editType = true;	//edit:true, add:false
var editID = "";

function LoadMACList()
{
	$.ajax({
		url: "/js/MacList.json",
		type: "GET",
		dataType: "json",
		success: function(data) {
			macList = data;
		},
	});
}

function Base92(input)
{
	var BASE = 92;

	function getCharCode(i)
	{
		if((i+35) == 92)	//skip '\'  (ASCII 92)
			return '!';
		return String.fromCharCode(i+35);
	}

	var Q = Math.floor(input/BASE);
	var R = input%BASE;
	var str = "";

	if(Q >= BASE)
	{
		str += Base92(Q);
	}
	else
	{
		str += getCharCode(Q);		
	}

	str += getCharCode(R);
	return str;
}
function getVendor(mac)
{
	var unknown = "Unknown Vendor";
	var str;

	if((typeof macList == 'undefined')||(macList == ""))
		return unknown;
	try{
		var macUnit = mac.split(':');
		var L1 = Base92(parseInt(macUnit[0], 16));
		var L2 = Base92(parseInt(macUnit[1]+macUnit[2], 16));
		str = macList[L1][L2];
		if(typeof str == 'undefined')
			str = unknown;
	}
	catch(e)
	{
		str = unknown;
	}

	return decode_char_text(str);
}

function Get_ClientInfo()
{
	var deferred = $.Deferred();
	var clientInfo = new SOAPGetClientInfoResponse();
	var macfilterInfo = new SOAPGetMACFilters2Response();
	var soapAction = new SOAPAction();
	var clientinfo_query = soapAction.sendSOAPAction("GetClientInfo", null, clientInfo);
	var macfilter_query = null;

	if(currentState == "clientInfo")	//get client details
	{
		macfilter_query = soapAction.sendSOAPAction("GetMACFilters2", null, macfilterInfo);
	}
	//type Host or Guest
	$.when(clientinfo_query, macfilter_query).done(function(obj)
	{
		//update client
		datalist.prepareUpdate();
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];
			if(obj.Type == "Blocked")
				obj.Live = true;
			if(obj.Type == "New")
				obj.Live = true;
		}
		for(var clientUnit in clientInfo.ClientInfoLists.ClientInfo)
		{
			var client = clientInfo.ClientInfoLists.ClientInfo[clientUnit];
			var data = new Data(client.MacAddress, client.IPv4Address, client.IPv6Address, client.Type, client.DeviceName, decode_char_text(client.NickName), client.ReserveIP, client.ExtenderMacAddress, client.SignalStrength);
			var rowNum = datalist.getDataRowNumByMac(client.MacAddress);
			if(rowNum != null)	//update
			{
				datalist.editData(datalist.list[rowNum].rowid, data);
			}
			else	//new push
			{
				datalist.push(data);
			}

		}

		//Remainingnumber
		Remainingnumber=24;
		for(var i=0;i<datalist.list.length;i++){
			var a = datalist.list[i].ReserveIP;
			if(a != ""){
                Remainingnumber--;
			}
		}

		//Remainingnumber control
		$('.Remainingtext').find('span').text(Remainingnumber);
		if(Remainingnumber<=0){
			Remainingnumber=0;
			if($('#enableReserveIP').is(':checked')){
				$('#enableReserveIP').attr('disabled',false);
			}else{
				$('#enableReserveIP').attr('disabled',true);
			}
		}
				

		datalist.clean();
		
		if(currentState == "clientInfo")
		{
			var hasMacfilter = false;

			//update macfilter
			datalist.prepareUpdate();
			for(var row in datalist.list)
			{
				var obj = datalist.list[row];
				if(obj.Type != "Blocked")
					obj.Live = true;
			}

			// get isallowlist value from hnap
			datalist.isAllowList = macfilterInfo.IsAllowList;

			for(var macfilterUnit in macfilterInfo.MACList.MACInfo)
			{
				var macfilter = macfilterInfo.MACList.MACInfo[macfilterUnit];

				//search client list
				var rowNum = datalist.getDataRowNumByMac(macfilter.MacAddress);
				if(rowNum != null)
				{
					datalist.list[rowNum].MacfilterEnabled = true;
					datalist.list[rowNum].ScheduleName = macfilter.ScheduleName;
					if(macfilter.Status == "false")
					{
						datalist.list[rowNum].Status = true;
						datalist.list[rowNum].Type = "Blocked";
						hasMacfilter = true;
					}
					datalist.list[rowNum].Live = true;
				}
				else
				{
					var data = new Data(macfilter.MacAddress, "", "", "LAN", decode_char_text(macfilter.DeviceName), "", "", "", "");
					data.MacfilterEnabled = true;
					data.ScheduleName = macfilter.ScheduleName;
					data.Type = "Blocked";
					if(macfilter.Status == "true")
						data.Status = true;
					datalist.push(data);
					hasMacfilter = true;
				}
			}
			datalist.clean();
			//showClientTypeBtn("block", hasMacfilter);
			showClientLists(Client_Type);
		}
		//show count
		//$("#Total_ConnectedClients").html(clientInfo.ClientInfoLists.ClientInfo.length);
		$("#Total_ConnectedClients").html(datalist.connectedLength);
		$("#Total_Extenders").html(datalist.extenderLength);

		deferred.resolve();
	});
	
	return deferred.promise();
}

function nextExtender(currentIndex) {
	if(datalist.list.length > 0) {
		var extenderItemsHTML;
		var mac;
		var obj = datalist.list[currentIndex+1];
        var column = 0;
        var clientItemsHTML = '<table class="block" border="0" ondragstart="return false" onselectstart="return false" width="900px">'
            + '<tbody><tr style="height:120px">';

        if(obj.Type.indexOf("Extender") > 0 || obj.Type.indexOf("AP") > 0 || obj.Type.indexOf("PowerLine") > 0)
		{
			var name = obj.DeviceName;
            mac = obj.ExtenderMacAddress;

            if (name == "")
			{
				name = "Unknown";
			} else {
                if (mac != "" && mac != undefined) {
                    var macStr = mac.split(":");
                    name += macStr[4] + macStr[5];
                }
            }

			if((currentIndex + 1) < (datalist.list.length - 2)) {
				extenderItemsHTML = 
				'<img src="image/extender_prev.png" alt="" width="23" height="23" style="padding-right: 5px;" onclick="prevExtender('+ (currentIndex+1) +')"/>'
				+ '<label>' + name + '</label>'
				+'<img src="image/extender_next.png" alt="" width="23" height="23" style="padding-left: 5px;" onclick="nextExtender('+ (currentIndex+1) +')"/>';
				$("#extenderName").html(extenderItemsHTML);
			} else if ((currentIndex + 1) == (datalist.list.length - 2)){
				extenderItemsHTML = 
				'<img src="image/extender_prev.png" alt="" width="23" height="23" style="padding-right: 5px;" onclick="prevExtender('+ (currentIndex+1) +')"/>'
				+ '<label>' + name + '</label>'
				$("#extenderName").html(extenderItemsHTML);
			}

            column++;
            clientItemsHTML += obj.showExtenderInfo();

            if(column % 3 == 0)
            {
                clientItemsHTML += "</tr><tr style='height:120px'>";
            }
		}

		for(var row in datalist.list)
		{
			if ( row == (currentIndex+1) )
				continue;

			var obj = datalist.list[row];
			if((obj.Type.indexOf("Extender") > 0 || obj.Type.indexOf("AP") > 0 || obj.Type.indexOf("PowerLine") > 0) && (mac != "" && mac == obj.ExtenderMacAddress))
			{
				column++;
				clientItemsHTML += obj.showExtenderInfo();

				if(column % 3 == 0)
				{
					clientItemsHTML += "</tr><tr style='height:120px'>";
				}
			}
		}
		clientItemsHTML += "</tr></tbody>";

		$("#Extender_Group").html(clientItemsHTML);
		
		if(column>0)
		{
			$("#Extender_Info").hide();
		}
		else
		{
			$("#Extender_Info").show();
		}
		$("#Extender_Group").css("display", "table-row");
	}
}

function prevExtender(currentIndex) {
	if(datalist.list.length > 0) {
		var extenderItemsHTML;
		var mac;
		var obj = datalist.list[currentIndex-1];
        var column = 0;
        var clientItemsHTML = '<table class="block" border="0" ondragstart="return false" onselectstart="return false" width="900px">'
            + '<tbody><tr style="height:120px">';

		if(obj.Type.indexOf("Extender") > 0 || obj.Type.indexOf("AP") > 0 || obj.Type.indexOf("PowerLine") > 0)
		{
			var name = obj.DeviceName;
            mac = obj.ExtenderMacAddress;

            if (name == "")
			{
				name = "Unknown";
			} else {
                if (mac != "" && mac != undefined) {
                    var macStr = mac.split(":");
                    name += macStr[4] + macStr[5];
                }
			}

			if ((currentIndex - 1) > 0) {
				extenderItemsHTML = 
				'<img src="image/extender_prev.png" alt="" width="23" height="23" style="padding-right: 5px;" onclick="prevExtender('+ (currentIndex-1) +')"/>'
				+ '<label>' + name + '</label>'
				+'<img src="image/extender_next.png" alt="" width="23" height="23" style="padding-left: 5px;" onclick="nextExtender('+ (currentIndex-1) +')"/>';
				$("#extenderName").html(extenderItemsHTML);
			} else if ((currentIndex - 1) == 0) {
				extenderItemsHTML = 
			 	'<label>' + name + '</label>'
				+'<img src="image/extender_next.png" alt="" width="23" height="23" style="padding-left: 5px;" onclick="nextExtender('+ (currentIndex-1) +')"/>';
				$("#extenderName").html(extenderItemsHTML);
			}

            column++;
            clientItemsHTML += obj.showExtenderInfo();

            if(column % 3 == 0)
            {
                clientItemsHTML += "</tr><tr style='height:120px'>";
            }
		}

		for(var row in datalist.list)
		{
			if (row == (currentIndex - 1))
				continue;

			var obj = datalist.list[row];
			if((obj.Type.indexOf("Extender") > 0 || obj.Type.indexOf("AP") > 0 || obj.Type.indexOf("PowerLine") > 0) && (mac != "" && mac == obj.ExtenderMacAddress))
			{
				column++;
				clientItemsHTML += obj.showExtenderInfo();

				if(column % 3 == 0)
				{
					clientItemsHTML += "</tr><tr style='height:120px'>";
				}
			}
		}
		clientItemsHTML += "</tr></tbody>";

		$("#Extender_Group").html(clientItemsHTML);
		
		if(column>0)
		{
			$("#Extender_Info").hide();
		}
		else
		{
			$("#Extender_Info").show();
		}
		$("#Extender_Group").css("display", "table-row");
	}
}

function getExtenderList() 
{
	var extenderItemsHTML;
	var mac = "";
    var column = 0;
    var clientItemsHTML = '<table class="block" border="0" ondragstart="return false" onselectstart="return false" width="900px">'
        + '<tbody><tr style="height:120px">';

	if(datalist.list.length > 0) {
		var obj = datalist.list[0];

		if(obj.Type.indexOf("Extender") > 0 || obj.Type.indexOf("AP") > 0 || obj.Type.indexOf("PowerLine") > 0)
		{
			var name = obj.DeviceName;
			mac = obj.ExtenderMacAddress;

			if (name == "")
			{
				name = "Unknown";
			} else {
                if (mac != "" && mac != undefined) {
                	var macStr = mac.split(":");
					name += macStr[4] + macStr[5];
                }
			}

			if(obj != null) {
				if(datalist.list.length > 1) {
					extenderItemsHTML = 
					//'<img src="image/extender_prev.png" alt="" width="23" height="23" onclick="prevExtender()"/>'
					'<label>' + name + '</label>'
					+'<img src="image/extender_next.png" alt="" width="23" height="23" style="padding-left: 5px;" onclick="nextExtender(0)"/>';
				} else {
					extenderItemsHTML = 
					'<label>' + name + '</label>';
				}
				$("#extenderName").html(extenderItemsHTML);

                column++;
                clientItemsHTML += obj.showExtenderInfo();

                if(column % 3 == 0)
                {
                    clientItemsHTML += "</tr><tr style='height:120px'>";
                }
			}
		}
	}

	for(var row in datalist.list)
	{
		if (row == 0)
			continue;

		var obj = datalist.list[row];
		if((obj.Type.indexOf("Extender") > 0 || obj.Type.indexOf("AP") > 0 || obj.Type.indexOf("PowerLine") > 0) && (mac != "" && mac == obj.ExtenderMacAddress))
		{
			column++;
			clientItemsHTML += obj.showExtenderInfo();

			if(column % 3 == 0)
			{
				clientItemsHTML += "</tr><tr style='height:120px'>";
			}
		}
	}
	clientItemsHTML += "</tr></tbody>";

	$("#Extender_Group").html(clientItemsHTML);
	
	if (column > 0)
	{
		$("#Extender_Info").hide();
	}
	else
	{
		$("#Extender_Info").show();
	}
	$("#Extender_Group").css("display", "table-row");
}

function showClientLists(type)
{
	var column = 0;
	var clientItemsHTML = '<table class="block" border="0" ondragstart="return false" onselectstart="return false" width="900px">'
	+ '<tbody><tr style="height:120px">';
	for(var row in datalist.list)
	{
		var obj = datalist.list[row];
		var objIpv4  = obj.IPv4Address.toString();
		var curTypeStr = objIpv4.substring(0,9);
		if(type == "Guest")
		{
			if (curTypeStr != "192.168.7")
			{
				continue;
			}
		}
		else if(type == "Host")
		{
			if (curTypeStr == "192.168.7")
			{
				continue;
			}
			if (obj.Type == "Blocked")
			{
				continue;
			}
		}
		else if(type == "Blocked")
		{
			if(obj.Type != "Blocked")
			{
				continue;
			}
		}
		column++;

		clientItemsHTML += obj.showClientInfo();

		if(column % 3 == 0)
		{
			clientItemsHTML += "</tr><tr style='height:120px'>";
		}
	}
	clientItemsHTML += "</tr></tbody>";

	$("#Client_Group").html(clientItemsHTML);
	
	if(column>0)
	{
		$("#Client_Info").hide();
	}
	else if(type == "Blocked")	//no Blocked list, maybe all deleted
	{
		//showClientTypeBtn("block", false);
		//showClientLists("Host");
	}
	else
	{
		$("#Client_Info").show();
	}
	$("#Client_Group").css("display", "table-row");
}

function addData(isDevice)
{
	changeTimeoutAction();

	$("#popTitle").html(I18N("j", "Add Rule"));
	$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);

	$("#show_Vendor").hide();
	$("#client_MACAddress").hide();
	$("#show_editMac").show();
	$("#show_IPAdr").hide();

	$("#show_IPAdrReserve").hide();
	$("#enableReserveIP").prop("checked", false);

	if (isDevice == "true") {
        $("#show_Schedule").show();
        $("#show_select_editMac").show();
    } else {
        $("#enableAccess").prop("checked", false);
        $("#show_Schedule").hide();
    }

	$("#editPop").show();

	editType = false;
	editID = "";
}

function editData(id)
{
	changeTimeoutAction();
	$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);

	$("#popTitle").html(I18N("j", "Edit Rule"));

	$("#show_Vendor").show();
	$("#client_MACAddress").show();
	$("#show_editMac").hide();
	$("#show_IPAdr").show();
	
	//insert data to pop form
	var data = datalist.getData(id);
	var ipString = data.IPv4Address;

	document.getElementById("client_IPAdrReserve").value = data.ReserveIP;
	document.getElementById("client_MACAddress").innerHTML = data.MacAddress;
	$("#client_Name").val(XMLDecode(data.NickName));

	if(ipString == "")
	{
		ipString = I18N("j", "Not Available");
	}
	$("#client_IPAdr").html(ipString);
		
	if(data.ReserveIP == ""){
		document.getElementById("show_IPAdrReserve").style.display = "none";
		document.getElementById("show_IPAdr").style.display = "table-row";
		$("#enableReserveIP").prop("checked", false);
	}
	else{
		document.getElementById("show_IPAdrReserve").style.display = "table-row";
		document.getElementById("show_IPAdr").style.display = "none";
		$("#enableReserveIP").prop("checked", true);
	}

	if (data.Vendor != "")
	{
		document.getElementById("client_Vendor").innerHTML = data.Vendor;
	}
	else
	{
		document.getElementById("client_Vendor").innerHTML = "N/A";
	}

	if(data.MacfilterEnabled)
	{
		$("#schedule").selectbox('detach');
		$("#schedule").val(data.ScheduleName);
		$("#schedule").selectbox('attach');
		$("#enableAccess").prop("checked", true);
		$("#show_Schedule").show();
	}
	else
	{
	    $("#schedule").selectbox('detach');
        $("#schedule").val("Always");
        $("#schedule").selectbox('attach');
		$("#enableAccess").prop("checked", false);
		$("#show_Schedule").hide();
	}


	$("#editPop").show();
	editType = true;
	editID = id;
}

function closeEditPOP()
{
	changeTimeoutAction();
	var reverseIP_enabled = $("#enableReserveIP").prop("checked");
	var reserveIP =$("#client_IPAdrReserve").val();
	var name = $("#client_Name").val();
	var macfilter_enabled = $("#enableAccess").prop("checked");
	var schedule = $("#schedule").val();
	
	$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);
	
	var data = datalist.getData(editID);
	data.NickName = name;
	
	if(reverseIP_enabled && (reserveIP != ""))
	{
		data.ReserveIP = reserveIP;
	}
	else
	{
		data.ReserveIP = "";
	}
	
	data.MacfilterEnabled = macfilter_enabled;
	data.isAllowList = datalist.isAllowList;
	if(macfilter_enabled)
	{
		data.ScheduleName = schedule;
	}
	else
	{
		data.ScheduleName = "";
	}
	datalist.editData(data.rowid,data);
	
	//store client into Device
	var soapAction = new SOAPAction();
	var setClientInfo = new SOAPSetClientInfo();
	var client = new SOAPClientInfo();
	client.MacAddress = data.MacAddress;
	client.NickName = encode_char_text(data.NickName);
	client.ReserveIP = data.ReserveIP;
	setClientInfo.ClientInfoLists.push(client);
	var setClientResult = soapAction.sendSOAPAction("SetClientInfo", setClientInfo, null);

	setClientResult.done(function(obj){
		if(obj.SetClientInfoResult == "ERROR_RESERVEIP_CONFLICT")
		{
			alert(I18N("j", "IP address cannot be the same."));
			$("#check_btn").attr("class", "styled_button_s").prop("disabled", false);
		}
		else
		{
			//set client info OK, store mac filter into Device
			var soapAction = new SOAPAction();
			var setMacFilters2 = new SOAPSetMACFilters2();
			setMacFilters2.IsAllowList = datalist.isAllowList;

			for(var i in datalist.list)
			{
				if(datalist.list[i].MacfilterEnabled)
				{	
					var client = new SOAPMacInfo();
					client.DeviceName = encode_char_text(datalist.list[i].NickName);
					client.MacAddress = datalist.list[i].MacAddress;
					client.ScheduleName = datalist.list[i].ScheduleName;
					setMacFilters2.MACList.push(client);
				}
			}
			var result = soapAction.sendSOAPAction("SetMACFilters2", setMacFilters2, null);

			resetRulePOP();
		}
	});

}

function closeAddPOP()
{
	changeTimeoutAction();
	var reverseIP_enabled = $("#enableReserveIP").prop("checked");
	var reserveIP =$("#client_IPAdrReserve").val();
	var name = $("#client_Name").val();
	var macfilter_enabled = $("#enableAccess").prop("checked");
	var schedule = $("#schedule").val();
	var mac = $("#client_editMac").val();
	
	$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);
	
	//store client into Device
	var soapAction = new SOAPAction();
	var clientResult = null;
	var macfilterResult = null;

	if(reverseIP_enabled)
	{
		var setClientInfo = new SOAPSetClientInfo();
		var client = new SOAPClientInfo();
		client.MacAddress = mac
		client.NickName = encode_char_text(name);
		client.ReserveIP = reserveIP;
		setClientInfo.ClientInfoLists.push(client);
		
		clientResult = soapAction.sendSOAPAction("SetClientInfo", setClientInfo, null);

		clientResult.done(function(obj){
			if(obj.SetClientInfoResult == "ERROR_RESERVEIP_CONFLICT")
			{
				alert(I18N("j", "IP address cannot be the same."));
				$("#check_btn").attr("class", "styled_button_s").prop("disabled", false);
			}
		});
	}


	//set client info OK, store mac filter into Device
	if(macfilter_enabled)
	{
		var setMacFilters2 = new SOAPSetMACFilters2();
		setMacFilters2.IsAllowList = datalist.isAllowList;

		for(var i in datalist.list)
		{
			if(datalist.list[i].MacfilterEnabled)
			{	
				var client = new SOAPMacInfo();
				client.DeviceName = datalist.list[i].DeviceName;
				client.MacAddress = datalist.list[i].MacAddress;
				client.ScheduleName = encode_char_text(datalist.list[i].ScheduleName);
				setMacFilters2.MACList.push(client);
			}
		}
		var newClient = new SOAPMacInfo();
		newClient.DeviceName = encode_char_text(name);
		newClient.MacAddress = mac;
		newClient.ScheduleName = encode_char_text(schedule);
		setMacFilters2.MACList.push(newClient);

		macfilterResult = soapAction.sendSOAPAction("SetMACFilters2", setMacFilters2, null);
	}

	$.when(clientResult, macfilterResult).done(function(obj)
	{
		resetRulePOP();
	});
}

function clearCreateRulePOP()
{
	changeTimeoutAction();
	resetRulePOP();
}

function resetRulePOP()
{
	//renew GUI
	Get_ClientInfo().done(function(){
		$("#editPop").hide();
		$("input").val('');
		$("#editMac_Info").hide();
		
		$("select").selectbox('detach');
		$("#schedule").val('Always');
		$("select").selectbox('attach');

		$("#client_IPAdrReserve_warning").remove();

		var validator = $( "#client_form" ).validate();
		validator.resetForm();
		$("#client_form input").removeClass("error");
	});
}

function getClientList(str)
{
	if (str == "Host")
	{
		$("#client_btn_Blocked").attr("class", "v4v6_linkstyle_2");
		$("#client_btn_Guest").attr("class", "v4v6_linkstyle_2");
		$("#client_btn_Host").attr("class", "v4v6_linkstyle_1");
	}
	else if (str == "Guest")
	{
		$("#client_btn_Blocked").attr("class", "v4v6_linkstyle_2");
		$("#client_btn_Guest").attr("class", "v4v6_linkstyle_1");
		$("#client_btn_Host").attr("class", "v4v6_linkstyle_2");
	}
	else	//Blocked
	{
		$("#client_btn_Blocked").attr("class", "v4v6_linkstyle_1");
		$("#client_btn_Guest").attr("class", "v4v6_linkstyle_2");
		$("#client_btn_Host").attr("class", "v4v6_linkstyle_2");
	}
	
	Client_Type = str;
	Get_ClientInfo();
}

function buttonStyleChange()
{
	var result = true;

	changeTimeoutAction();
	if(editType == true)
	{
		if(($("#enableReserveIP").prop("checked") == true)&&($("#client_IPAdrReserve").val() == ""))
		{
			result = false;
		}
	}
	else
	{
		if($("#client_editMac").val() == "")
		{
			result = false;
		}	

		if(($("#enableReserveIP").prop("checked") == false)&&($("#enableAccess").prop("checked") == false))
		{
			result = false;
		}

		if(($("#enableReserveIP").prop("checked") == true)&&($("#client_IPAdrReserve").val() == ""))
		{
			result = false;
		}
	}

	if(result)
		$("#check_btn").attr("class", "styled_button_s").prop("disabled", false);
	else
		$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);		
}

function setEvents(){
	$("#enableReserveIP").on("change", function(event){
		buttonStyleChange();
		if($(this).prop("checked"))
		{
			$("#show_IPAdrReserve").show();
		}
		else
		{
			$("#show_IPAdrReserve").hide();
		}
	});

	$("#enableAccess").on("change", function(event){
		buttonStyleChange();
		if($(this).prop("checked"))
		{
			$("#show_Schedule").show();
		}
		else
		{
			$("#show_Schedule").hide();
		}
	});
}

function setClientValidate() {
	$.validator.addMethod("checkIP", function( value, element ) {
		if(value == "")
			return true;

		if(!COMM_ValidV4Format(value) || !COMM_ValidV4Addr(value)){
			return false;
		}
		return true;
	}, jQuery.validator.messages.address_Check);

	$.validator.addMethod("checkMac", function( value, element ) {
		if(COMM_IsMAC(value) == false)
		{
			return false;
		}
		return true;
	}, jQuery.validator.messages.mac_Check);

	$.validator.addMethod("checkMacConflict", function( value, element ) {
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];

			if(obj.MacAddress.toLowerCase() == value.toLowerCase())
				return false;			
		}
		return true;
	}, jQuery.validator.messages.mac_Conflict);
	
	$.validator.addMethod("checkIPConflict", function( value, element ) {
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];

			if((editType == true)&&(editID == obj.rowid))
			{
				continue;
			}

			if((value != "")&&(obj.ReserveIP == value))
				return false;			
		}
		return true;
	}, jQuery.validator.messages.ip_Conflict);


	//checkDHCPrange
	$.validator.addMethod("checkDHCPrange", function( value, element ) {
		var returnVal = true;
		var valuelist=value.split('.');
		var ipaddresslist=(getNetworkSettings.IPAddress).split('.');
		var dhcpenabledchoice=getRouterLanSettingsResponse.DHCPServerEnabled;
		var iprangestartnum=parseInt(getNetworkSettings.IPRangeStart,10);
		var iprangeendnum=parseInt(getNetworkSettings.IPRangeEnd,10);
		if(dhcpenabledchoice=="true"){
			if(valuelist[0]!=ipaddresslist[0] || valuelist[1]!=ipaddresslist[1] || valuelist[2]!=ipaddresslist[2]){
				returnVal = false;
			}else{
				if(parseInt(valuelist[3],10)<iprangestartnum || parseInt(valuelist[3],10)>iprangeendnum){
					returnVal = false;
				}
			}
		}
		return returnVal;

	}, jQuery.validator.messages.address_Check);	
	//checkDHCPrange end

	$('#client_form').validate({
		rules: {
			client_editMac: {
				checkMac: true,
				checkMacConflict: true
			},
			client_IPAdrReserve: {
				checkIP: true,
				checkIPConflict: true,
				checkDHCPrange: true
			},
		},
		submitHandler: function(form) {
			if(editType == true)
			{
				closeEditPOP();
			}
			else
			{
				closeAddPOP();
			}
		},
		unhighlight:function(element, errorClass, validClass){
			$(element).removeClass(errorClass).addClass(validClass);

			if((element == $("input#client_IPAdrReserve")[0])&&(editType == true))
			{
				$("#client_IPAdrReserve_warning").remove();

				var obj = datalist.getData(editID);
				if((element.value)&&(element.value != obj.ReserveIP))
				{
					$(element).after("<div id='client_IPAdrReserve_warning' style='color:#00f;fone-size:12px' />");
					$("#client_IPAdrReserve_warning").html(I18N("j", "It will take effect after reconnecting"));
				}
			}
		},
		highlight:function(element, errorClass, validClass){
			$(element).addClass(errorClass).removeClass(validClass);
			if(element == $("input#client_IPAdrReserve")[0])
			{
				$("#client_IPAdrReserve_warning").remove();
			}
		},
		errorPlacement:function(error, element){
			error.insertAfter(element).css("display","inline");
		}
	});
}