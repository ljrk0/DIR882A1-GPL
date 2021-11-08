function renewWanSettings_2(status, wanSettings_2) {
	if(wanSettings_2.Type == "") {
		$('#index_ShowConnectionType_2').html(I18N('j','Not Available'));
	} else {
		$('#index_ShowConnectionType_2').html(I18N("j", wanSettings_2.Type));
	}

	if(status == "CONNECTED") {
		$('#index_ShowNetworkStatus_2').html(I18N('j','Connected'));
		GetConnectionUpTime(null);
	} else {
		$('#index_ShowNetworkStatus_2').html(I18N('j', 'Disconnected'));
		clearInterval(Time_GetConnectionUpTime_2);
		connTime_2 = 0;
		runConnUpTime(null);
	}

	if(wanSettings_2.IPAddress == "") {
		$('#index_ShowIPAddress_2').html(I18N('j','Not Available'));
	} else {
		$('#index_ShowIPAddress_2').html(wanSettings_2.IPAddress);
	}

	if(wanSettings_2.SubnetMask == "") {
		$('#index_ShowSubnetMask_2').html(I18N('j','Not Available'));
	} else {
		$('#index_ShowSubnetMask_2').html(wanSettings_2.SubnetMask.toUpperCase());
	}

	if(wanSettings_2.Gateway == "") {
		$('#index_ShowDefaultGateway_2').html(I18N('j','Not Available'));
	} else {
		$('#index_ShowDefaultGateway_2').html(wanSettings_2.Gateway);
	}

	if(wanSettings_2.RuntimeDNS.Primary == "") {
		$('#index_ShowPrimaryDNS_2').html(I18N('j','Not Available'));
	} else {
		$('#index_ShowPrimaryDNS_2').html(wanSettings_2.RuntimeDNS.Primary);
	}

	if(wanSettings_2.RuntimeDNS.Secondary == "") {
		$('#index_ShowSecondaryDNS_2').html(I18N('j','Not Available'));
	} else {
		$('#index_ShowSecondaryDNS_2').html(wanSettings_2.RuntimeDNS.Secondary);
	}

	var btn = $('#RenewRelease_btn_2');
	switch(status) {
		case "CONNECTED":
			btn.html(I18N("j", "Disconnect")).css("display", "inline");
			btn.attr("class", "styled_button_s").prop("disabled", false)
			.off("click").click(function(){
				WANConnectionDisconnect_2(wanSettings_2.Type);
			});
			break;
		case "CONNECTING":
			btn.css("display", "inline");
			btn.attr("class", "styled_button_dis").prop("disabled", true);
			btn.html(I18N("j", "Connecting"));
			break;				
		case "DISCONNECTED":
		case "LIMITED_CONNECTION":
			btn.css("display", "inline");
			btn.html(I18N("j", "Connect"));
			btn.attr("class", "styled_button_s").prop("disabled", false)
			.off("click").click(function(){
				WANConnectionConnect_2(wanSettings_2.Type);
			});
			break;
	}
}

function WANConnectionConnect_2(type) {
	$("#RenewRelease_btn_2").attr("class", "styled_button_dis").prop("disabled", true);
	var renewType = "";
	click_Statuschanged();
	var renewWan2Connection = new SOAPRenewWanConnection();
	var soapAction = new SOAPAction();
	soapAction.timeout = 5000;

	switch (type) {
		case "DHCP":
			renewType = "DHCPRenew";
			break;
		case "PPPoE":
			renewType = "PPPoEConnect";
			break;
		case "PPTP":
			renewType = "PPTPConnect";
			break;
		case "L2TP":
			renewType = "L2TPConnect";
			break;
		case "3G":
			renewType = "3GConnect";
			break;
	}
	renewWan2Connection.Action = renewType;
	var result = soapAction.sendSOAPAction("RenewWan2Connection", renewWan2Connection, null);
	result.done(function(obj) {
		Time_ResetButton = setTimeout("resetRenewReleaseBtn_2()", 10000);
	}).fail(function() {
		$("#RenewRelease_btn_2").attr("class", "styled_button_s").prop("disabled", false);
	});
}

function WANConnectionDisconnect_2(type) {
	$("#RenewRelease_btn_2").attr("class", "styled_button_dis").prop("disabled", true);
	click_Statuschanged();
	var renewType = "";
	var renewWan2Connection = new SOAPRenewWanConnection();
	var soapAction = new SOAPAction();
	soapAction.timeout = 5000;
	switch (type)
	{
		case "DHCP":
			renewType = "DHCPRelease";
			break;
		case "PPPoE":
			renewType = "PPPoEDisconnect";
			break;
		case "PPTP":
			renewType = "PPTPDisconnect";
			break;
		case "L2TP":
			renewType = "L2TPDisconnect";
			break;
		case "3G":
			renewType = "3GDisconnect";
			break;
	}
	renewWan2Connection.Action = renewType;
	var result = soapAction.sendSOAPAction("RenewWan2Connection", renewWan2Connection, null);
	result.done(function(obj) {
		Time_ResetButton = setTimeout("resetRenewReleaseBtn_2()", 10000);
	}).fail(function() {
		$("#RenewRelease_btn_2").attr("class", "styled_button_s").prop("disabled", false);
	});
}

function resetRenewReleaseBtn_2() {
	wanStatus_2 = new SOAPGetWanStatusResponse();
}
