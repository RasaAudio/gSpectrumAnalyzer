//************************************************************************
// Webserver related code.
//************************************************************************

#ifndef WIN32

#include "html_setupindex.h"
#include "html_configindex.h"

#endif

Preferences preferences;

int IsSetupMode=0;
//int IsLoggedIn=0;

const byte DNS_PORT = 53;
IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;

#define CONFIG_VAR_LEN	32
#define CONFIG_NUM_LEN	4

/*
char MAIN_PARAMS[MP_SSID][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_PWD][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_PIN][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_WS_KEY][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_WS2_KEY][CONFIG_VAR_LEN+2];

char MAIN_PARAMS[MP_NTPSERVER][CONFIG_VAR_LEN+2];
char _GMTOFFSEC[CONFIG_VAR_LEN+2];
char _NTP_POLL[CONFIG_NUM_LEN+2];

char MAIN_PARAMS[MP_IPTYPE][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_IP][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_MASK][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_GW][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_DNS1][CONFIG_VAR_LEN+2];
char MAIN_PARAMS[MP_DNS2][CONFIG_VAR_LEN+2];
*/

#define MP_SSID			0
#define MP_PWD			1
#define MP_PIN			2
#define MP_WS_KEY		3
#define MP_WS2_KEY		4
#define MP_NTPSERVER	5
#define MP_GMTOFFSEC	6
#define MP_NTP_POLL		7
#define MP_IPTYPE		8
#define MP_IP			9
#define MP_MASK			10
#define MP_GW			11
#define MP_DNS1			12
#define MP_DNS2			13
#define MP_WIFI2SER		14
#define MP_COMM_WDT		15

#define MAIN_PARAM_COUNT	16

char MAIN_PARAM_NAMES[MAIN_PARAM_COUNT+2][10]=
{
	"SSID",
	"PWD",
	"PIN",
	"WS_KEY",
	"WS2_KEY",

	"NTPSERVER",
	"GMTOFFSEC",
	"NTP_POLL",

	"IPTYPE",
	"IP",
	"MASK",
	"GW",
	"DNS1",
	"DNS2",

	"WIFI2SER",
	"COMM_WDT"
};

char MAIN_PARAMS[MAIN_PARAM_COUNT+2][CONFIG_VAR_LEN+2];

//******************************************************************************

#define OP_EFFECT		0
#define OP_AUTOEFF		1
#define OP_SPEED		2
#define OP_PEAKDELAY	3
#define OP_BRIGHT		4
#define OP_AUTOTIME		5
#define OP_VUPOS		6
#define OP_PEAKDIR		7
#define OP_CALIBTYPE	8

#define OTH_PARAM_COUNT	9

char OTH_PARAM_NAMES[OTH_PARAM_COUNT+2][10]=
{
	"EFFECT",
	"AUTOEFF",
	"PEAKDELAY",
	"SPEED",
	"BRIGHT",
	"AUTOTIME",
	"VUPOS",
	"PEAKDIR",
	"CALIBTYPE",
};

char OTH_PARAMS[OTH_PARAM_COUNT+2][CONFIG_NUM_LEN+2];
/*
char EFFECT[CONFIG_NUM_LEN+2];
char AUTOEFF[CONFIG_NUM_LEN+2];
char SPEED[CONFIG_NUM_LEN+2];
char PEAKDELAY[CONFIG_NUM_LEN+2];
char BRIGHT[CONFIG_NUM_LEN+2];
*/

unsigned long previousTS = 0;
unsigned long CommCheckTS = 0;
unsigned long CurrentTS =0;
unsigned long NTP_TS =0;

unsigned long WifiCheckInterval = 10;// Check Every 10 Seconds
unsigned long CommCheckInterval = 30;
unsigned long NTPCheckInterval = 30;
int NoComCount=0;



#ifdef WIN32
	AsyncWebServer server(8080);
#else
	AsyncWebServer server(80);
#endif

bool shouldReboot = false;

const char *restartIndex = "Restarting ... Click Here after 30 Seconds <a href='/'>Home</a>";

const char* otaIndex = ""
"<!DOCTYPE html><html>"
"<head>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0, minimum-scale=0.5, maximum-scale=3.0'>"
"<title>GW-DAM Firmware Update</title></head>"
"<body style='font-family:monospace'>"
"<form method='POST' action='/update' enctype='multipart/form-data'>"
"BinFile Type:<select name=utype><option value='fw'>Firmware</option><option value='files'>FileSystem</option></select><br>"
"<input type='file' name='update'><br>"
"<input type='submit' value='Update'></form><br>"
"Select the Bin file and click on 'Update'. Please wait for atleast 2 Mins till you see OK before restarting."
"</body></html>";


void DumpConfig()
{
	/*
	Serial.print("MAIN_PARAMS[MP_SSID]:");Serial.println(MAIN_PARAMS[MP_SSID]);
	Serial.print("MAIN_PARAMS[MP_PWD]:");Serial.println(MAIN_PARAMS[MP_PWD]);
	Serial.print("MAIN_PARAMS[MP_PIN]:");Serial.println(MAIN_PARAMS[MP_PIN]);

	Serial.print("MAIN_PARAMS[MP_IPTYPE]:");Serial.println(MAIN_PARAMS[MP_IPTYPE]);
	Serial.print("MAIN_PARAMS[MP_IP]:");Serial.println(MAIN_PARAMS[MP_IP]);
	Serial.print("MAIN_PARAMS[MP_MASK]:");Serial.println(MAIN_PARAMS[MP_MASK]);
	Serial.print("MAIN_PARAMS[MP_GW]:");Serial.println(MAIN_PARAMS[MP_GW]);
	Serial.print("MAIN_PARAMS[MP_DNS1]:");Serial.println(MAIN_PARAMS[MP_DNS1]);
	Serial.print("MAIN_PARAMS[MP_DNS2]:");Serial.println(MAIN_PARAMS[MP_DNS2]);

	Serial.print("MAIN_PARAMS[MP_NTPSERVER]:");Serial.println(MAIN_PARAMS[MP_NTPSERVER]);
	Serial.print("MAIN_PARAMS[MP_GMTOFFSEC]:");Serial.println(MAIN_PARAMS[MP_GMTOFFSEC]);
	Serial.print("MAIN_PARAMS[MP_NTP_POLL]:");Serial.println(MAIN_PARAMS[MP_NTP_POLL]);
	*/
	//Serial.print("EFFECT:");Serial.println(OTH_PARAMS[OP_EFFECT]);
	//Serial.print("AUTOEFF:");Serial.println(OTH_PARAMS[OP_AUTOEFF]);
	//Serial.print("SPEED:");Serial.println(OTH_PARAMS[OP_SPEED]);
	int i;
	for(i=0;i<MAIN_PARAM_COUNT;i++)
	{
		Serial.print(MAIN_PARAM_NAMES[i]);
		Serial.print(":");
		Serial.println(MAIN_PARAMS[i]);
	}


	for(i=0;i<OTH_PARAM_COUNT;i++)
	{
		Serial.print(OTH_PARAM_NAMES[i]);
		Serial.print(":");
		Serial.println(OTH_PARAMS[i]);
	}

}



void configindex_GET(AsyncWebServerRequest *request)
{
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	request->send(200, "text/html", configindex);
}

void root_GET(AsyncWebServerRequest *request)
{
	if (IsSetupMode)
	{
		request->send(200, "text/html", setupindex);
	}
	else
	{
		//if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
		//request->send(200, "text/html", configindex);
		configindex_GET(request);
	}
}


void NotFound(AsyncWebServerRequest *request)
{
	root_GET(request);
}

void setup_GET(AsyncWebServerRequest *request)
{
	//MAIN_PARAMS[MP_SSID] MAIN_PARAMS[MP_PWD] in Config Mode
	//request->sendHeader("Connection", "close");
	//if (IsLoggedIn==0) {request->send(200, "text/html", loginIndex);return;}
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	
	request->send(200, "text/html", setupindex);
}

void config_GET(AsyncWebServerRequest *request)
{
String serverIndex = "{";
/*
  "\"MAIN_PARAMS[MP_SSID]\" : \"";serverIndex+=MAIN_PARAMS[MP_SSID];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_PWD]\" : \"";serverIndex+=MAIN_PARAMS[MP_PWD];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_PIN]\" : \"";serverIndex+=MAIN_PARAMS[MP_PIN];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_IPTYPE]\" : \"";serverIndex+=MAIN_PARAMS[MP_IPTYPE];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_IP]\" : \"";serverIndex+=MAIN_PARAMS[MP_IP];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_MASK]\" : \"";serverIndex+=MAIN_PARAMS[MP_MASK];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_GW]\" : \"";serverIndex+=MAIN_PARAMS[MP_GW];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_DNS1]\" : \"";serverIndex+=MAIN_PARAMS[MP_DNS1];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_DNS2]\" : \"";serverIndex+=MAIN_PARAMS[MP_DNS2];serverIndex+="\",\n"

  "\"MAIN_PARAMS[MP_NTPSERVER]\" : \"";serverIndex+=MAIN_PARAMS[MP_NTPSERVER];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_GMTOFFSEC]\" : \"";serverIndex+=MAIN_PARAMS[MP_GMTOFFSEC];serverIndex+="\",\n"
  "\"MAIN_PARAMS[MP_NTP_POLL]\" : \"";serverIndex+=MAIN_PARAMS[MP_NTP_POLL];serverIndex+="\",\n"
  ;
*/
	for(int i=0;i<MAIN_PARAM_COUNT;i++)
	{
		serverIndex+="\"";
		serverIndex+=MAIN_PARAM_NAMES[i];
		serverIndex+="\" : \"";
		serverIndex+=MAIN_PARAMS[i];
		serverIndex+="\",\n";
	}

	serverIndex+=" \"build\" : \"" __DATE__ " " __TIME__ "\"}\n";

	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	request->send(200, "application/json", serverIndex);
}

void configoth_GET(AsyncWebServerRequest *request)
{
String serverIndex = "{";

	for(int i=0;i<OTH_PARAM_COUNT;i++)
	{
		serverIndex+="\"";
		serverIndex+=OTH_PARAM_NAMES[i];
		serverIndex+="\" : \"";
		serverIndex+=OTH_PARAMS[i];
		serverIndex+="\",\n";
	}

	serverIndex+=" \"Build\" : \"" __DATE__ __TIME__ "\"}\n";

	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	request->send(200, "application/json", serverIndex);
}



String GetScanSSIDList()
{
	String st;
	int n = WiFi.scanNetworks();
	
	for (int i = 0; i < n; i++) 
	{
		st+=WiFi.SSID(i);st+="\n";
		st+=WiFi.SSID(i);st+=" ";st+=WiFi.RSSI(i);
		st+=" ";st+=((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
		st+="\n";
    }
  	return st;
}


void scanssid_GET(AsyncWebServerRequest *request)
{
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) 
			request->requestAuthentication();

	request->send(200, "text/html", GetScanSSIDList());
}


void ota_GET(AsyncWebServerRequest *request)
{
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	
	request->send(200, "text/html", otaIndex);
}

void restart_GET(AsyncWebServerRequest *request)
{
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	
	request->send(200, "text/html", restartIndex);
	delay(3000);
	ESP.restart();
}

void SaveVarHttp(AsyncWebServerRequest *request,char *Name,char *Var,int len=CONFIG_VAR_LEN)
{
	if (request->hasArg(Name)==0) return;
	preferences.putString(Name, request->arg(Name));
	preferences.getString(Name, Var,len);
}


void savesetup_POST(AsyncWebServerRequest *request)
{
	//request->sendHeader("Connection", "close");
	//if (IsLoggedIn==0) {request->send(200, "text/html", loginIndex);return;}
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();

	/*
	SaveVarHttp(request,"MAIN_PARAMS[MP_SSID]",MAIN_PARAMS[MP_SSID]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_PWD]",MAIN_PARAMS[MP_PWD]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_PIN]",MAIN_PARAMS[MP_PIN]);

	SaveVarHttp(request,"MAIN_PARAMS[MP_IPTYPE]",MAIN_PARAMS[MP_IPTYPE]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_IP]",MAIN_PARAMS[MP_IP]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_MASK]",MAIN_PARAMS[MP_MASK]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_GW]",MAIN_PARAMS[MP_GW]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_DNS1]",MAIN_PARAMS[MP_DNS1]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_DNS2]",MAIN_PARAMS[MP_DNS2]);

	SaveVarHttp(request,"MAIN_PARAMS[MP_NTPSERVER]",MAIN_PARAMS[MP_NTPSERVER]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_GMTOFFSEC]",MAIN_PARAMS[MP_GMTOFFSEC]);
	SaveVarHttp(request,"MAIN_PARAMS[MP_NTP_POLL]",MAIN_PARAMS[MP_NTP_POLL]);
	*/

	for(int i=0;i<MAIN_PARAM_COUNT;i++)
	{
		SaveVarHttp(request,MAIN_PARAM_NAMES[i],MAIN_PARAMS[i],CONFIG_VAR_LEN);
	}

	NTPCheckInterval=atol(MAIN_PARAMS[MP_NTP_POLL]);
	if ((NTPCheckInterval>0)&&(NTPCheckInterval<60)) NTPCheckInterval=60;


	DumpConfig();

	if (IsSetupMode)
	{
		request->send(200, "text/html", restartIndex);
		delay(3000);
		ESP.restart();
	}
	else
	{
		// When Save is called from Non Setupmode then go back to config page.
		request->send(200, "text/html", setupindex );
	}
}

void FixParms()
{
	DemoFlag=false;
	buttonPushCounter=atol(OTH_PARAMS[OP_EFFECT]);
	if ((buttonPushCounter<0)||(buttonPushCounter>MAX_EFFECTS)) buttonPushCounter=0;
	
	autoChangePatterns=atol(OTH_PARAMS[OP_AUTOEFF]);
	if ((autoChangePatterns<0)||(autoChangePatterns>2)) autoChangePatterns=0;
	
	if ((Peakdelay<1)||(Peakdelay>100)) Peakdelay=50;
	
	speed=atol(OTH_PARAMS[OP_SPEED]);
	if ((speed<0)||(speed>255)) speed=255;
	TWINKLE_SPEED=map(speed,1,255,0,8);
	
	BRIGHTNESSMARK=atol(OTH_PARAMS[OP_BRIGHT]);
	if ((BRIGHTNESSMARK<BRIGHTNESSMIN)||(BRIGHTNESSMARK>BRIGHTNESSMAX)) BRIGHTNESSMARK=BRIGHTNESSMIN;
	if (OBRIGHTNESSMARK!=BRIGHTNESSMARK)
	{
		OBRIGHTNESSMARK=BRIGHTNESSMARK;
#ifndef WIN32
		FastLED.setBrightness(BRIGHTNESSMARK);
#endif
	}

	SecToChangePattern=(uint8_t)atol(OTH_PARAMS[OP_AUTOTIME]);
	if ((SecToChangePattern<2)||(SecToChangePattern>20)) SecToChangePattern=20;

	VUMeterPos=atol(OTH_PARAMS[OP_VUPOS]);
	if ((VUMeterPos<0)||(VUMeterPos>2)) VUMeterPos=1;
	
	PeakDirection=atol(OTH_PARAMS[OP_PEAKDIR])?1:0;
	CalibrationType=atol(OTH_PARAMS[OP_CALIBTYPE]) % 4;
}

void LoadConfigPreferences()
{
	/*
	preferences.getString("EFFECT",	OTH_PARAMS[OP_EFFECT],CONFIG_NUM_LEN);
	preferences.getString("AUTOEFF",OTH_PARAMS[OP_AUTOEFF],CONFIG_NUM_LEN);
	preferences.getString("SPEED",  OTH_PARAMS[OP_SPEED],CONFIG_NUM_LEN);
	*/
	for(int i=0;i<OTH_PARAM_COUNT;i++)
	{
		preferences.getString(OTH_PARAM_NAMES[i], OTH_PARAMS[i],CONFIG_NUM_LEN);
	}
	FixParms();
}

void saveconfig_POST(AsyncWebServerRequest *request)
{
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();


	/*
	SaveVarHttp(request,"EFFECT",EFFECT);
	SaveVarHttp(request,"AUTOEFF",AUTOEFF);
	*/
	for(int i=0;i<OTH_PARAM_COUNT;i++)
	{
		SaveVarHttp(request,OTH_PARAM_NAMES[i],OTH_PARAMS[i]);
	}



	DumpConfig();
	request->send(200, "text/html", configindex );


	LoadConfigPreferences();
}

const char *ws_ok="{ \"result\" :\"ok\"  }";

void GetFixNumParam(AsyncWebServerRequest *request,char*ParamName,char *Val)
{
	if (request->hasArg(ParamName))
	{
		strncpy(Val,request->arg(ParamName).c_str(),CONFIG_NUM_LEN);
		FixParms();
		Serial.print(ParamName);Serial.println(Val);
	}
}

void ws_GET(AsyncWebServerRequest *request)
{
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();

	/*
	GetFixNumParam(request,"EFFECT",EFFECT);
	GetFixNumParam(request,"AUTOEFF",AUTOEFF);
	GetFixNumParam(request,"PEAKDELAY",PEAKDELAY);
	GetFixNumParam(request,"SPEED",SPEED);
	GetFixNumParam(request,"BRIGHT",BRIGHT);
	*/
	for(int i=0;i<OTH_PARAM_COUNT;i++)
	{
		GetFixNumParam(request,OTH_PARAM_NAMES[i],OTH_PARAMS[i]);
	}

	request->send(200, "text/html", ws_ok);
}


void StartWebServer()
{
#ifdef WIN32
	server.enableCORS();
#else
	DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
#endif
	
	server.onNotFound(NotFound);
	server.on("/",				HTTP_GET,root_GET);
	server.on("/ota",			HTTP_GET,ota_GET);
	server.on("/configindex",	HTTP_GET,configindex_GET);

	server.on("/config",		HTTP_GET,config_GET);
	server.on("/configoth",		HTTP_GET,configoth_GET);
	server.on("/scanssid",		HTTP_GET,scanssid_GET);
	server.on("/setup",			HTTP_GET,setup_GET);	// MAIN_PARAMS[MP_SSID] MAIN_PARAMS[MP_PWD] in Config Mode
	server.on("/restart",		HTTP_GET,restart_GET);	

	server.on("/savesetup",	HTTP_POST,savesetup_POST);// MAIN_PARAMS[MP_SSID] MAIN_PARAMS[MP_PWD]
	server.on("/saveconfig",HTTP_POST,saveconfig_POST);

	server.on("/ws",		HTTP_GET,ws_GET);

#ifndef WIN32

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
	  
	if ((IsSetupMode==0)&&(!request->authenticate("admin", MAIN_PARAMS[MP_PIN]))) request->requestAuthentication();
	  
	shouldReboot = !Update.hasError();
	
	AsyncWebServerResponse *response = request->beginResponse(200, "text/html", shouldReboot?"OK <a href='/'>Home</a>":"FAIL <a href='/'>Home</a>");
    response->addHeader("Connection", "close");
    request->send(response);
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
	{
      //Update.runAsync(true);
      //if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
		  int cmd = ( request->arg("utype")  == "files") ? U_SPIFFS : U_FLASH;
		  
		  Serial.printf("Update Start: %s %d\n", filename.c_str(),cmd);
		  
		  if(!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
		  {
			Update.printError(Serial);
		  }
    }
    if(!Update.hasError())
	{
		  if(Update.write(data, len) != len)
		  {
			Update.printError(Serial);
		  }
    }
    if(final)
	{
		  if(Update.end(true))
		  {
			Serial.printf("Update Success: %uB\n", index+len);
		  } 
		  else 
		  {
			Update.printError(Serial);
		  }
    }
  });

#endif

    server.begin();
}


void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print(timeinfo.tm_hour);Serial.print(":");
  Serial.print(timeinfo.tm_min);Serial.print(":");
  Serial.print(timeinfo.tm_sec);Serial.print(" ");

  Serial.print(timeinfo.tm_year+1900);Serial.print("-");
  Serial.print(timeinfo.tm_mon);Serial.print("-");
  Serial.print(timeinfo.tm_mday);Serial.print("\n");
}


void GetTimeFromNTP()
{
	if ((NTPCheckInterval>0)&&(strlen(MAIN_PARAMS[MP_NTPSERVER])>0))
	{
		int gmtsec=atol(MAIN_PARAMS[MP_GMTOFFSEC]);
		Serial.print("Getting Time from NTP Server.Local GMT Offset(Sec) - ");
		Serial.print(MAIN_PARAMS[MP_GMTOFFSEC]);
		Serial.print(" ");
		Serial.println(MAIN_PARAMS[MP_NTPSERVER]);
		configTime(atol(MAIN_PARAMS[MP_GMTOFFSEC]), 0, MAIN_PARAMS[MP_NTPSERVER]);
		printLocalTime();
	}
}


void SetupCaptivePortal()
{
	WiFi.mode(WIFI_AP);
	String APName="GW-DAM-";
	APName+=WiFi.macAddress().substring(6);
	WiFi.softAP(APName.c_str());
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	// if DNSServer is started with "*" for domain name, it will reply with
	// provided MAIN_PARAMS[MP_IP] to all DNS request
	dnsServer.start(DNS_PORT, "*", apIP);

	StartWebServer();
}	

void HandleCaptivePortal()
{
  dnsServer.processNextRequest();

#ifdef WIN32
	server.handleClient();
#endif

  delay(2);
  
 if(shouldReboot){
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }
}

void WaitForWifiElseRestart()
{
	for(int i=0;i<30;i++)
	{
		
		if ((i%2)==0)
			digitalWrite(LED_PIN,1);
		else
			digitalWrite(LED_PIN,0);
		

		if (WiFi.status() == WL_CONNECTED) break;
		delay(1000);
		Serial.print(".");
	}

	if (WiFi.status() != WL_CONNECTED) ESP.restart();

	digitalWrite(LED_PIN,1);
}



void SetupClientMode()
{
	Serial.print("Client Mode. Connecting to Wifi :");
	Serial.println(MAIN_PARAMS[MP_SSID]);

	if (strcmp(MAIN_PARAMS[MP_IPTYPE],"STATIC")==0)
	{
		Serial.print("STATIC MAIN_PARAMS[MP_IP] Config.");

		// Set your Static MAIN_PARAMS[MP_IP] address
		IPAddress local_IP;local_IP.fromString(MAIN_PARAMS[MP_IP]);
		// Set your Gateway MAIN_PARAMS[MP_IP] address
		IPAddress gateway;gateway.fromString(MAIN_PARAMS[MP_GW]);
		IPAddress subnet;subnet.fromString(MAIN_PARAMS[MP_MASK]);
		IPAddress primaryDNS;primaryDNS.fromString(MAIN_PARAMS[MP_DNS1]);   //optional
		IPAddress secondaryDNS;secondaryDNS.fromString(MAIN_PARAMS[MP_DNS2]); //optional

		 if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) 
		 {
			Serial.println("STA Failed to configure for STATIC MAIN_PARAMS[MP_IP].");
		 }
	}

	WiFi.begin(MAIN_PARAMS[MP_SSID], MAIN_PARAMS[MP_PWD]);

	WaitForWifiElseRestart();

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("MAIN_PARAMS[MP_IP] address: ");
    Serial.println(WiFi.localIP());

	String HostName="GW-DAM-";
	HostName+=WiFi.macAddress().substring(6);
	MDNS.begin(HostName.c_str());
	StartWebServer();
	MDNS.addService("http", "tcp", 80);


//*********************************************************************

	CurrentTS = millis()/1000; // Seconds Counter

	GetTimeFromNTP();
	NTP_TS = CurrentTS;

}

void HandleClientMode()
{
//***************************************************************
#ifdef WIN32
	server.handleClient();
#endif
	delay(2);//allow the cpu to switch to other tasks
//***************************************************************

 if(shouldReboot){
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }

}


void LoadPreferences()
{
	/*
	preferences.getString("MAIN_PARAMS[MP_SSID]", MAIN_PARAMS[MP_SSID],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_PWD]", MAIN_PARAMS[MP_PWD],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_PIN]", MAIN_PARAMS[MP_PIN],CONFIG_VAR_LEN);

	preferences.getString("MAIN_PARAMS[MP_IPTYPE]", MAIN_PARAMS[MP_IPTYPE],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_IP]",		MAIN_PARAMS[MP_IP],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_MASK]",	MAIN_PARAMS[MP_MASK],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_GW]",		MAIN_PARAMS[MP_GW],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_DNS1]",	MAIN_PARAMS[MP_DNS1],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_DNS2]",	MAIN_PARAMS[MP_DNS2],CONFIG_VAR_LEN);

	preferences.getString("MAIN_PARAMS[MP_NTPSERVER]",		MAIN_PARAMS[MP_NTPSERVER],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_GMTOFFSEC]",	MAIN_PARAMS[MP_GMTOFFSEC],CONFIG_VAR_LEN);
	preferences.getString("MAIN_PARAMS[MP_NTP_POLL]",		MAIN_PARAMS[MP_NTP_POLL],CONFIG_NUM_LEN);
	*/

	for(int i=0;i<MAIN_PARAM_COUNT;i++)
	{
		preferences.getString(MAIN_PARAM_NAMES[i], MAIN_PARAMS[i],CONFIG_VAR_LEN);
	}

	
	NTPCheckInterval=atol(MAIN_PARAMS[MP_NTP_POLL]);
	if (NTPCheckInterval<60) NTPCheckInterval=60;

	LoadConfigPreferences();
}




void setup_server()
{
	pinMode(SETUP_MODE_PIN, INPUT_PULLUP);

#ifdef WIN32
	LoadFile();
#endif

	preferences.begin("gw-dam",false);
	LoadPreferences();
	DumpConfig();
	
	if (digitalRead(SETUP_MODE_PIN)==LOW) IsSetupMode=1;
	if (strlen(MAIN_PARAMS[MP_SSID])==0) IsSetupMode=1;

#ifdef WIN32
	// Debugging
	IsSetupMode=0;
#endif
	
    if (IsSetupMode)
	{
		Serial.println("Setup Mode. AP Started...");
		//IsLoggedIn=1;
		SetupCaptivePortal();	
	}
	else
	{
		Serial.println("Client Mode....");
		SetupClientMode();	
	}

}

int bcounter=0;
void loop_server()
{
	if (IsSetupMode)
	{
		CurrentTS = millis(); // ms counter
		HandleCaptivePortal();
		if (CurrentTS - previousTS >=200) 
		{
			
			bcounter++;
			if ((bcounter%2)==0)
				digitalWrite(LED_PIN,1);
			else
				digitalWrite(LED_PIN,0);
				

			previousTS = CurrentTS;
		}
	}
	else
	{
		CurrentTS = millis()/1000; // Seconds Counter

		HandleClientMode();

		/*
		if ( (CurrentTS - CommCheckTS) >=CommCheckInterval) 
		{
			if (COMM_WDT[0]=='1')
			{
				Serial.print("No Communication ...");
				Serial.println(NoComCount);
				if (NoComCount==10)
				{
					ESP.restart();
				}
				NoComCount++;
			}
			CommCheckTS=CurrentTS;
		}
		*/
	
		// if WiFi is down, try reconnecting
		if ( (CurrentTS - previousTS) >=WifiCheckInterval) 
		{
			//Serial.print(".");Serial.print(WiFi.status());

			if (WiFi.status() != WL_CONNECTED)
			{
			  //Serial.print(millis());
			  Serial.println("Reconnecting to WiFi...");
			  WiFi.disconnect();
			  WiFi.reconnect();

			  WaitForWifiElseRestart();

			}
		  previousTS = CurrentTS;
		}

		if (NTPCheckInterval>0)
		{
			if ( (CurrentTS - NTP_TS) >=NTPCheckInterval) 
			{
				GetTimeFromNTP();
				NTP_TS = CurrentTS;
			}
		}


	}
}
