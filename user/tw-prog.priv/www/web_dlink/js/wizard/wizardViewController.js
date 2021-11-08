// p1  start
// p2  wps
// p3  ehternet plug
// p4  site survey
// p5  wifi
// p6  password
// p7  summary
//requirejs.config({
//	baseUrl: 'js/wizard'
//});
require.config({
	paths: {
		"wizPath": "wizard"
	},
	baseUrl: '/js/'
});

define(['wizPath/wizardModel', 'wizPath/wizardView', 'wizPath/wizStartViewCtrl', 'wizPath/wizWpsViewCtrl', 'wizPath/wizSiteSurveyViewCtrl', 'wizPath/wizEthViewCtrl', 'wizPath/wizWiFiViewCtrl', 'wizPath/wizPwdViewCtrl', 'wizPath/wizSummaryViewCtrl', 'wizPath/wizCovrWiFiViewCtrl', 'wizPath/wizEndViewCtrl', 'wizPath/addextender_WpsViewCtrl'], 
	function(WizModel, WizView, WizStartViewCtrl, WizWpsViewCtrl, WizSiteSurveyViewCtrl, WizEthViewCtrl, WizWiFiViewCtrl, WizPwdViewCtrl, WizSummaryViewCtrl, WizCovrWiFiViewCtrl, WizEndViewCtrl, addextender_WpsViewCtrl){

//var viewCtrl;
var deviceInfo = new WizModel.WizDeviceInfo();
var pageStack = [];

function wizEvents(start){
	if(start){
		$("input").on("tap", function(){
			$(this).focus();
		});
	}
}

function ShowWizard()
{
	_initWizLanguage();

	WizModel.setWizardStatus(true).fail(function(){

	});

	_pageCtrl(WizStartViewCtrl);
	WizView.initView();

	$("#WizardSection, #wizard_background").show();
	//renewInfos(false);
	wizEvents(true);
}

function _addextender() {
	_initWizLanguage();

	WizModel.setWizardStatus(true).fail(function(){

	});

	_pageCtrl(addextender_WpsViewCtrl);
	WizView.initView();

	$("#WizardSection, #wizard_background").show();
	//renewInfos(false);
	wizEvents(true);
}

function _pageCtrl(viewCtrl){
	_clearView();
	viewCtrl.init(deviceInfo).progress(function(ctrl){
		if(ctrl == "done"){
			_hideWizard();
		}
		if(ctrl == "end"){
			_hiddenWizard();
		}
	}).then(function(nextPage){
		var newCtrl = _getViewCtrl(nextPage);

		if(nextPage == "back"){
			var backPage = pageStack.pop();
			_pageCtrl(backPage);
		}
		else if(nextPage == "done"){
			_finishWizard();
		}
		else if(newCtrl != null)
		{
			pageStack.push(viewCtrl);
			_pageCtrl(newCtrl);
		}
	}).fail(function(){
		closeWizard();
	});
	WizView.initView();
	changeTimeoutAction();
}

function _getViewCtrl(page){
	var nextPage = null;
	switch(page)
	{
		case "start":
			nextPage = WizStartViewCtrl;
			break;
		case "wps":
			nextPage = WizWpsViewCtrl;
			break;
		case "sitesurvey":
			nextPage = WizSiteSurveyViewCtrl;
			break;
		case "eth":
			nextPage = WizEthViewCtrl;
			break;
		case "wifi":
			nextPage = WizWiFiViewCtrl;
			break;
		case "pwd":
			nextPage = WizPwdViewCtrl;
			break;
		case "summary":
			nextPage = WizSummaryViewCtrl;
			break;
		case "covrwifi":
			nextPage = WizCovrWiFiViewCtrl;
			break;
		case "end":
			nextPage = WizEndViewCtrl;
			break;
		case "addextender_WpsViewCtrl":
			nextPage = addextender_WpsViewCtrl;
			break;
	}
	return nextPage;
}

function closeWizard()
{
	_hideWizard();
	WizModel.setWizardStatus(false);
	// WizModel.setWizardStatus(false).always(function(){
	// 	_hideWizard();
	// });
}

function _clearView(){
	for (var i = 1; i <= 9; i++)
	{
		$("#Wizard"+i).hide();
	}

	$("#wizButton_back").html(I18N("j", "Back"));
	$("#wizButton_next").html(I18N("j", "Next"));
	WizView.unlockBtn($(".btn-step"));
	_resetTopology();

	//wizEvents();
}

function _resetTopology(){
	if(currentDevice.featureCovrWIFI_e){
		$('#topology').find('.wizard-topology-icon').eq(3).show();
		$('#topology').find('.wizard-topology-icon').eq(4).show();

		$('#Internet').attr('src', 'image/router3_off.png');
		$('#Internet').css({
			'width':'90px',
			'height':'64px',
			'margin-bottom':'1px'
		});
			
		$('#RT').attr('src','image/DHP-W730AV-gray.png');
		$('#RT').css({
			'width':'60px',
			'height':'60px',
			'margin-bottom':'5px'
		});

		$('#RTToDUT').attr('src', 'image/line_z_off2.png');
		$('#RTToDUT').css({
			'width':'40px',
			'height':'5px'
		});

		$('#DUT').attr('src', "image/DHP-W730AV-gray.png");
		$('#DUT').css({
			'width':'60px',
			'height':'60px',
			'margin-bottom':'5px'
		});
	}else{
		$("#RTToDUTRight").attr("src", "image/wireless_off.gif");
		$("#RTToDUTLeft").attr("src", "image/wireless_off_180.gif");

		$("#DUT").attr("src", "image/Device_gary.png");
		$("#DUTToClientRight").attr("src", "image/wireless_off.gif");
		$("#DUTToClientLeft").attr("src", "image/wireless_off_180.gif");
	}
}

function _initWizLanguage(){
	var lang = localStorage.getItem('language');
	$("#Language").selectbox('detach');
	$("#Language").val(lang);
	$("#Language").selectbox({width:150});

	$("#Language").off("change").on("change", function(evt){
		switch_Language(evt.target.value);
	});
}

function _hideWizard(){
	wizEvents(false);
	localStorage.removeItem('RunningWizard');
	pageStack = [];
	wizardCovrwifi = 0;
	$("#WizardSection, #wizard_background").hide();
	changeTimeoutAction();
	//renewInfos(true);
}

function _hiddenWizard(){
	$("#WizardSection, #wizard_background").hide();
	changeTimeoutAction();	
}

function _finishWizard(){
	// if(currentDevice.featureCovrWIFI){
	// 	if(wizardCovrwifi == 1){
	// 		_pageCtrl("end");
	// 	}else{
	// 		RedirectLoginPage();	
	// 	}
	// }else{
		RedirectLoginPage();
	// }
}

function _finishWizard2()
{
	RedirectLoginPage();
}

return {
	init: ShowWizard,
	closeWizard: closeWizard,
	addextender: _addextender
}

});
