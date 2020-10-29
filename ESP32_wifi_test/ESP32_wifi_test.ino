#include <WiFi.h>

uint port = 8888;
WiFiServer server(port);
WiFiClient host;

const char* ssid = "RockoNet";
const char* password = "Sasroc0882!";

int init_tcp_connect();

int state = 0;
const String states[4] = {"BOOT", "WIFI_CONNECT", "TCP_ENABLED", "OPERATIONAL"};

#define BUF_SIZE 128
int   buf_cur = 0;
int   iter;
char  rx_buf[BUF_SIZE];
char  rx_msg[BUF_SIZE];
char  in_byte;
long  rx_time;


void setup() {
  // connect to wifi network
  Serial.begin(115200);
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("Connecting to Wifi network: ");
    Serial.println(ssid);
    }

  state++;
  Serial.println("Success!");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
  
  // set up a server for tcp communication
  
  server.begin();
  Serial.println("Server set up.");
  Serial.print("IP after server init:");
  Serial.println(WiFi.localIP());
}

void loop() {

  while(state < 2){
    Serial.println("Waiting for TCP connection...");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    if (server.hasClient()) { 
      if (host.connected()) {
        server.available().stop();
        }
      else {
        host = server.available();
        state = 2;
        }
      }
    delay(200);
    }
  Serial.println("Client found!");
  

  // listen for msg from client
  while(host.connected()){
    if (host.available()){
      
      in_byte = host.read();

      Serial.println(rx_buf);
      
      if(in_byte == 'M'){
        buf_cur = 0;
        }

      else if(in_byte == 'T'){
        rx_buf[buf_cur++] = '\0';
        strcpy(rx_msg,rx_buf);
        buf_cur = 0;
        }
        
      else if(in_byte == 'E'){
        rx_buf[buf_cur++] = '\0';
        rx_time = atoi(rx_buf);
        Serial.println(rx_msg);
        Serial.println(rx_time);
        }

      else{
        // prevent array from overflowing, data will circularly overwrite the buffer
        if(buf_cur >= BUF_SIZE)
          buf_cur = 0;
  
        // add new character to the buffer
        rx_buf[buf_cur++] = in_byte;
        }
    }
    // this delay prevents crashes when the host PC closes the connection
    else{
      delay(1);
      }
    
  }

  Serial.print("UNDEFINED: ");
  Serial.println(state);


  host.stop();
  state = 1;
}
