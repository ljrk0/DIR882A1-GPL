
define(['wizPath/wizardModel', 'wizPath/wizardView'], function(WizModel, WizView){
	var dtd;
	var deviceInfo;
	var pageStack_wizStartViewCtrl = [];

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		$('.wizard_projectName').html(projectName);
		$("#wizard_title").html(I18N("j", "Welcome"));

		if(currentDevice.featureCovrWIFI_e == true){
			$('#topology').find('.wizard-topology-icon').eq(0).find('label').html('Existing Router');
			$('#topology').find('.wizard-topology-icon').eq(2).find('label').html(projectName);
			$('#Wizard1').find('li').eq(0).html('Step 1: Install the Covr Extender');
			$('#Wizard1').find('li').eq(1).html('Step 2: Configure your Wi-Fi settings');
			$('#Wizard1').find('li').eq(2).html('Step 3: Set your Covr Extender password');
			$('#Wizard1').find('li').eq(3).html('Step 4: Relocate the Covr Extender');
		}

		$('#topology').show();
		$("#Wizard1").show();
		$('#wizard1start').show();
		$('#Wizard1InstallCovr').hide();

		$("#wizButton_back").hide();
		$("#wizButton_next").show().off("click").on("click", function(){
			//test
			if(currentDevice.featureCovrWIFI_e == true){
				_next();
			}else{
				WizView.lockBtn($(this));
				_next();				
			}
			//test end
		});

		$('#wizButton_back').off("click").on("click", function(){
			_back();
		});

		if(localStorage.getItem("RunningWizard") == "true"){
			$("#language_table").show();
		}

		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		return dtd.promise();
	}

	function _next(){
		var nextPage;

		$("#language_table").hide();

			if(currentDevice.featureCovrWIFI_e){
				if(pageStack_wizStartViewCtrl.length < 1){
					$("#wizard_title").html("Install The Covr Extender");
					$('#topology').hide();
					$('#wizard1start').hide();
					$('#Wizard1InstallCovr').show();
					$("#wizButton_back").show();
					pageStack_wizStartViewCtrl.push("1");
				}else{
					pageStack_wizStartViewCtrl = [];
					nextPage = "covrwifi";
					dtd.resolve(nextPage);				
				}
			}


			var result1 = InitInfo();
			var result2 = WizModel.getWizDeviceInfo(deviceInfo);

			$.when(result1, result2).done(function(){
				if(!currentDevice.featureCovrWIFI_e){
					if(deviceInfo.hasInternet){
						nextPage = "wifi";
						deviceInfo.opMode = "";

						if(deviceInfo.uplink.toUpperCase() == "WLAN"){
							$("#RT_to_DUT_Wired").hide();
							$("#RT_to_DUT_Wireless").show();
						}
					}else{
						nextPage = "wps";
					}

					dtd.resolve(nextPage);
				}
			});


	}

	function _back(){
		pageStack_wizStartViewCtrl.pop();
		$("#wizard_title").html(I18N("j", "Welcome"));
		$('#topology').show();
		$('#wizard1start').show();
		$('#Wizard1InstallCovr').hide();
		$("#wizButton_back").hide();
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

					pageStack_wizStartViewCtrl = [];
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

			pageStack_wizStartViewCtrl = [];
			dtd.reject();
		}
	}

	//public
	return {
		init: function(device){
			return initViewCtrl(device);
		},
	}

});