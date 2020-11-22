#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

bool ap_mode = true;    //indicate if ESP needs AP mode
bool sta_mode = false;    //indicate if ESP needs STA mode

String ssid_home;  // ssid of home network
String pass_home; // password of the home network
String Wifiarr[10];

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

void setup() {
  init_device();
  createHTTPServer();
  Serial.println();
  Serial.println("ap_mode:" + String(ap_mode));
  Serial.println("sta_mode:" + String(sta_mode));
   if (sta_mode) {
        WiFiconnect();
    }
}


void loop() 
{
    delay(100);
    server.handleClient();
    if (ap_mode && sta_mode) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
        }
        else {
            Serial.println();
            Serial.println("Home WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            WiFi.mode(WIFI_STA);
            disableAP();
            Serial.println();
            Serial.println("ap_mode:" + String(ap_mode));
            Serial.println("sta_mode:" + String(sta_mode));
        }
    }

    else if (sta_mode) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Home WiFi dropped, reconnecting...");
            WiFi.mode(WIFI_AP_STA);
            enableAP();
        }
    }
}



void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void init_device()
{
  Serial.begin(115200);
  EEPROM.begin(512);
  if (strlen(readEEPROM(128, 160)) == 0 || strlen(readEEPROM(160, 192)) == 0) 
    {
        sta_mode = false;
    }
    else 
    {
        sta_mode = true;
    }
}

void configure()
{
   Serial.println();
   Serial.println("--------Set configuration start--------");
   
   if (server.hasArg("ssid") && server.hasArg("psk"))
   {
    clearEEPROM(128, 256);
    writeEEPROM(128, 160, server.arg("ssid").c_str());
    writeEEPROM(160, 192, server.arg("psk").c_str());
    Serial.println("ssid_home:" + String(readEEPROM(128,160)));
    Serial.println("pass_home:" + String(readEEPROM(160,192)));
    Serial.println("--------Set configuration end----------");
    sta_mode = true;
    bool ssid_ispresent = listWiFi(readEEPROM(128,160));
    if (ssid_ispresent)
    {
      server.send(200, "text/plain", "1");
      delay(1000);
      WiFiconnect();
    }
    
    else 
    {
      server.send(200, "text/plain","0");
    }
   }

   else
   {
    server.send(200, "text/html", "Invalid Network credentials!\nTry Again");
    Serial.println("Invalid Network credentials!\nTry Again");
    Serial.println("--------Set configuration end----------");
   }
}

void enableAP() 
{
    ap_mode = true;
    Serial.println("Access Point is enabled.");
}

void disableAP() {
    ap_mode = false;
    Serial.println("Access Point is disabled.");
}

void WiFiconnect() {
    WiFi.disconnect();
    Serial.println();
    Serial.print("Connecting to HomeWiFi...");
    WiFi.mode(WIFI_STA);
    //WiFi.begin(ssid_home.c_str(), pass_home.c_str());
    WiFi.begin(readEEPROM(128,160), readEEPROM(160,192));
    //Serial.println(ssid_home.c_str());
}

void createHTTPServer() {
  if (sta_mode == 0)
  {
    Serial.println("--------Create HTTPServer start--------");
    Serial.println("Configuring access point...");
    WiFi.softAP(ssid, password);
    Serial.println("Access Point is created.");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("IP address: ");
    Serial.println(myIP);
    server.on("/configure", configure);
    server.on("/sendWifiList",sendWifiList);
    server.onNotFound(handle_NotFound);
    server.begin();
    Serial.println("HTTP server is created.");
  }
}



void clearEEPROM(int st, int ed) {
    for (int i = st; i < ed; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    //Serial.println("EEPROM Cleared!");
    Serial.printf("clearEEPROM (%d, %d).\n", st, ed);
}

char *readEEPROM(int st, int ed) {
    String str;
    int block;
    //Serial.print("readEEPROM: ");
    for (int i = st; i < ed; i++) {
        block = EEPROM.read(i);
        //Serial.print(char(block));
        if (block == 0) {
            break;
        }
        str += char(block);
    }
    //Serial.println();
    char *chr = new char[str.length() + 1];
    strcpy(chr, str.c_str());
    return chr;
}

void writeEEPROM(int st, int ed, const char *chr) {
    for (int i = 0; i < strlen(chr); i++) {
      EEPROM.write(i + st, chr[i]);
    }
    EEPROM.commit();
    Serial.printf("writeEEPROM (%d, %d): %s\n", st, ed, chr);
}

bool listWiFi(char* q)
{
  bool flag = false;
  int numSsid = WiFi.scanNetworks();
  //Serial.println(numSsid);
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }

  //Serial.println(String(q));
  for(int i=0; i<numSsid; i++)
  {
    //Serial.println(WiFi.SSID(i));
    
    if (WiFi.SSID(i)==String(q))
    {
      flag = true;
      break;
    }
    else
    {
      continue;
    }
  }
  //Serial.println("");
  return flag;
}

void sendWifiList()
{
  Serial.println("scan start");
    
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    //Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        //Serial.print(n);
        Serial.println(" networks found");
        for (int k = 0; k < n; k++) {
            // Print SSID and RSSI for each network found
            Serial.print(k + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(k));
            Wifiarr[k] = String(WiFi.SSID(k));
            Serial.println("");
            server.send(200, "text/html", String(WiFi.SSID(k)));
        }
    }
    Serial.println("");
    Serial.println(Wifiarr[10]);
    // Wait a bit before scanning again
    //delay(5000);
}
