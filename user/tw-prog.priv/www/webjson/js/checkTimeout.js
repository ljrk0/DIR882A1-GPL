var Time_GetReciprocalTime;
var DetectSecond = new Date().getTime() / 1000;
var ReadyStartTimeout;
var timeout_Value = 0;
var timeout_Click = 0;
var timeout_Draft = 0;
var changeFlag = false;
var checkFlag = false;
var Host_Name = "";
var CurrentTime = 0;
var timeout_Standard = 300;
var timeout_Range = 10;

function checkTimeout()
{
	sessionStorage.setItem('timeout', timeout_Value);
	Time_GetReciprocalTime = window.clearInterval(Time_GetReciprocalTime);
	Time_GetReciprocalTime = self.setInterval("GetReciprocalTime()",1000);
}

function GetReciprocalTime()
{
	var tmpTime = new Date().getTime() / 1000;
	
	if (CurrentTime == timeout_Standard - timeout_Range)
	{
		if (checkFlag == true)
		{
			var HNAP = new HNAP_XML();
			var xml_request = HNAP.GetXML("GetTimeSettings");
			CurrentTime = parseInt(tmpTime) - parseInt(DetectSecond) - timeout_Click;
			sessionStorage.setItem('timeout', CurrentTime);
			DetectSecond = new Date().getTime() / 1000;
			timeout_Draft = CurrentTime;
			timeout_Click = 0;
			checkFlag = false;
		}
		else
		{
			CurrentTime = parseInt(tmpTime) - parseInt(DetectSecond) + timeout_Draft;
			sessionStorage.setItem('timeout', CurrentTime);
		}
	}
	else if (sessionStorage.getItem('timeout') >= timeout_Standard)
	{
		sessionStorage.removeItem('timeout');
		readyLogout();
	}
	else
	{
		CurrentTime = parseInt(tmpTime) - parseInt(DetectSecond) + timeout_Draft;
		sessionStorage.setItem('timeout', CurrentTime);
	}
}

function GetClickTime()
{
	var tmpTime = new Date().getTime() / 1000;
	timeout_Click = parseInt(tmpTime) - parseInt(DetectSecond);
}

function readyLogout()
{
	var HNAP = new HNAP_XML();
	var soapAction = new SOAPAction();
	var setLogin = new SOAPLogin();

	setLogin.Action = "logout";
	setLogin.Username = "Admin";
	setLogin.Captcha = "";

	var body = soapAction.createJsonBody("Logout", setLogin);
	var json = new StringDoc(body);
	var xml_LogoutResult = HNAP.SetXML("Logout", json);//xml_Logout
	
	var logout_Result = xml_LogoutResult.Get("LogoutResponse/LogoutResult");
	
	if (logout_Result == "OK")	{	redirect_URL();	}
	else	{	setTimeout("redirect_URL()", 1000);	}
}

function redirect_URL()	{	location.assign("/");	}

function confirmExit()
{
	if (sessionStorage.getItem('timeout') > 0)
	{
		if (changeFlag)
		{
			if (!confirm(I18N("j", "There is unsaved data on this page. Do you want to abandon it?")))	{	return false;	}
			else	{	return true;	}
		}
	}
}