/** @define {string} */
//test
var SOAP_NAMESPACE = "http://purenetworks.com/HNAP1/";

var currentFile = null;

function prepareUpload(evt)
{
	var str = "";
	var id;

	if(typeof evt.target != 'undefined')
	{
		var files = evt.target.files;
		currentFile = files[0];
	}
	return currentFile;
}

function uploadFile(aSoapAction, name, file)
{
	var deferred = $.Deferred();
	var fileReader = new FileReader();
	//console.log(fileReader)
	fileReader.onload = function(event){
		upload(aSoapAction, name, event.target.result, file, 0).done(function(){
			deferred.resolve();
		}).fail(function(){
			deferred.reject();
		});
	};

	fileReader.readAsArrayBuffer(file);
	return deferred.promise();
}


function upload(aSoapAction, name, data, file, retry)
{
	var deferred = $.Deferred();
	var blob = new Blob([data]);
	var formData = new FormData();
	var headers = {};

	formData.append(name, blob, file.name);

	var soapActionURI = '"'+SOAP_NAMESPACE + aSoapAction + '"';
	headers = {"SOAPAction":soapActionURI};

	//auth
	var PrivateKey = sessionStorage.getItem('PrivateKey');
	if(PrivateKey != null)
	{
		// Set Cookie
		var cookie = sessionStorage.getItem('Cookie');
		$.cookie('uid', cookie, { expires: 1, path: '/' });
	
		//The current time length should fit the size of integer in Code. The period of the current time is almost 30 years.
		var current_time = new Date().getTime();
		current_time = Math.floor(current_time) % 2000000000000;
		current_time = current_time.toString();
		var auth = hex_hmac_md5(PrivateKey, current_time+soapActionURI);
		auth = auth.toUpperCase() + " " + current_time;

		headers["HNAP_AUTH"] = auth;
	}

	$.ajax({
		url: "/HNAP1/",
		type: 'POST',
		data: formData,
		cache: false,
		processData: false,
		headers:headers,
		//timeout: 10000,
		contentType:false,
		success: function(data, textStatus, jqXHR){
			if(typeof data.error === 'undefined')
			{
				deferred.resolve();
			}
			else
			{
				//console.log('ERROR: '+data.error);
				deferred.reject();
			}
		},
		error: function(xhr, ajaxOptions, thrownError){
			if(xhr.status == 401)
			{
				location.assign("/");
			}
			deferred.reject();
		}
	});
	return deferred.promise();
}
