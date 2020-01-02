var ROW_BASE = 1; // first number (for display)
var TABLE_NAME = 'tblDeviceList';
var RULE_NAME = 'rulename_';
var MAC_ADDRESS = 'macaddress_';
var Enable_Status = "status_";
//var IP_ADDRESS = "ipaddress_";
var PARENTAL_CONTROL = "parentalcontrol_";
//var RESERVEIP_STATUS = "reserveipstatus_";
var OFF_SCHEDULE = "offschedule_";
var EDIT_ICON = 'editicon_';
var DELETE_ICON = 'delicon_';
// Value
var a, b ,c ,d ,e, f, g, h;

// Pop Window Get Temp Information
var tmp_Iteration;
var tmp_RuleName;
var tmp_MACAddress;
//var tmp_IPAddress;
var tmp_ParentalControl;
var tmp_OffSchedule;
var tmp_ParentalControlStatus;
//var tmp_ReserveIPStatus;

var IsLoaded = true;

function GetHNAPInformation(Status, RuleName, MACAddress, ParentalControl, ParentalControlStatus, OffSchedule)
{
	a = Status;
	b = RuleName;
	c = MACAddress;
	//d = IPAddress;
	d = ParentalControl;
	e = ParentalControlStatus;
	f = OffSchedule;
	//g = ReserveIPStatus;
}

function Get_DeviceClientInfo()
{
	var deferred = $.Deferred();
	var clientInfo = new SOAPGetClientInfoResponse();
	var macfilterInfo = new SOAPGetMACFilters2Response();
	var soapAction = new SOAPAction();
	var clientinfo_query = soapAction.sendSOAPAction("GetClientInfo", null, clientInfo);
	var macfilter_query = soapAction.sendSOAPAction("GetMACFilters2", null, macfilterInfo);
    var Add_Select = document.getElementById("select_client_editMac");
    var ListCount = 1;

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
            Add_Select.options.add(new Option(client.MacAddress, ListCount));
            ListCount++;
			var data = new Data(client.MacAddress, client.IPv4Address, client.IPv6Address, client.Type, client.DeviceName, decode_char_text(client.NickName), client.ReserveIP);
			//console.log(data)
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
        $("#select_client_editMac").selectbox();
		datalist.clean();
		
		if (macfilterInfo.IsAllowList == "true") {
			orig_isAllow = true;
			$("#ruleType").selectbox('detach');
			$("#ruleType").val("2");
			$("#ruleType").selectbox({width:330});
			$("#ruleType").selectbox('attach');
		} else {
			orig_isAllow = false;
			$("#ruleType").selectbox('detach');
			$("#ruleType").val("1");
			$("#ruleType").selectbox({width:330});
			$("#ruleType").selectbox('attach');
		}

		//update macfilter
		datalist.prepareUpdate();
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];
			if(obj.Type != "Blocked")
				obj.Live = true;
		}

		for(var macfilterUnit in macfilterInfo.MACList.MACInfo)
		{
			var macfilter = macfilterInfo.MACList.MACInfo[macfilterUnit];
			macAddrList.push(macfilter.MacAddress);

			//search client list
			var rowNum = datalist.getDataRowNumByMac(macfilter.MacAddress);
			if(rowNum != null)
			{
				datalist.list[rowNum].MacfilterEnabled = true;
				datalist.list[rowNum].ScheduleName = AllEncode(decode_char_text(macfilter.ScheduleName));
				if(macfilter.Status == "false")
				{
					datalist.list[rowNum].Status = true;
					datalist.list[rowNum].Type = "Blocked";
				}
				datalist.list[rowNum].isAllowList = macfilterInfo.IsAllowList;
				datalist.list[rowNum].isMacfilterEnabled = macfilterInfo.Enabled;
				datalist.list[rowNum].Live = true;
			}
			else
			{
				var data = new Data(macfilter.MacAddress, "", "", "LAN", decode_char_text(macfilter.DeviceName), "", "");
				data.MacfilterEnabled = true;
				data.ScheduleName = AllEncode(decode_char_text(macfilter.ScheduleName));
				data.Type = "Blocked";
				if(macfilter.Status == "true")
					data.Status = true;
				data.isAllowList = macfilterInfo.IsAllowList;
				data.isMacfilterEnabled = macfilterInfo.Enabled;
				datalist.push(data);
			}
		}

		datalist.clean();
		var ruleType = $("#ruleType").val();

		for(var row in datalist.list)
		{
			var obj = datalist.list[row];

			if (obj.MacfilterEnabled) {
				if (ruleType == "1") {
					if(obj.isAllowList == "true") 
						continue;
				} else {
					if(obj.isAllowList == "false")
						continue;
				}
				
				var ScheduleName, ParentalControl;
				if (obj.ScheduleName == "") {
					ScheduleName = "-";
					ParentalControl = "false";
				} else if (obj.ScheduleName == "Always") {
					if (ruleType == "1") {
						ScheduleName = "Always OFF";
					} else {
						ScheduleName = "Always";
					}
					ParentalControl = "true";
				} else {
					ScheduleName = obj.ScheduleName;
					ParentalControl = "true";
				}

				/*
				var ReservIPStatus;
				if(obj.ReserveIP == "") {
					ReservIPStatus = "false";
					obj.ReserveIP = "-";
				} else {
					ReservIPStatus = "true";
				}
				*/

				GetHNAPInformation(obj.isMacfilterEnabled, obj.NickName, obj.MacAddress, ScheduleName, ParentalControl, obj.ScheduleName);
				addRowToTable(null);
				Total_Device += 1;
				check_TotalDevice(Limit_TotalDevice, Total_Device);
			}
		}
		deferred.resolve();
	});
	
	
	return deferred.promise();
}

function setDevice() 
{
	document.getElementById("CreatePopAlertMessage").style.display = "inline";

	var tbl = document.getElementById(TABLE_NAME);
	var soapAction = new SOAPAction();
	var clientResult = null;
	var macfilterResult = null;
	var setMacFilters2 = new SOAPSetMACFilters2();

	if(delMACList.length > 0) {
		for (var j = 0; j < delMACList.length; j++) {
			var delMAC = new SOAPDelMACFilters2();
			delMAC.UserName = delMACList[j];
			soapAction.sendSOAPAction("DelMACFilters2", delMAC, null);
		}
	}

	var isAllow;

	if($("#ruleType").val() == "1")
		isAllow = false;
	else
		isAllow = true;

	if(tbl.rows.length > 1) {
		for(var i = 1; i <= Total_Device; i ++)
		{
			//var reserveIP = tbl.rows[i].childNodes[3].childNodes[0].innerHTML;
			var mac = tbl.rows[i].childNodes[2].childNodes[0].innerHTML;
			var name = encode_char_text(HTMLDecode(tbl.rows[i].childNodes[1].childNodes[0].innerHTML));
			var enabled = tbl.rows[i].childNodes[0].childNodes[0].checked;
			var schedule = encode_char_text(AllDecode(tbl.rows[i].childNodes[3].childNodes[0].innerHTML));

			//if(reserveIP != "-")
			//{
				var setClientInfo = new SOAPSetClientInfo();
				var client = new SOAPClientInfo();
				client.MacAddress = mac;
				client.NickName = name;
				//client.ReserveIP = reserveIP;
				setClientInfo.ClientInfoLists.push(client);
				clientResult = soapAction.sendSOAPAction("SetClientInfo", setClientInfo, null);

				clientResult.done(function(obj){
					if(obj.SetClientInfoResult == "ERROR_RESERVEIP_CONFLICT")
					{
						alert(I18N("j", "IP address cannot be the same."));
					}
				});
			//}

			var newClient = new SOAPMacInfo();
			setMacFilters2.Enabled = enabled;
			setMacFilters2.IsAllowList = isAllow;
			newClient.DeviceName = name;
			newClient.MacAddress = mac;
			if(schedule == "-")
				schedule = "";
			newClient.ScheduleName = schedule;
			setMacFilters2.MACList.push(newClient);
		}
	} else {
			setMacFilters2.Enabled = false;
			setMacFilters2.IsAllowList = isAllow;
	}

	macfilterResult = soapAction.sendSOAPAction("SetMACFilters2", setMacFilters2, null);
	macfilterResult.done(function(obj){
		waitSettingFinished();
	});
}

function getIp(mac) 
{
	var rowNum = datalist.getDataRowNumByMac(mac);

	if(rowNum != null)	//update
	{
		return datalist.list[rowNum].IPv4Address;
	}

	return "-";
}

function editDeviceData(id)
{
	changeTimeoutAction();
	//$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);

	$("#popTitle").html(I18N("j", "Edit Rule"));

	$("#show_Vendor").show();
	$("#client_MACAddress").show();
	$("#show_editMac").hide();
    $("#show_select_editMac").hide();
    $("#show_IPAdr").show();
	
	var tbl = document.getElementById(TABLE_NAME);
	var tmp_b = tbl.rows[id].childNodes[1].childNodes[0].innerHTML; // name
	var tmp_c = tbl.rows[id].childNodes[2].childNodes[0].innerHTML; // mac
	//var tmp_d = tbl.rows[id].childNodes[3].childNodes[0].innerHTML; // ip
	var tmp_d = tbl.rows[id].childNodes[3].childNodes[0].innerHTML; // parental control
	if(tbl.rows[id].childNodes[6].childNodes[0].innerHTML =="Always" || tbl.rows[id].childNodes[6].childNodes[0].innerHTML =="Always OFF"){
		var tmp_e = tbl.rows[id].childNodes[6].childNodes[0].innerHTML; // off schedule 
	}else{
		var tmp_e = encode_char_text(AllDecode(tbl.rows[id].childNodes[6].childNodes[0].innerHTML)); // off schedule 
	}	
	//var tmp_g = tbl.rows[id].childNodes[8].childNodes[0].innerHTML; // reserve ip status
	document.getElementById("client_Name").value = HTMLDecode(tmp_b);
	document.getElementById("client_MACAddress").innerHTML  = tmp_c;
	document.getElementById("client_Vendor").innerHTML  = getVendor(tmp_c);
	
	/*
	if (tmp_g == "true") {
		document.getElementById("show_IPAdrReserve").style.display = "table-row";
		document.getElementById("show_IPAdr").style.display = "none";
		document.getElementById("client_IPAdrReserve").value = tmp_d;
		$("#enableReserveIP").prop("checked", true);
	} else {
		document.getElementById("show_IPAdrReserve").style.display = "none";
		document.getElementById("show_IPAdr").style.display = "table-row";
		document.getElementById("client_IPAdr").innerHTML = getIp(tmp_c);
		$("#enableReserveIP").prop("checked", false);
	}
	*/
	//tmp_IPAddress = tmp_d;
	tmp_ParentalControl = tmp_d;
	tmp_OffSchedule = tmp_e;

	if(tmp_d == "-") {
		//document.getElementById("edit_status_parentalControl").checked = true;
		$("#schedule").selectbox('detach');
		$("#schedule").val("Always");
		$("#schedule").selectbox('attach');
		//$("#enableAccess").prop("checked", false);
		//$("#show_Schedule").hide();
        $("#show_Schedule").show();
	} else {
		//document.getElementById("edit_status_parentalControl").checked = false;
		$("#schedule").selectbox('detach');
		$("#schedule").val(tmp_e);
		$("#schedule").selectbox('attach');
		//$("#enableAccess").prop("checked", true);
		$("#show_Schedule").show();
	}

	//tmp_ReserveIPStatus = tmp_g;

	tmp_RuleName = tmp_b;
	tmp_MACAddress = tmp_c;

	setIteration(id);

	$("#editPop").show();
	editType = true;
	editID = id;
}

function closeDeviceAddPOP()
{
	changeTimeoutAction();
		
	var GetResult = AddRowToIndex();
	if (GetResult == "Success")
	{
		save_button_changed();

		$("#editPop").hide();
		$("input").val('');
		$("#editMac_Info").hide();
		
		$("#schedule").selectbox('detach');
		$("#schedule").val('Always');
		$("#schedule").selectbox('attach');
        $("#select_client_editMac").selectbox('detach');
        $("#select_client_editMac").val(0);
        $("#select_client_editMac").selectbox('attach');
		//$("#client_IPAdrReserve_warning").remove();

		var validator = $( "#device_form" ).validate();
		validator.resetForm();
		$("#device_form input").removeClass("error");
	}
}

function closeDeviceEditPOP()
{
	changeTimeoutAction();
	//var reverseIP_enabled = $("#enableReserveIP").prop("checked");
	//var reserveIP =$("#client_IPAdrReserve").val();
	var name = $("#client_Name").val();
	//var macfilter_enabled = $("#enableAccess").prop("checked");
	var schedule = $("#schedule").val();
	//console.log(schedule)
	var macfilter_enabled;	
	var data = datalist.getData(editID);
	data.NickName = name;
	
	/*
	if(reverseIP_enabled && (reserveIP != ""))
	{
		data.ReserveIP = reserveIP;
	}
	else
	{
		data.ReserveIP = "";
	}
	*/

	if (schedule == "Always")
		macfilter_enabled = false;
	else
		macfilter_enabled = true;
	
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

	var the_Iteration = set_Iteration;
	var GetResult = assignRowToIndex(the_Iteration);
	if (GetResult == "Success")
	{
		clearCreateDeviceRulePOP();
		save_button_changed();
	}
	else if (GetResult == "Error")		{	alert("Name cannot be the same.");	}
	else if (GetResult == "NameNull")	{	alert("Name cannot be null.");		}
	else		{	document.getElementById("editPop").style.display = "none";	}
}

function check_TotalDevice(Limit_TotalDevice, Total_Device)
{
	var IsFull = Limit_TotalDevice - Total_Device;
	document.getElementById("RemainingRules").innerHTML = IsFull;
	
	if (IsFull == 0)
		{
			document.getElementById("addRuleButton").disabled = true;
		}
		else
		{
			document.getElementById("addRuleButton").disabled = false;
		}
}

function AddRowToIndex()
{
	var tbl = document.getElementById(TABLE_NAME);
	var nowDeviceName = $("#client_Name").val();
	
	for(var i = 1; i <= Total_Device; i ++)
	{
		if (nowDeviceName === HTMLDecode(tbl.rows[i].childNodes[1].childNodes[0].innerHTML))
		{
			alert("Rule name can not be the same.");
			return "Error";
		}
	}
	
	if (nowDeviceName == "")
	{
		alert("Rule name can not be null.");
		return "Error";
	}
	a = "true";
	b = nowDeviceName;
	c = $("#client_editMac").val();

	/*
	if($("#enableReserveIP").prop("checked")) {
		d = $("#client_IPAdrReserve").val();
		h = "true";
	} else {
		d = "-";
		h = "false";
	}
	*/
	
	var schedule = $("#schedule").val();
	//if ($("#enableAccess").prop("checked")) {
		if (schedule == "Always")
		{
			var rule = $("#ruleType").val();
			if (rule == "1") {
				d = "Always OFF";
			} else {
				d = "Always";
			}
		} else {
			d = AllEncode(decode_char_text(schedule));
            e = "true";
        }
	/*
	} else {
		d = "-";
		e = "false";
	}
	*/

	f = AllEncode(decode_char_text(schedule));
	//console.log(f)

	addRowToTable(null);
	Total_Device += 1;
	check_TotalDevice(Limit_TotalDevice, Total_Device);

	return "Success";
}

function myRowObject(one, two, three, four, five, six, seven, eight, nine)
{
	this.one = one;			// input Status
	this.two = two;			// input Rule Name
	this.three = three;		// input MAC Address
	this.four = four;		// input Parental Control
	this.five = five;		// input Edit icon
	this.six = six;			// input Delete icon
	this.seven = seven;		// input Off Schedule
	//this.eight = eight;		// input Off Schedule
	//this.nine = nine;		// input Reserve IP Status
}

function addRowToTable(num)
{
	if (IsLoaded)
	{
		var tbl = document.getElementById(TABLE_NAME);
		var nextRow = tbl.rows.length;
		var iteration = nextRow + ROW_BASE - 1;
		var macAddress = c;
		if (num == null)
		{
			num = nextRow;
		}
		else
		{
			iteration = num + ROW_BASE - 1;
		}
		// add the Row
		var row = tbl.insertRow(num);
		
		// Status
		var cell_0 = row.insertCell(0);
		var Input1 = document.createElement('input');
		Input1.setAttribute('id', Enable_Status + iteration);
		Input1.setAttribute('type', 'checkbox');
		if (a == "true")
			Input1.checked = true;
		else
			Input1.checked = false;
		Input1.onclick = function () { save_button_changed() };
		cell_0.appendChild(Input1);

		// Rule Name
		var cell_1 = row.insertCell(1);
		var Input2 = document.createElement('label');
		Input2.setAttribute('id', RULE_NAME + iteration);
		Input2.setAttribute('size', '10');
		Input2.innerHTML = HTMLEncode(b);
		cell_1.appendChild(Input2);
		
		// MAC Address
		var cell_2 = row.insertCell(2);
		var Input3 = document.createElement('label');
		Input3.setAttribute('id', MAC_ADDRESS + iteration);
		Input3.setAttribute('size', '10');
		Input3.innerHTML = macAddress;
		cell_2.appendChild(Input3);
		
		// IP Address
		/*
		var cell_3 = row.insertCell(3);
		var Input4 = document.createElement('label');
		Input4.setAttribute('id', IP_ADDRESS + iteration);
		Input4.setAttribute('size', '10');
		Input4.innerHTML = d;
		cell_3.appendChild(Input4);
		*/

		// Parental Control
		var cell_3 = row.insertCell(3);
		var Input4 = document.createElement('label');
		Input4.setAttribute('id', PARENTAL_CONTROL + iteration);
		Input4.setAttribute('size', '10');
		Input4.innerHTML = d;
		cell_3.appendChild(Input4);

		// Input Edit Icon
		var cell_4 = row.insertCell(4);
		var Input5 = document.createElement('img');
		Input5.setAttribute('src', 'image/edit_btn.png');
		Input5.setAttribute('width', '28');
		Input5.setAttribute('height', '28');
		Input5.setAttribute('id', EDIT_ICON + iteration);
		Input5.setAttribute('style', 'cursor:pointer');
		Input5.onclick = function () { editDeviceData(iteration) };
		cell_4.appendChild(Input5);
		
		// Input Delete Icon
		var cell_5 = row.insertCell(5);
		var Input6 = document.createElement('img');
		Input6.setAttribute('src', 'image/trash.png');
		Input6.setAttribute('width', '41');
		Input6.setAttribute('height', '41');
		Input6.setAttribute('id', DELETE_ICON + iteration);
		Input6.setAttribute('style', 'cursor:pointer');
		Input6.onclick = function () { deleteCurrentRow(this, macAddress) };
		cell_5.appendChild(Input6);

		// Hide Off Schedule to get
		var cell_6 = row.insertCell(6);
		var Input7 = document.createElement('label');
		Input7.setAttribute('id', OFF_SCHEDULE + iteration);
		Input7.setAttribute('size', '10');
		Input7.innerHTML = f;
		Input7.setAttribute('style', 'display:none');
		cell_6.appendChild(Input7);

		// Hide Parental Control Status to get
		/*
		var cell_8 = row.insertCell(8);
		var Input9 = document.createElement('label');
		Input9.setAttribute('id', RESERVEIP_STATUS + iteration);
		Input9.setAttribute('size', '10');
		Input9.innerHTML = h;
		Input9.setAttribute('style', 'display:none');
		cell_8.appendChild(Input9);
		*/

		row.myRow = new myRowObject(Input1, Input2, Input3, Input4, Input5, Input6, Input7);
		
		b = null;
		c = null;
		d = null;
		e = null;
		f = null;
		//g = null;
		//h = null;
	}
}

function assignRowToIndex(id)
{
	var tbl = document.getElementById(TABLE_NAME);
	var edit_Name = document.getElementById("client_Name").value;
	
	if (edit_Name != "")
	{
		for(var i = 1; i <= Total_Device; i ++)
		{
			if (edit_Name === tbl.rows[i].childNodes[1].childNodes[0].innerHTML)
			{
				if (editID == i)
				{
					continue;
				}
				else
				{
					return "Error";
				}
			}
		}
	}
	
	//tbl.rows[id].childNodes[0].childNodes[0].checked = $("#enableAccess").prop("checked");
	tbl.rows[id].childNodes[1].childNodes[0].innerHTML = HTMLEncode(document.getElementById("client_Name").value);

	tbl.rows[id].childNodes[2].childNodes[0].innerHTML = document.getElementById("client_MACAddress").innerHTML;

	/*
	if($("#enableReserveIP").prop("checked")) {
		tbl.rows[id].childNodes[3].childNodes[0].innerHTML = document.getElementById("client_IPAdrReserve").value;
	} else {
		tbl.rows[id].childNodes[3].childNodes[0].innerHTML = "-";
	}
	*/

	//if ($("#enableAccess").prop("checked"))
	//{
		if ($("#schedule").val() == "Always") {
			var rule = $("#ruleType").val();
			if (rule == "1") {
				tbl.rows[id].childNodes[3].childNodes[0].innerHTML = "Always OFF";
			} else {
				tbl.rows[id].childNodes[3].childNodes[0].innerHTML = "Always";
			}
		} else {
			tbl.rows[id].childNodes[3].childNodes[0].innerHTML = AllEncode(decode_char_text($("#schedule").val()));
			
	/*}
	else
	{
		tbl.rows[id].childNodes[3].childNodes[0].innerHTML = "-";
	}
	*/
			//alert(AllEncode(decode_char_text($("#schedule").val())));
			tbl.rows[id].childNodes[6].childNodes[0].innerHTML = AllEncode(decode_char_text($("#schedule").val()));

		}
	//tbl.rows[id].childNodes[8].childNodes[0].innerHTML = $("#enableReserveIP").prop("checked");
	
	check_TotalDevice(Limit_TotalDevice, Total_Device);
	
	return editID;
}

function deleteCurrentRow(obj, macAddress)
{
	if (IsLoaded)
	{
		for(i = 0; i < macAddrList.length; i++)
		{
			if(this.macAddrList[i] == macAddress)
			{
				delMACList.push(macAddress);
			}
		}

		var delRow = obj.parentNode.parentNode;
		var tbl = delRow.parentNode.parentNode;
		var rIndex = delRow.sectionRowIndex;
		var rowArray = new Array(delRow);
		deleteRows(rowArray);
		reorderRows(tbl, rIndex);
		
		Total_Device -= 1;
		check_TotalDevice(Limit_TotalDevice, Total_Device);
		save_button_changed();
	}
}
function OnChangeDeviceName(num)
{
	tmp_RuleName = num;
}
function deleteRows(rowObjArray)
{
	if (IsLoaded)
	{
		for (var i = 0; i < rowObjArray.length; i ++)
		{
			var rIndex = rowObjArray[i].sectionRowIndex;
			rowObjArray[i].parentNode.deleteRow(rIndex);
		}
	}
}
function changeRowIndex(count)
{
	// Change Index List
	var tbl = document.getElementById(TABLE_NAME);
	tbl.rows[count].childNodes[6].childNodes[0].onclick = function() { editData(count) };
}
function reorderRows(tbl, startingIndex)
{
	var tbl = document.getElementById(TABLE_NAME);
	if (IsLoaded)
	{
		if (tbl.rows[startingIndex])
		{
			var count = startingIndex + ROW_BASE;
			for (var i = startingIndex; i < tbl.rows.length; i ++)
			{
				tbl.rows[i].myRow.one.id = Enable_Status + count; // input text
				tbl.rows[i].myRow.two.id = RULE_NAME + count; // input text
				tbl.rows[i].myRow.three.id = MAC_ADDRESS + count; // input text
				//tbl.rows[i].myRow.four.id = IP_ADDRESS + count; // input text
				tbl.rows[i].myRow.four.id = PARENTAL_CONTROL + count; // input text
				tbl.rows[i].myRow.five.id = EDIT_ICON + count; // input text
				tbl.rows[i].myRow.six.id = DELETE_ICON + count; // input text
				tbl.rows[i].myRow.seven.id = OFF_SCHEDULE + count; // input text
				//tbl.rows[i].myRow.nine.id = RESERVEIP_STATUS + count; // input text

				changeRowIndex(count);
				count++;
			}
		}
	}
}

function clearCreateDeviceRulePOP()
{
	changeTimeoutAction();
	$("#editPop").hide();
	$("input").val('');
	$("#editMac_Info").hide();
	
	$("#schedule").selectbox('detach');
	$("#schedule").val('Always');
	$("#schedule").selectbox('attach');
    $("#select_client_editMac").selectbox('detach');
    $("#select_client_editMac").val(0);
    $("#select_client_editMac").selectbox('attach');
	//$("#client_IPAdrReserve_warning").remove();

	var validator = $( "#device_form" ).validate();
	validator.resetForm();
	$("#device_form input").removeClass("error");
}

function setDeviceValidate() {
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

	$.validator.addMethod("checkDeviceMacConflict", function( value, element ) {
		/*
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];

			if(obj.MacAddress.toLowerCase() == value.toLowerCase() && obj.MacfilterEnabled)
				return false;			
		}
		*/

		var tbl = document.getElementById(TABLE_NAME);
		for(var i = 1; i <= Total_Device; i ++)
		{
			if (value === tbl.rows[i].childNodes[2].childNodes[0].innerHTML)
			{
				if ((editType == true) && editID == i)
				{
					continue;
				}
				else
				{
					return false;
				}
			}
		}

		return true;
	}, jQuery.validator.messages.mac_Conflict);
	
	$.validator.addMethod("checkDeviceIPConflict", function( value, element ) {
		/*
		for(var row in datalist.list)
		{
			var obj = datalist.list[row];

			if((editType == true)&&(editID == obj.rowid))
			{
				continue;
			}

			if((value != "")&&(obj.ReserveIP == value) && obj.MacfilterEnabled)
				return false;			
		}
		*/
		var tbl = document.getElementById(TABLE_NAME);
		for(var i = 1; i <= Total_Device; i ++)
		{
			if (value === tbl.rows[i].childNodes[3].childNodes[0].innerHTML)
			{
				if ((editType == true) && editID == i)
				{
					continue;
				}
				else
				{
					return false;
				}
			}
		}
		return true;
	}, jQuery.validator.messages.ip_Conflict);

	$('#device_form').validate({
		rules: {
			client_editMac: {
				checkMac: true,
				checkDeviceMacConflict: true
			},
			/*
			client_IPAdrReserve: {
				checkIP: true,
				checkDeviceIPConflict: true
			},
			*/
		},
		submitHandler: function(form) {
			if(editType == true)
			{
				closeDeviceEditPOP();
			}
			else
			{
				closeDeviceAddPOP();
			}
		},
		unhighlight:function(element, errorClass, validClass){
			$(element).removeClass(errorClass).addClass(validClass);
			/*
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
			*/
		},
		highlight:function(element, errorClass, validClass){
			$(element).addClass(errorClass).removeClass(validClass);
			/*
			if(element == $("input#client_IPAdrReserve")[0])
			{
				$("#client_IPAdrReserve_warning").remove();
			}
			*/
		},
		errorPlacement:function(error, element){
			error.insertAfter(element).css("display","inline");
		}
	});
}