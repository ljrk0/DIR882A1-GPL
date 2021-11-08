
function LoadMap(deviceInfo) 
{
	//if init
	if(deviceInfo == null){
		var deviceInfo = {};
		deviceInfo.covrwifi = false;
		if(sessionStorage.getItem('WiFiSONRole') === null || sessionStorage.getItem('WiFiSONRole') === ""){
			deviceInfo.WiFiSONRole = "Master";
		}else{
			var sessWifisonrole = sessionStorage.getItem('WiFiSONRole');
			deviceInfo.WiFiSONRole = sessWifisonrole;			
		}
	};

	if(currentDevice.featurePLC_e == true && currentDevice.featureCovrWIFI_e == true){
		//if(deviceInfo == null || deviceInfo.covrwifi == false){
		if(deviceInfo.WiFiSONRole == "Master"){
			$("#ConnStatusIcon2, #goto_wizard2, #line_uplink2").hide();
			$('#map').hide();
			$('#map').css({'width':'695px','height':'350px'});
			$('#internetIcon').css({'position':'absolute','top':'100px','left':'0px','z-index':99});
			$('#powerlineicon').css({'position':'absolute','top':'200px','left':'460px','z-index':99});
			$('#ConnStatusIcon').css({'position':'absolute','top':'148px','left':'164px','z-index':99});
			$('#line_uplink').css({'position':'absolute','top':'156px','left':'90px','z-index':1});
			$('#goto_wizard').css({'position':'absolute','top':'183px','left':'108px','z-index':99});
			//$('#ConnStatusIcon2').css({'position':'absolute','top':'85px','left':'369px','z-index':99});
			//$('#line_uplink2').css({'position':'absolute','top':'94px','left':'290px','z-index':1});
			//$('#goto_wizard2').css({'position':'absolute','top':'100px','left':'326px','z-index':99});
			$('#routerIcon').css({'position':'absolute','top':'100px','left':'250px','z-index':99});
			$('#line_downlink').css({'width':'300px','position':'absolute','top':'156px','left':'340px','z-index':1});
			$('#clientIcon').css({'position':'absolute','top':'0px','left':'460px','z-index':99});
			$('#router_circle > img').attr('src', 'image/DHP-W730AV.png');
			$('#extenderIcon').css({'position':'absolute','top':'100px','left':'577px','z-index':99});
			$('#line_downlink2').css({'position':'absolute','top':'73px','left':'280px','z-index':1});
			$('#line_downlink3').css({'position':'absolute','top':'156px','left':'280px','z-index':1});		
		}else{
			$('#map, #line_downlink2, #line_downlink3, #extenderIcon, #goto_wizard, #goto_wizard2').hide();
			$('#map').hide();
			$('#map').css({'width':'810px','height':'237px'});
			$('#internetIcon').css({'position':'absolute','top':'17px','left':'0px','z-index':99});
			$('#powerlineicon').css({'position':'absolute','top':'17px','left':'230px','z-index':99});
			$('#ConnStatusIcon').css({'position':'absolute','top':'65px','left':'154px','z-index':99});
			$('#line_uplink').css({'position':'absolute','top':'74px','left':'75px','z-index':1});
			$('#goto_wizard').css({'position':'absolute','top':'100px','left':'98px','z-index':99});
			$('#ConnStatusIcon2').css({'position':'absolute','top':'65px','left':'381px','z-index':99});
			$('#line_uplink2').css({'position':'absolute','top':'74px','left':'290px','z-index':1});
			$('#goto_wizard2').css({'position':'absolute','top':'100px','left':'326px','z-index':99});
			$('#routerIcon').css({'position':'absolute','top':'17px','left':'460px','z-index':99});
			$('#line_downlink').css({'width':'190px','position':'absolute','top':'74px','left':'530px','z-index':1});
			$('#clientIcon').css({'position':'absolute','top':'17px','left':'690px','z-index':99});			
			$('#router_circle > img').attr('src', 'image/DHP-W730AV.png');
		}
	}

	$("#map").delay(200).fadeIn(1500);
	$("#map_wording, #map_info, #internetIcon, #routerIcon, #clientIcon").delay(200).fadeIn(1500);
	
	if(currentDevice.featurePLC_e == true && currentDevice.featureCovrWIFI_e == true){
		if(deviceInfo.WiFiSONRole == "Master"){
			$("#line_downlink2, #line_downlink3, #extenderIcon").delay(500).fadeIn(1500);
			$('#powerlineicon').delay(200).fadeIn(1500);
		}else{
			$('#line_uplink2').delay(200).fadeIn(1500);
			$('#powerlineicon, #ConnStatusIcon2').delay(500).fadeIn(1500);
		}
	}


	$("#line_uplink, #line_downlink").delay(500).fadeIn(1500);
	$("#ConnStatusIcon").delay(500).fadeIn(1500);



	sleep(600).done(function(){
		topologyLayout("internetIcon");
		topologyLayout("powerlineicon");
		topologyLayout("routerIcon");
		topologyLayout("clientIcon");
		topologyLayout("extenderIcon");
	});
}	

function ToggleCircle(whichCircle)
{
	$(".circle_icon.active").removeClass("active");
	if(whichCircle != null)
		$("#"+whichCircle).addClass("active");
}

function show_uplink_disconnected(deviceInfo){

	$("#map_status").attr("src", "image/internet_status_off.png");
	$("#map_title").html(I18N("j", "Existing Network Disconnected"));
	$("#ConnStatus").attr("src", "image/disconnected.png");	

	$("#UplinkRouter").attr("src", "image/eth_uplink_disconnect.png");

	$("#UplinkStatus").html(I18N("j", "Disconnected"));

	$("#internet_warm").show();

	//powerline
	if(currentDevice.featurePLC_e == true && currentDevice.featureCovrWIFI_e == true){
		if(deviceInfo.WiFiSONRole == "Master"){
			$("#ConnStatusIcon2, #goto_wizard2").hide();
			$("#goto_wizard").delay(1000).fadeIn(200);
		}else{
			if(parseInt(deviceInfo.plcinfo.PLCStationNumber) <=0 || deviceInfo.plcinfo.PLCStationNumber == "" || deviceInfo.plcinfo == ""){
				// $("#goto_wizard").hide();
				$("#map_title").html(I18N("j", "Powerline Disconnected"));
				$("#ConnStatus2").attr("src", "image/disconnected.png");
				// $("#goto_wizard2").delay(1000).fadeIn(200);
			}else{
				// $("#goto_wizard2").hide();
				$("#ConnStatus2").attr("src", "image/connected.png");
				// $("#goto_wizard").delay(1000).fadeIn(200);
			}

			$("#goto_wizard, #goto_wizard2").hide();
		}
	}else{
		$("#goto_wizard").delay(1000).fadeIn(200);
	}	
}

function show_uplink_connected(deviceInfo){
	$("#map_status").attr("src", "image/internet_status_on.png");
	$("#map_title").html(I18N("j", "Existing Network Connected"));
	$("#ConnStatus").attr("src", "image/connected.png");		

	$("#UplinkRouter").attr("src", "image/eth_uplink_connect.png");

	$("#UplinkStatus").html(I18N("j", "Connected"));

	$("#internet_warm, #goto_wizard").hide();

	//powerline
	if(currentDevice.featurePLC_e == true && currentDevice.featureCovrWIFI_e == true){
		if(deviceInfo.WiFiSONRole == "Master"){
			$("#ConnStatusIcon2, #goto_wizard2").hide();
		}else{
			$("#goto_wizard2").hide();
			$("#ConnStatus2").attr("src", "image/connected.png");
		}
	}
}

function deviceInfoLayout(){

	var v4H = $("#ipv4_block").height();
	//$("#wifi24_block").css("min-height", v4H);

	var leftH = v4H+ $("#ipv6_block").height();
	var rightH = $("#router_right").height();

	if(leftH < rightH)
	{
		v4H = $("#ipv4_block").outerHeight(true);
		$("#ipv6_block").css("height", rightH-v4H);
	}

}

function topologyLayout(iconID){
	var icon = $("#"+iconID);
	var iconName = $("#"+iconID+" > .IconName");
	var width = iconName.width();
	var iconWidth = icon.width();

	var left = iconWidth/2 - width/2;
	iconName.css({"left":left});

}