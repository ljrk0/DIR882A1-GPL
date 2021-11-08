
define(['wizPath/wizardModel', 'wizPath/wizardView'], function(WizModel, WizView){
	var dtd;
	var deviceInfo;
	var validator = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		_getData();


		if(currentDevice.featureCovrWIFI_e){
			$("#wizButton_back").hide();
			$("#wizard_title").html("Covr Wi-Fi Settings");
			$('#wizard5_text').html('To setup a Covr Wi-Fi network you will need to give your Wi-Fi network a name(SSID) and password.');
			
			if(wizardCovrwifi == 1){
				$('#topology').find('.wizard-topology-icon').eq(3).show();
				$('#topology').find('.wizard-topology-icon').eq(4).show();	
			}else{
				$('#topology').find('.wizard-topology-icon').eq(3).hide();
				$('#topology').find('.wizard-topology-icon').eq(4).hide();
			}
		}else{
			$("#wizard_title").html(I18N("j", "Configuration") + " - " + I18N("j", "Extended Network"));
		}

		$('#topology').show();

		$("#DUTToClientRight").attr("src", "image/wireless_on.gif");
		$("#DUTToClientLeft").attr("src", "image/wireless_on_180.gif");


		$("#Wizard5").show();
		
		if(isTouch() == false)
		{
			$("#APssid_24").focus();
		}

		if(!currentDevice.featureCovrWIFI_e){
			$("#wizButton_back").show().off("click").on("click", function(){
				_back();
			});
		}

		$("#wizButton_next").show().off("click").on("click", function(){
			$('#Wizard5_form').submit();
		});
		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		_setValidate();

		return dtd.promise();
	}

	function _getData(){
		if(deviceInfo.wps == true)
		{
			$("#APssid_24").val(deviceInfo.apcSSID+"-ext");
			$("#APkey_24").val(deviceInfo.apcPWD);			
		}
		else
		{
			$("#APssid_24").val(deviceInfo.apSSID);
			$("#APkey_24").val(deviceInfo.apPWD);
		}
	}

	function _storeData(){
		deviceInfo.apSSID = $("#APssid_24").val();
		deviceInfo.apPWD = $("#APkey_24").val();
	}

	function _next(){
		_storeData();

		dtd.resolve("pwd");
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
			//WPA
			if (value.length > 63 || value.length < 8)
			{
				return false;
			}
			return true;
		}, I18N("j","Your password must be between 8-63 characters length"));
		
		// check wifi ssid
		$.validator.addMethod("checkssid", function( value, element )
		{	
			if(value.length > 32)
			{
				return false;
			}
			return true;
		}, I18N("j", "The Wi-Fi Network Name can be up to 32 characters long."));
		
		validator = $('#Wizard5_form').validate({
			rules: {
				APssid_24: {
					required: true,
					checkssid: true
				},
				APkey_24: {
					checkASCIIChar: true,
					checkWPAPassword: true
				}
			},
			messages: {
				APssid_24: {
					required: I18N("j", "Please enter a Wi-Fi Name(SSID).")
				}
			},
			submitHandler: function(form) {
				_next();
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