var PopView = (function(){

var count = 0;
var timer = null;
var dtd;

function showDialog(msg, infoIMG){
	dtd = $.Deferred();
	
	hidePopView();
	_setType("dialog");

	// display info img
	if(infoIMG == true) {
		$("#popalert_InformationIMG").show();
		$("#popalert_desc").addClass("infoText");
	}

	$("#popalert_desc").html(msg);

	$("#popalert_ok").off("click").on("click", function(){
		dtd.resolve();

		// hide info img
		$("#popalert_InformationIMG").hide();
		$("#popalert_desc").removeClass("infoText");
	});

	$("#popalert_cancel").off("click").on("click", function(){
		dtd.reject();

		// hide info img
		$("#popalert_InformationIMG").hide();
		$("#popalert_desc").removeClass("infoText");
	});

	$("#popalert").show();

	return dtd.promise();
}

function showConfirm(msg){
	dtd = $.Deferred();

	hidePopView();
	_setType("confirm");
	$("#popalert_desc").html(msg);

	$("#popalert_ok").off("click").on("click", function(){
		dtd.resolve();
	});

	$("#popalert").show();

	return dtd.promise();
}

function showWarningMSG(title, msg) {
	dtd = $.Deferred();

	hidePopView();
	_setType("warningMSG");

	if(title) {
		$("#popalert_title").show();
		$("#popalert_title").html(title);
	}

	$("#popalert_desc").html(msg);

	$("#popalert_ok").off("click").on("click", function(){
		$("#popalert_title").hide();
		dtd.resolve();
	});

	$("#popalert").show();

	return dtd.promise();
}

function showWithCountdown(msg, countdown, rebootresult) {
	dtd = $.Deferred();

	hidePopView();
	count = parseInt(countdown);
	timer = setInterval(function(){
		count--;
		$("#popalert_countdown").html(count + I18N("j", "Sec"));

		if(count <= 0){
			clearInterval(timer);
			dtd.resolve(rebootresult);
		}
	}, 1000);

	_setType("countdown");
	$("#popalert_countdown").html(count + I18N("j", "Sec"));
	$("#popalert_desc").html(msg);

	$("#popalert").show();

	return dtd.promise();
}

function showWithPercent(msg, totalTime, rebootresult) {
	dtd = $.Deferred();

	var totalTimeNum = parseInt(totalTime);

	hidePopView();
	count = 0;
	timer = setInterval(function(){
		var percent = Math.ceil((++count / totalTimeNum) * 100);
		$("#popalert_countdown").html(percent + ' %');

		if(percent >= 100){
			clearInterval(timer);
			dtd.resolve(rebootresult);
		}
	}, 1000);

	_setType("countdown");
	$("#popalert_countdown").html(count + ' %');
	$("#popalert_desc").html(msg);

	$("#popalert").show();

	return dtd.promise();
}

function show(msg){
	hidePopView();
	_setType("waiting");
	$("#popalert_desc").html(msg);
	$("#popalert").show();
}

function showcount(msg, countdown) {
	hidePopView();
	count = countdown;

	_setType("countdown");
	$("#popalert_countdown").html(count);
	$("#popalert_desc").html(msg);

	$("#popalert").show();
}

function hidePopView(){
	clearInterval(timer);
	$("#popalert").hide();
}

function _setType(type){
	var element = $("#popalert > .popup_window_border > .popup_window");
	element.removeClass("dialog countdown confirm waiting warningMSG");

	// remove popup_window_border warning className
	$("#popalert > .popup_window_border").removeClass("warning");

	switch(type){
		case "dialog":
			element.addClass("dialog");
			break;
		case "countdown":
			element.addClass("countdown");
			break;
		case "confirm":
			element.addClass("confirm");
			break;
		case "warningMSG":
			element.addClass("warningMSG");
			$("#popalert > .popup_window_border").addClass("warning");
			break;
		default:
			element.addClass("waiting");
			break;
	}

}


return {
show: show,
showcount: showcount,
showWithCountdown: showWithCountdown,
showWithPercent: showWithPercent,
showDialog: showDialog,
showConfirm: showConfirm,
hide: hidePopView,
showWarningMSG: showWarningMSG,
}

})();