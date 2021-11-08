
function SOAPGetOperationModeResponse() {
	this.OperationModeList = new SOAPOperationModeList();
}

function SOAPOperationModeList() {
	var operationModeInfo = new SOAPOperationModeInfo();

	this.OperationModeInfo = $.makeArray(operationModeInfo);
}

function SOAPOperationModeInfo() {
	var availableOpMode = new Array();

	this.RadioID = "";
	this.CurrentOPMode = "";
	this.AvailableOPMode = $.makeArray(availableOpMode);
}

function SOAPSetOperationModeResponse() {
	this.RadioID = "";
	this.CurrentOPMode = "";
}
