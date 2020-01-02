/**
 * @constructor
 */
function SOAPTimeInfo()
{
	this.TimeHourValue = "";
	this.TimeMinuteValue = "";
	this.TimeMidDateValue = false;
}

/**
 * @constructor
 */
function SOAPScheduleInfo()
{
	this.ScheduleDate = "";
	this.ScheduleIsAllDay = false;
	this.ScheduleTimeFormat = false;
	this.ScheduleStartTImeInfo = new SOAPTimeInfo();
	this.ScheduleEndTimeInfo = new SOAPTimeInfo();
} 

/**
 * @constructor
 */
function SOAPScheduleInfoLists()
{
	var scheduleInfo = new SOAPScheduleInfo();

	this.ScheduleName = "";
	this.ScheduleInfo = $.makeArray(scheduleInfo);
} 

/**
 * @constructor
 */
function SOAPGetScheduleSettingsResponse()
{
	var scheduleInfoLists = new SOAPScheduleInfoLists();

	this.ScheduleInfoLists = $.makeArray(scheduleInfoLists);
}
