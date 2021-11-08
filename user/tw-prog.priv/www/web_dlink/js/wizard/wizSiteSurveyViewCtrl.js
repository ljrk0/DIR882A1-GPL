
define(['wizPath/wizardViewController', 'wizPath/wizardModel', 'wizPath/wizardView', 'wizPath/wizSiteSurveyView'], function(WizViewCtrl, WizModel, WizView, WizSiteSurveyView){
	var dtd;
	var deviceInfo;
	var spinnerObj;
	var stopFlag = false;
	var siteSelected = null;
	var siteListCache;
	var pageStack = [];
	var validator = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		$("#wizard_title").html(I18N("j", "Install")+": "+I18N("j", "Select a Wi-Fi network to extend"));

		
		$("#RTToDUTRight").attr("src", "image/wireless_on.gif");
		$("#RTToDUTLeft").attr("src", "image/wireless_on_180.gif");

		//events
		$("#wizard4_refreshBtn").off("vclick").on("vclick", function(){
			pageStack = [];
			_doScaning();
		});

		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});
		$("#wizButton_back").off("click").on("click", function(){
			_back();
		});;

		//jump page
		$("#Wizard4").show();
		showViews();

		return dtd.promise();
	}

	function showViews(){
		var page = pageStack.pop();
		if(page == "pwd"){
			_enterPassword(siteSelected);
		}
		else if(page == "survey"){
			_showSurveyResult(siteListCache);
		}
		else{
			_doScaning();
		}
	}

	function _hideViews(){
		$("#Wizard4SiteSurveySection, #wizard4_scan, #wizard4_survey_result, #Wizard4ManualSection").hide();	
		WizView.initView();
	}

	function _doScaning(){
		_hideViews();
		spinnerObj = CreateSpinnerObj("SurveySpinner", opts);
		$("#wizard4_scan_text").html(I18N("j", "Scanning..."));
		$("#Wizard4SiteSurveySection").show();

		$("#wizButton_back").hide();
		$("#wizButton_next").hide();
		
		$("#wizard4_scan").show();
		doSiteSurvey().done(function(obj){
			if(!stopFlag){	//not a good solution
				siteListCache = obj;
				_showSurveyResult(obj);
			}
		});
	}

	function _showSurveyResult(siteList){
		_hideViews();
		$("#wizButton_back").show();
		$("#wizButton_next").show().html(I18N("j", "Manual")).off("click").on("click", function(){
			pageStack.push("survey");
			siteSelected = null;
			_enterPassword(null);
		});
		$("#Wizard4SiteSurveySection").show();
		$("#wizard4_survey_result").show();

		var table = $("#SurveyListTable");
		table.html("");
		for(var i in siteList)
		{
			var site = siteList[i];
			var siteHTML = '<button type="button" class="list-group-item">'+HTMLEncode(site.SSID);
			siteHTML += '<span class="stationInfo">';
			siteHTML += _getSecurityIcon(site.SupportedSecurity.SecurityInfo[0].SecurityType);
			siteHTML += _getSignalStrengthIcon(parseInt(site.SignalStrength));
			siteHTML += '</span></button>';

			var siteDOM = $(siteHTML);
			_setSiteEvent(siteDOM, site);
			table.append(siteDOM);
		}
		WizSiteSurveyView.initView();
	}

	function _setSiteEvent(dom, model){
		dom.on("vclick", function(e){
			e.preventDefault();
			changeTimeoutAction();
			_selectAP(model);
		});
	}

	function _selectAP(site){
		deviceInfo.apcSSID = site.SSID;
		
		pageStack.push("survey");

		if((siteSelected == null)||(siteSelected.SSID != site.SSID)){
			$("#Wizard4_Key").val("");
		}
		siteSelected = site;

		if(site.SupportedSecurity.SecurityInfo[0].SecurityType.toUpperCase() != "NONE")
		{
			_enterPassword(site);
		}
		else
		{
			_manualNext();
		}
	}

	function _enterPassword(site){
		_hideViews();
		$("#wizButton_next").show().html(I18N("j", "Next")).off("click").on("click", function(){
			$('#Wizard4_form').submit();
		});
		$("#wizButton_back").show();

		$("#Wizard4ManualSection").show();

		if(site != null)
		{
			$("#Wizard4Manual_desc").html(I18N("j", "Please enter the Wi-Fi password for your existing Wi-Fi network"));
			$("#Wizard4_autoSSID").html(HTMLEncode(site.SSID)).show();
			$("#Wizard4_SSID").hide();
			
			if(isTouch() == false)
			{
				$("#Wizard4_Key").focus();
			}
		}
		else
		{
			$("#Wizard4Manual_desc").html(I18N("j", "Please enter Wireless Network Name, Security and Password of the existing Wi-Fi network."));
			$("#Wizard4_SSID").show();
			$("#Wizard4_autoSSID").hide();
			$("#Wizard4_Key").val("");
			
			if(isTouch() == false)
			{
				$("#Wizard4_SSID").focus();
			}
		}
		_setValidate();
	}

	function _doChecking(){
		_hideViews();
		spinnerObj = CreateSpinnerObj("SurveySpinner", opts);
		$("#wizard4_scan_text").html(I18N("j", "Please wait")+" ...");
		$("#Wizard4SiteSurveySection").show();

		$("#wizButton_back").hide();
		$("#wizButton_next").hide();
		
		$("#wizard4_scan").show();

		var setting = createAPCSettings(deviceInfo.apcSSID, deviceInfo.apcPWD);
		var soapAction = new SOAPAction();
		soapAction.SetMultipleSOAP("SetAPClientSettings", setting, null);

		soapAction.SendMultipleSOAPAction("SetMultipleActions").done(function(obj){
			//console.log(obj);
			//console.log(obj.SetAPClientSettingsResult);
		});
	}

	function _storeData(){
		if(siteSelected == null){	// by manual
			deviceInfo.apcSSID = $("#Wizard4_SSID").val();
		}
		deviceInfo.apcPWD = $("#Wizard4_Key").val();
		
		//_doChecking();
	}

	function _manualNext(){
		_storeData();

		dtd.resolve("wifi");
	}

	function _back(){
		if(pageStack.length == 0){
			dtd.resolve("back");
		}
		else
		{
			showViews();
		}
	}

	function _getSecurityIcon(type)
	{
		var img_str = "";
		
		if (type.toUpperCase() != "NONE")
		{
			img_str = "<img src='image/wisp_security.png' width='15' height='21' />";
		}
		
		return img_str;
	}

	function _getSignalStrengthIcon(strength)
	{
		var img_str = "";
		var img_src = "";
		
		if(strength <= 35)
		{
			img_src = "wisp_strength_low.png";
		}
		else if((strength > 35) && (strength <= 70))
		{
			img_src = "wisp_strength_medium.png";
		}
		else if( strength > 70)
		{
			img_src = "wisp_strength_high.png";
		}
		
		img_str = "<img src='image/" + img_src + "' width='25' height='20' />";
		
		return img_str;
	}

	function _closeWizard(){
		if(currentDevice.featureCovrWIFI_e) {
			$('#WizardSection').hide();
				$('#CreatePopAlertMessage_wizard').show();

				$('.closewizard_cancel').on('click',function(){
					$('#WizardSection').show();
					$('#CreatePopAlertMessage_wizard').hide();
				});

				$('.closewizard_ok').on('click',function(){
					$('#CreatePopAlertMessage_wizard').hide();

					//start timer
					if(hometimer == null){
						initHomeViewController(false);
						hometimer = setInterval("initHomeViewController(false)",5000);
					}
					if(hometimer2 == null){
						Get_ClientInfo();
						hometimer2 = setInterval("Get_ClientInfo()",6000);
					}
					if(hometimer3 == null){
						GetXMLPLC();
						hometimer3 = setInterval("GetXMLPLC()",6000);
					}
					
					spinnerObj.stop();
					stopFlag = true;
					dtd.reject();
				});			
			} else {
				//start timer
				if(hometimer == null){
					initHomeViewController(false);
					hometimer = setInterval("initHomeViewController(false)",5000);
				}
				if(hometimer2 == null){
					Get_ClientInfo();
					hometimer2 = setInterval("Get_ClientInfo()",6000);
				}
				if(hometimer3 == null){
					if(currentDevice.featurePLC_e) {
						GetXMLPLC();
						hometimer3 = setInterval("GetXMLPLC()",6000);
					}
				}
				
				spinnerObj.stop();
				stopFlag = true;
				dtd.reject();
			}
	
	}

	function _setValidate()
	{
		if(validator != null){
			validator.resetForm();
		}

		// Check Is WPA
		$.validator.addMethod("checkASCIIChar", function( value, element ) {
			//no security
			if(value.length = 0)
			{
				return true;
			}
			return CheckASCII(value);
		}, I18N("j","Text field contains illegal characters."));

		$.validator.addMethod("checkWPAPassword", function( value, element ) {
			if(siteSelected != null){	// not by manual
				//WPA
				if (value.length <= 63 && value.length >= 8)
				{
					return true;
				}
				return false;
			}

			//no security
			if(value.length == 0)
			{
				return true;
			}
			else if (value.length <= 63 && value.length >= 8)
			{
				return true;
			}
			return false;
		}, I18N("j","Your password must be between 8-63 characters length"));
		
		// check wifi ssid
		$.validator.addMethod("checkssid", function( value, element )
		{	
			if(value == "")
			{
				return false;
			}
			return true;
		}, I18N("j", "Please enter a Wi-Fi Name(SSID)."));
		
		validator = $('#Wizard4_form').validate({
			rules: {
				Wizard4_SSID: {
					checkssid: true
				},
				Wizard4_Key: {
					checkASCIIChar: true,
					checkWPAPassword: true
				}
			},
			submitHandler: function(form) {
				pageStack.push("pwd");
				_manualNext();
				return false;
			},
			skipBr: function(element){
				var div = element.parent("div");

				if(div.length == 0)
				{
					return false;
				}
				return true;
			}
		});

	};

	//public
	return {
		init: function(device){
			return initViewCtrl(device);
		}
	}

});