#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

WiFiClientSecure securenet = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

# define SECRET
# define THINGNAME "MyESP32"

// Define the MQTT Topics that this device should subsribe/publish
# define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"      
# define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"  

// Define Network credentials
const char* Wifi_ssid = "bhavesh v2";
const char* Wifi_pass = "9821087287";

// Define the AWS API Endpoint with which the the device should communicate
const char* AWS_IOT_ENDPOINT = "a2szh7brcv4t4r-ats.iot.us-east-2.amazonaws.com" ;

// Amazon Root CA 1
const char AWS_CERT_CA[]=
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";

// Device Certificate
const char AWS_CERT_CRT[] = 
"-----BEGIN CERTIFICATE-----\n" \
"MIIDWjCCAkKgAwIBAgIVAL2ODhnbBOuxSe9qtfxgTfdDcbIbMA0GCSqGSIb3DQEB\n" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDExMTExODIy\n" \
"MDJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDvlrYrs6hn6tFW1yiu\n" \ 
"tCIHcztPFbOeH8e2VmIHilqHusEi+nwZkEs3f9BxVWpymFTwkN3AIuQbOtlleFQT\n" \
"YwLZKqPWTivrBpK+UA7vJSqY3npjRloCORMqCgdTNS8COxGp9hWChEIe5HNAuQXV\n" \
"bmr+fVz9HexDt+ysuuvT+iEYtniqDDySCjAvby0giKt9a5LD+UGd8Oi5NnWP7wzv\n" \
"TB3m+6BE6myboRFReuw5hFqr2J3MwVRT5DsM6wxSKop1pEMiQqUPXaQCuaS4wg8+\n" \
"/H//WWMyxtYk7dVgStqiuOcl0QT9gTrdUk/OZJHVGdE+494FT+vgfCjeD5dCGAIw\n" \
"924LAgMBAAGjYDBeMB8GA1UdIwQYMBaAFIplkQoE9KKCw1t38r30o1puSu9/MB0G\n" \
"A1UdDgQWBBRepnPpkPeBCTYjvAlr7KuUcusMNjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAhIjuXX2Vj0nPGLe6BLfcQZzE\n" \
"rBip+y9ZqWY2ToGurPKVlFyCjYHq3a/SLDmXe2nZeYunbbwCotWZcT3kQVxJlcht\n" \
"pa4ocEinl2hLGKlzi7Z6FTxvOKPWzqe61MtT6D+bR+RK/77RDPZiqd0qwoVT9Mpm\n" \
"2OHNNJxGPl4chNbY1qxWrMgB2iUo/BKVjmAXUUb18GHntX01VttsNVtz9/9kb2Hm\n" \
"WzeCj3AnZyOSZCxGJeIhLv6o12pg7NPAkys2ewFIxAMkIpDZGT7sCPqN6VL6x9y1\n" \
"Gtr8A19ZyVF2xzMiYoqcvgNo70ZiKEBKoHXR1ZORl3uZFhONdXxpIxjmhrZ2SQ==\n" \
"-----END CERTIFICATE-----\n";

// Device Private Key
const char AWS_CERT_PRIVATE[] =
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEowIBAAKCAQEA75a2K7OoZ+rRVtcorrQiB3M7TxWznh/HtlZiB4pah7rBIvp8\n" \
"GZBLN3/QcVVqcphU8JDdwCLkGzrZZXhUE2MC2Sqj1k4r6waSvlAO7yUqmN56Y0Za\n" \
"AjkTKgoHUzUvAjsRqfYVgoRCHuRzQLkF1W5q/n1c/R3sQ7fsrLrr0/ohGLZ4qgw8\n" \
"kgowL28tIIirfWuSw/lBnfDouTZ1j+8M70wd5vugROpsm6ERUXrsOYRaq9idzMFU\n" \
"U+Q7DOsMUiqKdaRDIkKlD12kArmkuMIPPvx//1ljMsbWJO3VYEraorjnJdEE/YE6\n" \
"3VJPzmSR1RnRPuPeBU/r4Hwo3g+XQhgCMPduCwIDAQABAoIBACiejZ4VJH0Rw5IM\n" \
"glo0Mfy/XZPH4x7w1mQkexQ6MaUHVY0/mGChJQ9wvP8p8xUgnG34sY/Mmo2/SXI5\n" \
"l09+HtKAtywzLqD3CVP4sL98tp8aQPepysLLuGMNNja2MpUXPKRrEzSAhdFE/AFC\n" \
"HSoVQVCKIoTLWO4WNasHOcpDgu70VMA92gTDMfAyBsCH9Mq9zDYFaYZoA6lCDRci\n" \
"hvLyCbMKhJv0PK2SFXu2jAPjoi8ZWStfo9RGsQS4N9ePYVt+X9+PfsEGPjhEqlt/\n" \
"3opQ+CiR1ficnrXKvha/LEHB1zxyDdKq2jqWs/APoWR3DfUWeLWaTwlzb3MEb7Tm\n" \
"xDXmFXkCgYEA+Y1KgPjxYfmGL9XVwJ4HSbFHAtbkw32FnOnqCzJA4uUD51GY/fDP\n" \
"vZCTSUgcFv/KB47LrojWIyJdUopM4KVGaKSOFdBF4hBnICyg8mXUEOR366TRrnOc\n" \
"KmE0WW0wD8qsRKBqaBM6FUJs2XFrCdoM1t0ipOJGruaa0olpLOjwWC0CgYEA9ceE\n" \
"XuSeK1SICY80R+qBr5VMBrOofrEt+EFjfir4F5228P8YXsA6QEoKIkyhs7MqBYhi\n" \
"RxOkj/gqOZzv0ikIFtuhwGEZqhCrQRrV1t7t5qATkSo51J+Fgt9DaPjvvTLitkFh\n" \
"FZ51QMXml0dDijgEToLFSDJWip1Bmjo6LYh8yhcCgYEAohU43MfTz7THiO40JST+\n" \
"yshPvPJiJu6yB5aOIrODHVtty6Jd4iG0bAUpBKHHTNDaIUqH1Faf7rX1MxUMlaQD\n" \
"NRgcCOL7YLcSo/0YlBe7QzWBBhZI8iu4tHXqEGMwojEa7n/aXLqQMRqxpBFcWku7\n" \
"pxp90+X7zsGMsU36d+bbpAkCgYB0J/gSVN+/wLKOOgTOXDEwR1Hn9O6T+a6tHlIk\n" \
"JbCckKgOG7FrzlN5YMSLU0Y0xF7+wGcaClmCxRlHozjgTQ4Kp4A5iGp1qm0VVtZr\n" \
"N26dyVeWCqvMJhwWLugP6156P1tQIXP9oKbvltgU9NQhtbmrVThhl5C47/s6JXyr\n" \
"Wk8HvwKBgF5u0EivCN11uaq0QbFy+5NpN0autuk0ofyy2YSuALYZb7js+xypcjGh\n" \
"XOlXf0n6qDFmgv5+h0icGT6WTxjTp9WNntddnSgW5XDunp0zgu+cjitx1YM8MODV\n" \
"sPujnFglQFXbT7PD94IQ5IeydlXZSxMnud1OlvP4Vm23lfcXjOMK\n" \
"-----END RSA PRIVATE KEY-----\n";


void setup() 
{
  Serial.begin(115200);
  AWS_connect();
}

void loop() 
{
  publish_message();
  client.loop();
  delay(5000);
}

void AWS_connect()
{
  WiFi.begin(Wifi_ssid, Wifi_pass);

  Serial.println("");
  Serial.print("Connecting to WiFi");
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  securenet.setCACert(AWS_CERT_CA);
  securenet.setCertificate(AWS_CERT_CRT);
  securenet.setPrivateKey(AWS_CERT_PRIVATE);

  client.begin(AWS_IOT_ENDPOINT,8883,securenet); // Begin connection with the host at port 8883 (encrypted connection for secure clients)

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  // Create a message handler(function) which is called whenever a message is received from a subscribed topic
  client.onMessage(messageHandler);
  
  Serial.println("");
  Serial.println("AWS IoT Connected!");
}


// Publishes message to the MQTT Broker for a given Publish Topic
void publish_message()
{
  String item = "Dal Makhni";
  
  StaticJsonDocument<200> doc;
  doc["Item Name"] = item;
  char pub[512];
  serializeJson(doc, pub); // print to client
  
  client.publish(AWS_IOT_PUBLISH_TOPIC, pub);
}

// Handles the incoming message from the subscribed topic
void messageHandler(String &topic, String &payload)
{
  Serial.println("Incoming message from AWS IOT at the topic "+topic+" -");
  Serial.println(payload);
}
