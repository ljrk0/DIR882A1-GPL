
define(['wizPath/wizardView'], function(WizView){

function initView(){
	$(window).resize(function(){
		_tuneView();
	});
	_tuneView();
}
function _tuneView(){
	$("#WizardSection").css({"top":"", "max-height":""});
	$("#SurveyListTable").css({"max-height":""});

	var availHeight = $(window).height();
	var wizardHeight = $("#WizardSection").outerHeight();

	if(availHeight < 300)
	{
		availHeight = 300;
	}

	var buttonOffset = 0;
	var top = availHeight/2 - wizardHeight/2;
	if((top > 0)&&(top < 80))
	{
		$("#WizardSection").css({"top":top, "max-height":""});
	}
	else if(top >= 80)
	{
		$("#WizardSection").css({"top":80, "max-height":""});
	}
	else
	{
		if(isTouch())
		{
			$("#WizardSection").css({"top":0, "max-height":""});
		}
		else
		{
			$("#WizardSection").css({"top":0, "max-height":availHeight});
			buttonOffset = 10;			
		}
	}

	var tableHeight = $("#WizardSection").height() - 220 - buttonOffset;
	$("#SurveyListTable").css({"max-height":tableHeight})
}

return{
	initView: initView
}

});