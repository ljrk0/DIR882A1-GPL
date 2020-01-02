/* include menu.js */
document.write('<script type="text/javascript" charset="utf-8" src="/js/menu.js' + ini_ver + '"></script>');
/* include DetectRouterConnection.js */
document.write('<script type="text/javascript" charset="utf-8" src="/js/DetectRouterConnection.js' + ini_ver + '"></script>');

//<-- For DIR-890L
function CleanTable(tblID)
{
        table = document.getElementById(tblID);
        var rows = table.getElementsByTagName("tr");
        while (rows.length > 0) table.deleteRow(rows.length - 1);
}
// For DIR-890L -->

function save_button_changed()
{
	changeFlag = true;
	document.getElementById("Save_Disable_btn").style.display = "none";
	document.getElementById("Save_btn").style.display = "block";
}

function SetCheckBoxEnable(id, init, ckstatus)
{
	var stren = I18N("j", "Enabled");
	var strdis = I18N("j", "Disabled");
	
	SetCheckBox(id, stren, strdis, ckstatus, init);
}

function SetCheckBoxAllow(id, init, ckstatus)
{
	var stren = I18N("j", "Allowed");
	var strdis = I18N("j", "Blocked");
	
	SetCheckBox(id, stren, strdis, ckstatus, init);
}

function SetCheckBox(id, stren, strdis, ckstatus, init)
{
	var checkbox = id+"_ck";
	var now_check;
	var status;
	
	if(init)
	{
		now_check = ckstatus;
		now_check?status=false:status=true;
	}
	else
	{
		now_check = document.getElementById(checkbox).checked;
		now_check?status=true:status=false;
		save_button_changed();
	}
	
	if(status)
	{
		document.getElementById(id).className = "checkbox_off";
		document.getElementById(id).innerHTML = '<input type="checkbox" id="'+checkbox+'" name="'+checkbox+'" checked>'+strdis;
		document.getElementById(checkbox).checked = false;
	}
	else
	{
		document.getElementById(id).className = "checkbox_on";
		document.getElementById(id).innerHTML = '<input type="checkbox" id="'+checkbox+'" name="'+checkbox+'" checked>'+stren;
		document.getElementById(checkbox).checked = true;
	}
}

/////////////////////////////////////////////////////////////////////
function presetCheckBox(id, ck) {
	var targetId = 	document.getElementById(id);
	var checkboxId =  id +'_ck';
	
	if(ck == true) {
		var enable = I18N("j","Enabled");
	//	document.getElementById(checkboxId).checked = true;
		$("#"+id).attr("class", "checkbox_on");
		targetId.innerHTML='<input type="checkbox" name=' + id + ' id=' + checkboxId + ' checked>'+enable;
		document.getElementById(checkboxId).checked = true;
	}else {	
		var disable = I18N("j","Disabled");
	//	document.getElementById(checkboxId).checked = false;
		$("#"+id).attr("class", "checkbox_off");
		targetId.innerHTML='<input type="checkbox" name=' + id + ' id=' + checkboxId + ' checked>'+disable;
		document.getElementById(checkboxId).checked = false;
	}	
}

function showAdv(id) {
	var block = document.getElementById(id);
	//alert("showAdv1");
	if(block.style.display == "none" || block.style.display == "") {
		//alert("block");
		block.style.display = "block";
	} else {
		//alert("none");
		block.style.display = "none";
	}
	//alert("showAdv2");
}

function showOnlineHelp(str)	
{
		var helpString = encodeURIComponent(str);
		var modelInfo = JSON.parse(sessionStorage.getItem('modelInfomation'));	
		var hwVer = modelInfo.hwVer.charAt(0) + "x";
	
		var helpURL = "http://dcms.dlink.com.tw/"+localStorage.language +"/" 
							+ modelInfo.modelName + "/" 
							+ hwVer + "/"
							+ currentDevice.helpVer +"/"
							+ helpString;
	
		window.open(helpURL);	
	}

function redirectPage(whichPage)
{
	var Host_Name = sessionStorage.getItem('RedirectUrl');
	
	if(whichPage != null)
		Host_Name = whichPage;
	
	if(Host_Name == null)
		Host_Name = "/";

	self.location.href = Host_Name;	
}