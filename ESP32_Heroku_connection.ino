#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "bhavesh v2";
const char* password = "9821087287";
String payload;
String data2;
int i=0;

//Your Domain name with URL path or IP address with path
const char* serverName = "https://acmg90.herokuapp.com/v1/graphql";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  Serial.println("");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      http.addHeader("x-hasura-admin-secret", "acmcfi");

      data2 = "{\"query\":\"query MyQuery {\n  orders {\n    item_name\n    id\n   is_cooked\n    }\n}\n\",\"variables\":null,\"operationName\":\"MyQuery\"}";
      int httpResponseCode = http.POST(data2);
   
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      if (httpResponseCode > 0) 
      {
          payload = http.getString();
          Serial.print("PAYLOAD: ");
          Serial.println(payload);
          Serial.println("");
      }

      StaticJsonBuffer<800> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload); // Decoding/parsing the json string to a JSON object 'root'
      //JsonArray& items = jsonBuffer.createArray();
      
      if(root.success())
      {
        JsonArray& items = root["data"]["orders"];
      
      for(i=0;i<items.size();i++)
      {
        Serial.print("The item selected for cooking is ");
        JsonObject& data_orders_i = items[i];
        const char* item_name_i = data_orders_i["item_name"];
        Serial.print(item_name_i);
        bool data_orders_i_is_cooked = data_orders_i["is_cooked"];
        if (data_orders_i_is_cooked == 1)
        {
          Serial.println(". This item is cooked");
        }
        else
        {
          Serial.println(". This item is yet to be cooked");
        }
      }
//      Serial.print("item names: ");
//      items.printTo(Serial);
        
      }

      
      http.end();

     Serial.println("");
     Serial.println("");
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

//var req ='{"query":"mutation MyMutation {\\n  insert_orders(objects: {item_id: \\"${item.menuid}\\" , item_name: \\"${item.menuTitle}\\", machine_id: \\"7da727c4-87d4-428e-b72e-07505195e9c1\\", user_id: \\"7da727c4-87d4-428e-b72e-07505195e9c1\\"}) {\\n    affected_rows\\n  }\\n}\\n","variables":null,"operationName":"MyMutation"}';
