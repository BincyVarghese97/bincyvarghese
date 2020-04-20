// Based on the Paho C code example from www.eclipse.org/paho/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <time.h>
#include "MQTTClient.h"
//export MQTT_C_CLIENT_TRACE=ON
//export MQTT_C_CLIENT_TRACE_LEVEL=PROTOCOL
#define  CPU_TEMP "/sys/class/thermal/thermal_zone0/temp"
using namespace std;

//Please replace the following address with the address of your server
#define ADDRESS    "tcp://192.168.1.11:1883"
#define CLIENTID   "bincy"
#define AUTHMETHOD "bincyvarghese"
#define AUTHTOKEN  "bincy"
#define TOPIC      "ee513/CPUTemp"
#define PAYLOAD     "online"//last will example
#define PAYLOAD_OFF "offline"
#define QOS        1
#define TIMEOUT    10000L

//MQTT_C_CLIENT_TRACE=ON
//MQTT_C_CLIENT_TRACE_LEVEL=PROTOCOL
  double  theseSecs = 0.0;//Cpu load and time 
  double  startSecs = 0.0;
  double  secs;
  double  CPUsecs = 0.0;
  double  CPUutilisation = 0.0;
  double  answer = 0;
  clock_t starts;

  void start_CPU_time()
  {      
      starts = clock();;
      return;
  }

  void end_CPU_time()
  {
      CPUsecs = (double)(clock() - starts)/(double)CLOCKS_PER_SEC;
      return;
  }    

  struct timespec tp1;
  void getSecs()
  {
     clock_gettime(CLOCK_REALTIME, &tp1);
     theseSecs =  tp1.tv_sec + tp1.tv_nsec / 1e9;           
     return;
  }

  void start_time()
  {
      getSecs();
      startSecs = theseSecs;
      return;
  }

  void end_time()
  {
      getSecs();
      secs    = theseSecs - startSecs;
      return;
  }    

  void calculate()
  {
      int i, j;
      for (i=1; i<100001; i++)
      {
          for (j=1; j<10001; j++)
          {
              answer = answer + (float)i / 100000000.0;
          }
      }
  }
float getCPUTemperature() {        // get the CPU temperature
   int cpuTemp;                    // store as an int
   fstream fs;
   fs.open(CPU_TEMP, fstream::in); // read from the file
   fs >> cpuTemp;
   fs.close();
   return (((float)cpuTemp)/1000);
}

int main(int argc, char* argv[]) {
   char str_payload[100];          // Set your max message size here
   MQTTClient client;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;//last will
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   //MQTT_C_CLIENT_TRACE=ON
  // MQTT_C_CLIENT_TRACE_LEVEL=PROTOCOL
   will_opts.topicName = TOPIC;//last will
   will_opts.message = PAYLOAD_OFF;
   will_opts.qos = QOS;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   opts.keepAliveInterval = 20;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   opts.will = &will_opts;
   int rc;
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }
   sprintf(str_payload, "{\"d\":{\"CPUTemp\": %f }}", getCPUTemperature());
   pubmsg.payload = str_payload;
   pubmsg.payloadlen = strlen(str_payload);
   pubmsg.qos = QOS;
   pubmsg.retained = 0;
   MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
   cout << "Waiting for up to " << (int)(TIMEOUT/10000) <<
        " seconds for publication of " << str_payload <<
        " \non topic " << TOPIC << " for ClientID: " << CLIENTID << endl;
     
   rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
   cout << "Message with token " << (int)token << " delivered." << endl;
   start_time();//cpu load and time
     start_CPU_time();
     calculate();
     end_time();
     end_CPU_time();
     CPUutilisation = CPUsecs /  secs * 100.0;
     cout<<"\n Answer for CPU detils is " <<answer<<" Elapsed Time "<<secs<< "CPU Time " <<CPUsecs<< "CPU Ut "<<CPUutilisation<<endl;  
     time_t timeStamp;
     time(&timeStamp);
     cout<<"It is now " << ctime(&timeStamp) <<endl;
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
   return rc;
}

