/**
 * @constructor
 */
function SOAPTimeInfo() {
	this.TimeHourValue = "";
	this.TimeMinuteValue = "";
	this.TimeMidDateValue = false;
}

/**
 * @constructor
 */
function SOAPScheduleInfo() {
	this.ScheduleDate = "";
	this.ScheduleAllDay = false;
	this.ScheduleTimeFormat = false;
	this.ScheduleStartTimeInfo = new SOAPTimeInfo();
	this.ScheduleEndTimeInfo = new SOAPTimeInfo();
} 

/**
 * @constructor
 */
function SOAPScheduleInfoLists() {
	var scheduleInfo = new SOAPScheduleInfo();

	this.ScheduleName = "";
	this.ScheduleInfo = $.makeArray(scheduleInfo);
} 

// @prototype
SOAPScheduleInfoLists.prototype = {
	_init:true,
	push : function(data){
		if(this._init) {
			this._init = false;
			this.ScheduleInfo.splice(0,1);
		}
		this.ScheduleInfo.push(data);
		return true;
	}
};

function SOAPSetScheduleSettingsResponse() {
	var scheduleInfoLists = new SOAPScheduleInfoLists();

	this.ScheduleInfoLists = $.makeArray(scheduleInfoLists);
}

// @prototype
SOAPSetScheduleSettingsResponse.prototype = {
	_init:true,
	push : function(data){
		if(this._init) {
			this._init = false;
			this.ScheduleInfoLists.splice(0,1);
		}
		this.ScheduleInfoLists.push(data);
		return true;
	}
};


function SOAPSetScheduleSettingsResponse_null() {

}

/**
 * @constructor
 */
function SOAPGetScheduleSettingsResponse() {
	var scheduleInfoLists = new SOAPScheduleInfoLists();

	this.ScheduleInfoLists = $.makeArray(scheduleInfoLists);
}

/**
 * @constructor
 */
function SOAPGetScheduleRebootResponse() {
	this.Schedule = new SOAPScheduleInfoLists();
}

/**
 * @constructor
 */
function SOAPSetScheduleReboot() {
	this.Schedule = new SOAPScheduleInfoLists();
}

