
define(['wizPath/wizardModel', 'wizPath/wizardView'], function(WizModel, WizView){
	var dtd;
	var deviceInfo;
	var uiTimer = null;
	var statusTimer = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		$('.wizard_projectName').html(projectName);

		_runWPS();

		$("#DUTToClientRight").attr("src", "image/wireless_on.gif");
		$("#DUTToClientLeft").attr("src", "image/wireless_on_180.gif");
		
		$("#wizard_title").html(I18N("j", "Install"));

		$('#topology').hide();

		$("#language_table").hide();
		
		$('.wizard2_setupTable').hide();
		$('.wizard2_setupTable_Covrwifi').show();

		$('#wizButton_back').hide();
		$('#wizButton_next').hide();

		$("#Wizard2").show();

		$("#closeWizBtn").off("vclick").on("vclick", function(){
			if($('#WizardSection').css('display') != 'none') {
				_closeWizard3();
			}
		});

		return dtd.promise();
	}

	//default wps
	var GetWiFiSONSettingsfn = null;
	var GetWiFiSONSettings2fn = null;
	var startWPSPBCfn = null;

	function _runWPS(){
		//stop runwps
		clearTimeout(wpstimer);
		clearInterval(wpstimer2);
		wpstimer2 = null;
		wpstimer = null;
		//stop soap
		GetWiFiSONSettingsfn = null;
		GetWiFiSONSettings2fn = null;
		startWPSPBCfn = null;

		var wps_count = 120;
		var countHTML = '<span id="wps_countdown">'+wps_count+'</span>';
		if(currentDevice.featureCovrWIFI_e){
			$("#Wizard2Desc").html(LocalLang.Read("Wizard.WIZARD102", countHTML));
		}else{
			$("#Wizard2Desc").html(LocalLang.Read("Wizard.WIZARD10", countHTML));
		}
		var countDOM = $("#wps_countdown");

		uiTimer = setInterval(function(){
			if (wps_count > 0)
			{
				wps_count--;
				countDOM.html(wps_count);
			} else {
				showFail();
			}
		}, 1000);


		var wifiCovr1 = new SOAPGetWiFiSONSettingsResponse();
    	var soapAction1 = new SOAPAction();
 		var wifiCovr2 = new SOAPGetWiFiSONSettingsResponse();
    	var soapAction2 = new SOAPAction();
    	   	
		GetWiFiSONSettingsfn = soapAction1.sendSOAPAction("GetWiFiSONSettings", null, wifiCovr1).done(function(obj){
			// console.log(wifiCovr1.WiFiSONList.WiFiSON.length)
			startWPSPBCfn = startWPSPBC(wps_count).progress(function(timer){
				statusTimer = timer;
			}).done(function(info){

				GetWiFiSONSettings2fn = setInterval(function(){
					soapAction2.sendSOAPAction("GetWiFiSONSettings", null, wifiCovr2).done(function(obj){
						if(wifiCovr2.WiFiSONList.WiFiSON.length > wifiCovr1.WiFiSONList.WiFiSON.length) {
							_clearTimer();
							_closeWizard2();
							// console.log("success");
						};
					}).fail(function(){
						// console.log("fail");
					});
				}, 1500);

			}).fail(function(){
				// console.log("wps-fail")
				// showFail();
			});
		}).fail(function(){
			// console.log("wifi2")
			// showFail();
		});	


		var showFail = function() {
			_clearTimer();

			if($('#WizardSection').css('display') != 'none') {
				_closeWizard();	
			}
		}
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
	
		clearInterval(GetWiFiSONSettings2fn);
		GetWiFiSONSettingsfn = null;
		GetWiFiSONSettings2fn = null;
		startWPSPBCfn = null;
	}

	function _closeWizard(){
		$('#WizardSection').hide();
		$('#CreatePopAlertMessage_addextenderwizard').show();

		//stop runwps
		_clearTimer();

		// $('.closewizard_cancel').on('click',function(){
		$('.extenderwizard_ok').off("click").on('click',function(){
			$('#WizardSection').show();
			_runWPS();
			// console.log("run_close")
			$('#CreatePopAlertMessage_addextenderwizard').hide();
		});

		// $('.closewizard_ok').on('click',function(){
		$('.extenderwizard_cancel').off("click").on('click',function(){	
			$('#CreatePopAlertMessage_addextenderwizard').hide();

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
	}

	function _closeWizard2(){
		$('#WizardSection').hide();
		$('#CreatePopAlertMessage_addextenderwizard2').show();

		//stop runwps
		_clearTimer();

		$('.success_closewizard_ok').on('click',function(){

			$('#CreatePopAlertMessage_addextenderwizard2').hide();

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
	}


	function _closeWizard3(){
		$('#WizardSection').hide();

		//stop runwps
		_clearTimer();

		$('#CreatePopAlertMessage_addextenderwizard').hide();

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

	}

	//public
	return {
		init: function(device){
			return initViewCtrl(device);
		}
	}

});