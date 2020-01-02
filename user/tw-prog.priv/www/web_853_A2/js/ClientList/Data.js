define("Data", function(){
	function Data(macAddress, ipv4Address, ipv6Address, type, deviceName, nickName, reserveIP){
		this.MacAddress = macAddress;
		this.IPv4Address = ipv4Address;
		this.IPv6Address = ipv6Address;
		this.Type = type;
		this.DeviceName = deviceName;
		this.NickName = nickName;
		this.ReserveIP = reserveIP;
		this.DeviceType = "";
		this.SignalStrength = "";
		
		this.Vendor = getVendor(macAddress);
		
		//mac filter
		this.ScheduleName = "Always";
		this.MacfilterEnabled = false;
		this.Status = false;

		this.Live = true;
	};

	//method
	Data.prototype = {
		get NickName(){
			if(this._NickName == "")
				return this.DeviceName;
			
			return this._NickName;
		},
		
		set NickName(val){
			this._NickName = val;
			if(val == this.DeviceName)
				this._NickName = "";
		},
		get ScheduleName(){
			return this._ScheduleName;
		},
		
		set ScheduleName(val){
			if((val == "")||(val.toLowerCase() == 'always'))
			{
				this._ScheduleName = "Always";
			}
			else
			{
				this._ScheduleName = val;
			}
		},

		setRowid: function(rowid)
		{
			this.rowid = rowid;
		}
	}
	return Data;
});