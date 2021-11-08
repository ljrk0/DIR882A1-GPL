
define(['wizPath/wizardViewController', 'wizPath/wizardModel', 'wizPath/wizardView'], function(WizViewCtrl, WizModel, WizView){
	var dtd;
	var deviceInfo;
	var uiTimer = null;
	var statusTimer = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		$("#RetryBtn").off("click").on("click", function(){
			_runWPS();
			$(this).hide();
		}).hide();

		_runWPS();

		$("#DUTToClientRight").attr("src", "image/wireless_on.gif");
		$("#DUTToClientLeft").attr("src", "image/wireless_on_180.gif");
		
		$("#wizard_title").html(I18N("j", "Install"));

		if(currentDevice.featureCovrWIFI_e){
			$('.wizard2_setupTable').hide();
			$('.wizard2_setupTable_Covrwifi').show();

			$('#wizButton_back').hide();
			$('#wizButton_next').hide();

			$("#wizButton_next").on("click", function(){
				wizardCovrwifi = 1;
				_gotoWiFi();
			});			
		}else{
			$('.wizard2_setupTable').show();
			$('.wizard2_setupTable_Covrwifi').hide();

			$("#wizButton_next").html(I18N("j", "Wi-Fi")).show().off("click").on("click", function(){
				_gotoAPC();
			});
			$("#wizButton_back").html(I18N("j", "Ethernet")).show().off("click").on("click", function(){
				_gotoEth();
			});
		}

		$("#Wizard2").show();

		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		return dtd.promise();
	}

	var startWPSPBCfn_wizard = null;

	function _runWPS(){
		//stop runwps
		clearTimeout(wpstimer);
		clearInterval(wpstimer2);
		wpstimer2 = null;
		wpstimer = null;
		//stop soap
		startWPSPBCfn_wizard = null;

		var wps_count = 120;
		var countHTML = '<span id="wps_countdown">'+wps_count+'</span>';
		if(currentDevice.featureCovrWIFI_e){
			// $("#Wizard2Desc").html(LocalLang.Read("Wizard.WIZARD102", countHTML));
			$("#Wizard2Desc").html(I18N("j", "Please ensure the Covr Extender is next to the Covr Router, then press the WPS button on the left side of the Covr Extender within %s seconds. If done correctly, the four Wireless LEDs will start flashing.", countHTML));
		}else{
			// $("#Wizard2Desc").html(LocalLang.Read("Wizard.WIZARD10", countHTML));
			$("#Wizard2Desc").html(I18N("j", "Please press the WPS button on the router or access point you want to extend with in %s seconds, or select the type of existing network below.", countHTML));
		}
		var countDOM = $("#wps_countdown");

		uiTimer = setInterval(function(){
			if (wps_count > 0)
			{
				wps_count--;
				countDOM.html(wps_count);
			}
			else
			{
				showFail();	
			}
		}, 1000);

		startWPSPBCfn_wizard = startWPSPBC(wps_count).progress(function(timer){
			statusTimer = timer;
		}).done(function(info){
			_clearTimer();
			//console.log("startWPSPBC done!!");
			//get apc setting
			
			if(currentDevice.featureCovrWIFI_e) {
				wizardCovrwifi = 1;
				$("#wizButton_next").html(I18N("j", "Next")).show();
				$("#wizButton_back").hide();
			} else {
				_storeData(info.RadioID);
			}
		}).fail(function(){
			//console.log("startWPSPBC fail....");
			showFail();
		});

		var showFail = function(){
			_clearTimer();
			if(currentDevice.featureCovrWIFI_e){
				wizardCovrwifi = -1;
				_covrwifi();
			}else{
				$("#Wizard2Desc").html(I18N("j", "We couldn't find a Wi-Fi network through WPS. Click <b>Retry</b> to try again, or select a connection method below."));
				$("#RetryBtn").show();
			}
		}

	}

	function _storeData(radioID){
		getAPCsettings(radioID).then(function(apcObj){
			_setAPCMode();
			deviceInfo.apcSSID = apcObj.SSID;
			deviceInfo.apcPWD = apcObj.Key;
			deviceInfo.wps = true;

			var soapAction = new SOAPAction();
			var setting = createOPmode(radioID, deviceInfo.opMode);
			return soapAction.sendSOAPAction("SetOperationMode", setting, null);
		}).done(function(){
			if(!currentDevice.featureCovrWIFI_e){
				_gotoWiFi();	
			}
		});
	}

	function _setAPCMode(){
		deviceInfo.opMode = "WirelessRepeaterExtender";
		$("#RT_to_DUT_Wired").hide();
		$("#RT_to_DUT_Wireless").show();
	}

	function _setAPMode(){
		deviceInfo.opMode = "WirelessAp";
		$("#RT_to_DUT_Wired").show();
		$("#RT_to_DUT_Wireless").hide();
	}

	function _clearTimer(){
		clearInterval(statusTimer);
		clearInterval(uiTimer);
		statusTimer = null;
		uiTimer = null;
	
		clearTimeout(wpstimer);
		clearInterval(wpstimer2);
		wpstimer2 = null;
		wpstimer = null;
	}

	function _gotoAPC(){
		_clearTimer();
		_setAPCMode();

		dtd.resolve("sitesurvey");
	}

	function _gotoWiFi(){
		_clearTimer();		
		dtd.resolve("wifi");
	}

	function _covrwifi(){
		dtd.resolve("covrwifi");
	}

	function _gotoEth(){
		_clearTimer();
		_setAPMode();

		dtd.resolve("eth");
	}

	function _closeWizard(){
		if(currentDevice.featureCovrWIFI_e) {
			$('#WizardSection').hide();
			$('#CreatePopAlertMessage_wizard').show();

			$('.closewizard_cancel').off("click").on('click',function(){
				$('#WizardSection').show();
				$('#CreatePopAlertMessage_wizard').hide();
			});

			$('.closewizard_ok').off("click").on('click',function(){
				$('#CreatePopAlertMessage_wizard').hide();			

				startWPSPBCfn_wizard = null;
				_clearTimer();

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

			dtd.reject();
		}

	}

	//public
	return {
		init: function(device){
			return initViewCtrl(device);
		}
	}

});