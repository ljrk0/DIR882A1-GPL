define("Datalist", ['Data'], function(Data){
	function Datalist()
	{
		this.list = new Array();
		this.maxrowid = 0;

		//Add tag
//		var newdata = new Data("", "", "", "New", "", "", "");
//		this.push(newdata);
	}

	Datalist.prototype =
	{
		get length(){
			return this.list.length;
		},
		
		set length(val){
			if(parseInt(val, 10) >= 0)
				this.list.length = parseInt(val,10);
		},
		
		get connectedLength(){
			var macfilterCount = 0;
			var offline = 0;
			for(var i in this.list)
			{
				if(this.list[i].Type == "Blocked")
				{
					macfilterCount++;
				}
				if(this.list[i].Type.toLowerCase() == "offline")
				{
					offline++;
				}
			}
			return this.list.length - macfilterCount - offline;
		}
	}

	Datalist.prototype.prepareUpdate = function(){
		for(var num in this.list)
		{
			this.list[num].Live = false;
		}
	}

	Datalist.prototype.clean = function(){
		for(var num in this.list)
		{
			var obj = this.list[num];
			if(obj.Live == false)
			{
				//obj.removeFromHTML();
				this.list.splice(num, 1);
			}
		}
	}

	Datalist.prototype.getData = function(rowid){
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

	Datalist.prototype.getDataRowNumByMac = function(mac){
		var i;
		var data = null;
		var rowNum = null;
		var macLower = mac.toLowerCase();
		
		for(i = 0; i < this.list.length; i++)
		{
			data = this.list[i];
			if(data.MacAddress.toLowerCase() == macLower)
			{
				rowNum = i;
				break;
			}
		}
		
		//assume data exist
		return rowNum;
	}

	Datalist.prototype.getRowNum = function(rowid){
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
		var obj = this.list[this.getRowNum(id)];
		
		obj.IPv4Address = newdata.IPv4Address;
		obj.IPv6Address = newdata.IPv6Address;
		obj.Type = newdata.Type;
		obj.DeviceName = newdata.DeviceName;
		obj.NickName = newdata.NickName;
		obj.ReserveIP = newdata.ReserveIP;
		obj.Vendor = newdata.Vendor;
		obj.ScheduleName = newdata.ScheduleName;
		obj.MacfilterEnabled = newdata.MacfilterEnabled;
		obj.Status = newdata.Status;
		obj.DeviceType = newdata.DeviceType;
		obj.SignalStrength = newdata.SignalStrength;

		obj.Live = true;

		return true;
	}

	Datalist.prototype.deleteData = function(id){
		var rowNum = this.getRowNum(id);
		this.list.splice(rowNum, 1);
	}

	Datalist.prototype.push = function(data){
		data.setRowid(this.maxrowid);
		this.list.push(data);
		
		this.maxrowid++;

		//sort
		this.list.sort(function(a,b){

			function getPriority(type)
			{
				if(type == "LAN")
				{
					return 0;
				}
				else if(type.toUpperCase().indexOf("WIFI_") >= 0)
				{
					return 1;
				}
				else
				{
					return 2;
				}
			};

			return getPriority(a.Type) - getPriority(b.Type);
		});

		return true;
	}


	return Datalist;
});