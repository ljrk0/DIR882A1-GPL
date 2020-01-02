//temp 
var org_pppoe_enable_pd = true;
var org_static_wan_ip = "";
var org_wan_prefix_length = "";

$(document).ready(function(){

	$("#ipv6_dns_obtain").change(function(){
		var value = $(this).val();

		switch(value)
		{
			case "auto":
				$(".ipv6_dns").hide();
				break;
			case "manual":
				$(".ipv6_dns").show();
				$("#primaryDNS").focus();
				break;
		}

	});

	$("#autoconfig_type").change(function(){
		var value = $(this).val();
		
		var enabled = $("#enable_pd").prop("checked");
		
		switch(value)
		{
			case "rdnss":
			case "stateless":
				$(".lan_dhcp").hide();
				$(".lan_ra").show();
				$("#ra_lifetime").focus();
				$(".lan_dhcp2").hide();
				break;
			case "stateful":
				$(".lan_ra").hide();
				$(".lan_dhcp").show();
				$("#dhcp_start").focus();
				
				if(enabled){
					$('.lan_dhcp2').hide();
				}else{
					$('.lan_dhcp2').show();
				}

				break;
		}

	});

	$("#enable_lan_autoconfig").on("change", function(){
		var enabled = $(this).prop("checked");
		if(enabled)
		{
			$("#enable_pd").trigger("change");
			$("#show_autoconfig_type").show();
			$("#autoconfig_type").trigger("change");
		}
		else
		{
			$("#show_enable_lan_pd, #show_autoconfig_type, .lan_dhcp, .lan_ra").hide();

			$('.lan_dhcp2').hide();

		}

	});

	$("#enable_pd").on("change", function(){
		var enabled = $(this).prop("checked");
		if(enabled)
		{
			var lan_autoconfig = $("#enable_lan_autoconfig").prop("checked");
			if (lan_autoconfig)
			{
				$("#show_enable_lan_pd").show();
			}

			$("#show_LAN_IPv6_Address").hide();

			$('.lan_dhcp2').hide();

		}
		else
		{
			$("#show_LAN_IPv6_Address").show();
			$("#show_enable_lan_pd").hide();
			$("#ipv6_lan_ip").focus();

			if($("#autoconfig_type").val()=="stateful"){
				$('.lan_dhcp2').show();
			}

		}

	});

	$("#pppoe_Session").on("change", function(){
		var value = $(this).val();

		switch(value)
		{
			case "share":
				$(".pppoe_newSession").hide();
				break;
			case "new":
				$(".pppoe_newSession").show();
				$("#pppoe_Username").focus();
				//$("#internet_form").validate().resetForm();
				break;
		}
	});

	$("#pppoe_AddressMode").on("change", function(){
		var value = $(this).val();

		switch(value)
		{
			case "auto":
				$("#enable_pd").prop("checked", org_pppoe_enable_pd);
				$("#show_PPPoE_IP_Address").hide();

				$(".lanBlock_pd").show();
				break;
			case "manual":
				org_pppoe_enable_pd = $("#enable_pd").prop("checked");
				$("#enable_pd").prop("checked", false);
				$("#show_PPPoE_IP_Address").show();
				$(".lanBlock_pd").hide();
				break;
		}
		$("#enable_pd").trigger("change");

		if(value == "manual")	//must after enable_pd trigger
		{
			$("#pppoe_IPAddress").focus();
		}
	});

	$("#ipv6rd_DHCPv4Option").on("change", function(){
		var value = $(this).val();

		switch(value)
		{
			case "auto":
				$(".6rd_manual").hide();
				break;
			case "manual":
				$(".6rd_manual").show();
				$("#ipv6rd_IPv6Prefix").focus();
				break;
		}

	});

	$("#staticIPv6_useLinkLocalAddress").on("change", function(){
		var enabled = $(this).prop("checked");
		/*
		if(enabled)
		{
			$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength").attr("disabled", true);
			org_wan_prefix_length = $("#staticIPv6_subnetPrefixLength").val();
			$("#staticIPv6_subnetPrefixLength").val("64");
			org_static_wan_ip = $("#staticIPv6_IPv6Address").val();
			$("#staticIPv6_IPv6Address").val(ipv6Status.IPv6_WanLinkLocalAddress);
		}
		else
		{
			$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength").attr("disabled", false);
			if(org_wan_prefix_length != "")
				$("#staticIPv6_subnetPrefixLength").val(org_wan_prefix_length);
			if(org_static_wan_ip != "")
				$("#staticIPv6_IPv6Address").val(org_static_wan_ip);

			$("#staticIPv6_IPv6Address").focus();
		}
		*/

		if(staticSettings.IPv6_UseLinkLocalAddress=="Enable" || ipv6Status.IPv6_ConnectionType!="IPv6_Static"){
			if(enabled)
			{
				$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength").attr("disabled", true);
				//org_wan_prefix_length = $("#staticIPv6_subnetPrefixLength").val();
				$("#staticIPv6_subnetPrefixLength").val("64");
				//org_static_wan_ip = $("#staticIPv6_IPv6Address").val();
				$("#staticIPv6_IPv6Address").val(ipv6Status.IPv6_WanLinkLocalAddress);
			}
			else
			{
				$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength").attr("disabled", false);
				/*
				if(org_wan_prefix_length != "")
					$("#staticIPv6_subnetPrefixLength").val(org_wan_prefix_length);
				if(org_static_wan_ip != "")
					$("#staticIPv6_IPv6Address").val(org_static_wan_ip);
				*/
				$("#staticIPv6_subnetPrefixLength").val("");
				$("#staticIPv6_IPv6Address").val("");

				$("#staticIPv6_IPv6Address").focus();
			}
		}else{
			if(enabled)
			{
				$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength").attr("disabled", true);
				org_wan_prefix_length = $("#staticIPv6_subnetPrefixLength").val();
				$("#staticIPv6_subnetPrefixLength").val("64");
				org_static_wan_ip = $("#staticIPv6_IPv6Address").val();
				$("#staticIPv6_IPv6Address").val(ipv6Status.IPv6_WanLinkLocalAddress);
			}
			else
			{
				$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength").attr("disabled", false);
				if(org_wan_prefix_length != "")
					$("#staticIPv6_subnetPrefixLength").val(org_wan_prefix_length);
				if(org_static_wan_ip != "")
					$("#staticIPv6_IPv6Address").val(org_static_wan_ip);

				$("#staticIPv6_IPv6Address").focus();
			}
		}		

		//trigger validate
		$("#staticIPv6_IPv6Address, #staticIPv6_subnetPrefixLength, #staticIPv6_DefaultGateway").trigger("focusout");
	});

	$("#staticIPv6_subnetPrefixLength").on("change", function(){
		$("#staticIPv6_IPv6Address").trigger("focusout");
	});

	$("#linklocal_EnableULA").on("change", function(){
		var enabled = $(this).prop("checked");

		if(enabled)
		{
			$("#UseDefaultULAPrefix_tr").show();
			$("#linklocal_UseDefaultULAPrefix").trigger("change");
		}
		else
		{
			$("#UseDefaultULAPrefix_tr").hide();
			$("#ULAPrefix_tr").hide();
		}
	});

	$("#linklocal_UseDefaultULAPrefix").on("change", function(){
		var enabled = $(this).prop("checked");

		if(enabled)
		{
			$("#ULAPrefix_tr").hide();
		}
		else
		{
			$("#ULAPrefix_tr").show();
			$("#linklocal_ULAPrefix").focus();
		}

	});

	$("#ipv6_lan_ip").on("change", function(){
		var value = $(this).val();
		var prefix = "ffff::";
		if(value != "")
		{
			prefix = getPrefix($(this).val(), 64);
		}

		$("#dhcp_prefix_start").html(prefix);
		$("#dhcp_prefix_end").html(prefix);
	});

	$("#connectionIPv6Type").on("change", function(){
		storeData(ipv6_wan_type);
		showData($(this).val());

		$("input:visible").first().focus();
		$('#internet_form').validate().resetForm();
	});

	$("select").on("change", function(){
		var value = $(this).val();
		var selectWidth = new Object;
		selectWidth.width = 340;
		
		$(this).selectbox("detach").selectbox(selectWidth);
	});

});

function initSaveButtonTrigger(){
	$("select").change(function(){
		save_button_changed();
	});
	$("input").keydown(function(){
		save_button_changed();
	});
	$('input[type="checkbox"]').click(function(){
		save_button_changed();
	});
}