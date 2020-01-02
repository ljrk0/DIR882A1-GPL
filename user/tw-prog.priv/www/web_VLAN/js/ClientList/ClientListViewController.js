require.config({
	paths: {
		"Websocket": "/js/SOAP/Websocket"
	}
});

require(['Data', 'Datalist', 'ClientListView', 'QoSViewController', 'Websocket'], function(Data, Datalist, ClientListView, QoSViewController, WS){

	var datalist = new Datalist();
	var view = new ClientListView();
	var macList = "";
	var editType = true;
	var editID = "";
	var clientType = "Host";

	//websocket
	function wsGetClientInfo(){
		initWebsocket(WS).done(function(ws){	
			ws.errorHandler = clientInfoErrorHandler;
			registerClientInfo(ws);
		});
	}
	
	var registerClientInfo = function(ws)
	{
		var getClientListcmd = {
			"Command":"GetAllClientList"
		};
		var listenClientInfocmd = {
			"Command":"GetClientList",
			"Action":"start"
		}				
		ws.register(getClientListcmd, clientinfoDelegate, null);
		ws.register(listenClientInfocmd, clientinfoDelegate, null);	
	}
	
	//delegate
	var clientinfoDelegate = function(jsondata, viewcontroller){

		for(var i in jsondata.ClientInfo)
		{
			var client = jsondata.ClientInfo[i];
			
			//delete
			if(client.Action.toLowerCase() == "delete")
			{
				var rowNum = datalist.getDataRowNumByMac(client.MacAddress);
				view.deleteClientHTML(datalist.list[rowNum]);
				
				datalist.list.splice(rowNum, 1);			
			}
			else	//update or add
			{
				var data = new Data(client.MacAddress, client.IPv4Address, client.IPv6Address, client.Type, client.DeviceName, decode_char_text(client.NickName), client.ReserveIP);
				//for iQoS
				data.DeviceType = client.DeviceType;
				data.SignalStrength = client.SignalStrength;

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
		}
		getMacFilter().done(function(){
			showClient();
			//show count
			$("#Total_ConnectedClients").html(datalist.connectedLength);
		});
	}
	
	var clientInfoErrorHandler = function(ws){
		//console.log("clientInfo error handler!");
		registerClientInfo(ws);
	}
	
	function getMacFilter(){
		var deferred = $.Deferred();
		var macfilterInfo = new SOAPGetMACFilters2Response();
		
		soapAction.sendSOAPAction("GetMACFilters2", null, macfilterInfo).done(function(){
			var hasMacfilter = false;

			//update macfilter
			datalist.prepareUpdate();
			for(var row in datalist.list)
			{
				var obj = datalist.list[row];
				if(obj.Type != "Blocked")
					obj.Live = true;
			}
			for(var i in macfilterInfo.MACList.MACInfo)
			{
				var macfilter = macfilterInfo.MACList.MACInfo[i];
				
				//search client list
				var rowNum = datalist.getDataRowNumByMac(macfilter.MacAddress);
				if(rowNum != null)
				{
					datalist.list[rowNum].MacfilterEnabled = true;
					datalist.list[rowNum].ScheduleName = decode_char_text(macfilter.ScheduleName);
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
					var data = new Data(macfilter.MacAddress, "", "", "Blocked", decode_char_text(macfilter.DeviceName), "", "");
					data.MacfilterEnabled = true;
					data.ScheduleName = decode_char_text(macfilter.ScheduleName);
					if(macfilter.Status == "true")
						data.Status = true;
					datalist.push(data);
					hasMacfilter = true;
				}
			}
			datalist.clean();
			showClientTypeBtn("block", hasMacfilter);
			deferred.resolve();
		});
		return deferred.promise();
	}
	
	function getClientInfo_ajax(){
		var clientInfo = new SOAPGetClientInfoResponse();
		var soapAction = new SOAPAction();
		
		var jsondata = {
			"Command":"GetClientList",
			"ClientInfo":[]
		};
		
		soapAction.sendSOAPAction("GetClientInfo", null, clientInfo).done(function()
		{
			//xml to json
			for(var clientUnit in clientInfo.ClientInfoLists.ClientInfo)
			{
				var client = clientInfo.ClientInfoLists.ClientInfo[clientUnit];
				//compatible
				client["Action"] = "Update";
				client["DeviceType"] = "Not Available";
				client["SignalStrength"] = "Not Available";
				
				jsondata["ClientInfo"].push(client);
			}
			
			clientinfoDelegate(jsondata, null);
		});
		
	}



	//controller
	function convertClientType(modelType)
	{
		var showType = "Host";
		
		if (modelType.toLowerCase().indexOf("guest") > 0)
		{
			showType = "Guest";
		}
		else if(modelType.toLowerCase() == "blocked")
		{
			showType = "Blocked";
		}
		
		return showType;
	}
	
	function switchShowClient(showType)
	{
		if(clientType != showType)
		{
			//clear all
			view.lineList.splice(0, view.lineList.length);
			$("#client_items").html("");
		
			if (showType == "Host")
			{
				$("#client_btn_Blocked").attr("class", "v4v6_linkstyle_2");
				$("#client_btn_Guest").attr("class", "v4v6_linkstyle_2");
				$("#client_btn_Host").attr("class", "v4v6_linkstyle_1");
			}
			else if (showType == "Guest")
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
		
		
			clientType = showType;
			showClient();		
		}
	}

	function showClient()
	{
		var add = false;
		view.prepareUpdate();
		for(var i in datalist.list)
		{
			var data = datalist.list[i];
			
			var dataType = convertClientType(data.Type);
			
			var client = view.getClientView(data);
			if(dataType == clientType)
			{
				if(client != null)
				{
					view.updateClientHTML(client, data);
					client.Live = true;
				}
				else
				{
					addClient(data);
					add = true;
				}
				
			}
		}
		var change = view.clean();
		
		if((add||change) && (clientType == "Host"))
		{
			view.refreshView();
			addNew();
		}
		
	}

	function addClient(model)
	{
		var newClient = view.generateClientHTML(model);
		newClient.MacAddress = model.MacAddress;

		//search line
		var line = null;
		for(var i=0; i < view.lineList.length; i++)
		{
			if(view.lineList[i].objList.length < view.lineList[i].MaxCount)
			{
				line = view.lineList[i];
				break;
			}
		}
		
		if(line == null)
		{
			line = view.createLine();
		}

		line.push(newClient);
		newClient.DOM.appendTo(line.DOM);

		//events
		newClient.DOM.on('click', function(){
			onClickClient(newClient, model);
		});
		newClient.DOM.find(".client_EditImage").off("click").click(function(event){
			event.stopPropagation();
			view.showEditData(model);
			editType = true;
			editID = model.rowid;
		});
	}
	
	function addNew()
	{
		$(".client_add_Tag").remove();
		var newClient = view.generateNewHTML();

		//search line
		var line = null;
		for(var i=0; i < view.lineList.length; i++)
		{
			if(view.lineList[i].objList.length < view.lineList[i].MaxCount)
			{
				line = view.lineList[i];
				break;
			}
		}
		
		if(line == null)
		{
			line = view.createLine();
		}

		newClient.DOM.appendTo(line.DOM);

		//events
		newClient.DOM.on('click', function(){
			view.showAddData();
			editType = false;
			editID = "";
		});
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
				
				for(var i in datalist.list)
				{
					if(datalist.list[i].MacfilterEnabled)
					{	
						var client = new SOAPMacInfo();
						client.DeviceName = encode_char_text(datalist.list[i].NickName);
						client.MacAddress = datalist.list[i].MacAddress;
						client.ScheduleName = encode_char_text(datalist.list[i].ScheduleName);
						setMacFilters2.MACList.push(client);
					}
				}
				var result = soapAction.sendSOAPAction("SetMACFilters2", setMacFilters2, null);

				view.resetRulePOP();
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
			view.resetRulePOP();
		});
	}

	function onClickClient(client, model)
	{
		var currentLine = client.line;
		var position;

		for(var i in view.lineList)
		{
			var line = view.lineList[i];
			
			for(var j in line.objList)
			{
				if(line.objList[j].qosViewController != null)
				{
					line.objList[j].qosViewController.hideQoS();
				}
				
				//get position
				if(client == line.objList[j])
				{
					position = j;
				}
			}
		}

		if(currentLine.QoSShowStatus == client)
		{	
			currentLine.QoSShowStatus = null;
		}
		else
		{
			var qosviewcontroller = new QoSViewController(currentLine.DOM, model);
			qosviewcontroller.showQoS(model, position);
			client.qosViewController = qosviewcontroller;
			currentLine.QoSShowStatus = client;
			
			var position =  qosviewcontroller.qosView.DOM.offset().top - 130;
			setTimeout(function(){
				$("html, body").scrollTop(position);
			},500);
			/*$("html,body").animate({
				scrollTop: position
			}, 500);*/
		}

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

		$("select").change(function () {
			buttonStyleChange();
		});	
		$("input").keydown(function () {
			changeTimeoutAction();
		});
		$("#client_form  input").keyup(function(){
			buttonStyleChange();
		});
		
		$("#client_btn_Host").off("click").click(function(){
			switchShowClient("Host");
		});
		
		$("#client_btn_Guest").off("click").click(function(){
			switchShowClient("Guest");
		});
		
		$("#client_btn_Blocked").off("click").click(function(){
			switchShowClient("Blocked");
		});

	}

	LoadMACList();
	setEvents();
	setClientValidate();
	wsGetClientInfo();
	//getClientInfo_ajax();
	

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
		
		$('#client_form').validate({
			rules: {
				client_editMac: {
					checkMac: true,
					checkMacConflict: true
				},
				client_IPAdrReserve: {
					checkIP: true,
					checkIPConflict: true
				},
			},
			submitHandler: function(form) {
				changeTimeoutAction();
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
	};



});

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