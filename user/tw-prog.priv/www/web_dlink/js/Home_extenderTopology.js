$(window).load(function() {

	var btnClick = false;

	$('#extenderTopolotyNextBtn').on('click', function() {
		if(btnClick == false) {
			btnClick = true;

			//clear timer
			clearInterval(getClientInfo_timer);
			getClientInfo_timer = null;

			$('svg').remove();  //移除舊的 svg

			//please wait 視窗
			$('#extenderMsgBlock').html(I18N('j','Please wait') + '...');
			$('#extenderMsgBlock').show();

			$('#content').animate({'left':-900}, 1500);
			$('#content2').animate({'left':0}, 1500);

			//Total_MeshExtender
			$('#Total_MeshExtender').html('');	

			setTimeout(function() {
				homeTreeMapFn.get1_1();
				btnClick = false;
			}, 1600);
		}
	});

	$('#extenderTopolotyBackBtn').on('click', function() {
		if(btnClick == false) {
			btnClick = true;

			homeTreeMapFn.closeFn();

			$('#content').css({
				'height': 'auto'
			});
			
			$('#contentBox').animate({"height":'100%'},1500);
			$('#content').animate({'left':0,}, 1500);
			$('#content2').animate({'left':900}, 1500);
			setTimeout(function() {
				$('svg').remove();
				btnClick = false;
			},1600);

			if(getClientInfo_timer == null) {
				getClientInfo_timer = setInterval("Get_ClientInfo()",5000);	
			}			
		}
	});
});

function homeTreeMapFn_datalist(MacAddress, WiFiSONRole, HOP, UplinkMacAddress, UplinkType, ErrorCode, VarInfo) {
	this.MacAddress = MacAddress;
	this.WiFiSONRole = WiFiSONRole;
	this.HOP = HOP;
	this.UplinkMacAddress = UplinkMacAddress;
	this.UplinkType = UplinkType;
	this.ErrorCode = ErrorCode;
	this.VarInfo = VarInfo;
	this.testRoot = 0;
}

var homeTreeMapFn = {
	nowversion:2,
	getxmldata: [],
	getxmldata2: [],
	dataJson: {},
	dataJson2: {},
	hasRoot:0,
	loopNum:0,
	count: 0,
	nodeCount: 0,
	hop: 0,
	width: 0,
	height: 0,

	nowRun: false,
	get_result_getWiFiSONSettings: null,
	get_result_getClientInfo: null,
	get_result_getDeviceSettings: null,
	get_result_finish: null,

	get3Timer: null,
	get2_2Timer: null,
	failFnTimer: null,

	get1_1: function() {
		var me = this;

		var soapAction = new SOAPAction();
		var getWiFiSONSettings = new SOAPGetWiFiSONSettingsResponse();
		var getClientInfo = new SOAPGetClientInfoResponse();
		var getDeviceSettings = new SOAPGetDeviceSettingsResponse();

		var list;

		//run function
		me.nowRun = true;

		if(me.nowversion == 1) {
			me.nowversion = 2;
		} else {
			me.nowversion = 1;
		}

	    me.get_result_getWiFiSONSettings = soapAction.sendSOAPAction("GetWiFiSONSettings", null, getWiFiSONSettings);

	    me.get_result_getClientInfo = me.get_result_getWiFiSONSettings.then(function() {
	    	var dtd = $.Deferred();

	    	me.loopNum = 0;
	    	if(me.nowversion == 1) {
	    		me.getxmldata.length = 0;
	    		me.getxmldata = [];
	    		list = me.getxmldata;
	    	} else {
	    		me.getxmldata2.length = 0;
	    		me.getxmldata2 = [];
	    		list = me.getxmldata2;
	    	}
			
			var obj = getWiFiSONSettings.WiFiSONList.WiFiSON;
			for(var i in getWiFiSONSettings.WiFiSONList.WiFiSON) {
				var data = new homeTreeMapFn_datalist(obj[i].MacAddress, obj[i].WiFiSONRole, obj[i].HOP, obj[i].UplinkMacAddress, obj[i].UplinkType, obj[i].ErrorCode, obj[i].VarInfo);
				list.push(data);
			}	

			soapAction.sendSOAPAction("GetClientInfo", null, getClientInfo).then(function() {
				dtd.resolve();
			}, function() {
				dtd.reject();
			});

			return dtd.promise();

	    }, function() {
	    	me.failFn();
	    });

	    me.get_result_getDeviceSettings = me.get_result_getClientInfo.then(function() {
	    	var dtd = $.Deferred();

	    	var obj = getClientInfo.ClientInfoLists.ClientInfo;
	    	for(var x in getClientInfo.ClientInfoLists.ClientInfo) {
	    		for(var y in list) {
	    			if(list[y].MacAddress == obj[x].MacAddress) {
	    				list[y].NickName = obj[x].NickName;
	    				list[y].SignalStrength = obj[x].SignalStrength;
	    			}
	    		}
	    	}

	    	soapAction.sendSOAPAction("GetDeviceSettings", null, getDeviceSettings).then(function() {
	    		dtd.resolve();
	    	}, function() {
	    		dtd.reject();
	    	});

	    	return dtd.promise();
	    	
	    }, function() {
	    	me.failFn();
	    });

	    me.get_result_finish = me.get_result_getDeviceSettings.then(function() {
	    	for(var y in list) {
	    		if(list[y].WiFiSONRole == 'Master') {
	    			list[y].HOP = -1;
	    			list[y].NickName = getDeviceSettings.DeviceName;
	    		}

	    		if(list[y].WiFiSONRole == 'Root') {
	    			list[y].testRoot = 1;
	    		}
	    	}

		    if(me.nowRun) {
		    	me.get1_2();
		    }
	    }, function() {
	    	me.failFn();
	    });
	},
	get1_2: function() {  //getmeshdiagnostics
		var me = this;

		var list;

	    if(me.nowversion == 1) {
	    	list = me.getxmldata;
	    } else {
	    	list = me.getxmldata2;
	    }

		if(me.nowRun) {
			me.get1_3(list);
		}
	},
	get1_3: function(list) {  // HOP 排序
		var me = this;

		list.sort(function(a, b) {
			return parseInt(a.HOP) - parseInt(b.HOP);
		});

		me.get1_4().then(function() {
			if(me.nowRun) {
				me.get1_5();
			}
		}, function() {
			me.failFn();
		});
	},
	get1_4: function() {  //驗證格式是否有問題 1
		var dtd = $.Deferred();

		var me = this;
		var list;

		if(me.nowversion == 1) {
			list = me.getxmldata;
		} else {
			list = me.getxmldata2;
		}

		//確認第一個HOP為 -1
		if(list.length < 1) {

			dtd.reject();
		} else {
			if(parseInt(list[0].HOP) != -1) {
				dtd.reject();	
			}			
		}

		dtd.resolve();

		return dtd.promise();
	},
	get1_5: function() {  //轉json
		var me = this;

		me.hasRoot = 0;

		if(me.nowversion == 1) {
			me.dataJson = {};

			me.dataJson.MacAddress = me.getxmldata[0].MacAddress;
			me.dataJson.NickName = me.getxmldata[0].NickName;
			me.dataJson.SignalStrength = me.getxmldata[0].SignalStrength;
			me.dataJson.WiFiSONRole = me.getxmldata[0].WiFiSONRole;
			me.dataJson.HOP = me.getxmldata[0].HOP;
			me.dataJson.UplinkMacAddress = me.getxmldata[0].UplinkMacAddress;
			me.dataJson.UplinkType = me.getxmldata[0].UplinkType;
			me.dataJson.ErrorCode = me.getxmldata[0].ErrorCode;
			me.dataJson.VarInfo = me.getxmldata[0].VarInfo;

			if(me.nowRun) {
				me.get1_6(me.getxmldata, me.getxmldata[0], me.dataJson);
			}

			if(me.nowRun == true && me.loopNum < 500) {
				//Root 問題
				for(var i in me.getxmldata) {
					if(me.getxmldata[i].HOP == me.getxmldata[me.getxmldata.length - 1].HOP) {
						if(me.getxmldata[me.getxmldata.length - 1].testRoot == 1) {
							me.hasRoot = 1;
						}
					}
				}

				me.get2_1(me.getxmldata.length, parseInt(me.getxmldata[me.getxmldata.length - 1].HOP) + 1);
			} else {
				me.failFn();
			}
		} else {
			me.dataJson2 = {};

			me.dataJson2.MacAddress = me.getxmldata2[0].MacAddress;
			me.dataJson2.NickName = me.getxmldata2[0].NickName;
			me.dataJson2.SignalStrength = me.getxmldata2[0].SignalStrength;
			me.dataJson2.WiFiSONRole = me.getxmldata2[0].WiFiSONRole;
			me.dataJson2.HOP = me.getxmldata2[0].HOP;
			me.dataJson2.UplinkMacAddress = me.getxmldata2[0].UplinkMacAddress;			
			me.dataJson2.UplinkType = me.getxmldata2[0].UplinkType;
			me.dataJson2.ErrorCode = me.getxmldata2[0].ErrorCode;
			me.dataJson2.VarInfo = me.getxmldata2[0].VarInfo;

			if(me.nowRun) {
				me.get1_6(me.getxmldata2, me.getxmldata2[0], me.dataJson2);
			}

			if(me.nowRun == true && me.loopNum < 500) {
				//Root 問題
				for(var i in me.getxmldata2) {
					if(me.getxmldata2[i].HOP == me.getxmldata2[me.getxmldata2.length - 1].HOP) {
						if(me.getxmldata2[me.getxmldata2.length - 1].testRoot == 1) {
							me.hasRoot = 1;
						}
					}
				}

				me.get2_1(me.getxmldata2.length, parseInt(me.getxmldata2[me.getxmldata2.length - 1].HOP) + 1);
			} else {
				me.failFn();
			}
		}	
	},
	get1_6: function(data, parent, newobj) {
		var me = this;

		for(var i in data) {
			me.loopNum++;

	        if(parseInt(data[i].HOP) == -1) {
	            continue;
	        }

	        if(data[i].UplinkMacAddress == parent.MacAddress) {
	        	if(parent.testRoot == 1) {
	        		data[i].testRoot = 1;	
	        	}
	        	
		        if(newobj.children == undefined) {
		            newobj.children = [];
		            newobj.children.push(data[i]);
		            var a = newobj.children.length - 1;
		            if(me.loopNum < 500 && me.nowRun == true) {
		            	me.get1_6(data,data[i],newobj.children[a]);
		            }
		        } else {
		            newobj.children.push(data[i]);
		            var a = newobj.children.length - 1;
		            if(me.loopNum < 500 && me.nowRun == true) {
		            	me.get1_6(data,data[i],newobj.children[a]);
		        	}
		        }
	        }
		}
	},
	get2_1: function(count, hop) {
		var me = this;

		me.count = count - 1;

		if(hop == 0 || hop == 1 || me.hasRoot == 1) {  //解決只有 Master 或有 Root 問題
			me.hop = hop + 1;
		} else {
			me.hop = hop;
		}

		if(me.hop > 5) {  //最大 1 + 4
			me.hop = 5;
		}

		me.get2_2().done(function() {
  			me.width = 170 * me.hop;  //180
  			me.height = 150 * me.nodeCount;

  			$('svg').remove();  //移除舊的 svg

  			if(me.nowRun) {
  				me.get3();
  			}
		});
	},
	get2_2: function() {
		var me = this;

		var dtd = $.Deferred();

	    me.nodeCount = 0;
	    if(me.nowRun) {
			if(me.nowversion == 1) {
				if(me.dataJson.children == undefined) {  //當沒有 MeshExtender 時
					me.nodeCount = 1;
				} else {
					if(me.nodeCount < 50) {
						me.jsonCountFn(me.dataJson);
					}
				}
			} else {
				if(me.dataJson2.children == undefined) {  //當沒有 MeshExtender 時
					me.nodeCount = 1;
				} else {
					if(me.nodeCount < 50) {
						me.jsonCountFn(me.dataJson2);
					}
				}
			}
		}
			
	    dtd.resolve();

  		return dtd.promise();
	},
	jsonCountFn: function(data) {
		var me = this;

		var nodeNum = 0;

	    for(var i = 0; i < data.children.length; i++) {
	       	if(data.children[i].children != undefined) {
	           	if(me.nowRun && me.nodeCount < 50) {
	           		me.jsonCountFn(data.children[i]);
	           	}
	       	} else {
	           	me.nodeCount++;
	       	}
	    }
	},
	get3: function() {
		var me = this;

		//click text
		var HintBtn_text = I18N('j','Click for diagnostics_ford3');
		var HintBtn_textArray = HintBtn_text.split(",");

		$('#extenderMsgBlock').hide();

		//Total_MeshExtender
		$('#Total_MeshExtender').html(me.count);

		//定義數據轉換函數
		var tree = d3.layout.tree()
			.size([me.height, me.width - 100]);

		//定義svg  #extenderpainting
		var svg = d3.select("#extenderpainting")
			.append("svg")
			.attr("width", me.width)
			.attr("height", me.height)
			.append("g")
			.attr('class','largePaint')
			.attr("transform", "translate(40,10)"); //40,0

		//讀取json，進行繪圖
		var nodes;
		if(me.nowversion == 1) {
			nodes = tree.nodes(me.dataJson);
		} else {
			nodes = tree.nodes(me.dataJson2);
		}
		var links = tree.links(nodes);

		//畫線
		var link = svg.selectAll(".link")
			.data(links)
			.enter()
	    	.append('line')
	    	.attr('class', 'link')
	    	.attr('x1', function(d){return d.source.y})
	    	.attr('y1', function(d){return d.source.x})
	    	.attr('x2', function(d){return d.target.y})
	    	.attr('y2', function(d){return d.target.x})
	    	.attr('stroke', function(d) {  //源頭的 signal 強度
	      		if(parseInt(d.target.SignalStrength) <= 0) {
	      			return '#CCCCCC';
	      		} else if(parseInt(d.target.SignalStrength) >= 1 && parseInt(d.target.SignalStrength) <= 33) {
	      			return '#FF1600';
	      		} else if(parseInt(d.target.SignalStrength) >= 34 && parseInt(d.target.SignalStrength) <= 66) {
	          		return '#FDAE2D';
	        	} else {
	        		return '#63B236';
	        	}
	    	})
	    	.attr('stroke-dasharray',function(d) {  //虛線樣式，還未做漸層
	    		if(parseInt(d.target.SignalStrength) <= 0) {
	          		return '20, 7';
	    		} else {
	          		return '0';
	    		}
	      	});


		//畫點
		var node = svg.selectAll(".node")
			.data(nodes)
			.enter()
			.append("g")
			.attr("class", "node")
			.attr("transform", function(d) {
	    		return "translate(" + d.y + "," + d.x + ")"; 
	   		});

		//icon 圖片
		node.append("image")
			.attr("xlink:href", function(d) {
	    		if(d.WiFiSONRole == 'Master') {
	     			return 'image/extendertopology_router.png';  //圖片
	     		} else {
	     			return 'image/extendertopology_extender.png';
	     		}
	     		// return d.icon; 
	    	})
	    	.attr("x", "-45px")
	    	.attr("y", "-45px")
	    	.attr("width", "90px")
	    	.attr("height", "90px");

		//文字
		node.append("text")
			.attr("dy", -45)
			.attr("fill", "#7e7e7e")
			.attr("font-size", "14px")
			.style("text-anchor", "middle")
			.text(function(d) {
    			return decode_char_text(d.NickName);
    		});

	    //在 line 上增加警示 icon
	    var hintIcon = svg.selectAll(".hintIcon")
		    .data(links)
		    .enter()
		    .append("svg:image")
		    .attr("class","hintIcon")
		    .attr("xlink:href",  function(d) {
		    	return "image/warning.png";
		    })
	        .attr("x", function(d) { 
	        	return d.source.y + (d.target.y - d.source.y) / 2 - 15; 
	        })	    
	        .attr("y", function(d) { 
	        	return d.source.x + (d.target.x - d.source.x) / 2 - 18; 
	        })
		    .attr("height", function(d) { 
		   		return 30;
		    })
		    .attr("width", function(d) {
		    	return 30;
		    })
		    .style("display",function(d) {
		    	if(d.target.ErrorCode == "") {
		    		return "none";
		    	} else {
		    		return "block";
		    	}
		    });		    

	    //在 line 上增加警示按鈕
	    var hintButton = svg.selectAll(".meshExtenderHintBtn")
		    .data(links)
		    .enter()
		    .append("g")
		    .attr("class","meshExtenderHintBtn")
		    .style("display",function(d) {
		    	if(d.target.ErrorCode == "") {
		    		return "none";
		    	} else {
		    		return "block";
		    	}
		    });

		hintButton.append("path")
			.attr("d", function(d) {
				var x1 = d.source.y + (d.target.y - d.source.y) / 2 - 7;
				var x2 = d.source.y + (d.target.y - d.source.y) / 2 ;
				var x3 = d.source.y + (d.target.y - d.source.y) / 2 + 7;
				var y1 = d.source.x + (d.target.x - d.source.x) / 2 + 15;
				var y2 = d.source.x + (d.target.x - d.source.x) / 2 + 20;
				
				return 'M ' + x1 +' '+ y2 +
				       ' L' + x3 + ' ' + y2 +
				       ' L' + x2 + ' ' + y1 +
				       ' z';
			})
			.attr("stroke","red")
			.attr("stroke-width","2")
			.attr("fill","red");

		hintButton.append("svg:rect")
			.attr("class", "meshExtenderHintBtn_rect")
	        .attr("x", function(d) {
	        	if(HintBtn_textArray.length < 2) {
	        		return d.source.y + (d.target.y - d.source.y) / 2 - 75;
	        	} else {
	        		return d.source.y + (d.target.y - d.source.y) / 2 - 90;
	        	}
	        })	    
	        .attr("y", function(d) { 
	        	return d.source.x + (d.target.x - d.source.x) / 2 + 20;   //+往下移
	        })
		    .attr("height", function(d) {
		    	if(HintBtn_textArray.length < 2) {
					return 30;
				} else {
					return 50;
				} 
		    })
		    .attr("width", function(d) {
		    	if(HintBtn_textArray.length < 2) {
		    		return 150;
		    	} else {
		    		return 180;
		    	}
		    })
		    .attr("rx","5")
		    .attr("ry","5")
		    .attr("fill","red")
		    .attr("cursor","pointer");

		hintButton.append("svg:text")
			.attr("class","meshExtenderHintBtn_text")
			.attr("x", function(d) {
			    var w = this.getComputedTextLength(); //<-- length of this node
			    return d.source.y + (d.target.y - d.source.y) / 2 - w / 2;
		    })
	        .attr("y", function(d) { 
	        	return d.source.x + (d.target.x - d.source.x) / 2 + 18 + 22;  //20為文字再往下
	        })		    
		    .attr("height", function(d) {
		    	return 30;
		    })
		    .attr("width", function(d) {
		    	return 80;
		    })
		    .attr("fill","#FFFFFF")
		    .attr("cursor","pointer")
		    .style("text-anchor", "middle")
		    .text(function(d) {
				return HintBtn_textArray[0];
		    });

		hintButton.append("svg:text")
			.attr("class","meshExtenderHintBtn_text")
			.attr("x", function(d) {
			    var w = this.getComputedTextLength(); //<-- length of this node
			    return d.source.y + (d.target.y - d.source.y) / 2 - w / 2;
		    })
	        .attr("y", function(d) { 
	        	return d.source.x + (d.target.x - d.source.x) / 2 + 18 + 22 + 20;  //20為文字再往下 + 25
	        })		    
		    .attr("height", function(d) {
		    	if(HintBtn_textArray.length < 2) {
					return 0;
				} else {
					return 30;
				} 
		    })
		    .attr("width", function(d) {
		    	return 80;
		    })
		    .attr("fill","#FFFFFF")
		    .attr("cursor","pointer")
		    .style("text-anchor", "middle")
		    .text(function(d) {
		    	if(HintBtn_textArray.length < 2) {
					return '';
				} else {
					return HintBtn_textArray[1];
				} 
		    });
	
		svg.selectAll(".meshExtenderHintBtn_rect, .meshExtenderHintBtn_text")
		    .on("click", function(d) {
		    	$('.popup_window_border').css({'top':'150px'});
		    	$('#popalert_title').css({'font-size':'20px'});
		    	$('#popalert_desc').css({'text-align':'left'});

		    	var varInfoArray = d.target.VarInfo.split(",");
		    	var text;

		    	if(d.target.ErrorCode == '1') {
		    		text = I18N('j','The uplink signal strength from <span class="errorCodeText">[Audit AP]</span> to <span class="errorCodeText">[Uplink AP]</span> is too weak and may be affecting wireless performance.<br/>Try moving <span class="errorCodeText">[Audit AP]</span> closer to <span class="errorCodeText">[Uplink AP]</span> until the signal is strong enough. You may also place another access point between <span class="errorCodeText">[Audit AP]</span> and <span class="errorCodeText">[Uplink AP]</span> to improve signal strength.');
		    		text = text.replace(/\[Audit AP\]/g, varInfoArray[0]);
		    		text = text.replace(/\[Uplink AP\]/g, varInfoArray[2]);
		    	} else if(d.target.ErrorCode == '2') {
		    		text = I18N('j','<span class="errorCodeText">[Audit AP]</span> is <span class="errorCodeText">[Hops]</span> hops away within the mesh network. Too many hops may decrease overall wireless performance.<br/>Try connecting the AP <span class="errorCodeText">[Audit AP]</span> to a nearby access point using Ethernet to decrease the amount hops and improve wireless performance.');
		    		text = text.replace(/\[Audit AP\]/g, varInfoArray[0]);
		    		text = text.replace(/\[Hops\]/g, varInfoArray[2]);
		    	} else {
		    		text = I18N('j','<span class="errorCodeText">[Audit AP]</span> is currently connected to <span class="errorCodeText">[Num]</span> downlink APs. Too many downlink AP per device may decrease overall wireless performance.<br/>Try adding an additional root AP to share the load of <span class="errorCodeText">[Audit AP]</span> and improve performance.');
		    		text = text.replace(/\[Audit AP\]/g, varInfoArray[0]);
		    		text = text.replace(/\[Num\]/g, varInfoArray[2]);
		    	}

				PopView.showWarningMSG(I18N('j', 'Diagnostics'), text).done(function(){
					PopView.hide();
			    	$('.popup_window_border').css({'top':'15%'});
			    	$('#popalert_title').css({'font-size':'16px'});
			    	$('#popalert_desc').css({'text-align':'center'});
				});		    	
		    });
    


		//誤差調整
		var realheight = $('.largePaint')[0].getBBox().height;
		var margintop = (me.height - realheight) / 2;

		if(realheight > 0) {
			$('svg').css({'margin-top': '-' + margintop + 'px'});
			$('svg').css({'height':$('svg').height() - margintop + 50});
		}

		//讓小型topology對準backbtn位置
		var svgheight = $('svg').outerHeight(true);
		var forPaintingPaddingTop = 400 - (svgheight / 2);

		if(currentDevice.featureUsbPort == true && currentDevice.featureMeshExtenderTopology == true) {
			forPaintingPaddingTop = 290 - (svgheight / 2);
		}		

		if(forPaintingPaddingTop < 160) {
			forPaintingPaddingTop = 160;
		}

		$('#extenderpainting').css({
			'padding-top': forPaintingPaddingTop,
			'height': forPaintingPaddingTop + svgheight
		});

		$('#content, #content2').css({
			'height': $('#extenderpainting').outerHeight(true)
		});

		me.get3Timer = setTimeout(function() {
			if(me.nowRun) {
				me.get1_1();
			}
		}, 15000);
	},
	failFn: function() {
		var me = this;

		me.get_result_getWiFiSONSettings = null;
		me.get_result_getClientInfo = null;
		me.get_result_getDeviceSettings = null;
		me.get_result_finish = null;

		clearTimeout(me.get3Timer);
		clearTimeout(me.get2_2Timer);
		clearTimeout(me.failFnTimer);

		//Total_MeshExtender
		$('#Total_MeshExtender').html('');

		//顯示"請稍等"
		$('#extenderMsgBlock').html(I18N('j','Data error') + ", " + I18N('j', 'please wait') + '...');
		$('#extenderMsgBlock').show();
		$('svg').remove();  //移除舊的 svg
	
		me.failFnTimer = setTimeout(function() {
			if(me.nowRun) {
				me.get1_1();
			}
		}, 5000);

	},
	closeFn: function() {
		var me = this;
	
		me.nowRun = false;
		me.get_result_getWiFiSONSettings = null;
		me.get_result_getClientInfo = null;
		me.get_result_getDeviceSettings = null;
		me.get_result_finish = null;

		clearTimeout(me.get3Timer);
		clearTimeout(me.get2_2Timer);
		clearTimeout(me.failFnTimer);
	}
};