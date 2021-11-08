function doLogin(ifLogin_Password,ifLogin_Captcha)
{
	var PrivateKey = null;
	
	var loginObj = $.Deferred();
	
	var soapAction = new SOAPAction();
	var setLogin = new SOAPLogin();
	var getLogin = new SOAPLoginResponse();
	setLogin.Action = "request";
	setLogin.Username = "Admin";
	setLogin.Captcha = ifLogin_Captcha;

	// Login request
	soapAction.sendSOAPAction("Login", setLogin, getLogin).done(function(obj)
	{
		if (obj.Challenge != "" && obj.Cookie != "" && obj.PublicKey != "")
		{
			PrivateKey = hex_hmac_md5(obj.PublicKey + ifLogin_Password, obj.Challenge);
			PrivateKey = PrivateKey.toUpperCase();
			// Set Cookie
			$.cookie('uid', obj.Cookie, { expires: 1, path: '/' });
			// Storage data in DOM
			sessionStorage.setItem("Cookie", obj.Cookie);
			sessionStorage.setItem("PrivateKey", PrivateKey);

			var Login_Passwd = hex_hmac_md5(PrivateKey, obj.Challenge);
			Login_Passwd = Login_Passwd.toUpperCase();
			
			//rewrite login request
			setLogin.Action = "login";
			setLogin.LoginPassword = Login_Passwd;
			setLogin.Captcha = ifLogin_Captcha;
			
			// Do Login to DUT
			soapAction.sendSOAPAction("Login", setLogin, null).done(function(obj2)
			{
				//for compatibility
				if(obj2.LoginResult == "FAILED")
				{
					loginObj.reject();
				}
				else
				{
					loginObj.resolve();
				}
			})
			.fail(function(){
				loginObj.reject();
			});
		}
		else
		{
			loginObj.reject();
		}
	})
	.fail(function(){
		loginObj.reject();
	});
	return loginObj.promise();
}

/* generate the radom string with specific length. */
function COMM_RandomStr(len)
{
	var c = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	var str = '';
	for (var i = 0; i < len; i+=1)
	{
		var rand_char = Math.floor(Math.random() * c.length);
		str += c.substring(rand_char, rand_char + 1);
	}
	return str;
}
