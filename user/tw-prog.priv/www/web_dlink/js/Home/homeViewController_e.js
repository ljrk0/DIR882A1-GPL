var Time_GetConnectionUpTime = null;
var homeDeviceInfo = null;

function events()
{
	var iconEvt = "click";
	if(isTouch()){
		iconEvt = "touchstart";
	}


	$("#internetIcon, #powerlineicon, #routerIcon, #clientIcon, #extenderIcon").on(iconEvt, function(){
		$(".sectionInfo").hide();
		changeTimeoutAction();
	});

	$("#internetIcon").on(iconEvt, function(){
		ToggleCircle("uplink_circle");
		$("#UplinkDeviceSection").show();
	});

	$("#powerlineicon").on(iconEvt, function(){
		ToggleCircle("powerline_circle");
		$("#PowerlineSection").show();
	});

	$("#routerIcon").on(iconEvt, function(){
		ToggleCircle("router_circle");
		$("#DeviceSection").show();
		//deviceInfoLayout();
	});

	$("#clientIcon").on(iconEvt, function(){
		ToggleCircle("client_circle");
		$("#ClientSection").show();
	});

	$("#extenderIcon").on(iconEvt, function(){
		ToggleCircle("client_circle");
		$("#ExtenderSection").show();
		getExtenderList();
	});

	if(isTouch()){
		iconEvt = "touchend";
	}
	$("#goto_wizard, #goto_wizard2").on(iconEvt, function(){
		$(".sectionInfo").hide();
		ToggleCircle(null);
		initWizard(false);

		//clear hometimer
		clearInterval(hometimer);
		clearInterval(hometimer2);
		clearInterval(hometimer3);

		hometimer = null;
		hometimer2 = null;
		hometimer3 = null;
	});
}

function showUplinkDeviceInfo(deviceInfo)
{
	//Internet
	switch(deviceInfo.uplink.UplinkInterface.toUpperCase())
	{
		case "WLAN":
			$("#UplinkConnType").html(I18N("j", "Wi-Fi"));

			var ssid = deviceInfo.apcSettings.SSID;
			if(ssid == "")
			{
				ssid = I18N("j", "Not Available");
			}
			$("#WiFiSSID").html(HTMLEncode(ssid));
			var key = deviceInfo.apcSettings.Key;
			if(key == ""){
				key = I18N("j", "Not Available");
			}
			$("#WiFiKey").html(HTMLEncode(key));

			$("#Uplink_SSID_Row, #Uplink_PWD_Row").show();
			break;
		default:
			$("#UplinkConnType").html(I18N("j", "Ethernet"));

			$("#Uplink_SSID_Row, #Uplink_PWD_Row").hide();
			break;
	}


	//show
	if(deviceInfo.hasInternet)
	{
		GetConnectionUpTime();
		show_uplink_connected(deviceInfo);
		CheckNewFirmware();
	}
	else
	{
		clearInterval(Time_GetConnectionUpTime);
		$("#UpTime").html(I18N("j", "Not Available"));
		show_uplink_disconnected(deviceInfo);
	}
}

function showRuntimeLanInfo(deviceInfo)
{
	var lanSettings = deviceInfo.lanSettings;

	var wifiInfo = deviceInfo.wifi_datalist.slice(0,1)[0];
	$("#MacAddress").html(wifiInfo.setting.MacAddress.toUpperCase());

	var ip = lanSettings.IPAddr;
	if (ip == "")
	{
		ip = I18N("j", "Not Available");
	}
	$("#IPAddress").html(ip);
	
	var mask = lanSettings.SubnetMask;
	if (mask == "")
	{
		mask = I18N("j", "Not Available");
	}
	$("#SubnetMask").html(mask);

	var gateway = lanSettings.Gateway;
	if (gateway == "")
	{
		gateway = I18N("j", "Not Available");
	}
	$("#Gateway").html(gateway);

	
	var dns1 = lanSettings.PrimaryDNS;
	if (dns1 == "")
	{
		dns1 = I18N("j", "Not Available");
	}
	$("#PrimaryDNS").html(dns1);
	
	var dns2 = lanSettings.SecondaryDNS;
	if (dns2 == "")
	{
		dns2 = I18N("j", "Not Available");
	}
	$("#SecondaryDNS").html(dns2);
}

function show_ipv6_info(deviceInfo){
	var v6settings = deviceInfo.ipv6settings;

	var mode;
	var llr_only = false;
	switch(v6settings.IPv6_ConnectionType.toUpperCase())
	{
		case "IPV6_STATIC":
			mode = I18N("j", "Static IPv6");
			break;
		case "IPV6_AUTOCONFIGURATION":
			mode = I18N("j", "Auto Configuration (SLAAC/DHCPv6)");
			break;
		default:
			mode = I18N("j", "Local Connectivity Only");
			llr_only = true;
			break;
	}
	$("#IPv6Mode").html(mode);

	$("#IPv6LinkLocal").html(v6settings.IPv6_LinkLocalAddress.toUpperCase());

	if(llr_only)
	{
		$("#line_v6addr, #line_v6gw, #line_v6dns1, #line_v6dns2").hide();
	}
	else
	{	
		var ip = v6settings.IPv6_Address;
		if (ip == "")
		{
			ip = I18N("j", "Not Available");
		}
		$("#IPv6Address").html(ip);

		var gateway = v6settings.IPv6_DefaultGateway;
		if (gateway == "")
		{
			gateway = I18N("j", "Not Available");
		}	
		$("#IPv6Gateway").html(gateway);
		
		var dns1 = v6settings.IPv6_PrimaryDNS;
		if (dns1 == "")
		{
			dns1 = I18N("j", "Not Available");
		}
		$("#IPv6DNSServer").html(dns1);
		var dns2 = v6settings.IPv6_SecondaryDNS;
		if (dns2 == "")
		{
			dns2 = I18N("j", "Not Available");
		}
		$("#IPv6DNSServer2").html(dns2);

		$("#line_v6addr, #line_v6gw, #line_v6dns1, #line_v6dns2").show();
	}


}

function GetConnectionUpTime()
{		
	var connUpTime = new SOAPGetInternetConnUpTimeResponse();
	var soapAction = new SOAPAction();
	soapAction.sendSOAPAction("GetInternetConnUpTime", null, connUpTime).done(function(obj)
	{
		ipv4_connTime = parseInt(connUpTime.UpTime, 10);
		clearInterval(Time_GetConnectionUpTime);
		Time_GetConnectionUpTime = setInterval("runConnUpTime()", 1000);
	}).fail(function(){
		$("#UpTime").html(I18N("j", "Not Available"));
	});
}

function runConnUpTime()
{
	var sec = 0;
	var min = 0;
	var hour = 0;
	var day = 0;	
	var upTime = ipv4_connTime;
	
	if (upTime > 0)
	{
		sec = upTime % 60;
		min = Math.floor(upTime / 60) % 60;
		hour = Math.floor(upTime / 3600) % 24;
		day = Math.floor(upTime / 86400);
		upTime++;	
	}
	if(hour < 10)
	{
		hour = "0"+hour.toString();
	}
	if(min < 10)
	{
		min = "0"+min.toString();
	}
	if(sec < 10)
	{
		sec = "0"+sec.toString();
	}
	var timeString = day + " " + I18N("j", "Day") + ",&nbsp;&nbsp;" + hour + " : " + min + " : " + sec;
	
	$("#UpTime").html(timeString);
	ipv4_connTime = upTime;
}

function show_ap_info(deviceInfo)
{
	for(var i in deviceInfo.wifi_datalist)
	{
		var wifiSetting = wifi_datalist[i];
		//smart connect or covr wifi
		if((deviceInfo.smartconnect)&&(wifiSetting.band != "24"))
		{
			continue;
		}

		if((deviceInfo.covrwifi)&&(wifiSetting.band != "24"))
		{
			continue;
		}


		show_wireless_info(wifiSetting.band, wifiSetting.setting, wifiSetting.security);
	}

	//smart connect or covr wifi
	if(deviceInfo.smartconnect || deviceInfo.covrwifi)
	{
		$("#Wi-Fi_24_show").html(I18N("j", "Wi-Fi"));
		$("#Wi-Fi_5").hide();
	}
}

function show_wireless_info(radioID, settingObj, securityObj)
{
	var enabled = COMM_ToBOOL(settingObj.Enabled);
	$("#ExtendWiFiStatus_"+radioID).html(enabled);

	var status;
	if((settingObj.Enabled == "false")||(settingObj.RadioEnabled == "false"))
	{
		status = I18N("j", "Disabled");

		if(settingObj.Enabled == "false")
		{
			$("#ExtendWiFiStatus_" + radioID).html(status);
			$("#ExtendWiFiSSID_" + radioID).html(I18N("j", "Not Available"));
			$("#ExtendWiFiKey_" + radioID).html(I18N("j", "Not Available"));
			return;
		}
	}
	else
	{
		status = I18N("j", "Enabled");
	}

	$("#ExtendWiFiStatus_" + radioID).html(status);
	$("#ExtendWiFiSSID_" + radioID).html(HTMLEncode(settingObj.SSID)).attr("title", settingObj.SSID);
	$("#ExtendWiFiKey_" + radioID).html(HTMLEncode(securityObj.Key));

	if (securityObj.Enabled == "false")
	{
		$("#ExtendWiFiKey_" + radioID).html('"' + I18N("j", "Unsecured") + '"')
		.attr("class", "alertInfo_label")
		.on("click", function(){
			self.location.href = "Extender_e.html";
		});
		$("#alert_Info_"+radioID).css("display", "inline");
	}
	else
	{
		$("#ExtendWiFiKey_" + radioID).html(HTMLEncode(securityObj.Key)).attr("title", securityObj.Key);
	}
}

function CheckNewFirmware()
{
	var CurrentFW = sessionStorage.getItem('currentFWVersion');
	var NewFW = sessionStorage.getItem('newFWVersion');
	if (parseFloat(NewFW) > parseFloat(CurrentFW))
	{
		$("#map_fw_info").delay(200).fadeIn(1500);

		if (localStorage.getItem("ignoreFW") != sessionStorage.getItem('newFWVersion'))
		{
			ShowNewFWSection(CurrentFW.split("."), NewFW.split("."));
		}
	}

	return "";
}

function ShowNewFWSection(current_fw, latest_fw)
{
	var newFW = false;
	
	if (parseInt(latest_fw[0]) > parseInt(current_fw[0]))
	{
		newFW = true;
	}
	else if (parseInt(latest_fw[0]) == parseInt(current_fw[0]))
	{
		if (parseInt(latest_fw[1]) > parseInt(current_fw[1]))
		{
			newFW = true;
		}
	}
			
	if (newFW)
	{
		$("#NewFWSection").show();
		$("#CreatePopAlertMessage").show();
	}
}

//show_plc_info
function show_plc_info(deviceInfo){
	var plcinfosettings = deviceInfo.plcinfo;
	var PLCMAC;
	var DAK;
	var PLCStationNumber;

	if(plcinfosettings.PLCMAC == ""){
		PLCMAC = I18N("j", "Not Available");
	}else{
		PLCMAC = plcinfosettings.PLCMAC;
	}

	if(plcinfosettings.DAK == ""){
		DAK = I18N("j", "Not Available");
	}else{
		DAK = plcinfosettings.DAK;
	}

	if(plcinfosettings.PLCStationNumber == ""){
		PLCStationNumber = "0";
	}else{
		PLCStationNumber = plcinfosettings.PLCStationNumber;
	}


	$("#PowerLine_MacAddress").html(PLCMAC);
	$("#PowerLine_Key").html(DAK);
	$("#Total_PLCs").html(PLCStationNumber);

	if(parseInt(PLCStationNumber) <= 0 || parseInt(PLCStationNumber) == NaN){
		$('#internet_warm2').show();
	}else{
		$('#internet_warm2').hide();
	}


	$("#PowerLine").show();
}


function IngoreNewFWMsg()
{
	localStorage.setItem("ignoreFW", sessionStorage.getItem('newFWVersion'));
	$("#CreatePopAlertMessage").hide();
}	


function initHomeViewController(init){

	if(init){
		events();
		LoadMap(null);
		setTimeout(function(){$('#uplink_circle').trigger('click');}, 1500);
	}
	
	//var initnow = init;

	(function(init){

		getDeviceInfo(homeDeviceInfo).done(function(deviceInfo){

			showUplinkDeviceInfo(deviceInfo);

			showRuntimeLanInfo(deviceInfo);

			show_ap_info(deviceInfo);

			show_ipv6_info(deviceInfo);

			//plc
			if(currentDevice.featurePLC_e){
				show_plc_info(deviceInfo);
			}

			if(init != true){
				if(currentDevice.featurePLC_e == true && currentDevice.featureCovrWIFI_e == true){
					if(homeDeviceInfo.WiFiSONRole != deviceInfo.WiFiSONRole){
						sessionStorage.setItem("WiFiSONRole", deviceInfo.WiFiSONRole);
						LoadMap(deviceInfo);
					}
				}
			}else{
				if(sessionStorage.getItem('WiFiSONRole') === null || sessionStorage.getItem('WiFiSONRole') === ""){
					sessionStorage.setItem("WiFiSONRole", deviceInfo.WiFiSONRole);
					if(deviceInfo.WiFiSONRole != "Master") {
						LoadMap(deviceInfo);
					}
				}else{
					var sessWifisonrole = sessionStorage.getItem('WiFiSONRole');
					if(sessWifisonrole != deviceInfo.WiFiSONRole){
						sessionStorage.setItem("WiFiSONRole", deviceInfo.WiFiSONRole);
						LoadMap(deviceInfo);
					}
				}
			}

			homeDeviceInfo = deviceInfo;

		});
	})(init);


}
