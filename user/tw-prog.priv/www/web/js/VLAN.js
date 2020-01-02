
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
	
	return true;
}

Datalist.prototype.length = function(){
	return this.list.length;
}

Datalist.prototype.checkData = function(newdata, rowNum){
	var i;
	
	//check
	for(i = 0; i < this.list.length; i++)
	{
		if(i == rowNum)
			continue;
	
		if(this.list[i].service == newdata.service)
		{
			alert(I18N("j","Service cannot be the same."));
			return false;
		}		
	}
	return true;
}

//constructor
function Data(service, vid, priority){
	this.service = service;
	this.vid = vid;
	this.priority = priority;
}

Data.prototype = 
{
	//property
	rowid:null,
	
	//method
	setRowid : function(rowid)
	{
		this.rowid = rowid;
	}
}

//data list
function DatalistRule()
{
	this.list = new Array();
	this.maxrowid = 0;
}

DatalistRule.prototype .getData = function(rowid){
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

DatalistRule.prototype .getRowNum = function(rowid){
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

DatalistRule.prototype.editData = function(id, newdata){
	var rowNum = this.getRowNum(id);
	
	if(this.checkData(newdata, rowNum) == false)
	{
		return false;
	}
	
	newdata.setRowid(id);
	this.list.splice(rowNum,1,newdata);
	
	return true;
}

DatalistRule.prototype.deleteData = function(id){
	var rowNum = this.getRowNum(id);

	this.list.splice(rowNum, 1);
	
	$("#tr"+id).remove();
}

DatalistRule.prototype.push = function(data){
	if(this.checkData(data, null) == false)
	{
		return false;
	}

	data.setRowid(this.maxrowid);
	this.list.push(data);
	
	this.maxrowid++;
	
	return true;
}

DatalistRule.prototype.length = function(){
	return this.list.length;
}

DatalistRule.prototype.checkData = function(newdata, rowNum){
	var i;
	
	//check
	for(i = 0; i < this.list.length; i++)
	{
		if(i == rowNum)
			continue;
	
		if(this.list[i].interface_ == newdata.interface_)
		{
			alert(I18N("j","Interface cannot be the same."));
			return false;
		}		
	}
	return true;
}

function DataRule(interface_, mapping){
	this.interface_ = interface_;
	this.mapping = mapping;
}

DataRule.prototype = 
{
	//property
	rowid:null,
	
	//method
	setRowid : function(rowid)
	{
		this.rowid = rowid;
	}
}