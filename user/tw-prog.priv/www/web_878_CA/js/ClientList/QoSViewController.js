require.config({
	paths: {
		"Websocket": "/js/SOAP/Websocket"
	}
});

define("QoSViewController", ['QoSView', 'Websocket'], function(QoSView, WS){
	
	function QoSViewController(baseDOM, model){
		this.baseDOM = baseDOM;

		this.qosView = new QoSView();
		
		this.model = model;		
		
		this.querying = false;
	}

	QoSViewController.prototype.showQoS = function(qosObj, position){
		var self = this;
		
		this.startQoSQuery();
		
		var qosBlockDOM = this.qosView.generateQoSHTML(qosObj);
		
		//set arrow
		var offset = 50 + 250*position;
		qosBlockDOM.children("img.qos_pointer").css("left", offset+"px");
		
		//events
		qosBlockDOM.find(".QoS_more_icon").off("click").click(function(){
			//disable background scroll
			$("body").css("overflow", "hidden");
			
			$("#qosPop").show();
		});
		
		this.baseDOM.append(qosBlockDOM);
	}

	QoSViewController.prototype.hideQoS = function(){
		this.baseDOM.children(".QoS").remove();
		
		if(this.querying)
			this.stopQoSQuery();
	}

	QoSViewController.prototype.receiveData = function(data){
		if(data.MacAddress != this.model.MacAddress)
			return;
		
		this.qosView.updateHTML(data);
	}

	QoSViewController.prototype.startQoSQuery = function(){
		var self = this;

		var qoscmd = {
			"Command":"GetSpecificClientStatistics",
			"MacAddress":this.model.MacAddress,
			"Action":"start"
		}
		
		initWebsocket(WS).done(function(ws){
			
			ws.register(qoscmd, myDelegate, self);
			self.querying = true;
		});	
	}

	QoSViewController.prototype.stopQoSQuery = function(){
		var self = this;

		var qoscmd = {
			"Command":"GetSpecificClientStatistics",
			"MacAddress":this.model.MacAddress,
			"Action":"stop"
		}
		
		initWebsocket(WS).done(function(ws){
			ws.register(qoscmd, myDelegate, self);
			self.querying = false;
		});	
	}

	//delegate
	var myDelegate = function(jsondata, viewcontroller){
		viewcontroller.receiveData(jsondata);
	}


	return QoSViewController;
});