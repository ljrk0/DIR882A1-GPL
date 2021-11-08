var menu_Check_Settings		= 0;
var menu_Check_Advanced		= 0;
var menu_Check_Management	= 0;

function initialMenu() {
	var menu     = "<ul>";
	menu		+= "	<li><a id='menu_Home' href='javascript:CheckHTMLStatus(\"Home_e\");' onclick='return confirmExit();'>"+I18N("j", "Home")+"</a></li>";
	menu 		+= "	<li class='parent' id='m_Settings' onmouseover='this.className=\"parentOn\";menu_Check_Settings=0;' onmouseout='this.className=\"parent\";menu_Check_Settings=1;' onclick='javascript:CheckClickStatus(\"m_Settings\");'><a id='menu_Settings' href='#'>"+I18N("j", "Settings")+"</a>";
	menu 		+= "		<ul>";
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"Extender_e\");' onclick='return confirmExit()'>"+I18N("j", "Extender")+"</a></li>";
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"Network_e\");' onclick='return confirmExit();'>"+I18N("j", "Network")+"</a></li>";

if(currentDevice.featureOperationMode) {
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"OperationMode_e\");' onclick='return confirmExit();'>"+I18N("j", "Operation Mode")+"</a></li>";	
}

	menu 		+= "		</ul>";
	menu 		+= "	</li>";
	menu 		+= "	<li class='parent' id='m_Management' onmouseover='this.className=\"parentOn\";menu_Check_Management=0;' onmouseout='this.className=\"parent\";menu_Check_Management=1;' onclick='javascript:CheckClickStatus(\"m_Management\");'><a id='menu_Management' href='#'>"+I18N("j", "Management")+"</a>";
	menu 		+= "		<ul>";
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"Admin_e\");' onclick='return confirmExit()'>"+I18N("j", "Admin")+"</a></li>";
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"System_e\");' onclick='return confirmExit()'>"+I18N("j", "System")+"</a></li>";
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"UpdateFirmware_e\");' onclick='return confirmExit()'>"+I18N("j", "Upgrade")+"</a></li>";
	menu 		+= "			<li><a href='javascript:CheckHTMLStatus(\"Statistics_e\");' onclick='return confirmExit()'>"+I18N("j", "Statistics")+"</a></li>";
	menu 		+= "		</ul>";
	menu 		+= "	</li>";
	menu 		+= "</ul>";

	document.getElementById("menu").innerHTML = menu;

	$('#menu').css({
		'position': 'absolute',
    	'top': '38px',
    	'left': '450px',
    	'width':'450px',
	});
	$('#menu > ul').css({
		'width': '450px'
	});
}

function CheckClickStatus(menuId) {
	switch (menuId) {
		case "m_Settings":
			if (menu_Check_Settings == 0)	{	document.getElementById("m_Settings").className = "parent";	menu_Check_Settings = 1;	}
			else	{	document.getElementById("m_Settings").className = "parentOn";	menu_Check_Settings = 0;	}
			break;
		case "m_Advanced":
			if (menu_Check_Advanced == 0)	{	document.getElementById("m_Advanced").className = "parent";	menu_Check_Advanced = 1;	}
			else	{	document.getElementById("m_Advanced").className = "parentOn";	menu_Check_Advanced = 0;	}
			break;
		case "m_Management":
			if (menu_Check_Management == 0)	{	document.getElementById("m_Management").className = "parent";	menu_Check_Management = 1;	}
			else	{	document.getElementById("m_Management").className = "parentOn";	menu_Check_Management = 0;	}
			break;
	}
}
function setMenu(menuId) {
	if (localStorage.getItem('language') == "ru-ru")	{	document.getElementById(menuId).style.background = "url(./image/navigation_bg6.gif) right top no-repeat";	}
	else	{	document.getElementById(menuId).style.background = "url(./image/navigation_bg5.gif) right top no-repeat";	}
}

function CheckHTMLStatus(URLString) {
	if (URLString != "") {
		$.ajax({
			"cache" : false,
			"url" : URLString + ".html",
			"timeout" : 5000,
			"type" : "GET",
			"error" : function() { 
				// document.getElementById("DetectRouterConnection").style.display = "inline"; 
			},
			"success" : function(data) { 
				// document.getElementById("DetectRouterConnection").style.display = "none";
				self.location.href = URLString + ".html"; 
			}
		});
	} else {
		$.ajax({
			"cache" : false,
			"url" : "./js/CheckConnection",
			"timeout" : 5000,
			"type" : "GET",
			"error" : function() { 
				// document.getElementById("DetectRouterConnection").style.display = "inline";
			},
			"success" : function(data) { 
				// document.getElementById("DetectRouterConnection").style.display = "none";
			}
		});
	}
}
