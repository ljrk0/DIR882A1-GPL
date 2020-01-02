
//data list
function Datalist()
{
	this.list = new Array();
	this.maxrowid = 0;
}

Datalist.prototype .getData = function(rowid){
	var i;
	var data;
	
	for(i = 0; i < this.list.length; i++)
	{
		data = this.list[i];
		if(data.rowid == rowid)
		{
			break;
		}
	}
	
	//assume data exist
	return data;
}

Datalist.prototype .getRowNum = function(rowid){
	var rowNum = 0;
	for(rowNum = 0; rowNum < this.list.length; rowNum++)
	{
		if(rowid == this.list[rowNum].rowid)
		{
			break;
		}
	}
	return rowNum;
}

Datalist.prototype.editData = function(id, newdata){
	var rowNum = this.getRowNum(id);
	if(this.checkData(newdata, rowNum) == false)
	{
		return false;
	}
	
	newdata.setRowid(id);
	this.list.splice(rowNum,1,newdata);
	
	newdata.setDataToRow($("#tr"+newdata.rowid));
	return true;
}

Datalist.prototype.deleteData = function(id){
	var rowNum = this.getRowNum(id);
	this.list.splice(rowNum, 1);
	
	$("#tr"+id).remove();
}

Datalist.prototype.push = function(data){
	if(this.checkData(data, null) == false)
	{
		return false;
	}

	data.setRowid(this.maxrowid);
	this.list.push(data);
	
	this.maxrowid++;
	
	data.addRowToHTML('tblPortForwarding');
	
	return true;
}

Datalist.prototype.checkData = function(newdata, rowNum){
	var i;
	
	//check
	for(i = 0; i < this.list.length; i++)
	{
		if(i == rowNum)
			continue;
	
		if(this.list[i].name == newdata.name)
		{
			alert(I18N("j","Name cannot be the same."));
			return false;
		}
		
		if((this.list[i].ipAddress == newdata.ipAddress) &&
		    (this.list[i].tcpPort == newdata.tcpPort) &&
			(this.list[i].udpPort == newdata.udpPort) &&
			(this.list[i].schedule == newdata.schedule))
		{
			alert(I18N("j","Rule cannot be the same."));
			return false;	
		}		
	}
	return true;
}

Datalist.prototype.length = function(){
	return this.list.length;
}

//constructor
function Data(name, ipAddress, tcpPort, udpPort, schedule){
	this.name = name;
	this.ipAddress = ipAddress;
	this.tcpPort = this.reassemblePort(tcpPort);
	this.udpPort = this.reassemblePort(udpPort);
	this.schedule = schedule;
	
	this.setEnable("true");
}


Data.prototype = 
{
	//property
	rowid:null,
	enable:null,
	name:null,
	ipAddress:null,
	tcpPort:null,
	udpPort:null,
	schedule:null,
	
	//method
	setRowid : function(rowid)
	{
		this.rowid = rowid;
	},
	
	setEnable : function(enable)
	{
		this.enable = enable;
	},
	
	reassemblePort : function(port)
	{
		var rangeValue = port.split(",");
		var outputString = "";
		
		if(port == "")
		{
			return outputString;
		}
		
		for(var i = 0; i < rangeValue.length; i++)
		{
			if(i > 0)
			{
				outputString += ",";
			}
		
			var portValue = rangeValue[i].split("-");
			
			outputString += parseInt(portValue[0], 10);
			if(isNaN(portValue[1]) == false)
			{
				outputString += "-" + parseInt(portValue[1], 10);
			}
		}
		return outputString;
	},
	
	showEnable : function()
	{
		var outputString;
		var checkedString = "";
		var inverseChecked = "true";
		
		if(this.enable == "true")
		{
			checkedString = "checked";
			inverseChecked = "false";
		}
		
		outputString = '<input type="checkbox" onChange="datalist.list['+this.rowid+'].setEnable(\''+inverseChecked+'\')" '+checkedString+'/>';
	
		return outputString;
	},
	
	showName: function()
	{
		return HTMLEncode(this.name);
	},
	
	showTCPPort : function()
	{
		if(this.tcpPort == "")
		{
			return "N/A";
		}
		else
		{
			return this.tcpPort;
		}
	},
	
	showUDPPort : function()
	{
		if(this.udpPort == "")
		{
			return "N/A";
		}
		else
		{
			return this.udpPort;
		}
	},
	
	showSchedule : function()
	{
		if((this.schedule == "Always")||(this.schedule == ""))
		{
			return I18N("j", "Always");
		}
		else
		{
			return HTMLEncode(this.schedule);
		}
	},
	
	addRowToHTML : function(table)
	{
		var outputString;
		
		outputString = "<tr id='tr"+ this.rowid + "'></tr>"
		
		var selector = "#"+table+"> tbody";
		$(selector).append(outputString);
		
		this.setDataToRow($("#tr"+this.rowid));
		return;
	},
	
	setDataToRow : function(object)
	{
		var outputString;
	
		outputString = "<td>" + this.showEnable() + "</td>";
		outputString += "<td>" + this.showName() + "</td>";
		outputString += "<td>" + this.ipAddress + "</td>";
		outputString += "<td>" + this.showTCPPort() + "</td>";
		outputString += "<td>" + this.showUDPPort() + "</td>";
		outputString += "<td>" + this.showSchedule() + "</td>";
		outputString += "<td><img src='image/edit_btn.gif' width=28 height=28 style='cursor:pointer' onclick='editData("+this.rowid+")'/></td>";
		outputString += "<td><img src='image/trash.gif' width=41 height=41 style='cursor:pointer' onclick='deleteData("+this.rowid+")'/></td>";
	
		object.html(outputString);
		return;
	}
}
