function SOAPTimeInfo(){
	this.TimeHourValue = "";
	this.TimeMinuteValue = "";
	this.TimeMidDateValue = false;
}

/**
 * @constructor
 */
function SOAPStatisticInfo()
{
	this.Sent = "";
	this.Received = "";
	this.TXPackets = "";
	this.RXPackets = "";
	this.TXDropped = "";
	this.RXDropped = "";
	this.Time = new SOAPTimeInfo();
	this.Session = "";
	this.Errors = "";
};

/**
 * @constructor
 */
function SOAPGetInterfaceStatistics()
{
	this.Interface = "";
};


//===============Response==================

/**
 * @constructor
 */
function SOAPInterfaceStatistics()
{
	var statisticInfo = new SOAPStatisticInfo();

	this.StatisticInfo = $.makeArray(statisticInfo);
};

/**
 * @constructor
 */
 function SOAPGetInterfaceStatisticsResponse()
 {
	this.Interface = "";
	this.InterfaceStatistics = new SOAPInterfaceStatistics();
	this.MACAddress = "";
 }