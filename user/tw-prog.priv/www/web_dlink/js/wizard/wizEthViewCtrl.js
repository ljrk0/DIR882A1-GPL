
define(['wizPath/wizardViewController', 'wizPath/wizardModel', 'wizPath/wizardView'], function(WizViewCtrl, WizModel, WizView){
	var dtd;
	var deviceInfo;
	var timer = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		$("#wizard_title").html(I18N("j", "Install"));

		$("#Wizard3").show();

		$("#DUTToClientRight").attr("src", "image/wireless_on.gif");
		$("#DUTToClientLeft").attr("src", "image/wireless_on_180.gif");
		
		$("#wizButton_back").show().off("click").on("click", function(){
			_back();
		});
		$("#wizButton_next").html(I18N("j", "Skip")).show().off("click").on("click", function(){
			_next();
		});

		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		_detectEth();

		return dtd.promise();
	}

	function _detectEth(){
		stopTimeout();
		var task = function(){
			getUplinkInterface().done(function(obj){	//homeModels.js
				if((obj.UplinkInterface.toUpperCase() == "LAN")&&
					(COMM_ToBOOL(obj.NetworkLayerStatus) == true)){
					_next();
				}
			});
		}

		timer = setInterval(function(){
			task();
		}, 3000);
	}

	function _next(){
		startTimeout();
		clearInterval(timer);
		dtd.resolve("wifi");
	}

	function _back(){
		startTimeout();
		clearInterval(timer);
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
					if(currentDevice.featurePLC_e) {
						GetXMLPLC();
						hometimer3 = setInterval("GetXMLPLC()",6000);
					}
				}

				startTimeout();
				clearInterval(timer);
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

			startTimeout();
			clearInterval(timer);
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