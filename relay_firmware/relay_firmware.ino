#include <SPI.h>
#include <WiFi.h>

// buffer size for recieving TCP terminating character
#define BUF_SIZE    128
#define END_MSG     'E'
#define TIME_MSG    'T'
#define CMD_MSG     'C'
#define CONFIG_MSG  '!'

// pins for SPI on LCD
#define VSPI FSPI   // unclear what this is.....
#define VSPI_MISO   5
#define VSPI_MOSI   6
#define VSPI_SCLK   4
#define VSPI_SS     7
#define SCR_WIDTH   16
#define RELAY0      0
#define RELAY1      1
#define RELAY2      2
#define RELAY3      3

uint port = 8888;
WiFiServer server(port);
WiFiClient host;

const char* ssid = "BenPhone";
const char* password = "rocko2323";

// list of states, the number given to a state correlates by index to a meaning in this array
const String states[4] = {"BOOT", "WIFI_CONNECT", "TCP_ENABLED", "OPERATIONAL"};
// list of possible commands during OPERATIONAL state, should compare input to commands when deciding response
const String cmds[4] = {"set_wifi", "on", "off", "reboot"};

// glolbal variables
int   state = 0;
int   buf_cur = 0;
int   iter;
char  rx_buf[BUF_SIZE];
char  rx_msg[BUF_SIZE];
char  ip_arr[13];
char  in_byte;
long  rx_time;


// function prototypes for TCP communication
int init_tcp_connect();
char recv_msg();

// function prototypes for LCD interface
void init_lcd();
void clear_screen();
void print_all(char* data, bool clear_scr = true);    // print to all screen from start, set whether to clear rest of screen
void println1(char* data, bool keepln2 = true);       // print to line 1, set whether to overwrite the 2nd line
void println2(char* data, bool keepln1 = true);       // print the line 2, set whether to overwrite the 1st line
void setBacklight(uint8_t R, uint8_t G, uint8_t B);   // set background color using RGB values
void IpAddress2String(char* ip,const IPAddress& ipAddress); // convert IP address to a char array


// data for lcd using SPI, DO NOT EDIT OUTSIDE OF 
SPIClass * vspi = NULL;
char line1[SCR_WIDTH];
char line2[SCR_WIDTH];
uint8_t ln1_len = 0;
uint8_t ln2_len = 0; 
char cur_color = 0;

void setup() {
  // connect to wifi network
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  print_all((char*)"Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to Wifi network: ");
    Serial.println(ssid);
  }

  state++;
  Serial.println("WiFi Connected!");

  // get ip
  IpAddress2String(ip_arr, WiFi.localIP());
  
  // set up a server for tcp communication
  server.begin();
  Serial.println("Server set up.");
  Serial.print("IP after server init:");
  Serial.println(WiFi.localIP());

  pinMode(RELAY0, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  digitalWrite(RELAY0,HIGH);
  digitalWrite(RELAY1,HIGH);
  digitalWrite(RELAY2,HIGH);
  digitalWrite(RELAY3,HIGH);
}


void loop() {
  
  println1(ip_arr);
  println2((char*)"8888");
  
  while (state < 2) {
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


  // listen for msg from client while a connection is maintained
  /////// EDIT THIS WHILE LOOP FOR TESTING
  while (host.connected()) {
    // first get string msg
    char msg_type = recv_msg();
    
    if (msg_type == CMD_MSG || msg_type == CONFIG_MSG){
      strcpy(rx_msg, rx_buf);
      
      // activate the relay - active low
      if(rx_msg[0] == '1')
        digitalWrite((int)(rx_msg[1] - '0'), LOW);

      // deactivate relay
      else if(rx_msg[0] == '0')
        digitalWrite((int)(rx_msg[1] - '0'), HIGH);
    }
      
    else if (msg_type == TIME_MSG)
      rx_time = atoi(rx_buf);
    else{
      Serial.print("unknown msg: ");
      Serial.println(rx_buf);
    }

    Serial.print("msg: ");
    Serial.println(rx_msg);
    Serial.print("time: ");
    Serial.println(rx_time);
    delay(1);
  }

  Serial.print("UNDEFINED: ");
  Serial.println(state);

  host.stop();
  state = 1;
}


/*
   DESC: Function abstracts reciving msgs from the the client. Will populate global rx_buf
   This function terminates and returns when rx_buf is overflowed or found the 'E' terminating
*/
char recv_msg() {
  char in_byte;
  char msg_type = '\0';
  buf_cur = 0;
  
  // check that host is connected
  while (host.connected()) {
    // check that a new msg is available
    if (host.available()) {
      // check buffer overflow
      if (buf_cur >= BUF_SIZE) {
        Serial.println("RX BUFFER OVERFLOW");
        break;
      }
      // read one byte at a time
      in_byte = host.read();

      if (in_byte == TIME_MSG || in_byte == CMD_MSG || in_byte == CONFIG_MSG)
        msg_type = in_byte;
      
      // if terminated add the null terminator to the buf and return
      else if (in_byte == END_MSG) {
        rx_buf[buf_cur++] = '\0';
        break;
      }
      // else add to the buffer
      else
        rx_buf[buf_cur++] = in_byte;
    }
    // delay before next check to avoid crashes
    else
      delay(1);
  }
  return msg_type;
}


/* 
 *  function initialies the SPI object to transmit to LCD
 *  Uses global vspi pointer and predefined pins instead of default SPI
 *  Also sets the SS - Slave select at the predefined pin as OUTPUT
 */
void init_lcd(){
  vspi = new SPIClass(VSPI);
  vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS); 
  pinMode(VSPI_SS, OUTPUT); // set Slave Select as output
  digitalWrite(VSPI_SS, LOW); //Drive the CS pin low to select OpenLCD
  vspi->transfer(0x04); // set width to 16
  vspi->transfer(0x06); // set lines to 2
  digitalWrite(VSPI_SS, HIGH); //Release the CS pin to de-select OpenLCD
  vspi->endTransaction();
}

// function clears the entire screen and sets cursor to 0
void clear_screen(){
  digitalWrite(VSPI_SS, LOW); //Drive the CS pin low to select OpenLCD
  // need to let LCD move to setting mode before clearing screen, hence the delay
  vspi->transfer('|'); //Put LCD into setting mode
  delay(1);
  vspi->transfer('-'); //Send clear display command
  delay(1);
  digitalWrite(VSPI_SS, HIGH); //Release the CS pin to de-select OpenLCD
  //vspi->endTransaction();
}


//Sends a string over SPI to the first line, by defualt keeps line 2
void println1(char* data, bool keepln2){
  uint8_t len;
  int i;
  bool new_data = false;
  
  clear_screen();
  delay(1);
  
  // find length of string while also checking if string is equivalent
  for(len = 0; len < 16; len++){
    if (data[len] != line1[len])
      new_data = true;
    if (data[len] == '\0')
      break;
  }

  if (!new_data)
    return;
  
  ln1_len = len;
  
  digitalWrite(VSPI_SS, LOW); //Drive the CS pin low to select OpenLCD
  
  for(i = 0 ; i < len ; i++){ //Send chars until we hit the end of the string
    vspi->transfer(data[i]);
    line1[i] = data[i];
    delay(1);
  }
  
  if (keepln2){
    for(i = ln1_len; i < SCR_WIDTH; i++)
        vspi->transfer(' ');
    for(i = 0; i < ln2_len; i++)
        vspi->transfer(line2[i]);
    }
 
  digitalWrite(VSPI_SS, HIGH); //Release the CS pin to de-select OpenLCD
  //vspi->endTransaction();
}

// write string at line 2, by defualt keep line 1
void println2(char* data, bool keepln1){
  uint8_t len;
  int i;
  bool new_data = false;
  
  clear_screen();

  // get length of data 
  for(len = 0; len < 16; len++){
    if (data[len] != line2[len])
      new_data = true;
    if (data[len] == '\0')
      break;  
  }

  if (!new_data)
    return;
  
  ln2_len = len;
  
  digitalWrite(VSPI_SS, LOW); //Drive the CS pin low to select OpenLCD

  // write the first 16 bits
  if(keepln1){
    // print line
    for(i = 0 ; i < ln1_len ; i++){ //Send chars until we hit the end of the string
      vspi->transfer(line1[i]);
      delay(1);
    }
    // print spaces
    for(i = ln1_len; i < SCR_WIDTH; i++){
      vspi->transfer(' ');
      delay(1);
      }
  }
  // overwrite the first line with spaces
  else {
    for(i = 0; i < SCR_WIDTH; i++){
      vspi->transfer(' ');
      delay(1);
      }
    }
  for(i = 0 ; i < ln2_len ; i++){ //Send chars until we hit the end of the string
    vspi->transfer(data[i]);
    delay(1);
  }
  
  digitalWrite(VSPI_SS, HIGH); //Release the CS pin to de-select OpenLCD
  //vspi->endTransaction();
}

// print all string to screen, by defualt clears existing screen
void print_all(char* data, bool clear_scr){
  int len;
  
  if (clear_scr)
    clear_screen();
  
  for(len = 0; len < 32; len++)
    if (data[len] == '\0')
      break;  
  
  digitalWrite(VSPI_SS, LOW); //Drive the CS pin low to select OpenLCD
  
  for(int x = 0 ; x < len ; x++){ //Send chars until we hit the end of the string
    vspi->transfer(data[x]);
    delay(1);
  }
  
  digitalWrite(VSPI_SS, HIGH); //Release the CS pin to de-select OpenLCD
  //vspi->endTransaction();
}

// set backlight color using RGB values, 0 - 255 for each color
void setBacklight(uint8_t R, uint8_t G, uint8_t B){
  
  // enable CS
  digitalWrite(VSPI_SS, LOW); //Drive the CS pin low to select OpenLCD

  // command tells LCD to expect RGB
  vspi->transfer('|');
  delay(1);
  
  vspi->transfer('+');
  delay(1);

  // send RGB
  vspi->transfer(R);
  delay(1);
  vspi->transfer(G);
  delay(1);
  vspi->transfer(B);
  delay(1);

  // end communication
  digitalWrite(VSPI_SS, HIGH); //Release the CS pin to de-select OpenLCD
  vspi->endTransaction();
  return;
}

void IpAddress2String(char* ip, const IPAddress& ipAddress)
{
  String temp =  "IP:" + String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3]);

  for(int i = 0; i < 13; i++)
    ip[i] = temp[i];
}
