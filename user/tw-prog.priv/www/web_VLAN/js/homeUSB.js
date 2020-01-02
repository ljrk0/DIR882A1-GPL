//data list
function USBDatalist()
{
	this.list = new Array();
}

USBDatalist.prototype = {

	length : function(){
		return this.list.length;
	},

	prepareUpdate:function(){
		for(var num in this.list)
		{
			this.list[num].Live = false;
		}
	},

	updateData : function(soapUSBDevice){
		var found = false;
		var obj;

		for(var num in this.list)
		{
			obj = this.list[num];

			if(obj.Port == soapUSBDevice.Port)
			{
				obj.Live = true;
				found = true;
				break;
			}
		}

		if(found)
		{
			obj.updateData(soapUSBDevice);
			obj.updateHTML();
		}
		else
		{
			var data = new usb_data(soapUSBDevice);
			this.list.push(data);
			data.addRowToHTML("usb_list");
			data.updateHTML();
		}
	},
  
	updateDetailData : function(soapUSBDevice){

		var found = false;
		var obj;

		for(var num in this.list)
		{
			obj = this.list[num];

			if(obj.Port == soapUSBDevice.Port)
			{
				obj.Live = true;
				found = true;
				break;
			}
		}

		if(found)
		{		
			obj.updateData(soapUSBDevice);
			obj.updateDetailHTML();
		}
		else
		{					
			var data = new usb_data(soapUSBDevice);
			this.list.push(data);
			data.addRowToHTMLDetail("usbDetail_list");
			data.updateDetailHTML();
		}
	},

	clean : function(){
		for(var num in this.list)
		{
			obj = this.list[num];
			if(obj.Live == false)
			{
				obj.removeFromHTML();
				this.list.splice(num, 1);
			}
		}
	},
  
  cleanDetail : function(){
		for(var num in this.list)
		{
			obj = this.list[num];
			if(obj.Live == false)
			{
				obj.removeDetailFromHTML();
				this.list.splice(num, 1);
			}
		}
	}
}

function usb_data(soapUSBDevice)
{
	this.Live = true;

	this.updateData(soapUSBDevice);
}

usb_data.prototype = {
	//property
	Port:null,
	Model:null,
	TotalSpace:null,
	FreeSpace:null,
	SmartStatus:null,
	Live:null,
	PartitionInfoLists: null,
	
	//method
	calSpace :function(size, unitNum)
	{
		var unit = ["KB", "MB", "GB", "TB"];
		var output;

		if(size >= 1024)
		{
			size = parseFloat(size / 1024).toFixed(2);
			output = this.calSpace(size, unitNum+1);
		}
		else
		{
			output = size+" "+unit[unitNum];
		}
		return output;
	},

	updateData: function(soapUSBDevice)
	{
		this.Port = soapUSBDevice.Port;
		this.Model = soapUSBDevice.Model;
		this.TotalSpace = soapUSBDevice.TotalSpace;
		this.FreeSpace = soapUSBDevice.FreeSpace;
		this.SmartStatus = soapUSBDevice.SmartStatus;
		this.PartitionInfoLists = soapUSBDevice.PartitionInfoLists;
	},

	updateHTML: function()
	{
		var totalSpace = parseInt(this.TotalSpace);
		var freeSpace = parseInt(this.FreeSpace);
		// var percent = parseFloat((totalSpace - freeSpace)/totalSpace).toFixed(2) * 100;
		var percent = (parseFloat((totalSpace - freeSpace)/totalSpace)*100).toFixed(2);
		

		$("#usbdev"+this.Port+"_Model").html(HTMLEncode(this.Model));
		$("#usbdev"+this.Port+"_FreeSpace").html(this.calSpace(freeSpace, 0));
		$("#usbdev"+this.Port+"_TotalSpace").html(this.calSpace(totalSpace, 0));

		if(percent > 90)
		{
			$("#usbdev"+this.Port+"_Percent").removeClass("progress-bar-success progress-bar-warning").addClass("progress-bar-danger");
		}
		else if(percent > 70)
		{
			$("#usbdev"+this.Port+"_Percent").removeClass("progress-bar-danger progress-bar-info").addClass("progress-bar-warning");
		}
		else
		{
			$("#usbdev"+this.Port+"_Percent").removeClass("progress-bar-danger progress-bar-warning").addClass("progress-bar-info");
			$("#usbdev"+this.Port+"_Percent").css({'background-color': "#4497A9"}); // d-link green
		}

		$("#usbdev"+this.Port+"_Percent").css("width", percent+"%").attr("aria-valuenow", percent).html(percent+"%");
	
		if(this.SmartStatus.indexOf("ERROR") >= 0)
		{
			$("#usbdev"+this.Port+"_Warning").show();
		}
		else
		{
			$("#usbdev"+this.Port+"_Warning").hide();
		}
	},

	removeFromHTML:function(){
		$("#usbdev"+this.Port).remove();
	},

	addRowToHTML : function(table)
	{
		var outputString;

		outputString = '<div id="usbdev'+ this.Port +'" class="list-group-item" style="border:0px"></div>';
		
		var selector = "#"+table;
		$(selector).append(outputString);
		
		this.setDataToRow($("#usbdev"+this.Port));
		return;
	},
	
	setDataToRow : function(object)
	{
		var outputString;

		outputString = '<h4 class="list-group-item-heading" style="color:#4598aa">'
					 + '<img src="image/usb_Icon.png" alt="" width="28" height="28" />'
					 + I18N("j", "USB Port")+ ':&nbsp;'
					 + '<label id="usbdev'+this.Port+'_Model">'+ HTMLEncode(this.Model) +'</label>'
					 + '</h4>'
					 + '<div class="list-group-item-text">'
					 + '<table class="usbtable">'
					 + '<tr>'
					 + '<div class="progress" style="margin-left:0px;margin-bottom:-0px;width:80%;height:30px">'
					 + '<div id="usbdev'+ this.Port + '_Percent" class="progress-bar myProgressBar" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">'
					 + '</div>'
					 + '</tr>'
					 + '<tr>'
					 + '<th>' + I18N("j", "Available Space") + ':</th>'
					 + '<td id="usbdev'+ this.Port + '_FreeSpace"></td>'
					 + '</tr>'
					 + '<tr>'
					 + '<th>' + I18N("j", "Total Space") + ':</th>'
					 + '<td id="usbdev'+ this.Port + '_TotalSpace"></td>'
					 + '</tr>'
					 + '<tr id="usbdev'+ this.Port + '_Warning" style="display:none">'
					 + '<th colspan="2" class="usb_warning">' 
					 + '<span style="padding-right:20px;font-size:12pt">' + I18N("j", "Health Status: Warning") + '</span>'
					 + '<img src="image/warning_s.png" width="25" height="25" style="border:0" />'
					 + '</th>'
					 + '</tr>'					 
					 + '</table>'
					 + '</div>'
					 + '</div>';

		object.html(outputString);
		return;
	},

  	updateDetailHTML: function()
	{
		var totalSpace = parseInt(this.TotalSpace);
		var freeSpace = parseInt(this.FreeSpace);

		$("#usbDetaildev"+this.Port+"_Model").html(HTMLEncode(this.Model));
		$("#usbDetaildev"+this.Port+"_FreeSpace").html(this.calSpace(freeSpace, 0));
		$("#usbDetaildev"+this.Port+"_TotalSpace").html(this.calSpace(totalSpace, 0));
    	$("#usbDetaildev"+this.Port+"_SmartStatus").html(HTMLEncode(this.SmartStatus));

		var outputString = "";
		var count = 0;
		
		
		for (var partitionUnit in this.PartitionInfoLists.PartitionInfo) {
			var partition = this.PartitionInfoLists.PartitionInfo[partitionUnit];
			var partitionTotalSpace = partition.TotalSpace;
		
		//test
		/*
		for (var i=0;i<5;i++) {
			var partition = "a"+i;
			var partitionTotalSpace = partition.TotalSpace;


		partitionTotalSpace=123;
		partition.PartitionName="potia";
		*/
		//testend

			if (partitionTotalSpace != "") {
				var partitionFreeSpace = partition.FreeSpace;
				var percent = parseFloat((partitionTotalSpace - partitionFreeSpace)/partitionTotalSpace).toFixed(2) * 100;
				//test
				//percent=71;
				//testend


				if(percent > 90)
				{
					outputString +='<a href="#" style="margin-left:31px;" onclick="setFolderValue('+"'"+partition.PartitionName+"'"+')">'
								 +I18N("j", "Partition")+' '+String.fromCharCode(97 + count).toUpperCase()+'</a><br/>'
								 +'<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px;">'
								 +'<div id="usbDetaildev'+this.Port+'_Percent'+partition.PartitionName+'" style="width:'+percent+'%;font-size: 12pt;padding-top: 5px;" class="progress-bar myProgressBar progress-bar-danger" role="progressbar" text="'+percent+'" aria-valuenow="'+percent+'" aria-valuemin="0" aria-valuemax="100">'
								 +percent
								 +'%</div>'
								 +'</div><br/>';
				}
				else if(percent > 70)
				{
					outputString +='<a href="#" style="margin-left:31px;" onclick="setFolderValue('+"'"+partition.PartitionName+"'"+')">'
								 +I18N("j", "Partition")+' '+String.fromCharCode(97 + count).toUpperCase()+'</a><br/>'
								 +'<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px;">'
								 +'<div id="usbDetaildev'+this.Port+'_Percent'+partition.PartitionName+'" style="width:'+percent+'%;font-size: 12pt;padding-top: 5px;" class="progress-bar myProgressBar progress-bar-warning" role="progressbar" text="'+percent+'" aria-valuenow="'+percent+'" aria-valuemin="0" aria-valuemax="100">'
								 +percent
								 +'%</div>'
								 +'</div><br/>';
				}
				else
				{
					outputString +='<a href="#" style="margin-left:31px;" onclick="setFolderValue('+"'"+partition.PartitionName+"'"+')">'
								 +I18N("j", "Partition")+' '+String.fromCharCode(97 + count).toUpperCase()+'</a><br/>'
								 +'<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px;">'
								 +'<div id="usbDetaildev'+this.Port+'_Percent'+partition.PartitionName+'" style="width:'+percent+'%;background-color: rgb(68, 151, 169);font-size: 12pt;padding-top: 5px;" class="progress-bar myProgressBar" role="progressbar" text="'+percent+'" aria-valuenow="'+percent+'" aria-valuemin="0" aria-valuemax="100">'
								 +percent
								 +'%</div>'
								 +'</div><br/>';					
				}


				/*
				if(percent > 90)
				{
					
					outputString += '<a href="#" style="margin-left:31px;" onclick="setFolderValue('+"'"+partition.PartitionName+"'"+')">'+ I18N("j", "Partition") + " " + String.fromCharCode(97 + count).toUpperCase() +'</a><br/>'
								 + '<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px">'
								 + '<div id="usbDetaildev'+ this.Port + '_Percent'+ partition.PartitionName + '" style="width:'+ percent + '%" class="progress-bar myProgressBar progress-bar-danger" role="progressbar" text="'+ percent +'" aria-valuenow="'+ percent +'" aria-valuemin="0" aria-valuemax="100"/>'
								 + percent + '%</div><br/>';				

					$("#usbDetaildev"+this.Port+"_Percent").removeClass("progress-bar-success progress-bar-warning").addClass("progress-bar-danger");

				}
				else if(percent > 70)
				{
					outputString += '<a href="#" style="margin-left:31px;" onclick="setFolderValue('+"'"+ partition.PartitionName +"'"+')">'+ I18N("j", "Partition") + " " + String.fromCharCode(97 + count).toUpperCase() +'</a><br/>'
							     + '<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px">'
						         + '<div id="usbDetaildev'+ this.Port + '_Percent'+ partition.PartitionName + '" style="width:'+ percent + '%" class="progress-bar myProgressBar progress-bar-warning" role="progressbar" text="'+ percent +'" aria-valuenow="'+ percent +'" aria-valuemin="0" aria-valuemax="100"/>'
						         + percent + '%</div><br/>';
					$("#usbDetaildev"+this.Port+"_Percent").removeClass("progress-bar-danger progress-bar-info").addClass("progress-bar-warning");
				}
				else
				{
					outputString += '<a href="#" style="margin-left:31px;" onclick="setFolderValue('+"'"+ partition.PartitionName +"'"+')">'+ I18N("j", "Partition") + " " + String.fromCharCode(97 + count).toUpperCase() +'</a><br/>'
						         + '<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px">'
						         + '<div id="usbDetaildev'+ this.Port + '_Percent'+ partition.PartitionName + '" style="background-color:#4497A9;width:'+ percent + '%" class="progress-bar myProgressBar progress-bar-info" role="progressbar" text="'+ percent +'" aria-valuenow="'+ percent +'" aria-valuemin="0" aria-valuemax="100"/>'
						         + percent + '%</div><br/>';

					$("#usbDetaildev"+this.Port+"_Percent").removeClass("progress-bar-danger progress-bar-warning").addClass("progress-bar-info");
					$("#usbDetaildev"+this.Port+"_Percent").css({'background-color': "#4497A9"}); // d-link green
				}

				count++;
				$("#usbDetaildev"+this.Port+"_Percent").css("width", percent+"%").attr("aria-valuenow", percent).html(percent+"%");
				*/
				count++;
			}
		}

		$("#usbDetaildev"+this.Port+"_PartitionPart").html(outputString);

		/*
		if(this.SmartStatus.indexOf("ERROR") >= 0)
		{
			$("#usbDetaildev"+this.Port+"_Warning").show();
		}
		else
		{
			$("#usbDetaildev"+this.Port+"_Warning").hide();
		}
		*/
	},

	removeDetailFromHTML:function(){
		$("#usbDetaildev"+this.Port).remove();
	},

	addRowToHTMLDetail : function(table)
	{
		var outputString;

		outputString = '<div id="usbDetaildev'+ this.Port +'" class="list-group-item" style="border:0px"></div>';
		
		var selector = "#"+table;
		$(selector).append(outputString);
		
		this.setDataToRowDetail($("#usbDetaildev"+this.Port));
		return;
	},
	
	setDataToRowDetail : function(object)
	{
		var outputString;

		outputString = '<h4 class="list-group-item-heading" style="color:#4598aa">'
					 + '<img src="image/usb_Icon.png" alt="" width="28" height="28" />'
					 + I18N("j", "USB Port")+ ':&nbsp;'
					 + '<label id="usbDetaildev'+this.Port+'_Model">'+ HTMLEncode(this.Model) +'</label>'
					 + '</h4>'
					 + '<div class="list-group-item-text">'
					 + '<table class="usbtable">'
					 + '<tr>'
					 + '<th style="font-weight: bold">' + I18N("j", "S.M.A.R.T Status") + ':</th>'
					 + '<td id="usbDetaildev'+ this.Port + '_SmartStatus"></td>'
					 + '</tr>'
					 + '<tr>'
					 + '<th style="font-weight: bold">' + I18N("j", "Available Space") + ':</th>'
					 + '<td id="usbDetaildev'+ this.Port + '_FreeSpace"></td>'
					 + '</tr>'
					 + '<tr>'
					 + '<th style="font-weight: bold">' + I18N("j", "Total Space") + ':</th>'
					 + '<td id="usbDetaildev'+ this.Port + '_TotalSpace"></td>'
					 + '</tr>'
					 + '<tr>'
					 + '</tr>'					 
					 + '</table>'
					 + '</div>'
           			 + '<div id="usbDetaildev'+ this.Port + '_PartitionPart">'
					 + '</div>';
					 /*
           			 + '<div class="progress" style="margin-left:31px;margin-bottom:0px;width:90%;height:30px">'
					 + '<div id="usbDetaildev'+ this.Port + '_Percent" class="progress-bar myProgressBar" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100"/>'
					 + '</div>';
					 */

		object.html(outputString);
		return;
	}
}
