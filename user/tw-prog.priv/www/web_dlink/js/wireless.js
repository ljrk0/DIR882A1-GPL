/* Global variable */
/* init wireless support mode */
var wifi_datalist = new Array();
//var apc_data;
var RadioIDList;

function WiFiSetting(band, settingsObj, securityObj)
{
    this.band = band;
    this.setting = settingsObj;
    this.security = securityObj;
}

function getWiFiData(list, band)
{
    for(obj in list)
    {
        if(band == list[obj].band)
        {
            return list[obj];
        }
    }
    return null;
}

function RadioIDToBand(radioid)
{
    var band = null;
    switch(radioid)
    {
        case "RADIO_2.4GHz":
            band = "24";
            break;
        case "RADIO_5GHz":
            band = "5";
            break;
    }

    return band;
}

function GetWirelessXML()
{
    var dtd = $.Deferred();
    var result = [null, null];

    InitInfo().done(function(){

        for(var i in RadioIDList)
        {
            var radioID = RadioIDList[i];
            result[i] = get_wireless(radioID).then(function(obj){
                wifi_datalist.push(obj);
            });
        }

        $.when(result[0], result[1]).done(function(){
            dtd.resolve(wifi_datalist);
        });
    }).fail(function(){
        dtd.reject();
    });

    return dtd.promise();
}

function InitInfo()
{
    var dtd = $.Deferred();

    var soapAction = new SOAPAction();
    var getWLanRadiosResponse = new SOAPGetWLanRadiosResponse();
    var resultRadio = soapAction.sendSOAPAction("GetWLanRadios", null, getWLanRadiosResponse);

    $.when(resultRadio).done(function(){ 
        RadioIDList = new Array();

        for(var radioInfo in getWLanRadiosResponse.RadioInfos.RadioInfo)
        {
            var radioInfoObj = getWLanRadiosResponse.RadioInfos.RadioInfo[radioInfo];

            var band = RadioIDToBand(radioInfoObj.RadioID);

            RadioIDList.push(radioInfoObj.RadioID);

            if(typeof initChannel == 'function')
                initChannel(radioInfoObj, band);
        }
        dtd.resolve();
    }).fail(function(){
        dtd.reject();
    });

    return dtd.promise();
}



function getSmartConnectStatus()
{
    var dtd = $.Deferred();
    var smartconnect = new SOAPGetSmartconnectSettingsResponse();
    var soapAction = new SOAPAction();
    soapAction.sendSOAPAction("GetSmartconnectSettings", null, smartconnect).done(function(obj){
        dtd.resolve(obj);
    }).fail(function(){
        dtd.reject();
    });
    return dtd.promise();
}


function getSmartCovrStatus(){
    var dtd = $.Deferred();
    var smartconnect = new SOAPGetSmartconnectSettingsResponse();
    var wifiCovr = new SOAPGetWiFiSONSettingsResponse();
    var soapAction = new SOAPAction();

    if(currentDevice.featureSmartConnect_e){
        soapAction.sendSOAPAction("GetSmartconnectSettings", null, smartconnect).done(function(obj){
            dtd.resolve(obj);
        }).fail(function(){
            dtd.reject();
        });
    }else if(currentDevice.featureCovrWIFI_e){
        soapAction.sendSOAPAction("GetWiFiSONSettings", null, wifiCovr).done(function(obj){
            dtd.resolve(obj);
        }).fail(function(){
            dtd.reject();
        });
    }else{
        dtd.resolve();
    }

    return dtd.promise();
}


function getWPSbtnStatus(){
    var dtd = $.Deferred();
    var getWPSSettingsResponse = new SOAPGetWPSSettingsResponse();
    var soapAction = new SOAPAction();

    if(currentDevice.featureWPS_e){
        soapAction.sendSOAPAction("GetWPSSettings", null, getWPSSettingsResponse).done(function(obj){
            dtd.resolve(obj);
        }).fail(function(){
            dtd.reject();
        });
    }else{
        dtd.resolve();
    }

    return dtd.promise();
}


function get_wireless(radioID)
{
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getWLanRadioSettings = new SOAPGetWLanRadioSettings();
    var getWLanRadioSecurity = new SOAPGetWLanRadioSecurity();
    var settingResponse = new SOAPGetWLanRadioSettingsResponse();
    var securityResponse = new SOAPGetWLanRadioSecurityResponse();
    var radioStr;

    var radioStr = RadioIDToBand(radioID);

    getWLanRadioSettings.RadioID = radioID;

    var result = soapAction.sendSOAPAction("GetWLanRadioSettings", getWLanRadioSettings, settingResponse);

    getWLanRadioSecurity.RadioID = radioID;
    var result_security = soapAction.sendSOAPAction("GetWLanRadioSecurity", getWLanRadioSecurity, securityResponse);
    
    $.when(result, result_security)
    .done(function(obj){
        var wifiSetting = new WiFiSetting(radioStr, settingResponse, securityResponse);

        dtd.resolve(wifiSetting);
    })
    .fail(function(obj){
        dtd.reject();
    });
        
    return dtd.promise();
}

function getOPmode()
{
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getOperationModeResponse = new SOAPGetOperationModeResponse();

    soapAction.sendSOAPAction("GetOperationMode", null, getOperationModeResponse).done(function(obj){
        dtd.resolve(obj);
    })
    .fail(function(obj){
        dtd.reject();
    });

    return dtd.promise();
}

function getAPCsettings(radioid)
{
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getAPClientSettings = new SOAPGetAPClientSettings();
    var getAPClientSettingsResponse = new SOAPGetAPClientSettingsResponse();

    getAPClientSettings.RadioID = radioid;

    soapAction.sendSOAPAction("GetAPClientSettings", getAPClientSettings, getAPClientSettingsResponse).done(function(obj){
        if((obj.Enabled.toLowerCase() == "true") && (obj.SSID != ""))
        {
            dtd.resolve(obj);
        }
        else
        {
            dtd.reject();
        }   
    })
    .fail(function(obj){
        dtd.reject();
    });

    return dtd.promise();
}

function getAPCinfoByRadio(list, index){
    var dtd = $.Deferred();
    var p = index;

    getAPCsettings(list[p]).done(function(obj){
        dtd.resolve(obj);
    }).fail(function(){
        p++;
        if(p < list.length)
        {
            getAPCinfoByRadio(list, p).done(function(obj){
                dtd.resolve(obj);
            }).fail(function(){
                dtd.reject();
            });
        }
        else
        {
            dtd.reject();
        }
    }); 

    return dtd.promise();
}

function getAPCinfo(radioidList){
    var dtd = $.Deferred();

    getAPCinfoByRadio(radioidList, 0).done(function(obj){
        dtd.resolve(obj);
    }).fail(function(){
        //return empty struct
        var data = new SOAPGetAPClientSettingsResponse();
        dtd.resolve(data);
    });

    return dtd.promise();
}


function createWlanSettings(band)
{
    var setting = new SOAPSetWLanRadioSettings();
    var radioData = getWiFiData(wifi_datalist, band);
    var opmode = $("#CurrentOPMode").val();
    //smart connect
    var smart = $("#APenabled_Smart").prop('checked');

    switch(band)
    {
        case "24":
            setting.RadioID = "RADIO_2.4GHz";
            setting.Coexistence = $("#APcoexistence_"+band).prop("checked");
            setting.Mode = "802.11bgn";
            break;
        case "5":
            setting.RadioID = "RADIO_5GHz";
            setting.Mode = "802.11anac";
            setting.Coexistence = radioData.setting.Coexistence;
            break;
    }

    setting.Enabled = $("#APenabled_"+band).prop("checked");
    
    setting.SSID = XMLEncode($("#APssid_"+band).val());

    if(smart)
    {
        setting.ChannelWidth = 0;
        setting.Channel = 0;
    }
    else
    {
        if(opmode == "WirelessAp")
        {
            setting.ChannelWidth = $("#APchannelWidth_"+band).val();
            setting.Channel = $("#APchannel_"+band).val();      
        }
        else
        {
            setting.ChannelWidth = radioData.setting.ChannelWidth;
            setting.Channel = radioData.setting.Channel;    
        }
    }

    setting.SSIDBroadcast = radioData.setting.SSIDBroadcast;
    setting.SecondaryChannel = radioData.setting.SecondaryChannel;
    setting.QoS = radioData.setting.QoS;
    // setting.ScheduleName = radioData.setting.ScheduleName;
    setting.ScheduleName = $('#schedule_' + band).val();
    setting.TXPower = radioData.setting.TXPower;

    return setting;
}

function createWlanSecurity(band)
{
    var security = new SOAPSetWLanRadioSecurity();
    //var radioData = getWiFiData(wifi_datalist, band);
    var key = $("#APkey_"+band).val();

    switch(band)
    {
        case "24":
            security.RadioID = "RADIO_2.4GHz";
            break;
        case "5":
            security.RadioID = "RADIO_5GHz";
            break;
    }
    
    if(key.length == 0)
    {
        security.Enabled = false;
    }
    else
    {       
        security.Enabled = true;        
        security.Type = "WPAORWPA2-PSK";
        security.Encryption = "TKIPORAES";
        security.Key = key;
        security.KeyRenewal = 3600;
    }

    return security;
}

function createOPmode(radioID, opmode)
{
    var setOperationMode = new SOAPSetOperationMode();

    setOperationMode.RadioID = radioID;
    setOperationMode.CurrentOPMode = opmode;
    return setOperationMode;
}

function createAPCSettings(apcSSID, apcPWD)
{
    //var setting = apcSettings;
    var ssid = XMLEncode(apcSSID);
    var key = apcPWD;

    //if(ssid != setting.SSID)
    //{
        var setting = new SOAPSetAPClientSettings();
        setting.RadioID = "";
        setting.SSID = ssid;
    //}

    //if(setting.RadioID == "RADIO_5GHz")
    //{
    //  setting.ChannelWidth = 1;
    //}

    setting.Enabled = true;
    setting.Key = key;

    if(key.length == 0)
    {
        setting.SupportedSecurity.SecurityInfo[0].SecurityType = "NONE";
        setting.SupportedSecurity.SecurityInfo[0].Encryptions = ["NONE"];
    }
    else
    {
        setting.SupportedSecurity.SecurityInfo[0].SecurityType = "WPAORWPA2-PSK";
        setting.SupportedSecurity.SecurityInfo[0].Encryptions = ["TKIPORAES"];
    }

    return setting;
}

function SetWirelessXML()
{

    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var setting;
    var security;
    var opmode = $("#CurrentOPMode").val();
    var smart;
    var covrwifi;


    if(!currentDevice.featurePLC_e){
        //OP mode
        for(var i in RadioIDList){
            var radioID = RadioIDList[i];
            setting = createOPmode(radioID, opmode);
            soapAction.SetMultipleSOAP("SetOperationMode", setting, null);
        }

        if(opmode != "WirelessAp")
        {
            setting = createAPCSettings($("#APCssid").val(), $("#APCkey").val());
            soapAction.SetMultipleSOAP("SetAPClientSettings", setting, null);

            count = 80;
        }
    }

    //smart connect
    if(currentDevice.featureSmartConnect_e){
        smart = $("#APenabled_Smart").prop('checked');
        setSmartConnectStatus(soapAction, smart);
    }

    //covr wifi
    if(currentDevice.featureCovrWIFI_e){
        covrwifi = $('#status_covr').prop('checked');
        var mumimo = $('#mu_mimo').prop('checked');
        setCovrWifiStatus(soapAction, covrwifi, mumimo);
    }

    //wps
    if(currentDevice.featureWPS_e){
        var setWPSSettings = new SOAPSetWPSSettings();
        setWPSSettings.WPSPBC = $("#wps_pbc").prop("checked");

        if(currentDevice.featureWPSPIN_e) {
            setWPSSettings.WPSPIN = $("#wps_pin").prop("checked");
        }

        soapAction.SetMultipleSOAP("SetWPSSettings", setWPSSettings, null);
    }

    //2.4 G
    for(var i in RadioIDList)
    {
        var radioID = RadioIDList[i];
        var radioIDStr = RadioIDToBand(radioID);

        if((smart)&&(radioID != "RADIO_2.4GHz"))
        {
            continue;
        }

        if((currentDevice.featureCovrWIFI_e)&&(radioID != "RADIO_2.4GHz"))
        {
            continue;
        }


        setting = createWlanSettings(radioIDStr);
        soapAction.SetMultipleSOAP("SetWLanRadioSettings", setting, null);

        security = createWlanSecurity(radioIDStr);
        soapAction.SetMultipleSOAP("SetWLanRadioSecurity", security, null);
    }

    soapAction.SendMultipleSOAPAction("SetMultipleActions").done(function(a,b){
        dtd.resolve(a,b);
    });

    return dtd.promise();
}

function setSmartConnectStatus(soapAction, value){
    var setSmartconnectSettings = new SOAPSetSmartconnectSettings();
    setSmartconnectSettings.Enabled = value;

    soapAction.SetMultipleSOAP("SetSmartconnectSettings", setSmartconnectSettings, null);
}

function setCovrWifiStatus(soapAction, value, value2){
    var setWiFiSONSettings = new SOAPSetWiFiSONSettings();
    setWiFiSONSettings.WiFiSONEnabled = value;
    setWiFiSONSettings.MUMIMOEnabled = value2;
    setWiFiSONSettings.BandSteeringEnabled = "";
    setWiFiSONSettings.AirTimeFairnessEnabled = "";

    soapAction.SetMultipleSOAP("SetWiFiSONSettings", setWiFiSONSettings, null);
}

/*
function startWPSPBC(duration){
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var setTriggerWPS = new SOAPSetTriggerWPS();
    var setTriggerWPSResponse = new SOAPSetTriggerWPSResponse();
    stopTimeout();

    setTriggerWPS.Duration = duration;

    soapAction.sendSOAPAction("SetTriggerWPS", setTriggerWPS, setTriggerWPSResponse).done(function(obj){
        var time = parseInt(obj.WaitTime);
        
        getWPSStatus(time*1000).progress(function(statusTimer){
            dtd.notify(statusTimer);
        }).then(function(info){
            startTimeout();
            dtd.resolve(info);
        }).fail(function(){
            dtd.reject();
        });
        
    })
    .fail(function(obj){
        dtd.reject();
    });

    return dtd.promise();
}

function getWPSStatus(waitTime){
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getWPSStatusResponse = new SOAPGetWPSStatusResponse();

    var task = function(){
        soapAction.sendSOAPAction("GetWPSStatus", null, getWPSStatusResponse).done(function(obj){       
            for(var i in obj.WPSStatusLists.WPSStatus){
                var info = obj.WPSStatusLists.WPSStatus[i];

                if(info.Status.toUpperCase() == "WPS_IN_PROGRESS")
                {
                    break;
                }
                else if(info.Status.toUpperCase() == "WPS_SUCCESS")
                {
                    clearInterval(wpstimer);
                    dtd.resolve(info);
                }
                else
                {
                    clearInterval(wpstimer);
                    dtd.reject();
                }
            }
        })
        .fail(function(obj){
            clearInterval(wpstimer);
            dtd.reject();
        });
    }

    var wpstimer = setTimeout(function(){
        wpstimer = setInterval(function(){
            task();
        }, 2000);
        dtd.notify(wpstimer);
    }, waitTime);
    dtd.notify(wpstimer);

    return dtd.promise();
}
*/

function startWPSPBC(duration){
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var setTriggerWPS = new SOAPSetTriggerWPS();
    var setTriggerWPSResponse = new SOAPSetTriggerWPSResponse();
    stopTimeout();

    setTriggerWPS.Duration = duration;

    soapAction.sendSOAPAction("SetTriggerWPS", setTriggerWPS, setTriggerWPSResponse).done(function(obj){
        var time = parseInt(obj.WaitTime);
        // console.log("time: "+ time);
        getWPSStatus(time*1000).progress(function(statusTimer){
            dtd.notify(statusTimer);
        }).then(function(info){
            startTimeout();
            dtd.resolve(info);
        }).fail(function(){
            dtd.reject();
        });
        
    })
    .fail(function(obj){
        dtd.reject();
    });

    return dtd.promise();
}

var wpstimer = null;
var wpstimer2 = null;

function getWPSStatus(waitTime){
    // console.log("waitTime" + waitTime);
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getWPSStatusResponse = new SOAPGetWPSStatusResponse();

    var task = function(){
        // console.log("status task")
        soapAction.sendSOAPAction("GetWPSStatus", null, getWPSStatusResponse).done(function(obj){       
            for(var i in obj.WPSStatusLists.WPSStatus){
                var info = obj.WPSStatusLists.WPSStatus[i];

                if(info.Status.toUpperCase() == "WPS_IN_PROGRESS") {
                    break;
                } else if(info.Status.toUpperCase() == "WPS_SUCCESS") {
                    clearInterval(wpstimer2);
                    wpstimer2 = null;
                    dtd.resolve(info);
                }
                
                // else {
                //     clearInterval(wpstimer2);
                //     wpstimer2 = null;
                //     dtd.reject();
                // }
            }
        })
        .fail(function(obj){
            clearInterval(wpstimer2);
            wpstimer2 = null;
            dtd.reject();
        });
    }

    wpstimer = setTimeout(function(){
        if(wpstimer2 == null) {
            wpstimer2 = setInterval(function(){
                task();
            }, 2000);
        }
            dtd.notify(wpstimer);
    }, waitTime);
    dtd.notify(wpstimer);

    return dtd.promise();
}

function getSiteSurveyResult(radioID){
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getSiteSurvey = new SOAPGetSiteSurvey();
    var getSiteSurveyResponse = new SOAPGetSiteSurveyResponse();

    getSiteSurvey.RadioID = radioID;

    soapAction.sendSOAPAction("GetSiteSurvey", getSiteSurvey, getSiteSurveyResponse).done(function(obj){
        dtd.resolve(obj);
    }).fail(function(){
        dtd.reject();
    });
    return dtd.promise();
}

function doSiteSurveyByRadio(list, index){
    var dtd = $.Deferred();
    var p = index;

    var soapAction = new SOAPAction();
    var input = new SOAPSetTriggerWirelessSiteSurvey();
    var output = new SOAPSetTriggerWirelessSiteSurveyResponse();

    input.RadioID = list[p];

    soapAction.sendSOAPAction("SetTriggerWirelessSiteSurvey", input, output).done(function(obj){
        var waitTime = parseInt(obj.WaitTime);

        sleep(waitTime*1000).then(function(){
            return getSiteSurveyResult(list[p]);
        }).then(function(obj){
            var result = obj.APStatInfoLists.APStatInfo;
            //add radioid
            for(var i in result)
            {
                result[i]["radioID"] = list[p];
            }

            dtd.notify(result);

            nextRadio(true);
        }).fail(function(){
            nextRadio(false);
        });
    }).fail(function(){
        dtd.reject();
    });

    var nextRadio = function(success){
        p++;
        if(p < list.length)
        {
            doSiteSurveyByRadio(list, p).done(function(obj){
                dtd.resolve();
            }).progress(function(obj){
                dtd.notify(obj);
            }).fail(function(){
                dtd.reject();
            });
        }
        else
        {
            if(success)
            {
                dtd.resolve();
            }
            else
            {
                dtd.reject();
            }
        }   
    }

    return dtd.promise();
}

function sortSiteList(list){
    list.sort(function(a, b){
        return parseInt(b.SignalStrength) - parseInt(a.SignalStrength) || a.radioID.length - b.radioID.length;
    });
    return;
}

function doSiteSurvey(){
    var dtd = $.Deferred();
    var siteList = new Array();
    stopTimeout();

    doSiteSurveyByRadio(RadioIDList, 0).progress(function(obj){
        for(var i in obj)
        {
            siteList.push(obj[i]);
        }
    }).always(function(){
        sortSiteList(siteList);
        startTimeout();
        dtd.resolve(siteList);
    });

    return dtd.promise();
}



function getschedulesettings(){
    var dtd = $.Deferred();
    var soapAction = new SOAPAction();
    var getScheduleSettingsResponse = new SOAPGetScheduleSettingsResponse();
    var resultSchedule = soapAction.sendSOAPAction("GetScheduleSettings", null, getScheduleSettingsResponse);


    $.when(resultSchedule).done(function(){
        RadioIDList.map(function(element){
            var band= RadioIDToBand(element);
            var scheduleid = $('#schedule_'+band);

            for(var row in getScheduleSettingsResponse.ScheduleInfoLists){
                var name = getScheduleSettingsResponse.ScheduleInfoLists[row].ScheduleName;
                scheduleid.append('<option value="'+name+'">'+decode_char_text(name)+'</option>');

            }

            scheduleid.selectbox({width:"100%"});

        });

        dtd.resolve();

    }).fail(function(){
        dtd.reject();
    });

    return dtd.promise();
}