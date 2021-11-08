
define(['wizPath/wizardModel', 'wizPath/wizardView'], function(WizModel, WizView){
	var dtd;
	var deviceInfo;

	function initViewCtrl(device){
		dtd = $.Deferred();
		deviceInfo = device;

		$("#wizard_title").html('Covr Extender Replacement');
		$('#topology').hide();
		$("#Wizard9").show();
		
		$("#wizButton_back").hide();
		$("#wizButton_next").html(I18N("j", "Finish")).show().off("click").on("click", function(){
			// $('#Wizard6_form').submit();
			dtd.resolve("done");
		});

		$("#closeWizBtn").off("vclick").on("vclick", function(){
		 	// dtd.reject();
		 	dtd.resolve("done");
		});

		return dtd.promise();
	}

	//public
	return {
		init: function(device){
			return initViewCtrl(device);
		}
	}

});