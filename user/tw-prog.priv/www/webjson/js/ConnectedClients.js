
//data list
function Datalist()
{
	this.list = new Array();
	this.maxrowid = 0;
}

Datalist.prototype =
{
	get length(){
		return this.list.length;
	},
	
	set length(val){
		if(parseInt(val, 10) >= 0)
			this.list.length = parseInt(val,10);
	}
}

Datalist.prototype .getData = function(rowid){
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

Datalist.prototype .getDataRowNumByMac = function(mac){
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

Datalist.prototype .getRowNum = function(rowid){
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
	var rowNum = this.getRowNum(id);

	this.list[rowNum].IPv4Address = newdata.IPv4Address;
	this.list[rowNum].IPv6Address = newdata.IPv6Address;
	this.list[rowNum].Type = newdata.Type;
	this.list[rowNum].DeviceName = newdata.DeviceName;
	this.list[rowNum].NickName = newdata.NickName;
	this.list[rowNum].ReserveIP = newdata.ReserveIP;
	this.list[rowNum].Vendor = newdata.Vendor;
	this.list[rowNum].ScheduleName = newdata.ScheduleName;
	this.list[rowNum].MacfilterEnabled = newdata.MacfilterEnabled;
	this.list[rowNum].Status = newdata.Status;	

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

	return true;
}

//constructor
function Data(macAddress, ipv4Address, ipv6Address, type, deviceName, nickName, reserveIP){
	this.MacAddress = macAddress;
	this.IPv4Address = ipv4Address;
	this.IPv6Address = ipv6Address;
	this.Type = type;
	this.DeviceName = deviceName;
	this.NickName = nickName;
	this.ReserveIP = reserveIP;
	
	this.Vendor = getVendor(macAddress);
	
	//mac filter
	this.ScheduleName;
	this.MacfilterEnabled = false;
	this.Status = false;
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
	
	setRowid : function(rowid)
	{
		this.rowid = rowid;
	},
	
	showClientInfo: function()
	{
		var self = this;
		var name = self.NickName;
		var macFilterString = "";
		var fontColor;
		var cssBackGround = "link_IconE_";	//default type = LAN
			
		if (typeof(self.MacAddress) == "undefined" || self.MacAddress.length == 0)
		{
			return "";
		}
		if (name == "")
		{
			name = "Unknown";
		}
			
		var objType = self.Type.toUpperCase();
		if (objType == "WIFI_2.4G" || objType == "WIFI_5G" || objType == "WIFI_2.4G_GUEST" || objType == "WIFI_5G_GUEST")
		{
			cssBackGround = "link_IconW_";
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

		var outputString = "<td>"
			   + '<div class ="client_Tag" style ="cursor:default">'
			   + '<div class ="' + cssBackGround + '"></div>'
			   + '<div class ="client_Info">'
			   + '<div class ="client_Name">' + name + '</div>'
			   + '<div class ="client_EditImage" onclick ="editData(' + self.rowid + ')" style ="cursor:pointer"></div>'
			   + '<div class ="client_Vender">' + self.Vendor + '</div>'
			   + '<div class ="client_IPv4Address">' + self.IPv4Address + '</div>'
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

function getVendor(mac)
{
	var macUnit = mac.split(':');
	var vendorID = macUnit[0]+macUnit[1]+macUnit[2];
	var vendorName = "Unknown Vendor";

	vendorID = vendorID.toUpperCase();

	var StrIndexOfNumber = allText.indexOf(vendorID);
	if(StrIndexOfNumber >= 0)
	{
		StrIndexOfNumber+=7;
		var StrIndexOfNumber_ToString = allText.substring(StrIndexOfNumber).split(/[\s,]+/);
		vendorName = decode_char_text(StrIndexOfNumber_ToString[0]);
	}
	return vendorName;
}

function Get_ClientInfo()
{
	var deferred = $.Deferred();
	var clientInfo = new SOAPGetClientInfoResponse();
	var macfilterInfo = new SOAPGetMACFilters2Response();
	var soapAction = new SOAPAction();
	var soapAction2 = new SOAPAction();
	var clientinfo_query = soapAction.sendSOAPAction("GetClientInfo", null, clientInfo);
	var macfilter_query = null;
	
	if(currentState == "clientInfo")	//get client details
	{
		macfilter_query = soapAction2.sendSOAPAction("GetMACFilters2", null, macfilterInfo);
	}
	//type Host or Guest
	$.when(clientinfo_query, macfilter_query).done(function(obj)
	{
		//remove disconnected client
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];
			var found = false;
			if(obj.Type == "Blocked")
				continue;

			for(var clientUnit in clientInfo.ClientInfoLists.ClientInfo)
			{
				var client = clientInfo.ClientInfoLists.ClientInfo[clientUnit];
				if(client.MacAddress.toLowerCase() == obj.MacAddress.toLowerCase())
				{
					found = true;
					break;
				}
			}	
			if(found == false)
			{
				datalist.deleteData(obj.rowid);
			}
		}
		//update client
		for(var clientUnit in clientInfo.ClientInfoLists.ClientInfo)
		{
			var client = clientInfo.ClientInfoLists.ClientInfo[clientUnit];
			var data = new Data(client.MacAddress, client.IPv4Address, client.IPv6Address, client.Type, client.DeviceName, client.NickName, client.ReserveIP);
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
		
		if(currentState == "clientInfo")
		{
			var hasMacfilter = false;

			//remove disconnected macfilter
			for(var row in datalist.list)
			{
				var obj = datalist.list[row];
				var found = false;
				if(obj.Type != "Blocked")
					continue;

				for(var macfilterUnit in macfilterInfo.MACList.MACInfo)
				{
					var macfilter = macfilterInfo.MACList.MACInfo[macfilterUnit];
					if(macfilter.MacAddress.toLowerCase() == obj.MacAddress.toLowerCase())
					{
						found = true;
						break;
					}
				}
				if(found == false)
				{
					datalist.deleteData(obj.rowid);
				}
			}
			//update macfilter
			for(var macfilterUnit in macfilterInfo.MACList.MACInfo)
			{
				var macfilter = macfilterInfo.MACList.MACInfo[macfilterUnit];
				
				//search client list
				var rowNum = datalist.getDataRowNumByMac(macfilter.MacAddress);
				if(rowNum != null)
				{
					datalist.list[rowNum].MacfilterEnabled = true;
					datalist.list[rowNum].ScheduleName = macfilter.ScheduleName;
					if(macfilter.Status == "true")
						datalist.list[rowNum].Status = true;
				}
				else
				{
					var data = new Data(macfilter.MacAddress, "", "", "LAN", macfilter.DeviceName, "", "");
					data.MacfilterEnabled = true;
					data.ScheduleName = macfilter.ScheduleName;
					data.Type = "Blocked";
					if(macfilter.Status == "true")
						data.Status = true;
					datalist.push(data);
					
					hasMacfilter = true;
				}

			}
			showClientTypeBtn("block", hasMacfilter);
			showClientLists(Client_Type);
		}
		//show count
		$("#Total_ConnectedClients").html(clientInfo.ClientInfoLists.ClientInfo.length);

		deferred.resolve();
	});
	
	return deferred.promise();
}

function showClientLists(type)
{
	$(document).tooltip("disable");
	$(document).tooltip("enable");

	var column = 0;
	var clientItemsHTML = "<tr>";
	for(var row in datalist.list)
	{
		var obj = datalist.list[row];
		if(type == "Guest")
		{
			if (obj.Type.toLowerCase().indexOf("guest") < 0)
			{
				continue;
			}
		}
		else if(type == "Host")
		{
			if (obj.Type.toLowerCase().indexOf("guest") >= 0)
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
			//if (obj.MacfilterEnabled == false)
			if(obj.Type != "Blocked")
			{
				continue;
			}
		}
		column++;

		clientItemsHTML += obj.showClientInfo();

		if(column % 3 == 0)
		{
			clientItemsHTML += "</tr><tr>";
		}
	}
	clientItemsHTML += "</tr>";
	$("#Client_items").html(clientItemsHTML);
	
	if(column>0)
	{
		$("#Client_Info").hide();
	}
	else if(type == "Blocked")	//no Blocked list, maybe all deleted
	{
		showClientTypeBtn("block", false);
		showClientLists("Host");
	}
	else
	{
		$("#Client_Info").show();
	}
	$("#Client_Group").css("display", "table-row");
}

function editData(id)
{
	$("#editPop").show();
	$("#check_btn").attr("class", "styled_button_dis").attr("disabled", true)
	.off('click').click(function()
	{
		closeEditPOP(id);
	});
	
	//insert data to pop form
	var data = datalist.getData(id);
	var ipString = data.IPv4Address;
	
	document.getElementById("IPAdrReserve_Info").innerHTML= "";
	document.getElementById("client_IPAdrReserve").value = data.ReserveIP;
	document.getElementById("client_MACAddress").innerHTML = data.MacAddress;
	$("#client_Name").val(data.NickName);

	if(ipString == "")
	{
		ipString = I18N("j", "Not Available");
	}
	$("#client_IPAdr").html(ipString);
		
	if(data.ReserveIP == ""){
		document.getElementById("show_IPAdrReserve").style.display = "none";
		document.getElementById("show_IPAdr").style.display = "table-row";
		presetCheckBox("enableReserveIP", false);
	}
	else{
		document.getElementById("show_IPAdrReserve").style.display = "table-row";
		document.getElementById("show_IPAdr").style.display = "none";
		presetCheckBox("enableReserveIP", true);
	}

	if (data.Vendor != "")
	{
		document.getElementById("client_Vendor").innerHTML = data.Vendor;
	}
	else
	{
		document.getElementById("client_Vendor").innerHTML = "N/A";
	}
		
	if (data.MacfilterEnabled == false)
	{
		presetCheckBox("enableAccess", false);
		document.getElementById("show_Schedule").style.display = "none";
	}
	else
	{
		presetCheckBox("enableAccess", true);
		document.getElementById("show_Schedule").style.display = "table-row";
		$("#schedule").selectbox('detach');
		$("#schedule").val('Always');
		if (data.ScheduleName != "")
		{
			for (var i = 1; i <= tmpXML_ScheduleCnt; i ++)
			{
				if(schedule.options[i].value === data.ScheduleName)
				{
					$("#schedule").val(schedule.options[i].value);
					break;
				}
			}
		}
		$("#schedule").selectbox('attach');
	}
}

function closeEditPOP(id)
{
	var reverseIP_enabled = document.getElementById("enableReserveIP_ck").checked;
	var reserveIP =$("#client_IPAdrReserve").val();
	var name = $("#client_Name").val();
	var macfilter_enabled = document.getElementById("enableAccess_ck").checked;
	var schedule = $("#schedule").val();
	
	$("#check_btn").attr("class", "styled_button_dis").attr("disabled", true);
	
	var data = datalist.getData(id);
	data.NickName = name;
	
	if(reverseIP_enabled && (reserveIP != ""))
	{
		if(!COMM_ValidV4Format(reserveIP) || !COMM_ValidV4Addr(reserveIP)){
			document.getElementById("IPAdrReserve_Info").innerHTML= I18N("j","err_address_Check");
			return;
		}
		else
		{
			data.ReserveIP = reserveIP;
		}
	}
	else
	{
		data.ReserveIP = "";
	}
	
	data.MacfilterEnabled = macfilter_enabled;
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
	client.NickName = data.NickName;
	client.ReserveIP = data.ReserveIP;
	setClientInfo.ClientInfoLists.push(client);
	var setClientResult = soapAction.sendSOAPAction("SetClientInfo", setClientInfo, null);

	setClientResult.done(function(obj){
		if(obj.SetClientInfoResult == "ERROR_RESERVEIP_CONFLICT")
		{
			alert(I18N("j", "IP address cannot be the same."));
			$("#check_btn").attr("class", "styled_button_s").attr("disabled", false);
		}
		else
		{
			//set client info OK, store mac filter into Device
			var soapAction = new SOAPAction();
			var setMacFilters2 = new SOAPSetMACFilters2();
			
			for(var i in datalist.list)
			{
				if(datalist.list[i].MacfilterEnabled)
				{	
					var client = new SOAPMacInfo();
					client.MacAddress = datalist.list[i].MacAddress;
					client.ScheduleName = encode_char_text(datalist.list[i].ScheduleName);
					setMacFilters2.MACList.push(client);
				}
			}
			var result = soapAction.sendSOAPAction("SetMACFilters2", setMacFilters2, null);

			resetRulePOP();
		}
	});

}

function clearCreateRulePOP()
{
	resetRulePOP();
}

function resetRulePOP()
{
	//renew GUI
	Get_ClientInfo().done(function(){
		$("#editPop").hide();
		$("input").val('');
		
		$("select").selectbox('detach');
		$("#schedule").val('Always');
		$("select").selectbox('attach');
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