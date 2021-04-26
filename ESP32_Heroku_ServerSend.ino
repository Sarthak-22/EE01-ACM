#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "       ";
const char* password = "    ";
String payload;
String data1;
String data2;

//Your Domain name with URL path or IP address with path
const char* serverName = "https://acmg90.herokuapp.com/v1/graphql";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (15000)
unsigned long timerDelay = 15000;

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
 
  Serial.println("Timer set to 15 seconds (timerDelay variable), it will take 15 seconds before publishing the first reading.");
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

      String item_name = "Dal Makhni";
      String id = "d5e7d23a-0a5d-4616-ab95-aca08bbe765f";

      data1 =  "${\"query\":\"mutation ($_eq2: uuid\u0021) {\n update_orders(where: {id: $_eq2}}, _set: {is_cooked: false}) {\n returning {\n id\n item_name\n is_cooked\n }\n affected_rows\n }\n}\n \",\"variables\":{\"_eq2\":\"d5e7d23a-0a5d-4616-ab95-aca08bbe765f\"},\"operationName\":\"MyMutation\"}";
      data2 = "{\"query\":\"mutation MyMutation {\n  update_orders(where: {item_name: {_eq: \\\""+item_name+"\\\"}, _and: {id: {_eq: \\\""+id+"\\\"}}}, _set: {is_cooked: true}) {\n    returning {\n      is_cooked\n    }\n    affected_rows\n  }\n}\n \",\"variables\":null,\"operationName\":\"MyMutation\"}"; 
      Serial.println(data1);
      int httpResponseCode = http.POST(data1);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      if (httpResponseCode > 0) 
      {
          payload = http.getString();
          Serial.print("PAYLOAD: ");
          Serial.println(payload);
          Serial.println("");
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

//"{\"query\":\"mutation MyMutation {\n  insert_being_cooked(objects: {item_name: \"Palak Paneer\", item_id: \"cf43a02a-3014-45d0-a4f1-a48d67cfbd27\", machine_id: \"7da727c4-87d4-428e-b72e-07505195e9c1\", user_id: \"7da727c4-87d4-428e-b72e-07505195e9c1\"}) {\n returning {\n      item_name\n      item_id\n      machine_id\n      user_id\n    }\n    affected_rows\n  }\n}\n\",\"variables\":null,\"operationName\":\"MyMutation\"}";
 
