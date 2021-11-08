//xframeoption
if(top!=self) {top.location=self.location;}

//session
if(sessionStorage.getItem('PrivateKey') === null) {
	window.location.replace('../info/Login_Simple.html');
}

//jquery-----------------
$(function() {
	//start
	pageFn.Initial();
	pageFn.BtnChange();
});


//pageFn-----------------
var pageFn = {
	count: 80,
	getxmlValue_DeviceName:'',
	SOAP_NAMESPACE: "http://purenetworks.com/HNAP1/",
	currentFile:'',
	Initial: function() {
		initialDetectRouterConnection();

		//starTimeout
		startTimeout();

		//modelName
		var modelInfo = JSON.parse(XMLEncode_forheader(sessionStorage.getItem('modelInfomation')));
		$('#modelName').html(modelInfo.modelName);
	},
	BtnChange: function() {
		var me = this;

		$('input').on('click', function() {
			changeTimeoutAction();
		});

		$('button').on('click', function() {
			changeTimeoutAction();
		});

		//manual
		$('#manualFirmwareTable2_selectFileBtn').on('click', function() {
			var browser = $.client.browser;
        	var version = $.client.version;

			if (browser == "Explorer" && $.client.version < 10) {
				alert(I18N("j", "The system doesn't support your browser."));
			} else {
				$('#uploadfile').trigger('click');
			}
		});

		$('input[type=file]').on('change', function(evt) {
			var file = me.manual_prepareUpload(evt);
			me.manual_ShowUpgradeBtn(file);
		});
	},
	do_upgrade: function() {
	    var me = this;

	    var msg = I18N('j', 'Download New Firmware') + '...';
	    PopView.show(msg);
	    me.GetFirmwareValidation().done(function(totalTime) {
	    	var msg2 = I18N("j", "Firmware is under upgrading, please do not power off your device") + " ...";
	    	var msg3 = I18N("j","Firmware Upgrade success!");
	    	PopView.showWithPercent(msg2, totalTime).done(function() {
		    	PopView.showConfirm(msg3).done(function(){
					location.assign('/');
				});
	    	});
	    }).fail(function() {		    
		    var msg4 = I18N("j", "Firmware Upgrade failed!");
		    PopView.showConfirm(msg4).done(function(){
				location.reload();
			});
	    });
	},
	GetFirmwareValidation: function() {
		var deferred = $.Deferred();
		var getFirmwareValidationResponse = new SOAPGetFirmwareValidationResponse();
		var soapAction = new SOAPAction();
		soapAction.sendSOAPAction("GetFirmwareValidation", null, getFirmwareValidationResponse).done(function(obj) {
			if(obj.IsValid == "true") {
				deferred.resolve(parseInt(obj.CountDown));
			} else {
				deferred.reject();
			}

		}).fail(function(){
			deferred.reject();
		});

		return deferred.promise();
	},
	manual_prepareUpload: function(evt) {
		var me = this;

		var str = "";
		var id;

		if(typeof evt.target != 'undefined') {
			var files = evt.target.files;
			me.currentFile = files[0];
		}
		
		return me.currentFile;
	},
	manual_ShowUpgradeBtn: function(file) {
		var me = this;
		$('#manualFirmwareTable2_fileName').show();
		$('#manualFirmwareTable2_fileName td p').html(file.name);
		$('#manualFirmwareTable2_upgrade').show();

		$('#manualFirmwareTable2_upgradeBtn').off('click').on('click', function() {
			var msg = I18N("j", "Do you want to upgrade Firmware?");
			PopView.showDialog(msg).done(function(){
				me.manual_upgradeFirmware(file);
				stopTimeout();
			}).fail(function() {
				me.manual_resetFileSelect();
				PopView.hide();
			});

		});
	},
	manual_upgradeFirmware: function(file) {
		var me = this;

		var msg = I18N("j", "Please wait") + "..." ;
		PopView.show(msg);

		me.uploadFile("FirmwareUpload", "FWFile", file).done(function() {
			var msg2 = I18N("j", "Please wait") + "..." ;
			PopView.show(msg2);
			me.manual_upgrade();
		}).fail(function() {
			setTimeout(function() {
				var msg3 = I18N("j", "Firmware Upgrade failed!");
				PopView.showConfirm(msg3).done(function() {
					me.manual_resetFileSelect();
					PopView.hide();
				});
			}, 1000);
		});
	},
	uploadFile: function(aSoapAction, name, file) {
		var me = this;
		var deferred = $.Deferred();
		var fileReader = new FileReader();
		//console.log(fileReader)
		fileReader.onload = function(event){
			me.upload(aSoapAction, name, event.target.result, file, 0).done(function(){
				deferred.resolve();
			}).fail(function(){
				deferred.reject();
			});
		};

		fileReader.readAsArrayBuffer(file);
		return deferred.promise();
	},
	manual_upgrade: function() {
		var me = this;

		var msg = I18N("j", "Please wait") + "..." ;
		PopView.show(msg);

		me.GetFirmwareValidation().done(function(countdown) {
			var msg2 = I18N("j", "Upgrading");
			PopView.showWithPercent(msg2, countdown).done(function() {
				var msg3 =  I18N("j","Firmware Upgrade success!");
				PopView.showConfirm(msg3).done(function(){
					location.assign('/');
				});
			});
		}).fail(function() {
			var msg4 = I18N("j", "Firmware Upgrade failed!");
			PopView.showConfirm(msg4).done(function() {
				me.manual_resetFileSelect();
				PopView.hide();
			});
		});

	},
	upload: function(aSoapAction, name, data, file, retry) {
		var me = this;

		var deferred = $.Deferred();
		var blob = new Blob([data]);
		var formData = new FormData();
		var headers = {};

		formData.append(name, blob, file.name);

		var soapActionURI = '"'+ me.SOAP_NAMESPACE + aSoapAction + '"';
		headers = {"SOAPAction":soapActionURI};

		//auth
		var PrivateKey = sessionStorage.getItem('PrivateKey');
		if(PrivateKey != null) {
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
				if(typeof data.error === 'undefined') {
					deferred.resolve();
				} else {
					//console.log('ERROR: '+data.error);
					deferred.reject();
				}
			},
			error: function(xhr, ajaxOptions, thrownError) {
				if(xhr.status == 401) {
					location.assign("/");
				}

				deferred.reject();
			}
		});

		return deferred.promise();
	},
	manual_resetFileSelect: function() {
		$('#manualFirmwareTable2_fileName td p').html('');
		$('#uploadfile').val('');
		$('#manualFirmwareTable2_fileName').hide();
		$('#manualFirmwareTable2_upgrade').hide();
	}
}