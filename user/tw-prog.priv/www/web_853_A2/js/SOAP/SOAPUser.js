/**
 * @constructor
 */
function SOAPGetUsersSettings()
{
	this.StorageUsersLists = new SOAPStorageUsersLists();
};

/**
 * @constructor
 */
function SOAPSetUsersSettings()
{
	this.StorageUsersLists = new SOAPStorageUsersLists();
};
/**
 * @constructor
 */
function SOAPStorageUser()
{
	this.Enabled = true;
	this.UserName = "";
	this.Password = "";
	this.ServiceInfoLists = new SOAPServiceInfoLists();
};

/**
 * @constructor
 */
function SOAPStorageUsersLists()
{
	var storageUser = new SOAPStorageUser();

	this.StorageUser = $.makeArray(storageUser);
};

/**
 * @constructor
 */
function SOAPServiceInfoLists()
{
	var serviceInfo = new SOAPServiceInfo();

	this.ServiceInfo = $.makeArray(serviceInfo);
};

/**
 * @constructor
 */
function SOAPServiceInfo()
{
	this.Enabled = true;
	this.ServiceName = "";
	this.AccessPath = "";
	this.Permission  = true;
};

/**
 * @constructor
 */
function SOAPDelUserSettings()
{
	this.UserName = "";
};
/**
 * @constructor
 */
function SOAPGetVPNProfile(){
	this.Username = "";
}
/*
function SOAPGetQuickVPNProfile()
{
	this.Username ="";
};

function SOAPGetQuickVPNProfileResponse()
{

};
*/
// @prototype
SOAPServiceInfoLists.prototype = 
{
	_init:true,
	push : function(data){
		if(this._init)
		{
			this._init = false;
			this.ServiceInfo.splice(0,1);
		}
		this.ServiceInfo.push(data);
		return true;
	}
};

// @prototype
SOAPStorageUsersLists.prototype = 
{
	_init:true,
	push : function(data){
		if(this._init)
		{
			this._init = false;
			this.StorageUser.splice(0,1);
		}
		this.StorageUser.push(data);
		return true;
	}
};
