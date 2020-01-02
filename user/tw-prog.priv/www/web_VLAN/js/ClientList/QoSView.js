define("QoSView", function(){
	var baseDOM = $("#qos_obj").clone();
	
	var Category = function(name, DOM)
	{
		this.name = name;
		this.DOM = DOM;
		
		this.Live = true;
		
		this.upload = 0;
		this.download = 0;
	}
	var CategoryList = function(){
		this.list = new Array();
	}
	
	CategoryList.prototype =
	{
		get length(){
			return this.list.length;
		}
	}

	CategoryList.prototype.prepareUpdate = function(){
		for(var num in this.list)
		{
			this.list[num].Live = false;
			this.list[num].upload = 0;
			this.list[num].download = 0;
		}
	}

	CategoryList.prototype.clean = function(){
		for(var num in this.list)
		{
			var obj = this.list[num];
			if(obj.Live == false)
			{
				obj.DOM.remove();
				this.list.splice(num, 1);
			}
		}
	}
	
	CategoryList.prototype.getData = function(name){
		var i;
		var data = null;
		
		for(var i in this.list)
		{
			var obj = this.list[i];
			if(obj.name == name)
			{
				data = obj;
				break;
			}
		}
		
		return data;	
	}

	CategoryList.prototype.push = function(data){
		this.list.push(data);
		return true;
	}

	var QoSView = function()
	{
		this.DOM;
		this.categoryList = new CategoryList();
	}

	QoSView.prototype.generateQoSHTML = function(model)
	{
		var qosDOM = baseDOM.clone();
		qosDOM.children(".QoS_Content").remove();
		qosDOM.css("display", "block");
		
		/*var headerDOM = qosDOM.find(".QoS_Wifi_band");
		switch(model.Type)
		{
			case "LAN":
				headerDOM.children('img[name="QoS_icon"]').attr("src", "image/ethernet.png");
				break;
			case "WiFi_2.4G":
			case "WiFi_2.4G_Guest":
				headerDOM.children('span[name="QoS_Wifi_band_24"]').addClass("QoS_Wifi_band_active");
				break;
			case "WiFi_5G":
			case "WiFi_5G_Guest":
				headerDOM.children('span[name="QoS_Wifi_band_5"]').addClass("QoS_Wifi_band_active");
				break;
			case "WiFi_5G_2":
			case "WiFi_5G_2_Guest":
				headerDOM.children('span[name="QoS_Wifi_band_52"]').addClass("QoS_Wifi_band_active");
				break;
		}*/
		
		this.DOM = qosDOM;
		return this.DOM;
	}
	
	QoSView.prototype.updateHTML = function(qosmodel)
	{
		var total_upload = 0;
		var total_download = 0;
		
		for(var i in qosmodel.AppSummary)
		{
			var categoryObj = qosmodel.AppSummary[i];
			total_upload += parseInt(categoryObj.ULRate);
			total_download += parseInt(categoryObj.DLRate);
		}
		this.DOM.find(".QoS_Speed").children("span[name='uplink']").html(total_upload);
		this.DOM.find(".QoS_Speed").children("span[name='downlink']").html(total_download);
		
		this.categoryList.prepareUpdate();
		for(var i in qosmodel.AppSummary)
		{
			var show = true;
			var pic = "";
			var name = "";
			var categoryObj = qosmodel.AppSummary[i];
			var orgName = categoryObj['Category'];
			
			switch(orgName)
			{
				case "streamming":
					pic = "image/StreamingMedia.png";
					name = "Streamming media";
					break;
				case "web":
					pic = "image/WebSurfing.png";
					name = "Web Surfing";
					break;
				case "file":
					pic = "image/FileTransfer.png";
					name = "File transfer";
					break;
				case "others":
					pic = "image/Others (1).png";
					name = "Others";
					break;
				case "game":
					pic = "image/Games.png";
					name = "Game";
					break;
				case "chat":
					pic = "image/OnlineChat.png";
					name = "Online Chat";
					break;				
				default:
					show = false;
					break;
			}
			
			if(show)
			{
				var category = this.categoryList.getData(orgName);
				var contentDOM;
				if(category == null)
				{
					//Add
					contentDOM = baseDOM.children(".QoS_Content").clone();
					contentDOM.css("display", "inline-block");
					this.DOM.append(contentDOM);	
					
					category = new Category(orgName, contentDOM);
					this.categoryList.push(category);
				}
				else
				{
					//update
					contentDOM = category.DOM;
					category.Live = true;
				}
				
				contentDOM.children(".QoS_Content_icon").attr("src", pic);
				contentDOM.children(".QoS_Content_name").html(name);
				
				var upload = parseInt(categoryObj['ULRate']);
				var download = parseInt(categoryObj['DLRate']);
				category.upload += upload;
				category.download += download;
				var upload_rate = (category.upload/total_upload)*100;
				var download_rate = (category.download/total_download)*100;

				var contentBarDOM = contentDOM.children(".QoS_Content_bar");
				contentBarDOM.find("div[name='upload_bar']").css("width", upload_rate+"%").attr("aria-valuenow", upload_rate).html(category.upload);
				//contentBarDOM.find("span[name='upload_value']").html(upload);
				contentBarDOM.find("div[name='download_bar']").css("width", download_rate+"%").attr("aria-valuenow", download_rate).html(category.download);
				//contentBarDOM.find("span[name='download_value']").html(download);
			}
				
		}
		this.categoryList.clean();
		
		this.updateDetail(qosmodel.AppSummary);
	}
	
	QoSView.prototype.updateDetail = function(appsummary){
		var tbody = $("#qosPop_table_body");
		tbody.empty();
		
		for(var i in appsummary)
		{
			var app = appsummary[i];
			
			var tableRow = "<tr>"
					 + '<td style="width:30%">'+ app.Category +'</td>'
					 + '<td style="width:40%">'+ app.Application +'</td>'
					 + '<td style="width:15%">'+ app.ULRate +'</td>'
					 + '<td style="width:15%">'+ app.DLRate +'</td>'
					 + '</tr>';
		
			tbody.append(tableRow);	
		}
	}

	//global event
	function setGlobalEvents(){
		$("#closeQoSPopBtn").off("click").click(function(){
			$("#qosPop").hide();
			//enable background scroll
			$("body").css("overflow", "auto");
		});
		
	}
	setGlobalEvents();

	return QoSView;
});