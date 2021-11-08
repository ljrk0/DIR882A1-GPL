//xframeoption
if(top!=self) {top.location=self.location;}

//session
if(sessionStorage.getItem('PrivateKey') === null) {
	window.location.replace('../info/Login.html');
}

//jquery-----------------
$(function() {
	//start
	pageFn.Initial();
	if(currentDevice.featureAutoUpgrade) {
		$("#rightFunc").show();
		$("#AutoFirmwareUpgrade").show();
		$('#AutoFirmwareUpgrade_hr').show();

		pageFn.GetAutoUpgradeStatus().then(function(){
			return pageFn.GetAutoUpgradeSettings();
		}).then(function(){
			$('#AutoFirmwareUpgrade > table').show();
			pageFn.GetXML();
		});
	} else {
		pageFn.GetXML();
	}
	pageFn.BtnChange();

	// validate
	$("#autoUpgrade_form").validate({
		submitHandler: function(form) {
			pageFn.CheckConnectionStatus();
			return false;
		}
	});

	$("#status_AutoUpgrade").on('click', function() {
		if($(this).prop('checked') == true) {
			$('#upgradeTimeTr').show();
		} else {
			$('#upgradeTimeTr').hide();
		}
	});
});


//data-----------------
var datalist = new Datalist();

function Datalist() {
	this.list = new Array();
	this.maxrowid = 0;
}

Datalist.prototype.getRowId = function(rowid) {
	var num = 0;
	for(num = 0; num < this.list.length; num++) {
		if(rowid == this.list[num].rowid) {
			break;
		}
	}

	return num;
}


function Data(ClientInfo_Type, ClientInfo_ExtenderMacAddress, ClientInfo_DeviceName, FirmwareVersion) {
	this.rowid = 0;

	//GetFirmwareStatus
	this.Type = ClientInfo_Type;
	this.MacAddress = ClientInfo_ExtenderMacAddress;
	this.DeviceName = ClientInfo_DeviceName;

	this.CurrentFWVersion = FirmwareVersion;
	this.LatestFWVersion = "";
	this.FirmwareStatus = "";
}

Data.prototype = {
	setRowid: function(rowid) {
		this.rowid = rowid;
	},
	addRowToTable: function(table) {
		var outputString;
		outputString = '<tr id="tr'+ this.rowid +'"></tr>';

		var selector = '#' + table +'> tbody';
		$(selector).append(outputString);

		this.setDataToRow($('#tr' + this.rowid));
	},
	setDataToRow: function(object) {
		if(this.DeviceName == "") {
			this.DeviceName = 'Unknown';
		}

		var outputString;

		if(this.rowid == 0) {
			outputString  = '<td class="autoFirmware_td1">Master</td>';
		} else if(this.rowid == 1) {
			outputString  = '<td class="autoFirmware_td1">Extender</td>';
		} else {
			outputString  = '<td class="autoFirmware_td1"></td>';
		}
		outputString += '<td class="autoFirmware_td2">'+ HTMLEncode(decode_char_text(this.DeviceName)) +'&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;</td>';
		outputString += '<td class="autoFirmware_td3"></td>';

		object.html(outputString);
	},
	setFirmware: function(text) {
		$('#tr' + this.rowid + ' td').eq(2).html(text);
	}
}


//pageFn-----------------
var pageFn = {
	count: 80,
	getxmlValue_DeviceName:'',
	SOAP_NAMESPACE: "http://purenetworks.com/HNAP1/",
	currentFile:'',
	Initial: function() {
		//loadheader
		$("#header").load("header.html");

		//initial
		if(!currentDevice.featureRequireLevel) {
			initialMenu();
		} else {
			if(sessionStorage.getItem('FirmwareRequireLevel') != '1') {
				initialMenu();
			}
		}

		initialDetectRouterConnection();

		//setMenu
		if(!currentDevice.featureRequireLevel) {
			setMenu("menu_Management");
		} else {
			if(sessionStorage.getItem('FirmwareRequireLevel') != '1') {
				setMenu("menu_Management");
			}
		}

		//starTimeout
		startTimeout();

	},
	CheckConnectionStatus: function() {
		var me = this;
		$.ajax({
			cache: false,
			url: "./js/CheckConnection",
			timeout: 2000,
			type: "GET",
			success: function(data) {
				me.SetAutoUpgradeSettings();
			},
			error: function() {
				document.getElementById("DetectRouterConnection").style.display = "inline";
			}
		});
	},
	SetAutoUpgradeSettings: function() {
		var msg = I18N('j', 'Please wait') + " ...";
		PopView.show(msg);

		var soapAction = new SOAPAction();
		var setEventNotification = new SOAPSetEventNotification();
		var setFirmwareAutoUpdate = new SOAPSetFirmwareAutoUpdate();

		var upgradeStatus = $("#status_AutoUpgrade").prop("checked");

		setEventNotification.Enabled = upgradeStatus;
		setEventNotification.AutoFirmwareUpgrade = upgradeStatus;

		setFirmwareAutoUpdate.TimeToUpdate.TimeHourValue = $("#UpgradeTimeHour").val();
		setFirmwareAutoUpdate.TimeToUpdate.TimeMinuteValue = $("#UpgradeTimeMinute").val();
		setFirmwareAutoUpdate.AutoDownload = upgradeStatus;
		setFirmwareAutoUpdate.AutoUpdate = upgradeStatus;

		soapAction.SetMultipleSOAP("SetEventNotification", setEventNotification, null);
		soapAction.SetMultipleSOAP("SetFirmwareAutoUpdate", setFirmwareAutoUpdate, null);

		soapAction.SendMultipleSOAPAction('SetMultipleActions').done(function(a, response, time){
			var countdown = currentDevice.okcount;
			if(time != "") {
				countdown = time;
			}
			var msg2 = I18N('j', 'The new settings have been saved.');
			PopView.showWithCountdown(msg, countdown).always(function(){
				PopView.showConfirm(msg2).done(function(){
					window.location.reload();
				});
			});
		}).fail(function(){
			window.location.reload();
		});
	},
	GetAutoUpgradeStatus: function() {
		var deferred = $.Deferred();
		var soapAction = new SOAPAction();
		var getEventNotificationResponse = new SOAPGetEventNotificationResponse();

		soapAction.sendSOAPAction('GetEventNotification', null, getEventNotificationResponse).done(function(){
			if(getEventNotificationResponse.AutoFirmwareUpgrade == "true") {
				$("#status_AutoUpgrade").prop("checked", true);
				$('#upgradeTimeTr').show();
			} else {
				$("#status_AutoUpgrade").prop("checked", false);
				$('#upgradeTimeTr').hide();
			}

			deferred.resolve();
		}).fail(function(){
			deferred.reject();
		});

		return deferred.promise();
	},
	GetAutoUpgradeSettings: function() {
		var deferred = $.Deferred();
		var soapAction = new SOAPAction();
		var getFirmwareAutoUpdateResponse = new SOAPGetFirmwareAutoUpdateResponse();
		soapAction.sendSOAPAction('GetFirmwareAutoUpdate', null, getFirmwareAutoUpdateResponse).done(function(){
			$("select").selectbox('detach');
			var upgradeTimeHour = document.getElementById("UpgradeTimeHour");
			var upgradeTimeMinute = document.getElementById('UpgradeTimeMinute');
			for(var i = 0; i <= 23; i++){
				var time = i;
				if(i < 12) {
					if(i == 0) {
						time = 12;
					}
					upgradeTimeHour.options.add(new Option(time + " AM", i));
				} else {
					if(i - 12 == 0) {
						time = i;
					} else {
						time = i - 12;
					}
					upgradeTimeHour.options.add(new Option(time + " PM", i));
				}
			}
			for(var i = 0; i <= 59; i++) {
				if(i < 10) {
					upgradeTimeMinute.options.add(new Option("0" + i, i));
				} else {
					upgradeTimeMinute.options.add(new Option(i, i));
				}
			}
			upgradeTimeHour.value = getFirmwareAutoUpdateResponse.TimeToUpdate.TimeHourValue;
			upgradeTimeMinute.value = getFirmwareAutoUpdateResponse.TimeToUpdate.TimeMinuteValue;

			$("select").selectbox({width: 100});
			$("select").selectbox('attach');

			deferred.resolve();
		}).fail(function(){
			deferred.reject();
		});

		return deferred.promise();
	},
	GetXML: function() {  //get device and version
		var me = this;

		var clientInfo = new SOAPGetClientInfoResponse();
		var soapAction = new SOAPAction();
		var getDeviceSettings = new SOAPGetDeviceSettingsResponse();
		var soapAction2 = new SOAPAction();

		//auto table message and button
		$('#autoFirmwareTableMessage tr').hide();
		$('#autoFirmwareTableMessage_text').show();
		$('#autoFirmwareTableMessage_text td span').html(I18N('j','Please wait')+'...');

		//manual table message and button
		$('#manualFirmwareTable tr').hide();
		$('#manualFirmwareTable_text').show();
		$('#manualFirmwareTable_text td span').html(I18N('j','Please wait')+'...');

		//clear
		$('#autoFirmwareTable > tbody').html('');
		datalist.list = [];
		datalist.maxrowid = 0;

		var result1 = soapAction.sendSOAPAction("GetDeviceSettings", null, getDeviceSettings);

		var result2 = result1.then(function() {
			var dtd = $.Deferred();

			var data = new Data(getDeviceSettings.Type, 'Master', getDeviceSettings.DeviceName, getDeviceSettings.FirmwareVersion);
			data.setRowid(datalist.maxrowid);
			data.addRowToTable('autoFirmwareTable');
			datalist.list.push(data);
			datalist.maxrowid++;

			//manual selectbox
			me.getxmlValue_DeviceName = getDeviceSettings.DeviceName;
			
			//manual
			$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('detach');
			$('#manualFirmwareTable_seletDeviceSeletbox').html('');
			$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('attach');

			//manual selectbox
			$('#manualFirmwareTable_seletDeviceSeletbox').append('<option value="' + getDeviceSettings.DeviceName +'" selected data-mac="Master">' + getDeviceSettings.DeviceName + ' (Master)</option>');

			soapAction2.sendSOAPAction("GetClientInfo", null, clientInfo).done(function() {
				dtd.resolve();
			}).fail(function() {
				dtd.reject();
			});

			return dtd.promise();
		}, function() {
			var dtd = $.Deferred();

			$('#autoFirmwareTableMessage tr').hide();
			$('#autoFirmwareTableMessage_text').show();
			$('#autoFirmwareTableMessage_text td span').html(I18N('j','Please wait')+'...');

			//manual table message and button
			$('#manualFirmwareTable tr').hide();
			$('#manualFirmwareTable_text').show();
			$('#manualFirmwareTable_text td span').html(I18N('j','Please wait')+'...');

			var data = new Data('none', 'Master', 'Covr', 'N/A');
			data.setRowid(datalist.maxrowid);
			data.addRowToTable('autoFirmwareTable');
			datalist.list.push(data);
			datalist.maxrowid++;

			//manual selectbox
			me.getxmlValue_DeviceName = 'Covr';

			//manual
			$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('detach');
			$('#manualFirmwareTable_seletDeviceSeletbox').html('');
			$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('attach');

			//manual selectbox
			$('#manualFirmwareTable_seletDeviceSeletbox').append('<option value="' + me.getxmlValue_DeviceName +'" selected data-mac="Master">' + me.getxmlValue_DeviceName + ' (Master)</option>');					
		
			soapAction2.sendSOAPAction("GetClientInfo", null, clientInfo).done(function() {
				dtd.resolve();
			}).fail(function() {
				dtd.reject();
			});

			return dtd.promise();
		});

		var result3 = result2.then(function(obj) {
			var dtd = $.Deferred();
			for(var row in clientInfo.ClientInfoLists.ClientInfo) {
				if(clientInfo.ClientInfoLists.ClientInfo[row].Type == "WiFi_2.4G_MeshExtender") {
				//test
				// if((clientInfo.ClientInfoLists.ClientInfo[row].Type.indexOf("Extender") >= 0 || clientInfo.ClientInfoLists.ClientInfo[row].Type.indexOf("LAN") >= 0)) {
					(function(row) {
						var data = new Data(clientInfo.ClientInfoLists.ClientInfo[row].Type, clientInfo.ClientInfoLists.ClientInfo[row].MacAddress, clientInfo.ClientInfoLists.ClientInfo[row].NickName);
						data.setRowid(datalist.maxrowid);
						data.addRowToTable('autoFirmwareTable');
						datalist.list.push(data);
						datalist.maxrowid++;

						//manual selectbox
						if(clientInfo.ClientInfoLists.ClientInfo[row].NickName == '') {
							clientInfo.ClientInfoLists.ClientInfo[row].NickName = 'Unknown ';
						}
						$('#manualFirmwareTable_seletDeviceSeletbox').append('<option value="' + clientInfo.ClientInfoLists.ClientInfo[row].MacAddress +'" data-mac="' + clientInfo.ClientInfoLists.ClientInfo[row].MacAddress + '">' + decode_char_text(clientInfo.ClientInfoLists.ClientInfo[row].NickName) + ' (' + clientInfo.ClientInfoLists.ClientInfo[row].MacAddress.toUpperCase() + ')</option>');
					})(row);
				}
			}

			setTimeout(function() {
				dtd.resolve();
			}, 1000);

			return dtd.promise();
		}, function() {
			var dtd = $.Deferred();
			$('#autoFirmwareTableMessage tr').hide();
			$('#autoFirmwareTableMessage_text').show();
			$('#autoFirmwareTableMessage_text td span').html(I18N('j','Please wait')+'...');


			//manual table message and button
			$('#manualFirmwareTable tr').hide();
			$('#manualFirmwareTable_text').show();
			$('#manualFirmwareTable_text td span').html(I18N('j','Please wait')+'...');

			dtd.resolve();

			return dtd.promise();				
		});

		var result4 = result3.then(function() {
			var response = new Array();
			var result = [];
			var num = 0;

			for(var i in datalist.list) {
				if(datalist.list[i].MacAddress == "") {
					num++;
					continue;
				}

				if(datalist.list[i].MacAddress == "Master") {
					(function(i) {
						response[i] = "";
						result[i] = "";

						var rowid = datalist.list[i].rowid;
						var CurrentFWVersion = datalist.list[i].CurrentFWVersion
						$('#tr' + rowid).find('td').eq(1).html($('#tr' + rowid).find('td').eq(1).html() + '&nbsp;&nbsp;'+ I18N('j','Firmware Version') + ':&nbsp;' + CurrentFWVersion);
					
						num++;
					})(i);
				} else {
					//extenders have to add soapheader
					(function(i) {
						var soapAction_header = new SOAPAction2();
						var rowid = datalist.list[i].rowid;
						var extendermac = [datalist.list[i].MacAddress];

						response[i] = new SOAPGetDeviceSettingsResponse();
						result[i] = soapAction_header.sendSOAPAction("GetDeviceSettings", null, response[i], extendermac).done(function(obj) {
							datalist.list[i].CurrentFWVersion = obj.FirmwareVersion;

							$('#tr' + rowid).find('td').eq(1).html($('#tr' + rowid).find('td').eq(1).html() + '&nbsp;&nbsp;'+ I18N('j','Firmware Version') + ':&nbsp;' + obj.FirmwareVersion);
							num++;
						}).fail(function() {
							if(response != null && result != null) {
								datalist.list[i].CurrentFWVersion = 'N/A';

								$('#tr' + rowid).find('td').eq(1).html($('#tr' + rowid).find('td').eq(1).html() + '&nbsp;&nbsp;'+ I18N('j','Firmware Version') + ':&nbsp;N/A');
								num++;
							}
						});					
					})(i);	
				}
			}


			var timer = setInterval(function() {
				if(num == datalist.list.length) {
					//manual
					$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('detach');
					$('#manualFirmwareTable_seletDeviceSeletbox').val(me.getxmlValue_DeviceName);
					$('#manualFirmwareTable_seletDeviceSeletbox').selectbox({width:350});
					$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('attach');

					$('#autoFirmwareTableMessage tr').hide();
					$('#autoFirmwareTableMessage_text').hide();
					$('#autoFirmwareTableMessage_checkNewFirmware').show();
					$('#autoFirmwareTableMessage_checkNewFirmwareBtn').attr('disabled',false);

					//manual table message and button
					$('#manualFirmwareTable tr').hide();
					$('#manualFirmwareTable_text').hide();
					$('#manualFirmwareTable_selectDevice').show();
					$('#manualFirmwareTable2').show();
					$('#manualFirmwareTable2 tr').hide();
					$('#manualFirmwareTable2_selectFile').show();		
					
					clearInterval(timer);
					timer = null;
					clearTimeout(timerCheck);
					timerCheck = null;

					response = null;
					result = null;
				}	
			},1000);

			var timerCheck = setTimeout(function() {
				if(timer != null) {
					clearInterval(timer);
					timer = null;
					clearTimeout(timerCheck);
					timerCheck = null;

					response = null;
					result = null;

					for(var i in datalist.list) {
						var rowid = datalist.list[i].rowid;
						if(datalist.list[i].CurrentFWVersion == '') {
							datalist.list[i].CurrentFWVersion = 'N/A';
							$('#tr' + rowid).find('td').eq(1).html($('#tr' + rowid).find('td').eq(1).html() + '&nbsp;&nbsp;'+ I18N('j','Firmware Version') + ':&nbsp;N/A');
						}
					}

					//manual
					$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('detach');
					$('#manualFirmwareTable_seletDeviceSeletbox').val(me.getxmlValue_DeviceName);
					$('#manualFirmwareTable_seletDeviceSeletbox').selectbox({width:350});
					$('#manualFirmwareTable_seletDeviceSeletbox').selectbox('attach');

					$('#autoFirmwareTableMessage tr').hide();
					$('#autoFirmwareTableMessage_text').hide();
					$('#autoFirmwareTableMessage_checkNewFirmware').show();
					$('#autoFirmwareTableMessage_checkNewFirmwareBtn').attr('disabled',false);

					//manual table message and button
					$('#manualFirmwareTable tr').hide();
					$('#manualFirmwareTable_text').hide();
					$('#manualFirmwareTable_selectDevice').show();
					$('#manualFirmwareTable2').show();
					$('#manualFirmwareTable2 tr').hide();
					$('#manualFirmwareTable2_selectFile').show();														
				}
			}, 120000);	
		});

	},
	GetXML2_1: function() {  //getWanStatus
		 var me =this;

		$('#autoFirmwareTableMessage tr').hide();
		$('#autoFirmwareTableMessage_text').show();
		$('#autoFirmwareTableMessage_text td span').html(I18N('j','Please wait')+'...');
		$('#autoFirmwareTableMessage_checkNewFirmware').show()
		$('#autoFirmwareTableMessage_checkNewFirmwareBtn').attr('disabled',true);		 

		 var getWanStatus = new SOAPGetWanStatusResponse();
		 var soapAction = new SOAPAction();

		 var result = soapAction.sendSOAPAction("GetWanStatus", null, getWanStatus);
		 result.done(function(obj) {
		 	if(obj.Status == 'CONNECTED') {
		 	//test
		 	// if(obj.Status == 'DISCONNECTED') {
		 		me.GetXML2_2();	
		 	} else {
		 		$('#autoFirmwareTableMessage tr').hide();
				$('#autoFirmwareTableMessage_text').show();
				$('#autoFirmwareTableMessage_text td span').html(I18N("j","Internet Disconnected"));
				$('#autoFirmwareTableMessage_checkNewFirmware').show();
				$('#autoFirmwareTableMessage_checkNewFirmwareBtn').attr('disabled',false);
		 	}
		 }).fail(function() {
			$('#autoFirmwareTableMessage tr').hide();
			$('#autoFirmwareTableMessage_text').show();
			$('#autoFirmwareTableMessage_text td span').html(I18N("j","Internet Disconnected"));
			$('#autoFirmwareTableMessage_checkNewFirmware').show();
			$('#autoFirmwareTableMessage_checkNewFirmwareBtn').attr('disabled',false);
		 });
	},
	GetXML2_2: function() {  //check new firmware version
		var me = this;

		var response = new Array();
		var result = [];
		var num = 0;

		$('#autoFirmwareTableMessage tr').hide();
		$('#autoFirmwareTableMessage_text').show();
		$('#autoFirmwareTableMessage_text td span').html(I18N('j','Please wait')+'...');
		$('#autoFirmwareTableMessage_checkNewFirmware').show();
		$('#autoFirmwareTableMessage_checkNewFirmwareBtn').attr('disabled',true);

		for (var i in datalist.list) {
			if(datalist.list[i].MacAddress == "") {
				num++;
				continue;
			}

			if(datalist.list[i].MacAddress == "Master") {
				(function(i) {
					var soapAction = new SOAPAction();
					var rowid = datalist.list[i].rowid;

					response[i] = new SOAPGetFirmwareStatusResponse();
					result[i] = soapAction.sendSOAPAction("GetFirmwareStatus", null, response[i]).done(function(obj) {
						datalist.list[i].LatestFWVersion = obj.LatestFWVersion;

						$('#tr' + rowid).find('td').eq(2).html(I18N('j','New Firmware Version') + ':&nbsp;' + obj.LatestFWVersion);
						num++;
					}).fail(function() {
						datalist.list[i].LatestFWVersion = 'unknown';
						
						$('#tr' + rowid).find('td').eq(2).html('&nbsp;');
						num++;
					});
				})(i);
			} else {
				//extenders have to add soapheader
				(function(i) {
					var soapAction_header = new SOAPAction2();
					var rowid = datalist.list[i].rowid;
					var extendermac = [datalist.list[i].MacAddress];

					response[i] = new SOAPGetFirmwareStatusResponse();
					result[i] = soapAction_header.sendSOAPAction("GetFirmwareStatus", null, response[i], extendermac).done(function(obj) {
						datalist.list[i].LatestFWVersion = obj.LatestFWVersion;

						$('#tr' + rowid).find('td').eq(2).html(I18N('j','New Firmware Version') + ':&nbsp;' + obj.LatestFWVersion);
						num++;
					}).fail(function() {
						datalist.list[i].LatestFWVersion = 'unknown';

						$('#tr' + rowid).find('td').eq(2).html('&nbsp;');
						num++;
					});					
				})(i);
			}
		}

		var timer = setInterval(function() {
			if(num == datalist.list.length) {
				var checkstatue = 0;

				for(var x in datalist.list) {
					if(datalist.list[x].CurrentFWVersion == 'N/A') {
						datalist.list[x].FirmwareStatus = 'unknown';
					} else if(parseFloat(datalist.list[x].CurrentFWVersion) - parseFloat(datalist.list[x].LatestFWVersion) >= 0) {
						datalist.list[x].FirmwareStatus = 'new';
					} else if(parseFloat(datalist.list[x].CurrentFWVersion) - parseFloat(datalist.list[x].LatestFWVersion) < 0)  {
						datalist.list[x].FirmwareStatus = 'upgrade';
						checkstatue++;
					} else {
						datalist.list[x].FirmwareStatus = 'fail';
					}
				}

				$('#autoFirmwareTableMessage tr').hide();
				if(checkstatue == 0){
					$('#autoFirmwareTableMessage tr').hide();
					$('#autoFirmwareTableMessage_text').show();
					$('#autoFirmwareTableMessage_text td span').html(I18N("j", "This firmware is the latest version."));	
				} else {
					$('#autoFirmwareTableMessage tr').hide();
					$('#autoFirmwareTableMessage_upgradeFirmware').show();
					$('#autoFirmwareTableMessage_upgradeFirmwareBtn').attr('disabled', false);

				}

				clearInterval(timer);
				timer = null;
				clearTimeout(timerCheck);
				timerCheck = null;

				response = null;
				result = null;
			}	
		},1000);

		var timerCheck = setTimeout(function() {
			if(timer != null) {
				clearInterval(timer);
				timer = null;
				clearTimeout(timerCheck);
				timerCheck = null;

				response = null;
				result = null;

				var checkstatue = 0;

				for(var x in datalist.list) {
					if(datalist.list[x].CurrentFWVersion == 'N/A') {
						datalist.list[x].FirmwareStatus = 'unknown';
					} else if(parseFloat(datalist.list[x].CurrentFWVersion) - parseFloat(datalist.list[x].LatestFWVersion) >= 0) {
						datalist.list[x].FirmwareStatus = 'new';
					} else if(parseFloat(datalist.list[x].CurrentFWVersion) - parseFloat(datalist.list[x].LatestFWVersion) < 0)  {
						datalist.list[x].FirmwareStatus = 'upgrade';
						checkstatue++;
					} else {
						datalist.list[x].FirmwareStatus = 'fail';
					}
				}

				$('#autoFirmwareTableMessage tr').hide();
				if(checkstatue == 0){
					$('#autoFirmwareTableMessage tr').hide();
					$('#autoFirmwareTableMessage_text').show();
					$('#autoFirmwareTableMessage_text td span').html(I18N("j", "This firmware is the latest version."));	
				} else {
					$('#autoFirmwareTableMessage tr').hide();
					$('#autoFirmwareTableMessage_upgradeFirmware').show();
					$('#autoFirmwareTableMessage_upgradeFirmwareBtn').attr('disabled', false);

				}							
			}
		}, 120000);
	},
	BtnChange: function() {
		var me = this;

		$('input').on('click', function() {
			changeTimeoutAction();
		});

		$('button').on('click', function() {
			changeTimeoutAction();
		});

		$('#autoFirmwareTableMessage_checkNewFirmwareBtn').on('click', function() {
			me.GetXML2_1();
		});

		$('#autoFirmwareTableMessage_upgradeFirmwareBtn').on('click', function() {
			stopTimeout();
			me.AutoUpgradeFirmware();
		});

		//manual
		$('#manualFirmwareTable_seletDeviceSeletbox').on('change', function() {
			if($('#manualFirmwareTable_seletDeviceSeletbox option:selected').attr('data-mac') == 'Master') {
				$('#manualFirmwareTable2').show();
				$('#manualFirmwareTable2_selectFile').show();
				$('#manualFirmwareTable_goDevice').hide();
			} else {
				$('#manualFirmwareTable2').hide();
				$('#manualFirmwareTable_goDevice').show();
			}
		});

		$('#manualFirmwareTable_goDeviceBtn').on('click', function() {
			var orgmac = $('#manualFirmwareTable_seletDeviceSeletbox option:selected').attr('data-mac').split(':');
			var getmac = orgmac[4] + orgmac[5];
			var url = 'http://dlinkap' + getmac + '.local./info/Login_Simple.html';
			me.manual_openWindow(url, 'D-Link', 640, 420);
		});

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

		$('#status_AutoUpgrade, #UpgradeTimeHour, #UpgradeTimeMinute').change(function(){
			$("#Save_Disable_btn").hide();
			$("#Save_btn").show();
			changeFlag = true;
			changeTimeoutAction();
		});
	},
	AutoUpgradeFirmware: function() {
		var me = this;
		$('#autoFirmwareTableMessage tr').hide();
		$('#autoFirmwareTableMessage_upgradeFirmware').show();
		$('#autoFirmwareTableMessage_upgradeFirmwareBtn').attr('disabled', true);
		
		PopView.showcount(I18N('j','Download New Firmware'),'');

		me.StartFirmwareDownload().progress(function(val) {
			$("#popalert_countdown").html(val + ' %');  //percent
		}).done(function() {
			$("#popalert_countdown").html('100 %');  //percent
			setTimeout(function() {  //wait 2 seconds
				me.do_upgrade();
			}, 2000);	
		}).fail(function() {
			var msg = I18N("j","Firmware Upgrade failed!");
			PopView.showConfirm(msg).done(function(){
				location.reload();
			});
		});
		
	},
	StartFirmwareDownload: function() {
		var me = this;

		var deferred = $.Deferred();
		var soapAction = new SOAPAction();
		soapAction.sendSOAPAction("StartFirmwareDownload", null, null).done(function(obj) {
			if(obj.StartFirmwareDownloadResult == "OK" || obj.StartFirmwareDownloadResult == "RESTART") {
				me.GetPollingFirmwareDownloadStatus(3000).progress(function(val) {
					deferred.notify(val);
				}).done(function(){
					deferred.resolve();
				}).fail(function(){
					deferred.reject();
				});
			} else {
				deferred.reject();
			}
		}).fail(function(){
			deferred.reject();
		});
		
		return deferred.promise();
	},
	GetPollingFirmwareDownloadStatus: function(interval) {
		var deferred = $.Deferred();
		var pollingFirmwareDownloadResponse = new SOAPPollingFirmwareDownloadResponse();
		var soapAction = new SOAPAction();
		var timer = null;
		var failTimer = null;

		if(interval > 0){
			timer = setInterval(function() {
				soapAction.sendSOAPAction("PollingFirmwareDownload", null, pollingFirmwareDownloadResponse).done(function(obj) {
					var progress = parseInt(obj.DownloadPercentage);
					if(progress < 100) {
						deferred.notify(progress);
					} else {
						clearInterval(timer);
						clearInterval(failTimer);
						deferred.resolve();
					}	
				});

			}, interval);

			failTimer = setTimeout(function(){
				clearInterval(timer);
				deferred.reject();
			},180000);
		} else {
			clearInterval(timer);
			clearInterval(failTimer);
			deferred.resolve();
		}

		return deferred.promise();
	},
	do_upgrade: function() {
	    var me = this;

	    var msg = I18N("j", "Please wait") + " ...";
	    PopView.show(msg);
	    me.GetFirmwareValidation().done(function(totalTime) {
	    	var msg2 = I18N("j", "Firmware is under upgrading, please do not power off your device") + " ...";
	    	var msg3 = I18N("j", "Firmware Upgrade success!");
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
	manual_openWindow: function(url, title, w, h) {
	    var secondScreenLeft = window.screenLeft != undefined ? window.screenLeft : screen.left;
	    var secondScreenTop = window.screenTop != undefined ? window.screenTop : screen.top;

	    var width = window.innerWidth ? window.innerWidth : document.documentElement.clientWidth ? document.documentElement.clientWidth : screen.width;
	    var height = window.innerHeight ? window.innerHeight : document.documentElement.clientHeight ? document.documentElement.clientHeight : screen.height;

	    var left = ((width / 2) - (w / 2)) + secondScreenLeft;
	    var top = ((height / 2) - (h / 2)) + secondScreenTop;
	    var newWindow = window.open(url, title, 'scrollbars=yes, width=' + w + ', height=' + h + ', top=' + top + ', left=' + left);

	    if (window.focus) {
	        newWindow.focus();
	    }
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