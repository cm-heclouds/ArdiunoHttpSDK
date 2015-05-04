¼òµ¥Ê¾Àý£º

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
}

void loop() 
{
  float temp = 20, hum = 56;
  int noise = 160;
  int light = 1500;
  StaticJsonBuffer<300> jsonBuffer;

  JsonObject& jsonData = jsonBuffer.createObject();
  JsonArray& data = jsonData.createNestedArray("datastreams");  
  addJsonDataRecord("temp", (int)temp, data);
  addJsonDataRecord("hum", (int)hum, data);
  addJsonDataRecord("light", light, data);
  addJsonDataRecord("noise", noise, data);
  
  sendHttpPacketHead(jsonData);
  jsonData.printTo(Serial);  
  Serial.println();
}

void sendHttpPacketHead(JsonObject& buf)
{
  HttpPacketHead packetHead;
  packetHead.setHostAddress("api.heclouds.com");
  packetHead.setDevId("70290");
  packetHead.setAccessKey("YOU API KEY");
  packetHead.createCmdPacket(POST, TYPE_DATAPOINT, buf);
  Serial.print(packetHead.content);
}

void addJsonDataRecord(char key[], int value, JsonArray& array)
{
    JsonObject& root = array.createNestedObject();
    root.add("id", key);
    JsonArray& data = root.createNestedArray("datapoints");
    JsonObject& root1 = data.createNestedObject();
    root1.add("value", value);
}
