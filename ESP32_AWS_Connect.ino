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
const char* Wifi_ssid = "    ";
const char* Wifi_pass = "    ";

// Define the AWS API Endpoint with which the the device should communicate
const char* AWS_IOT_ENDPOINT = "a2szh7brcv4t4r-ats.iot.us-east-2.amazonaws.com" ;

// Amazon Root CA 1
const char AWS_CERT_CA[]=
"-----BEGIN CERTIFICATE-----\n" \

"-----END CERTIFICATE-----\n";

// Device Certificate
const char AWS_CERT_CRT[] = 
"-----BEGIN CERTIFICATE-----\n" \

"-----END CERTIFICATE-----\n";

// Device Private Key
const char AWS_CERT_PRIVATE[] =
"-----BEGIN RSA PRIVATE KEY-----\n" \

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
