define("ClientListView", ['QoSView'], function(QoSView){
	
	var qosView = new QoSView();

	var Line = function(dom){
		this.DOM = dom;
		this.objList = new Array();

		this.MaxCount = 3;

		this.QoSShowStatus = null;
	}

	Line.prototype.push = function(obj){
		//check
		if(this.objList.length >= this.MaxCount)
			return false;

		this.objList.push(obj);
		obj.line = this;

		return true;
	}

	Line.prototype.remove = function(obj){
		var i = this.objList.indexOf(obj);

		if(i >= 0)
		{
			this.objList.splice(i, 1);
			return true;
		}
		return false;
	}

	var Client = function(dom){
		this.DOM = dom;
		this.line;	//pointer to parent line
		this.type;
		this.qosViewController = null;
		this.MacAddress;
		
		this.Live = true;
	}

	var ClientListView = function()
	{
		this.lineList = new Array();
	}

	ClientListView.prototype.createLine = function()
	{
		var newLine = $("#client_line").clone();
		newLine.attr("id","");
		var line = new Line(newLine);

		this.lineList.push(line);

		newLine.appendTo("#client_items");

		newLine.css("display", "table");
		
		return line;
	}

	ClientListView.prototype.generateClientHTML = function(model)
	{
		var name = model.NickName;
		var macFilterString = "";
		var fontColor;
		var icon_base = "../image/";
		var icon;
		var ipv4Address = model.IPv4Address;
		var vendor = model.Vendor;
		var blocked = false;
		var newClient;

		//process data
		if (typeof(model.MacAddress) == "undefined" || model.MacAddress.length == 0)
		{
			return null;
		}
		if (name == "")
		{
			name = "Unknown";
		}

		/*if(model.MacfilterEnabled)
		{
			macFilterString = I18N("j", "Enabled");
			fontColor = "#FF0000";
			if((model.Type == "Blocked")||(model.Status))
			{
				blocked = true;
			}
		}
		else
		{
			macFilterString = I18N("j", "Disabled");
			fontColor = "#000000";
		}*/
		var phone = false;
		if(model.DeviceType.toUpperCase() == "PHONE"){
			phone = true;
		}

		var objType = model.Type.toUpperCase();
		if (objType.indexOf("WIFI_") >= 0)
		{
			if(blocked)
			{
				icon = icon_base+"Wireless_Block_.png";
			}
			else
			{
				switch(objType)
				{
					case "WIFI_5G":
					case "WIFI_5G_GUEST":
						if(phone)
						{
							icon = icon_base+"phone_5g.png";
						}
						else
						{
							icon = icon_base+"wifi_5g.png";
						}
						break;
					case "WIFI_5G_2":
					case "WIFI_5G_2_GUEST":
						if(phone)
						{
							icon = icon_base+"phone_5g2.png";
						}
						else
						{
							icon = icon_base+"wifi_5g2.png";
						}
						break;
					default:
						if(phone)
						{
							icon = icon_base+"MobileDevice.png";
						}
						else
						{
							icon = icon_base+"wireless_icon.png";
						}
						break;
				}

			}
		}
		else if(objType == "OFFLINE")
		{
			ipv4Address = model.ReserveIP;
			if(blocked)
			{
				icon = icon_base+"Ethernet_Block_.png";
			}
			else
			{
				icon = icon_base+"EthernetClient_Disconnected.png";
			}
		}
		else
		{
			if(blocked)
			{
				icon = icon_base+"Ethernet_Block_.png";
			}
			else
			{
				icon = icon_base+"EthernetClient.png";
			}
		}
		//---------

		var vendor = model.Vendor;
		if(vendor.length > 22)
		{
			vendor = vendor.substr(0, 19) + "...";
		}

		var newClientDOM = $("#client_obj").clone();
		newClientDOM.attr("id","");
		newClientDOM.css("display", "inline-block");
		newClientDOM.find(".link_icon").attr("src", icon);
		if(objType.indexOf("WIFI_5G") >= 0)
		{
			newClientDOM.find(".link_icon").addClass("link_icon_band");
		}
		else
		{
			newClientDOM.find(".link_icon").removeClass("link_icon_band");
		}
		newClientDOM.find(".client_Name").html(name);
		var vendorBlock = newClientDOM.find(".client_Vender")
		vendorBlock.html(vendor);
		var ipv4Block = newClientDOM.find(".client_IPv4Address");
		ipv4Block.html(ipv4Address);
		if(model.IPv6Address != "")
		{
			vendorBlock.addClass("client_IPv6Offset");
			ipv4Block.addClass("client_IPv6Offset");
			newClientDOM.find(".client_IPv6Address").css("display", "inline-block").html(model.IPv6Address);
		}
		else
		{
			vendorBlock.removeClass("client_IPv6Offset");
			ipv4Block.removeClass("client_IPv6Offset");
			newClientDOM.find(".client_IPv6Address").hide();
		}
		if(model.MacfilterEnabled)
		{
			newClientDOM.find(".client_Access").css("display", "inline-block");
		}
		else
		{
			newClientDOM.find(".client_Access").hide();
		}
		
		//newClientDOM.find(".client_Access > span").css("font-color",fontColor).html(macFilterString);

		if(objType == "OFFLINE")
		{
			newClientDOM.find(".client_Name").addClass('client_Name_Offline');
		}
		
		if(objType.indexOf("WIFI_") < 0 )
		{
			newClientDOM.find(".client_signal").hide();
		}
		else
		{
			switch(model.SignalStrength)
			{
				case "1":
				case "2":
				case "3":
				case "4":
					var client_signal = newClientDOM.find(".client_signal");
					client_signal.children("img").attr("src", icon_base+"signal"+model.SignalStrength+".png");
					client_signal.show();
					break;
				default:
					newClientDOM.find(".client_signal").hide();
					break;					
			}
		}
		

		newClient = new Client(newClientDOM);
		
		return newClient;
	}
	
	ClientListView.prototype.generateNewHTML = function()
	{
		//Add tag
		var outputString = '<div class ="client_add_Tag"></div>';
		var newClient = new Client($(outputString));

		return newClient;
	}

	ClientListView.prototype.getClientView = function(model)
	{
		for(var i in this.lineList)
		{
			var line = this.lineList[i];
			for(var j in line.objList)
			{
				var client = line.objList[j];
				if(client.MacAddress == model.MacAddress)
				{
					return client;
				}	
			}
		}
		return null;
	}
	
	ClientListView.prototype.updateClientHTML = function(client, model)
	{
		var clientDOM = client.DOM;
		var name = model.NickName;
		var macFilterString = "";
		var fontColor;
		var icon_base = "../image/";
		var icon;
		var ipv4Address = model.IPv4Address;
		var vendor = model.Vendor;
		var blocked = false;

		//process data
		if (typeof(model.MacAddress) == "undefined" || model.MacAddress.length == 0)
		{
			return null;
		}
		if (name == "")
		{
			name = "Unknown";
		}

		/*if(model.MacfilterEnabled)
		{
			macFilterString = I18N("j", "Enabled");
			fontColor = "#FF0000";
			if((model.Type == "Blocked")||(model.Status))
			{
				blocked = true;
			}
		}
		else
		{
			macFilterString = I18N("j", "Disabled");
			fontColor = "#000000";
		}*/

		var phone = false;
		if(model.DeviceType.toUpperCase() == "PHONE"){
			phone = true;
		}

		var objType = model.Type.toUpperCase();
		if (objType.indexOf("WIFI_") >= 0)
		{
			if(blocked)
			{
				icon = icon_base+"Wireless_Block_.png";
			}
			else
			{
				switch(objType)
				{
					case "WIFI_5G":
					case "WIFI_5G_GUEST":
						if(phone)
						{
							icon = icon_base+"phone_5g.png";
						}
						else
						{
							icon = icon_base+"wifi_5g.png";
						}
						break;
					case "WIFI_5G_2":
					case "WIFI_5G_2_GUEST":
						if(phone)
						{
							icon = icon_base+"phone_5g2.png";
						}
						else
						{
							icon = icon_base+"wifi_5g2.png";
						}
						break;
					default:
						if(phone)
						{
							icon = icon_base+"MobileDevice.png";
						}
						else
						{
							icon = icon_base+"wireless_icon.png";
						}
						break;
				}

			}
		}
		else if(objType == "OFFLINE")
		{
			ipv4Address = model.ReserveIP;
			if(blocked)
			{
				icon = icon_base+"Ethernet_Block_.png";
			}
			else
			{
				icon = icon_base+"EthernetClient_Disconnected.png";
			}
		}
		else
		{
			if(blocked)
			{
				icon = icon_base+"Ethernet_Block_.png";
			}
			else
			{
				icon = icon_base+"EthernetClient.png";
			}
		}
		//---------

		var vendor = model.Vendor;
		if(vendor.length > 22)
		{
			vendor = vendor.substr(0, 19) + "...";
		}

		clientDOM.find(".link_icon").attr("src", icon);
		if(objType.indexOf("WIFI_5G") >= 0)
		{
			clientDOM.find(".link_icon").addClass("link_icon_band");
		}
		else
		{
			clientDOM.find(".link_icon").removeClass("link_icon_band");
		}
		clientDOM.find(".client_Name").html(name);
		var vendorBlock = clientDOM.find(".client_Vender")
		vendorBlock.html(vendor);
		var ipv4Block = clientDOM.find(".client_IPv4Address");
		ipv4Block.html(ipv4Address);
		if(model.IPv6Address != "")
		{
			vendorBlock.addClass("client_IPv6Offset");
			ipv4Block.addClass("client_IPv6Offset");
			clientDOM.find(".client_IPv6Address").css("display", "inline-block").html(model.IPv6Address);
		}
		else
		{
			vendorBlock.removeClass("client_IPv6Offset");
			ipv4Block.removeClass("client_IPv6Offset");
			clientDOM.find(".client_IPv6Address").hide();
		}
		if(model.MacfilterEnabled)
		{
			clientDOM.find(".client_Access").css("display", "inline-block");
		}
		else
		{
			clientDOM.find(".client_Access").hide();
		}
		//clientDOM.find(".client_Access > span").css("font-color",fontColor).html(macFilterString);

		if(objType == "OFFLINE")
		{
			clientDOM.find(".client_Name").addClass('client_Name_Offline');
		}
		else
		{
			clientDOM.find(".client_Name").removeClass('client_Name_Offline');
		}
		
		if(objType.indexOf("WIFI_") < 0 )
		{
			clientDOM.find(".client_signal").hide();
		}
		else
		{
			switch(model.SignalStrength)
			{
				case "1":
				case "2":
				case "3":
				case "4":
					var client_signal = clientDOM.find(".client_signal");
					client_signal.children("img").attr("src", icon_base+"signal"+model.SignalStrength+".png");
					client_signal.show();
					break;
				default:
					clientDOM.find(".client_signal").hide();
					break;					
			}
		}
	}
	
	ClientListView.prototype.deleteClientHTML = function(model){
		var client = this.getClientView(model);
		
		if(client != null){
			client.DOM.remove();
			client.line.remove(client);
		}
		
	}

	ClientListView.prototype.showEditData = function(model)
	{
		changeTimeoutAction();
		$("#check_btn").attr("class", "styled_button_dis").prop("disabled", true);

		$("#popTitle").html(I18N("j", "Edit Rule"));

		$("#show_Vendor").show();
		$("#client_MACAddress").show();
		$("#show_editMac").hide();
		$("#show_IPAdr").show();
		
		//insert data to pop form
		var ipString = model.IPv4Address;

		document.getElementById("client_IPAdrReserve").value = model.ReserveIP;
		document.getElementById("client_MACAddress").innerHTML = model.MacAddress;
		$("#client_Name").val(model.NickName);

		if(ipString == "")
		{
			ipString = I18N("j", "Not Available");
		}
		$("#client_IPAdr").html(ipString);
			
		if(model.ReserveIP == ""){
			document.getElementById("show_IPAdrReserve").style.display = "none";
			document.getElementById("show_IPAdr").style.display = "table-row";
			$("#enableReserveIP").prop("checked", false);
		}
		else{
			document.getElementById("show_IPAdrReserve").style.display = "table-row";
			document.getElementById("show_IPAdr").style.display = "none";
			$("#enableReserveIP").prop("checked", true);
		}

		if (model.Vendor != "")
		{
			document.getElementById("client_Vendor").innerHTML = model.Vendor;
		}
		else
		{
			document.getElementById("client_Vendor").innerHTML = "N/A";
		}

		if(model.MacfilterEnabled)
		{
			$("#schedule").selectbox('detach');
			$("#schedule").val(model.ScheduleName);
			$("#schedule").selectbox('attach');
			$("#enableAccess").prop("checked", true);
			$("#show_Schedule").show();
		}
		else
		{
			$("#enableAccess").prop("checked", false);
			$("#show_Schedule").hide();
		}

		$("#editPop").show();
	}

	ClientListView.prototype.showAddData = function()
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

		$("#enableAccess").prop("checked", false);
		$("#show_Schedule").hide();



		$("#editPop").show();
	}

	ClientListView.prototype.resetRulePOP = function()
	{
		resetRulePOP();
	}
	
	ClientListView.prototype.prepareUpdate = function()
	{
		for(var i in this.lineList)
		{
			var line = this.lineList[i];
			for(var j in line.objList)
			{
				line.objList[j].Live = false;
			}
		}	
	}
	
	ClientListView.prototype.clean = function()
	{
		var change = false;
		for(var i in this.lineList)
		{
			var line = this.lineList[i];
			for(var j in line.objList)
			{
				var client = line.objList[j];
				if(client.Live == false)
				{
					client.DOM.remove();
					line.objList.splice(j, 1);
					change = true;
				}
			}
			if(line.objList.length == 0)
			{
				line.DOM.remove();
				this.lineList.splice(i, 1);
				change = true;
			}
		}
		return change;
	}	
	
	ClientListView.prototype.refreshView = function()
	{
		for(var i in this.lineList)
		{
			//skip latest line
			if(i == (this.lineList.length - 1))
			{
				continue;
			}
			
			var line = this.lineList[i];
			if(line.objList.length < line.MaxCount)
			{
				var nextLine = this.lineList[parseInt(i)+1];
				if(nextLine.objList.length > 0)
				{
					var obj = nextLine.objList[0];
					line.objList.push(obj);
					
					nextLine.objList.splice(0,1);
					
					obj.DOM.detach();
					obj.DOM.appendTo(line.DOM);
				}		
			}
			
		}
	}

	function clearCreateRulePOP()
	{
		changeTimeoutAction();
		resetRulePOP();
	}

	function resetRulePOP()
	{
		//renew GUI
		//Get_ClientInfo().done(function(){
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
		//});
	}
	
	//global
	function setGlobalEvents(){
		$("#closeCreatePopBtn").off("click").click(function(){
			clearCreateRulePOP();
		});
		
	}
	setGlobalEvents();


	return ClientListView;
});