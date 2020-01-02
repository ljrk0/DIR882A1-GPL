// Coding : Timmy Hsieh 2012/12/12

var ROW_BASE = 1; // first number (for display)
var IsLoaded = false;
var TABLE_NAME = 'tblUserList';
var USER_NAME = 'username_';
var USER_PASSWORD = 'userpassword_';
var Enable_Status = "status_";
var status_samba = "status_SAMBA";
var status_ftp = "status_FTP";
var status_vpn = "status_VPN";
var EDIT_ICON = 'editicon_';
var DELETE_ICON = 'delicon_';
var DOWNLOAD_ICON = 'downloadicon_';
var samba_folder = 'samba_folder';
var ftp_folder = 'ftp_folder';
// Value
var a, b ,c ,d ,e, f, g, h, m;

// Pop Window Get Temp Information
var tmp_Iteration;
var tmp_UserName;
var tmp_UserPassword;
var tmp_SAMBA_Permission;
var tmp_FTP_Permission;
var tmp_SAMBA_Status;
var tmp_FTP_Status;
var tmp_VPN_Status;

var tmp_ID;
var IsLoaded = true;

function GetHNAPInformatiom(ListNum, UserName, UserPassword, SAMBA, FTP, VPN, Status, SAMBAFolder, FTPFolder) {
	a = ListNum;
	b = UserName;
	c = UserPassword;
	d = SAMBA;
	e = FTP;
	f = VPN;
	g = Status;	
	h = SAMBAFolder;
	m = FTPFolder;
}

function myRowObject(one, two, three, four, five, six, seven, eight, nine, ten, eleven) {
	this.one = one;			// input Status
	this.two = two;			// input User Name
	this.three = three;		// input SAMBA Status
	this.four = four;		// input FTP Status
	this.five = five;		// input VPN Status
	this.six = six;			// input vpn Profile
	this.seven = seven;		// input Edit icon
	this.eight = eight;		// input Delete icon
	this.nine = nine; 		// input User Password
	this.ten = ten;			// input SAMBA Folder
	this.eleven = eleven;	// input FTP Folder
}

// function addRowToTable(num){
function addRowToTable(num, a, b, c, d, e, f, g, h, m) {
	if (IsLoaded) {
		var tbl = document.getElementById(TABLE_NAME);
		var nextRow = tbl.tBodies[0].rows.length;
		var iteration = nextRow + ROW_BASE;
		var userName = b;
		if (num == null) {
			num = nextRow;
		}else{
			iteration = num + ROW_BASE;
		}
		// add the Row
		var row = tbl.tBodies[0].insertRow(num);
		
		// Status
		var cell_0 = row.insertCell(0);
		var Input1 = document.createElement('input');
		Input1.setAttribute('id', Enable_Status + iteration);
		Input1.setAttribute('type', 'checkbox');
		if (g == "true"){
			Input1.checked = true;
		}else{
			Input1.checked = false;
		}
		Input1.onclick = function () { save_button_changed() };
		//admin checkbox hide
		if(num == 0){
			Input1.checked = true;
			Input1.style.display = "none";
		}
		cell_0.appendChild(Input1);

		// User Name
		var cell_1 = row.insertCell(1);
		var Input2 = document.createElement('label');
		Input2.setAttribute('id', USER_NAME + iteration);
		Input2.setAttribute('size', '10');
		//admin or not
		if(num == 0){
			Input2.innerHTML = "Admin";
		}else{
			Input2.innerHTML = userName;
		}		
		cell_1.appendChild(Input2);


		
		// SAMBA status
		var cell_2 = row.insertCell(2);
		var Input3 = document.createElement('label');
		Input3.setAttribute('id', status_samba + iteration);
		Input3.setAttribute('size', '10');
		//admin or not
		if(num == 0){
			Input3.innerHTML = I18N("j", "Read/Write");
		}else{		
			if (d == "2")
			{
				Input3.innerHTML = I18N("j", "Read/Write");
			}
			else if (d == "1")
			{
				Input3.innerHTML = I18N("j", "Read Only");
			} else {
				Input3.innerHTML = "-";
			}
		}	
		cell_2.appendChild(Input3);
		
		// FTP status
		var cell_3 = row.insertCell(3);
		var Input4 = document.createElement('label');
		Input4.setAttribute('id', status_ftp + iteration);
		Input4.setAttribute('size', '10');
		//admin or not
		if(num == 0){
			Input4.innerHTML = I18N("j", "Read/Write");
		}else{	
			if (e == "2")
			{
				Input4.innerHTML = I18N("j", "Read/Write");
			}
			else if (e == "1")
			{
				Input4.innerHTML = I18N("j", "Read Only");
			} else {
				Input4.innerHTML = "-";
			}
		}	
		cell_3.appendChild(Input4);

		
		// VPN status
		var cell_4 = row.insertCell(4);
		var Input5 = document.createElement('label');
		Input5.setAttribute('id', status_vpn + iteration);
		Input5.setAttribute('size', '10');
		if (f == "Enabled"){
			Input5.innerHTML = I18N("j", "Enabled");
		}else{
			// Input5.innerHTML = I18N("j", "Disabled");
			Input5.innerHTML = "-";
		}
		cell_4.appendChild(Input5);

		// Download Icon
		var cell_5 = row.insertCell(5);
		if (f == "Enabled"){
			var Input6 = document.createElement('img');
			Input6.setAttribute('src', 'image/download_btn.png');
			Input6.setAttribute('width', '28');
			Input6.setAttribute('height', '28');
			Input6.setAttribute('id', DOWNLOAD_ICON + iteration);
			Input6.setAttribute('style', 'cursor:pointer');
			Input6.onclick = function () { downloadData(userName) };
		}else{
			var Input6 = document.createElement('label');
			Input6.innerHTML = "-";
		}
		cell_5.appendChild(Input6);

		// Input Edit Icon
		var cell_6 = row.insertCell(6);
		var Input7 = document.createElement('img');
		Input7.setAttribute('src', 'image/edit_btn.png');
		Input7.setAttribute('width', '28');
		Input7.setAttribute('height', '28');
		Input7.setAttribute('id', EDIT_ICON + iteration);
		Input7.setAttribute('style', 'cursor:pointer');
		Input7.onclick = function () { editData(iteration) };
		//admin or not
		if(num == 0){
			cell_6.innerHTML = "-";
		}else{
			cell_6.appendChild(Input7);
		}
		
		// Input Delete Icon
		var cell_7 = row.insertCell(7);
		var Input8 = document.createElement('img');
		Input8.setAttribute('src', 'image/trash.png');
		Input8.setAttribute('width', '41');
		Input8.setAttribute('height', '41');
		Input8.setAttribute('id', DELETE_ICON + iteration);
		Input8.setAttribute('style', 'cursor:pointer');
		Input8.onclick = function () { deleteCurrentRow(this, userName) };
		//admin or not
		if(num == 0){
			cell_7.innerHTML = "-";
		}else{
			cell_7.appendChild(Input8);
		}

		// Hide password to get
		var cell_8 = row.insertCell(8);
		var Input9 = document.createElement('label');
		Input9.setAttribute('id', USER_PASSWORD + iteration);
		Input9.setAttribute('size', '10');
		Input9.innerHTML = c;
		Input9.setAttribute('style', 'display:none');
		cell_8.appendChild(Input9);
		cell_8.style.display = 'none';

		// Hide SAMBA Folder to get
		var cell_9 = row.insertCell(9);
		var Input10 = document.createElement('label');
		Input10.setAttribute('id', samba_folder + iteration);
		Input10.setAttribute('size', '10');
		Input10.innerHTML = h;
		Input10.setAttribute('style', 'display:none');
		cell_9.appendChild(Input10);
		cell_9.style.display = 'none';

		// Hide FTP Folder to get
		var cell_10 = row.insertCell(10);
		var Input11 = document.createElement('label');
		Input11.setAttribute('id', ftp_folder + iteration);
		Input11.setAttribute('size', '10');
		Input11.innerHTML = m;
		Input11.setAttribute('style', 'display:none');
		cell_10.appendChild(Input11);
		cell_10.style.display = 'none';
		
		row.myRow = new myRowObject(Input1, Input2, Input3, Input4, Input5, Input6, Input7, Input8, Input9, Input10, Input11);
		
		b = null;
		c = null;
		d = null;
		e = null;
		f = null;
		g = null;
		h = null;
		m = null;
	}
}

function AddRowToIndex() {
	var tbl = document.getElementById(TABLE_NAME);
	var nowUserName = document.getElementById("shareport_UserName").value;
	
	for(var i = 1; i <= Total_User; i ++){
		if (nowUserName === tbl.rows[i].childNodes[1].childNodes[0].innerHTML){
			alert("Username can not be the same");
			return "Error";
		}
	}
	
	if (nowUserName == ""){
		alert("Username can not be null");
		return "Error";
	}
	b = document.getElementById("shareport_UserName").value;
	c = document.getElementById("shareport_Password").value;
	if ($("#status_SAMBA").prop("checked")) {
		if (document.getElementById("SAMBA_Permission").value == "1"){
			d = "1";
		}else{
			d = "2";
		}
	} else {
		d = "-";
	}
	
	if ($("#status_FTP").prop("checked")) {
		if (document.getElementById("FTP_Permission").value == "1")
		{
			e = "1";
		}
		else
		{
			e = "2";
		}
	} else {
		e = "-";
	}

	if ($("#status_VPN").prop("checked")) {
		f = "Enabled";
	} else {
		f = "Disabled";
	}

	// status
	g = "true";
	h = document.getElementById("SAMBA_Folder").value;
	m = document.getElementById("FTP_Folder").value;

	// addRowToTable(null);
	addRowToTable(null, "", b, c, d, e, f, g, h, m);
	Total_User += 1;
	check_TotalUser(Limit_TotalUser, Total_User);
	//}
	
	// Clear
	document.getElementById("shareport_UserName").value = "";
	document.getElementById("shareport_Password").value = "";
	document.getElementById("status_SAMBA").checked = true;
	document.getElementById("SAMBA_Permission").value = "1";
	document.getElementById("SAMBA_Folder").value = "";
	document.getElementById("status_FTP").checked = false;
	//$("#status_FTP").prop("checked", "false");
	document.getElementById("FTP_Permission").value = "1";
	document.getElementById("FTP_Folder").value = "";
	document.getElementById("status_VPN").checked = false;
	//$("#status_VPN").prop("checked", "false");
	return "Success";
}

function editDataGet(id){
	var tbl = document.getElementById(TABLE_NAME);
	var tmp_b = tbl.rows[id].childNodes[1].childNodes[0].innerHTML;
	var tmp_c = tbl.rows[id].childNodes[8].childNodes[0].innerHTML;
	var tmp_d = tbl.rows[id].childNodes[9].childNodes[0].innerHTML;
	var tmp_e = tbl.rows[id].childNodes[2].childNodes[0].innerHTML;
	var tmp_f = tbl.rows[id].childNodes[10].childNodes[0].innerHTML;
	var tmp_g = tbl.rows[id].childNodes[3].childNodes[0].innerHTML;
	//var tmp_h = tbl.rows[id].childNodes[4].childNodes[0].innerHTML;
	var tmp_h = tbl.rows[id].childNodes[5].childNodes[0].innerHTML;
	document.getElementById("edit_UserName").value = tmp_b;
	document.getElementById("edit_Password").value = tmp_c;
	document.getElementById("edit_SAMBA_Folder").value = tmp_d;
	document.getElementById("edit_FTP_Folder").value = tmp_f;
	
	if (tmp_e == "-") {
		document.getElementById("edit_status_SAMBA").checked = false;
		$("#edit_SAMBA_Permission").selectbox();
		$("#edit_SAMBA_Permission").selectbox('disable');
		document.getElementById("edit_SAMBA_Folder").disabled = true;
		tmp_SAMBA_Status = "false";
		tmp_SAMBA_Permission = "-";
	} else if (tmp_e == I18N("j", "Read Only")){
		document.getElementById("edit_status_SAMBA").checked = true;
		$("#edit_SAMBA_Permission").selectbox('detach');
		$("#edit_SAMBA_Permission").val('1');
		$("#edit_SAMBA_Permission").selectbox('attach');
		$("#edit_SAMBA_Permission").selectbox('enable');
		document.getElementById("edit_SAMBA_Folder").disabled = false;
		tmp_SAMBA_Permission = "1";
		tmp_SAMBA_Status = "true";
	}else{
		document.getElementById("edit_status_SAMBA").checked = true;
		$("#edit_SAMBA_Permission").selectbox('detach');
		$("#edit_SAMBA_Permission").val('2');
		$("#edit_SAMBA_Permission").selectbox('attach');
		$("#edit_SAMBA_Permission").selectbox('enable');
		document.getElementById("edit_SAMBA_Folder").disabled = false;
		tmp_SAMBA_Permission = "2";
		tmp_SAMBA_Status = "true";
	}

	if (tmp_g == "-") {
		document.getElementById("edit_status_FTP").checked = false;
		$("#edit_FTP_Permission").selectbox();
		$("#edit_FTP_Permission").selectbox('disable');
		document.getElementById("edit_FTP_Folder").disabled = true;
		tmp_FTP_Status = "false";
		tmp_FTP_Permission = "-";
	} else if (tmp_g == I18N("j", "Read Only"))	{
		document.getElementById("edit_status_FTP").checked = true;
		$("#edit_FTP_Permission").selectbox('detach');
		$("#edit_FTP_Permission").val('1');
		$("#edit_FTP_Permission").selectbox('attach');
		$("#edit_FTP_Permission").selectbox('enable');
		document.getElementById("edit_FTP_Folder").disabled = false;
		tmp_FTP_Permission = "1";
		tmp_FTP_Status = "true";
	}else{
		document.getElementById("edit_status_FTP").checked = true;
		$("#edit_FTP_Permission").selectbox('detach');
		$("#edit_FTP_Permission").val('2');
		$("#edit_FTP_Permission").selectbox('attach');
		$("#edit_FTP_Permission").selectbox('enable');
		document.getElementById("edit_FTP_Folder").disabled = false;
		tmp_FTP_Permission = "2";
		tmp_FTP_Status = "true";
	}

	if (tmp_h == "-") {
		document.getElementById("edit_status_VPN").checked = false;
		tmp_VPN_Status = "false";
	} else {
		document.getElementById("edit_status_VPN").checked = true;
		tmp_VPN_Status = "true";
	}

	tmp_UserName = tmp_b;
	tmp_Password = tmp_c;

	setIteration(id);
	tmp_ID = id;
}

function assignRowToIndex(id){
	var tbl = document.getElementById(TABLE_NAME);
	var edit_Name = document.getElementById("edit_UserName").value;
	
	if (edit_Name != ""){
		for(var i = 2; i <= Total_User; i ++){
			// console.log(tmp_ID + " & " + i);
			if (edit_Name === tbl.rows[i].childNodes[1].childNodes[0].innerHTML){
				if (tmp_ID == i){
					continue;
				}else{
				// alert("Username can not be the same");
					return "Error";
				}
			}
		}
	}
	
	
	tbl.rows[id].childNodes[1].childNodes[0].innerHTML = document.getElementById("edit_UserName").value;
	
	if (document.getElementById("edit_status_SAMBA").checked) {
		if ($("#edit_SAMBA_Permission").val() == "1")
		{
			tbl.rows[id].childNodes[2].childNodes[0].innerHTML = I18N("j", "Read Only");
		}
		else if ($("#edit_SAMBA_Permission").val() == "2")
		{
			tbl.rows[id].childNodes[2].childNodes[0].innerHTML = I18N("j", "Read/Write");
		}
	} else {
		tbl.rows[id].childNodes[2].childNodes[0].innerHTML = "-";
	}

	if(document.getElementById("edit_status_FTP").checked) {
		if ($("#edit_FTP_Permission").val() == "1")
		{
			tbl.rows[id].childNodes[3].childNodes[0].innerHTML = I18N("j", "Read Only");
		}
		else if ($("#edit_FTP_Permission").val() == "2")
		{
			tbl.rows[id].childNodes[3].childNodes[0].innerHTML = I18N("j", "Read/Write");
		}
	} else {
		tbl.rows[id].childNodes[3].childNodes[0].innerHTML = "-";
	}


	if (document.getElementById("edit_status_VPN").checked) {
		tbl.rows[id].childNodes[4].childNodes[0].innerHTML = I18N("j", "Enabled");
		tbl.rows[id].childNodes[5].innerHTML = '<img src="image/download_btn.png" width="28" height="28" style="cursor:pointer;" onclick="downloadData('+ "'" + document.getElementById("edit_UserName").value + "'" + ')" id="'+ DOWNLOAD_ICON + id +'">'; 
	} else {
		// tbl.rows[id].childNodes[4].childNodes[0].innerHTML = I18N("j", "Disabled");
		tbl.rows[id].childNodes[4].childNodes[0].innerHTML = "-";
		tbl.rows[id].childNodes[5].innerHTML = "<label>-</label>";
	}
	
	tbl.rows[id].childNodes[8].childNodes[0].innerHTML = document.getElementById("edit_Password").value;
	tbl.rows[id].childNodes[9].childNodes[0].innerHTML = document.getElementById("edit_SAMBA_Folder").value;
	tbl.rows[id].childNodes[10].childNodes[0].innerHTML = document.getElementById("edit_FTP_Folder").value;
	
	check_TotalUser(Limit_TotalUser, Total_User);
	
	return tmp_ID;
}

function deleteCurrentRow(obj, userName){
	if (IsLoaded){
		for(i = 0; i < userList.length; i++){
			if(this.userList[i] == userName){
				delUserList.push(userName);
			}
		}

		var delRow = obj.parentNode.parentNode;
		var tbl = delRow.parentNode.parentNode;
		var rIndex = delRow.sectionRowIndex;
		var rowArray = new Array(delRow);
		deleteRows(rowArray);
		reorderRows(tbl, rIndex);
		
		Total_User -= 1;
		check_TotalUser(Limit_TotalUser, Total_User);
		save_button_changed();
	}
}

function OnChangeUserName(num){
	tmp_UserName = num;
}

function OnChangeFolder(num){
	tmp_AccessPath = num;
}

function OnChangePermission(num){
	tmp_UserPromission = num;
}

function deleteRows(rowObjArray){
	if (IsLoaded){
		for (var i = 0; i < rowObjArray.length; i ++){
			var rIndex = rowObjArray[i].sectionRowIndex;
			rowObjArray[i].parentNode.deleteRow(rIndex);
		}
	}
}

function changeRowIndex(count){
	// Change Index List
	var tbl = document.getElementById(TABLE_NAME);
	tbl.rows[count].childNodes[6].childNodes[0].onclick = function() { editData(count) };
}

function reorderRows(tbl, startingIndex){
	var tbl = document.getElementById(TABLE_NAME);
	if (IsLoaded){
		if (tbl.tBodies[0].rows[startingIndex]){
			var count = startingIndex + ROW_BASE;
			for (var i = startingIndex; i < tbl.tBodies[0].rows.length; i ++){
				tbl.tBodies[0].rows[i].myRow.one.id = Enable_Status + count; // input text
				tbl.tBodies[0].rows[i].myRow.two.id = USER_NAME + count; // input text
				tbl.tBodies[0].rows[i].myRow.three.id = status_samba + count; // input text
				tbl.tBodies[0].rows[i].myRow.four.id = status_ftp + count; // input text
				tbl.tBodies[0].rows[i].myRow.five.id = status_vpn + count; // input text
				tbl.tBodies[0].rows[i].myRow.six.id = DOWNLOAD_ICON + count; // input text
				tbl.tBodies[0].rows[i].myRow.seven.id = EDIT_ICON + count; // input text
				tbl.tBodies[0].rows[i].myRow.eight.id = DELETE_ICON + count; // input text
				tbl.tBodies[0].rows[i].myRow.nine.id = USER_PASSWORD + count; // input text
				tbl.tBodies[0].rows[i].myRow.ten.id = samba_folder + count; // input text
				tbl.tBodies[0].rows[i].myRow.eleven.id = ftp_folder + count; // input text

				changeRowIndex(count);
				count++;
			}
		}
	}
}

function check_TotalUser(Limit_TotalUser, Total_User){
	var IsFull = Limit_TotalUser - Total_User;
	document.getElementById("RemainingRules").innerHTML = IsFull;
	
	if (IsFull == 0){
		document.getElementById("createButton").disabled = true;
	}else{
		document.getElementById("createButton").disabled = false;
	}
}
