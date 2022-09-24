const char setupindex[]= R""""( 
<!DOCTYPE html><html><head>
<meta name='viewport' content='width=device-width, initial-scale=1.0, minimum-scale=0.5, maximum-scale=3.0'>
<title>GW-DAM</title>
</head><body style='font-family:monospace'>
<h1>SSID Setup.</h1>
<span id=loading>Loading...</span>
<form method=POST action='/savesetup'><table id=maintable style='display:none'>
<tr><td>SSID</td><td><input type=text name=SSID id=SSID ><select id=SSID_LIST onchange=ssidchange()></select>
		<input id=scanbtn type=button onclick='Scan()' value='Scan'></td></tr>
<tr><td> PWD</td><td><input type=password name=PWD id=PWD></td></tr>
<tr><td> admin PIN</td><td><input type=text name=PIN id=PIN>(Login:admin)</td></tr>
<tr><td> IPTYPE</td><td><select id=IPTYPE name=IPTYPE id=IPTYPE onchange='IPTYPEChange()'>
 <option value='DHCP'>DHCP</option><option value='STATIC'>STATIC</option></select></td></tr>
<tr class=staticips><td> IP</td><td><input type=text name=IP id=IP></td></tr>
<tr class=staticips><td> MASK</td><td><input type=text name=MASK id=MASK></td></tr>
<tr class=staticips><td> GW</td><td><input type=text name=GW id=GW></td></tr>
<tr class=staticips><td> DNS1</td><td><input type=text name=DNS1 id=DNS1></td></tr>
<tr class=staticips><td> DNS2</td><td><input type=text name=DNS2 id=DNS2></td></tr>
<tr ><td colspan=2> <hr></td></tr>
<tr ><td colspan=2> <a href='#' onclick='ToggleEl("NTPS");return false;'> Setup NTP.</a>
	<table id='NTPS' style='display:none;'>
	<tr ><td> NTPSERVER</td><td><input type=text name=NTPSERVER id=NTPSERVER> (pool.ntp.org) </td></tr>
	<tr ><td> GMTOFFSET_SEC</td><td><input type=text name=GMTOFFSEC id=GMTOFFSEC></td></tr>
	<tr ><td> NTP_POLL</td><td><input type=text name=NTP_POLL id=NTP_POLL></td></tr>
	</table>
</td></tr>
<tr ><td colspan=2> <hr></td></tr>
<tr ><td><input type='submit' value=Save></td></tr>
</table></form>
<hr>
<a href='/configindex'>[ Led Config ]</a>
<hr>
<a href='/'>[ BACK ]</a>
<a href='/restart'>[ Restart ]</a>
<hr>
Version:<span id=build></span>
<script>

function ToggleEl(ID) {
  var x = document.getElementById(ID);
  if (x.style.display === "none") {
    x.style.display = "block";
  } else {
    x.style.display = "none";
  }
}

function loadDoc(url, cFunction) {
  const xhttp = new XMLHttpRequest();
  xhttp.onload = function() {cFunction(this);}
  xhttp.open("GET", url);
  xhttp.send();
}

document.addEventListener("DOMContentLoaded", () => {

  console.log("Starting ... ");

	loadDoc("/config", function (data)
	{
		var myArr = JSON.parse(data.responseText);
		console.log(myArr);

		for (const [key, value] of Object.entries(myArr)) 
		{
			if (document.getElementById(key))
			{
				console.log(`${key}: ${value}`);
				document.getElementById(key).value = value;
			}
			
			if (key=='build') document.getElementById(key).innerText=value;
		}
		IPTYPEChange();
		ToggleEl('loading');
		ToggleEl('maintable');
	});

});

function ssidchange(){
	var select = document.getElementById('SSID_LIST');
	document.getElementsByName('SSID')[0].value = select.selectedOptions[0].value; 
}
function IPTYPEChange() {
	var select = document.getElementById('IPTYPE');
	console.log(select.value);
//	console.log(select.options[select.selectedIndex].text);
var staticips = document.getElementsByClassName('staticips');
	for(var i = 0; i < staticips.length; i++) {		
//	console.log(staticips[i]);
		if (select.value=='STATIC') 
			staticips[i].style.display = ''; 
		else 
			staticips[i].style.display = 'none'; 
	}
}
function Scan() {
document.getElementById('scanbtn').value='Scanning....';
const xhttp = new XMLHttpRequest();
xhttp.onload = function() {
	var select = document.getElementById('SSID_LIST');
	document.getElementById('scanbtn').value='Scan';
	console.log(this.responseText); 
	var data=this.responseText.split('\\n');
	select.innerHTML = '<option value=>-- Select --</option>';
	for(var i = 0; i < data.length-1; i+=2) {		
		var el = document.createElement('option');		
		el.textContent = data[i]+'('+data[i+1]+')';	
		el.value = data[i];			
		select.appendChild(el);		
	}	
}
xhttp.open('GET', '/scanssid');
xhttp.send();
}
</script>
</body></html>
)"""";