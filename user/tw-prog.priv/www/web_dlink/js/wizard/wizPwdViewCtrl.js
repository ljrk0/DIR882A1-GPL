
define(['wizPath/wizardModel', 'wizPath/wizardView'], function(WizModel, WizView){
	var dtd;
	var deviceInfo;
	var validator = null;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;


		// $("#wizard_title").html(I18N("j", "Configuration") + " - " + I18N("j", "Admin Password"));
		$("#wizard_title").html(I18N("j", "Device Admin Password"));

		if(currentDevice.featureCovrWIFI_e){
			$('#DUTToClientRight').attr('src','image/wireless_off.gif');
			$('#DUTToClientLeft').attr('src','image/wireless_off_180.gif');
			$('#RT').attr('src','image/DHP-W730AV-yellow.png');

			if(wizardCovrwifi == 1){
				$('#DUT').attr('src','image/DHP-W730AV-yellow.png');
				$('#topology').find('.wizard-topology-icon').eq(3).show();
				$('#topology').find('.wizard-topology-icon').eq(4).show();	
			}else{
				$('#topology').find('.wizard-topology-icon').eq(3).hide();
				$('#topology').find('.wizard-topology-icon').eq(4).hide();
			}
		}else{
			$("#DUT").attr("src", "image/Device_yellow.png");
		}

		$("#Wizard6").show();
		if(isTouch() == false)
		{
			$("#Wizard6_Admin_Pwd").focus();
		}
		

		$("#wizButton_back").show().off("click").on("click", function(){
			_back();
		});
		$("#wizButton_next").show().off("click").on("click", function(){
			$('#Wizard6_form').submit();
		});
		$("#closeWizBtn").off("vclick").on("vclick", function(){
			_closeWizard();
		});

		_setValidate();

		return dtd.promise();
	}

	function _storeData(){
		deviceInfo.password = $("#Wizard6_Admin_Pwd").val();
	}

	function _next(){
		_storeData();

		dtd.resolve("summary");
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

		$.validator.addMethod("requireDevicePassword", function( value, element ) {
			var returnVal = true;

			if(value.length == 0) {
				returnVal = false;
			}

			return returnVal;
		}, I18N("j", "err_check_Password"));	

		$.validator.addMethod("checkDevicePassword", function( value, element )
		{
			var returnVal = true;

			if (value.length > 15 || value.length < 6) {
				returnVal = false;	
			}

			return returnVal;
		}, I18N("j", "err_check_DeviceAdminPassword"));			

		$.validator.addMethod("checkASCIIChar", function( value, element ) {
			//empty
			if(value.length = 0)
			{
				return true;
			}
			return CheckASCII(value);
		}, I18N("j","Text field contains illegal characters."));

		$.validator.addMethod("checkpasswordsp", function( value, element ) {
			var regexwhitespace = /^\s{1,}$/;
			var returnVal=true;

			if(regexwhitespace.test(value)){
				returnVal=false;
			}
			
			return returnVal;
		}, I18N("j","Text field contains illegal characters."));
		
		validator = $('#Wizard6_form').validate({
			rules: {
				Wizard6_Admin_Pwd: {
					requireDevicePassword: true,
					checkDevicePassword: true,
					checkASCIIChar: true,
					checkpasswordsp: true
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