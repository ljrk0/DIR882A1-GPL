	var macList2 = "";

	
	function LoadMACList2()
	{
		$.ajax({
			url: "/js/MacList.json",
			type: "GET",
			dataType: "json",
			success: function(data) {
				macList2 = data;
				GetXMLPLC();
				hometimer3 = setInterval("GetXMLPLC()",6000);
			},
		});
	}

/*	
	function Base92(input)
	{
		var BASE = 92;

		function getCharCode(i)
		{
			if((i+35) == 92)	//skip '\'  (ASCII 92)
				return '!';
			return String.fromCharCode(i+35);
		}

		var Q = Math.floor(input/BASE);
		var R = input%BASE;
		var str = "";

		if(Q >= BASE)
		{
			str += Base92(Q);
		}
		else
		{
			str += getCharCode(Q);		
		}

		str += getCharCode(R);
		return str;
	}
*/

	function getVendor2(mac)
	{
		var unknown = "Unknown Vendor";
		var str;

		if((typeof macList2 == 'undefined')||(macList2 == ""))
			return unknown;
		try{
		var macUnit = mac.split(':');
		var L1 = Base92(parseInt(macUnit[0], 16));
		var L2 = Base92(parseInt(macUnit[1]+macUnit[2], 16));

		str = macList2[L1][L2];
		if(typeof str == 'undefined')
			str = unknown;
		}
		catch(e)
		{
			str = unknown;
		}

		return decode_char_text(str);
	}

	
	
	function GetXMLPLC() {
		var deferred = $.Deferred();
		var plctationInfo = new SOAPGetPLCStationInfoResponse();
		var soapAction = new SOAPAction();
		var plcstationInfo_query = soapAction.sendSOAPAction("GetPLCStationInfo", null, plctationInfo);
		$.when(plcstationInfo_query).done(function(obj){

			//prepare
			for(var num in deviceDataArray){
				deviceDataArray[num].Live = false;
			}

			var rowall = 0;

			if(plctationInfo.StationInfoLists.StationInfo.length <= 0){
			 	mySlideshow.update();	
			}else{
				rowall = deviceDataArray.length-1;

				//update
				for(var plcUnit in plctationInfo.StationInfoLists.StationInfo){
					var plc = plctationInfo.StationInfoLists.StationInfo[plcUnit];
					var rowNum = getDataRowNumByMac2(plc.StationMAC);
					var deviceDataarrayOld = null;
					var plcPHY = 0;

					if(parseInt(plc.StationTxPHY) >= parseInt(plc.StationRxPHY)){
						plcPHY = parseInt(plc.StationTxPHY);
					}else{
						plcPHY = parseInt(plc.StationRxPHY);
					}

					if(isNaN(plcPHY)){
						plcPHY = 0;
					}

					if(rowNum != null){
						deviceDataarrayOld = deviceDataArray[rowNum].rownumid;
						//deviceDataArray
						deviceDataArray[rowNum].deviceName = plc.StationName;
						deviceDataArray[rowNum].vendor = getVendor2(plc.StationMAC);
						deviceDataArray[rowNum].mac = plc.StationMAC;
						deviceDataArray[rowNum].PHYnum = plcPHY;
						deviceDataArray[rowNum].Live = true;
						deviceDataArray[rowNum].rownumid = rowNum;
						deviceDataArray[rowNum].stationMode = plc.StationMode;

						//innerhtml
						if(plc.StationName == "") {
							$('#plccard'+deviceDataarrayOld).find('.dataElement_deviceName').html(plc.StationMode);
						} else {
							$('#plccard'+deviceDataarrayOld).find('.dataElement_deviceName').html(plc.StationName);
						}
						
						$('#plccard'+deviceDataarrayOld).find('.dataElement_vendor').html(getVendor2(plc.StationMAC));
						$('#plccard'+deviceDataarrayOld).find('.dataElement_mac').html(plc.StationMAC);
						$('#plccard'+deviceDataarrayOld).find('.dataElement_phynum').find('p').html(plcPHY);
						$('#ss_itemContainer').find('.dataElement').eq(rowNum).attr('id',"plccard"+rowNum);
						$('#ss_itemContainer').find('.dataElement').eq(rowNum).attr('data-num', rowNum);

						//mySlideshow
						mySlideshow.itemArray[rowNum].id = "plccard"+rowNum;
						mySlideshow.dataArray[rowNum].element.id = "plccard"+rowNum;

						//master or slave UI > editbtn
						if(sessionStorage.getItem('WiFiSONRole') != "Slave") {
							$('.dataElement_editbtn').show();
						} else {
							$('.dataElement_editbtn').hide();
						}							
					}else{
						rowall = rowall+1;
						addDevice(plc.StationName,getVendor2(plc.StationMAC),plc.StationMAC,plcPHY,rowall,plc.StationMode);
					}
				}

				//delete
				// for(var num in deviceDataArray){
				for(var num = 0; num < deviceDataArray.length; num++){	
					if(deviceDataArray[num].Live == false){

						var plcid = 'plccard'+deviceDataArray[num].rownumid;


						//plccard remove
						//$('#ss_itemContainer').find('.dataElement').eq(num).remove();
						$('#plccard'+deviceDataArray[num].rownumid).remove();					

						
						//deviceDataArray remove
						deviceDataArray.splice(num, 1);

						//mySlideshow remove
						mySlideshow.itemArray.splice(num, 1);
						mySlideshow.dataArray.splice(num, 1);

						num--;

						/*
						for(var i = 0; i < mySlideshow.itemArray.length; i++) {
    						if(mySlideshow.itemArray[i].id == plcid) {
        						mySlideshow.itemArray.splice(i, 1);
        						break;
    						}
						}

						for(var i = 0; i < mySlideshow.dataArray.length; i++) {
    						if(mySlideshow.dataArray[i].element.id == plcid) {
        						mySlideshow.dataArray.splice(i, 1);
        						break;
    						}
						}
						*/					
					}
				}

				mySlideshow.update();
			}

			deferred.resolve();
		}).fail(function(){
			deferred.reject();
		});

		return deferred.promise();
	}


function getDataRowNumByMac2(mac){
	var i;
	var data = null;
	var rowNum = null;
	var macLower = mac.toLowerCase();
	// for(i = 0; i < this.list.length; i++)
	for(i = 0; i < deviceDataArray.length; i++)
	{
		// data = this.list[i];
		data = deviceDataArray[i];

		if(data.mac.toLowerCase() == macLower)
		{
			rowNum = i;
			break;
		}
	}
	
	//assume data exist
	return rowNum;
}





//--------------------------------------------------------------
//		DeviceData
//--------------------------------------------------------------
var deviceData = function(deviceName, vendor, mac, PHYnum,rownumid, stationMode) {

	this.deviceName 	= deviceName;
	this.vendor			= vendor;
	// this.ip				= ip;
	this.mac			= mac
	this.PHYnum 		= PHYnum;
	// this.deviceNameReal	= deviceNameReal;
	this.element;
	
	this.Live = true;
	this.rownumid = rownumid;
	this.proprity = "none";
	this.stationMode = stationMode;

	//--------------------------------------------------------------
	// create element
	this.element 						= document.createElement("div");
	this.element.style.width 			= "188px";
	this.element.style.height 			= "140px";  //86px
	this.element.style.fontSize			= "12px";
	//this.element.style.cursor			= "pointer";
	this.element.style.verticalAlign	= "top";
	this.element.style.margin			= "0 6px";
	this.element.className				= "dataElement";
	this.element.onmouseover = function() {
		this.style.backgroundPosition = "bottom left";
	};
	this.element.onmouseout = function() {
		this.style.backgroundPosition = "top left";
	};
	this.element.id = "plccard"+rownumid;
	this.element.setAttribute("data-num",rownumid);
	
	// DeviceName
	this.element_deviceName 			= document.createElement("div");
	this.element_deviceName.className	= "dataElement_deviceName";
	if(deviceName == "") {
		this.element_deviceName.innerHTML = stationMode;
	} else {
		this.element_deviceName.innerHTML 	= deviceName;
	}
	
	// Vendor
	this.element_vendor 				= document.createElement("div");
	this.element_vendor.className		= "dataElement_vendor";
	this.element_vendor.innerHTML 		= vendor;
	
	// MAC
	this.element_mac					= document.createElement("div");
	this.element_mac.className			= "dataElement_mac";
	this.element_mac.innerHTML 			= mac;
	
	// PHYnum
	this.element_phynum 				= document.createElement("div");
	this.element_phynum.className		= "dataElement_phynum";
	//this.element_phynum.innerHTML		= PHYnum;
	
	//edit_btn
	this.element_editbtn                = document.createElement("div");
	this.element_editbtn.className      = "dataElement_editbtn";

	// // deviceNameReal
	// this.element_deviceNameReal 				= document.createElement("div");
	// this.element_deviceNameReal.className		= "dataElement_deviceNameReal";
	// this.element_deviceNameReal.innerHTML 		= deviceNameReal;
	
	this.element_phynum_num = document.createElement("p");
	this.element_phynum_num.innerHTML = PHYnum;

	this.element_phynum_span = document.createElement("span");
	this.element_phynum_span.innerHTML = "Mbps";

	this.element.appendChild(this.element_deviceName);
	this.element.appendChild(this.element_vendor);
	this.element.appendChild(this.element_mac);
	this.element.appendChild(this.element_phynum);
	this.element_phynum.appendChild(this.element_phynum_num);
	this.element_phynum.appendChild(this.element_phynum_span);
	this.element.appendChild(this.element_editbtn);
	// this.element.appendChild(this.element_deviceNameReal);
	
	/*
	// preset priority status
	if (priority != "none") {
		this.element.style.width 	= "0px";
		this.element.style.opacity 	= "0";
		this.element.style.filter = "alpha(opacity=0)"; //for IE8
		this.element.style.margin	= "0";

		//for IE8
		this.element_deviceName.style.display 	= "none";
		this.element_vendor.style.display 	= "none";
		this.element_ip.style.display 	= "none";
	}
	*/
	
	// var data = this;
	//--------------------------------------------------------------
	// // set priority
	// this.setPriority = function(value) {
	// 	data.priority = value;
	// }
	//--------------------------------------------------------------

	
}


// createDeviceData
var deviceDataArray = new Array(); 		// contain all device in here!
var createDeviceData = function(deviceName, vendor, mac, PHYnum, rownumid, stationMode) {
	var data = new deviceData(deviceName, vendor, mac, PHYnum, rownumid, stationMode);
	deviceDataArray.push(data);
	return data;
}

//		Fill Devices
//--------------------------------------------------------------
// addDevice
function addDevice(deviceName, vendor, mac, PHYnum, rownumid, stationMode) {

	// create data
	var data = createDeviceData(deviceName, vendor, mac, PHYnum, rownumid, stationMode); 
	
	// add data.element to slideshow
	mySlideshow.addItem(data);

	//master or slave UI > editbtn
	if(sessionStorage.getItem('WiFiSONRole') != "Slave") {
		$('.dataElement_editbtn').show();
	} else {
		$('.dataElement_editbtn').hide();
	}
}



//--------------------------------------------------------------
//		Slideshow 
//--------------------------------------------------------------
// setup
var mySlideshow = new Slideshow("slideshow");




//dataElement_editbtn click
$(document).on('click','.dataElement_editbtn',function(){
	clearhometimer();

	var elementid = $(this).closest('.dataElement').attr('data-num');
	var elementid_num = parseInt(elementid);


	//name
	// $('#plc_Name').val($('#'+elementid).find('.dataElement_deviceName').html());
	if(deviceDataArray[elementid_num].deviceName == "") {
		$('#plc_Name').val(deviceDataArray[elementid_num].stationMode);
	} else {
		$('#plc_Name').val(deviceDataArray[elementid_num].deviceName);
	}

	//vendor
	// $('#Vendor_plc').html($('#'+elementid).find('.dataElement_vendor').html());
	$('#Vendor_plc').html(deviceDataArray[elementid_num].vendor.toUpperCase());

	//mac
	// $('#MACAddress_plc').html($('#'+elementid).find('.dataElement_mac').html());
	$('#MACAddress_plc').html(deviceDataArray[elementid_num].mac.toUpperCase());

	//link Rate
	// $('#linkrate_plc').html($('#'+elementid).find('.dataElement_phynum').find('p').html()+'&nbsp;Mbps');
	 $('#linkrate_plc').html(deviceDataArray[elementid_num].PHYnum+'&nbsp;Mbps');

	$('#editPop_plccard').show();
});


function clearCreateRulePOP_plccard(){
	$('#editPop_plccard').hide();
	$('#check_btn_plc').removeClass().addClass('styled_button_dis');
	$('#check_btn_plc').attr('disabled',true);
	$('#plc_Name').val('');
	$('#closeCreatePopBtn_plccard').show();
	$('#closeCreatePopBtn_plccard2').hide();
	$('.plccardform_editPop_savetext').hide();

	starthometimer();
}

function checkUpdate_plc(){
	$('#check_btn_plc').removeClass().addClass('styled_button_s');
	$('#check_btn_plc').attr('disabled',false);
}

function closeEditPOP_plc(){
	var soapAction = new SOAPAction();
	var setClientInfo = new SOAPSetClientInfo();
	var client = new SOAPClientInfo();
	client.MacAddress = $('#MACAddress_plc').html();
	client.NickName = $('#plc_Name').val();


	setClientInfo.ClientInfoLists.push(client);

	var setClientResult = soapAction.sendSOAPAction("SetClientInfo", setClientInfo, null);
	
	$('#check_btn_plc').removeClass().addClass('styled_button_dis');
	$('#check_btn_plc').attr('disabled',true);
	$('#closeCreatePopBtn_plccard').hide();
	$('#closeCreatePopBtn_plccard2').show();
	$('.plccardform_editPop_savetext').show();

	setClientResult.then(function(){
		GetXMLPLC().then(function(){
			clearCreateRulePOP_plccard();
		});		
	});

}


function setplcValidate() {
/*
	$.validator.addMethod("checkIP", function( value, element ) {
		if(value == "")
			return true;

		if(!COMM_ValidV4Format(value) || !COMM_ValidV4Addr(value)){
			return false;
		}
		return true;
	}, jQuery.validator.messages.address_Check);
*/
	$('#plccard_form').validate({
		rules: {
			client_editMac: {
				checkMac: true,
				checkMacConflict: true
			},
			client_IPAdrReserve: {
				checkIP: true,
				checkIPConflict: true,
				checkDHCPrange: true
			},
		},
		submitHandler: function(form) {
			closeEditPOP_plc();
		}
	});	

}