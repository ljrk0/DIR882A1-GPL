define([], function(){

function initView(){
	$(window).resize(function(){
		_tuneView();
	});
	_tuneView();
}

function _tuneView(){
	//x
	var availWidth = $(window).width();
	var wizardWidth = $("#WizardSection").outerWidth();

	var leftOffset = availWidth/2 - wizardWidth/2;
	if(leftOffset > 0)
	{
		$("#WizardSection").css({"left":leftOffset});
	}
	else
	{
		$("#WizardSection").css({"left":0});
	}

	//y
	var availHeight = $(window).height();
	var wizardHeight = $("#WizardSection").outerHeight();

	var top = availHeight/2 - wizardHeight/2;
	if((top > 0)&&(top < 80))
	{
		$("#WizardSection").css({"top":top, "max-height":""});	//always reset max height
	}
	else if(top >= 80)
	{
		$("#WizardSection").css({"top":80, "max-height":""});
	}
	else
	{
		$("#WizardSection").css({"top":0, "max-height":""});
	}
}

function lockBtn(btn){
	btn.addClass("active").attr("disabled", true);
}

function unlockBtn(btn){
	btn.removeClass("active").attr("disabled", false);
}

return{
	initView: initView,
	lockBtn: lockBtn,
	unlockBtn: unlockBtn
}

});