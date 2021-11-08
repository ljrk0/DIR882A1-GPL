
define(['wizPath/wizardViewController', 'wizPath/wizardModel', 'wizPath/wizardView'], function(WizViewCtrl, WizModel, WizView){
	var dtd;
	var deviceInfo;
	var uiTimer = null;
	var statusTimer = null;
	var spinnerObj2;
	var runcovrTimer = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		if(spinnerObj2 != undefined){
			spinnerObj2.stop();
		}

		$("#wizard_title").html("Install The Covr Extender");
		$('#Wizard8').show();
		$('#wizard8_scan').show();
		$('#wizard8_success').hide();
		$('#wizard8_fail').hide();
		$('#wizard8_scan_text').find('span').html('30');
		spinnerObj2 = CreateSpinnerObj("wizard8_SurveySpinner2", opts);
		$("#wizButton_back").hide();
		$("#wizButton_next").hide();
		
		if(wizardCovrwifi == -1){
			$('#wizButton_back').html('Skip Covr Setting');
			$('#wizButton_next').html(I18N("j", "Retry"));

			$('#wizard8_scan').hide();
			$('#wizard8_success').hide();
			$('#wizard8_fail').show();					
			$("#wizButton_back").show();
			$("#wizButton_next").show();
		}

		$("#wizButton_back").off("click").on("click", function(){
			wizardCovrwifi = -1;
			_gotoWiFi();
		});

		$("#wizButton_next").off("click").on("click", function(){
			if($('#wizard8_success').css('display') == 'none'){
				_gotowps();
			}else{
				wizardCovrwifi = 1;
				_gotoWiFi();
			}
		});

		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		if(wizardCovrwifi == 0 || wizardCovrwifi == 1){
			_runcovrwifi();	
		}
		

		return dtd.promise();
	}

	function _runcovrwifi(){
		var covrwifi_count = 30;
		var result = 0;
    	var wifiCovr = new SOAPGetWiFiSONSettingsResponse();
    	var soapAction = new SOAPAction();


    	runcovrTimer = setInterval(function() {
			soapAction.sendSOAPAction("GetWiFiSONSettings", null, wifiCovr).done(function(obj){
	            if(obj.WiFiSONStatus == "true"){
	            	result = 1;
	        	}else{
	        		result = -1;
	        	}
	        }).fail(function(){
	        	result = -1;
	        });
    	},1500);


		uiTimer = setInterval(function(){
			if (covrwifi_count > 0){
				covrwifi_count--;
				$('#wizard8_scan_text').find('span').html(covrwifi_count);
				//countDOM.html(wps_count);
				if(result == 1){
					_clearTimer();

					$('#wizButton_next').html(I18N("j", "Next"));

					$('#wizard8_scan').hide();
					$('#wizard8_success').show();
					$('#wizard8_fail').hide();					
					$("#wizButton_back").hide();
					$("#wizButton_next").show();					
				}
			}else{
				//showFail();
				_clearTimer();
				if(result < 0){
					$('#wizButton_back').html('Skip Covr Setting');
					$('#wizButton_next').html(I18N("j", "Retry"));

					$('#wizard8_scan').hide();
					$('#wizard8_success').hide();
					$('#wizard8_fail').show();
					$("#wizButton_back").show();
					$("#wizButton_next").show();
				}else{
					$('#wizButton_next').html(I18N("j", "Next"));

					$('#wizard8_scan').hide();
					$('#wizard8_success').show();
					$('#wizard8_fail').hide();					
					$("#wizButton_back").hide();
					$("#wizButton_next").show();
				}
			}
		}, 1000);

	}

	function _gotowps(){
		_clearTimer();
		dtd.resolve("wps");
	}

	function _gotoWiFi(){
		_clearTimer();		
		dtd.resolve("wifi");
	}

	function _clearTimer(){
		clearInterval(statusTimer);
		clearInterval(uiTimer);
		clearInterval(runcovrTimer);
	}

	function _closeWizard(){
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

			_clearTimer();
			dtd.reject();
		});
	}

	//public
	return {
		init: function(device){
			return initViewCtrl(device);
		}
	}


});