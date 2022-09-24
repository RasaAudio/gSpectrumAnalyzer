const char configindex[]= R""""( 
<!DOCTYPE html><html>
<head>
<meta name='viewport' content='width=device-width, initial-scale=1.0, minimum-scale=0.5, maximum-scale=3.0'>
<title>GW-DAM</title></head>
<body style='font-family:monospace'>
<h1>LED Configuration.</h1>
<hr>
<span id=loading>Loading...</span>
<form method=POST action='/saveconfig'>
	<table id=maintable style='display:none'>
		<tr><td>EFFECT</td><td id=EFFECT_TD>
		<input type=text id=EFFECT name=EFFECT size=3 Readonly></td></tr>

		
		<tr><td>VUPOS</td><td>
				<select name=VUPOS id=VUPOS onchange='OnChangeField("VUPOS")'>
					<option value='0'>Off</option>
					<option value='1'>Top</option>
				</select>
			</td>
		</tr>

		<tr><td>PEAKDIR</td><td>
				<select name=PEAKDIR id=PEAKDIR onchange='OnChangeField("PEAKDIR")'>
					<option value='0'>Down</option>
					<option value='1'>Up</option>
				</select>
			</td>
		</tr>
		
		<tr><td>CALIBTYPE</td><td>
				<select name=CALIBTYPE id=CALIBTYPE onchange='OnChangeField("CALIBTYPE")'>
					<option value='0'>None</option>
					<option value='1'>Pink</option>
					<option value='2'>White</option>
					<option value='3'>Brown</option>
				</select>
			</td>
		</tr>
		


		<tr><td>AUTOEFF</td><td>
				<select name=AUTOEFF id=AUTOEFF onchange='OnChangeField("AUTOEFF")'>
					<option value='0'>None</option>
					<option value='1'>Audio</option>
					<option value='2'>ScreenSaver</option>
				</select>
			</td>
		</tr>
		<tr><td>Auto Time</td>
			<td>
				<input type="range" min="1" max="20" value="5" id="AUTOTIME" name="AUTOTIME" 
					onchange='OnChangeField("AUTOTIME")'
					oninput="this.nextElementSibling.value = this.value"
					>
				<output></output>
				(Save&Restart After Change.)
			</td>
		</tr>
		<tr><td>PeakDelay</td>
			<td>
				<input type="range" min="1" max="100" value="50" id="PEAKDELAY" name="PEAKDELAY" 
					onchange='OnChangeField("PEAKDELAY")'
					oninput="this.nextElementSibling.value = this.value"
					>
				<output></output>
			</td>
		</tr>
		<tr><td>BRIGHT</td>
			<td>
				<input type="range" min="1" max="255" value="50" id="BRIGHT" name="BRIGHT" 
					onchange='OnChangeField("BRIGHT")'
					oninput="this.nextElementSibling.value = this.value"
					>
				<output></output>
			</td>
		</tr>
		<tr><td>Speed</td>
			<td>
				<input type="range" min="1" max="255" value="50" id="SPEED" name="SPEED" 
					onchange='LoadSpeed()'
					onchange='OnChangeField("SPEED")'
					oninput="this.nextElementSibling.value = this.value"
					>
				<output></output>
			</td>
		</tr>


		<tr><td><input type='submit' value=Save></td></tr>
	</table>
</form>
<br>
<a href='/setup'>[ SSID Setup ]</a>
<a href='/ota'>[ OTA ]</a>

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

	loadDoc("/configoth", function (data)
	{
		var myArr = JSON.parse(data.responseText);
		//console.log(myArr);

		for (const [key, value] of Object.entries(myArr)) 
		{
			//console.log(`${key}: ${value}`);
			if (document.getElementById(key))
			{
				document.getElementById(key).value = value;
			}
			
			if (key=='build') document.getElementById(key).innerText=value;
		}
		
		ToggleEl('loading');
		ToggleEl('maintable');

	});
});

var Effects=
[
	"RainBowBar CC",		//0
	"GreenBar RB Pk",		//1
	"RainbowBar WT Pk",//2
	"BlueBar WT Pk",//3
	"RedBar Bl Pk",//4
	"BlueBar Bl Pk",//5
	"WOR Center",//6
	"RW Center",//7
	"YW Center",//8
	"Rainbow Line",//9
	"Blue Line",//10
	"Green Line",//11
	"",//12 --- Demo Mode.
	"FireR",//13
	"FireG",//14
	"FireB",//15
	"Twinkle",//16
	"Plasma",//17
	"FireWorks",//18
	"Vortex",//19
	"StormyRain",//20
	"Fire",//21
	"TheMatrix",//22
	"ColoredRain",//23
	"Bpm",//24
	"Juggle",//25
	"Pride",//26
	"Rainbow",//27
	"RainbowWithGlitter",//28
	"Sinelon",//29

	"Attract",//30
	"Bounce",//31
	"Cube",//32
	"Flock",//33
	"FlowField",//34
	"Drift",//35
	"DriftRose",//36
	"Pendulum",//37
	"Radar",//38
	"Spiral",//39
	"Spiro",//40
	"Swirl",//41
	"Wave",//42

];

html="";
for(i=0;i<Effects.length;i++)
{
	console.log(Effects[i]);
	if ((Effects[i]+"").length>0)
		html+="<input type='button' onclick='LoadEff("+i+")' value='"+Effects[i]+"'>";
}
document.getElementById('EFFECT_TD').insertAdjacentHTML("beforeend", html);

//*******************************************************************
function LoadEff(i)
{
	document.getElementById('EFFECT').value=i;
	loadDoc("/ws?EFFECT="+i, function (xhttp) {
		// action goes here
		console.log(xhttp.status,xhttp.responseText);
		});
}

function OnChangeField(Fld)
{
	var aval=document.getElementById(Fld).value;
	console.log(aval);
	loadDoc("/ws?"+Fld+"="+aval, function (xhttp) {
		// action goes here
		console.log(xhttp.status,xhttp.responseText);
		});
}

</script>
</body></html>

)"""";
