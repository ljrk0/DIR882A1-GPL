
define(['wizPath/wizardModel', 'wizPath/wizardView'], function(WizModel, WizView){
	var dtd;
	var deviceInfo;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		_getData();

		$("#wizard_title").html(I18N("j", "Summary"));

		if(currentDevice.featureCovrWIFI_e){
			$('#wizard7_ssid_title').html('Covr Wi-Fi Network Name');
			$('#wizard7_key_title').html('Covr Wi-Fi Password');
			$('#RT').attr('src','image/DHP-W730AV-yellow.png');

			if(wizardCovrwifi == 1){
				$('#topology').find('.wizard-topology-icon').eq(3).show();
				$('#topology').find('.wizard-topology-icon').eq(4).show();	
			}else{
				$('#topology').find('.wizard-topology-icon').eq(3).hide();
				$('#topology').find('.wizard-topology-icon').eq(4).hide();
			}
		}

		$("#Wizard7").show();

		$("#wizButton_back").off("click").on("click", function(){
			_back();
		});
		$("#wizButton_next").html(I18N("j", "Finish")).off("click").on("click", function(){
			_next();
		});
		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		return dtd.promise();
	}

	function _getData(){
		if(deviceInfo.opMode != "WirelessRepeaterExtender")
		{
			$("#Wizard7_APC_heading, #Wizard7_APC").hide();
		}
		else
		{
			$("#Wizard7_APC_heading, #Wizard7_APC").show();		
			$("#Wizard7_APCSSID").html(HTMLEncode(deviceInfo.apcSSID));
			$("#Wizard7_APCKey").html(HTMLEncode(deviceInfo.apcPWD));
		}

		$("#Wizard7_SSID").html(HTMLEncode(deviceInfo.apSSID));
		$("#Wizard7_Key").html(deviceInfo.apPWD);
			
		$("#Wizard7_Admin_Pwd").html(HTMLEncode(deviceInfo.password));
	}


	function _storeData(){
		var msg = I18N("j", "Your changes are being saved, please wait...");
		var confirmMsg = I18N("j", "The new settings have been saved.");
		var countdown = 80;
		var wizardCovrwifiResult = wizardCovrwifi;
		if(wizardCovrwifiResult == 1){
			dtd.notify("end");
		}else{
			dtd.notify("done");
		}
	
		var result1 = WizModel.storeWizDeviceInfo(deviceInfo);
		var result2 = PopView.showWithCountdown(msg, countdown);

		$.when(result1, result2).always(function(){
			PopView.showConfirm(confirmMsg).done(function(){
				if(wizardCovrwifiResult == 1){
					PopView.hide();
					$("#WizardSection, #wizard_background").show();
					dtd.resolve("end");
				}else{
					PopView.hide();
					console.log("done")
					dtd.resolve("done");
				}

			});
		});
	}

	function _next(){
		_storeData();
	}

	function _back(){
		dtd.resolve("back");
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