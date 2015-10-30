/*
 * This program only provide a example about how create a http
 * message when use the SDK. Please refer to correlative document
 * to get more information.
 */

/*********************resource type***************************
enum type             |         resource
--------------------------------------------------------------
TYPE_USER                	user
TYPE_DEV                	device
TYPE_DATASTREAM     		datastream
TYPE_DATAPOINT       		datapoint
TYPE_TRIGGER          		trigger
TYPE_APIKEY            		API-KEY
TYPE_BINDATA           		bin data
TYPE_APILOG             	API log
TYPE_HISTORYDATA        	history data
**************************************************************

*********************Operation type***************************
enum type             |                action
--------------------------------------------------------------
POST					add resource
PUT				        updata resource
GET				        inqery resource
DELETE 					delete resource
*************************************************************/
#include <HttpPacket.h>

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

HttpPacketHead packet;
   
void loop() {
  // put your main code here, to run repeatedly:
  char *p = "{\"datastreams\":[{\"id\":\"sys_time\",\"datapoints\":[{\"value\":50}]}]}";
  
  packet.setHostAddress("api.heclouds.com");
  packet.setDevId("<device_id>");
  packet.setAccessKey("<Your access api>");
  packet.setDataStreamId("<datastream_id>");
  packet.setTriggerId("<trigger_id>");
  packet.setBinIdx("<bin_index>");

  /*create the http message about add datapoint */
  packet.createCmdPacket(POST, TYPE_DATAPOINT, p);
  if (strlen(packet.content))
    Serial.print(packet.content);
  Serial.print(p);
  Serial.println("\n");

  /*create the http message about inqerying single datastream */
  packet.setDataStreamId("<datastream_id>");
  packet.createCmdPacket(GET, TYPE_DATASTREAM);
  if (strlen(packet.content))
    Serial.println(packet.content);

  /*create the http message about batch inqerying datastreams */
  packet.addUrlParameter("datastream_ids", "id1");
  packet.addUrlParameter("datastream_ids", "id2");
  packet.addUrlParameter("datastream_ids", "id3");
  packet.createCmdPacket(GET, TYPE_DATASTREAM);
  if (strlen(packet.content))
    Serial.println(packet.content);

  /*delete datastream_ids url parameter */
  packet.removeUrlParameter("datastream_ids");
  packet.createCmdPacket(GET, TYPE_DATASTREAM);
  if (strlen(packet.content))
    Serial.println(packet.content);

  /*delete device */
  packet.createCmdPacket(DELETE, TYPE_DEV);
  if (strlen(packet.content))
    Serial.println(packet.content);
}

