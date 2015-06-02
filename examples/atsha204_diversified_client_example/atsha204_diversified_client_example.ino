/* ATSHA204 Library Simple Example
 by: Jim Lindblom
 SparkFun Electronics
 date: November 8, 2012
 
 This code shows how to wake up and verify that an SHA204 is
 connected and operational. And how to obtain an SHA204's unique serial
 number, and send it a MAC challenge.
 
 The ATSHA204's SDA pin can be connected to any of the Arduino's digital pins.
 When constructing your atsha204Class, pass the constructor the pin you want to use.
 In this example we'll attach SDA to pin 7.
 
 The ATSHA204 can be powered between 3.3V and 5V.
 */
#include <sha204_library.h>

#define SHA204_KEY_CHILD 10
#define SHA204_KEY_PARENT 13
#define MAC_MODE_CHALLENGE              ((uint8_t) 0x00)

const int sha204Pin = 7;

atsha204Class sha204(sha204Pin);
atsha204Class sha204_1(9);

void setup()
{
  Serial.begin(9600);
   //Serial1.begin(19200);
  //Serial.println("Sending a Wakup Command. Response should be:\r\n4 11 33 43:");
  //Serial.println("Response is:");
  wakeupExample();
  //Serial.println();
  //Serial.println("Asking the SHA204's serial number. Response should be:");
  //Serial.println("1 23 x x x x x x x EE");
  //Serial.println("Response is:");
  //serialNumberExample();
  //Serial.println();
  //Serial.println("Sending a MAC Challenge. Response should be:");
  //Serial.println("23 6 67 0 4F 28 4D 6E 98 62 4 F4 60 A3 E8 75 8A 59 85 A6 79 96 C4 8A 88 46 43 4E B3 DB 58 A4 FB E5 73");
  //Serial.println("Response is:");
  macChallengeExample();
 
}

void loop()
{
}

byte wakeupExample()
{
  uint8_t response[SHA204_RSP_SIZE_MIN];
  byte returnValue;

  returnValue = sha204.sha204c_wakeup(&response[0]);
  /*for (int i=0; i<SHA204_RSP_SIZE_MIN; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
*/
  memset(response, 0, sizeof(response));
  returnValue = sha204_1.sha204c_wakeup(&response[0]);
 /* for (int i=0; i<SHA204_RSP_SIZE_MIN; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
*/

  return returnValue;
}

byte serialNumberExample()
{
  uint8_t serialNumber[9];
  byte returnValue;

  returnValue = sha204.getSerialNumber(serialNumber);
  for (int i=0; i<9; i++)
  {
    Serial.print(serialNumber[i], HEX);
    Serial.print(" ");
  }
  Serial.println(); 

  memset(serialNumber, 0, sizeof(serialNumber));
  returnValue = sha204_1.getSerialNumber(serialNumber);
  for (int i=0; i<9; i++)
  {
    Serial.print(serialNumber[i], HEX);
    Serial.print(" ");
  }
  Serial.println(); 

  return returnValue;
}

byte macChallengeExample()
{
  uint8_t command[MAC_COUNT_LONG];
  //uint8_t response[35];
  uint8_t response_status[SHA204_RSP_SIZE_MIN];
//  uint8_t randomnumber[35];
  uint8_t response_random[RANDOM_RSP_SIZE];
  
  memset(response_random, 0, sizeof(response_random));
  uint8_t *randomnumber = &response_random[SHA204_BUFFER_POS_DATA];

  uint8_t command_derive_key[GENDIG_OTHER_DATA_SIZE];
  uint8_t command_mac[CHECKMAC_CLIENT_COMMAND_SIZE];
  // MAC response buffer
  uint8_t response_mac[SHA204_RSP_SIZE_MAX];
  
  //check mac
  uint8_t checkmac_other_data[CHECKMAC_OTHER_DATA_SIZE];

  //serial number
  uint8_t serial_numb[9];
  
//uint8_t ret_code = sha204.sha204e_configure_key();
  //ret_code = sha204_1.sha204e_configure_diversify_key();
  //Serial.println(ret_code);
  /*for (int i=0; i<SHA204_RSP_SIZE_MAX; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
    */
   //Read Config Zone
   uint8_t config_data[88];
  // Serial.println("Config Zone: Client");  
   sha204_1.sha204e_read_config_zone(config_data);
  /* for (int i=0; i<88; i++)
   {
   Serial.print(config_data[i], HEX);
   Serial.print(' ');
   }
   Serial.println();
   */
   uint8_t derive_key_command[] = {0x1C, 0x04, 0x0A, 0x00};
   //---------------------------------------------------------------------//
   //---------------- Configure the derive key -------------------------- //
   // ------------------------------------------------------------------- //
   
   //-------------------------SERIAL NUMBER--------------------------------//
   //------------get the serial number and pad it with zeros ------------- //
   
   
  sha204_1.getSerialNumber(serial_numb);
  
  uint8_t temp[NONCE_NUMIN_SIZE_PASSTHROUGH];
  
  memset(temp, 0x65 , sizeof(temp));
  for (int i=0; i<9; i++)
  {
    temp[i] = serial_numb[i];
  }
  /*
  //print it
  Serial.println("Serial Number:");
  for (int i=0; i<32; i++)
  {
    Serial.print(temp[i], HEX);
    Serial.print(" ");
  }
  Serial.println(); 
  */
   // ----------------------- DIVERSIFIED KEY -----------------------------//
   // ---------- create a diversified key on the client device ----------- //
  
  //  Put padded serial number into TempKey (fixed Nonce).
  uint8_t ret_code_t = sha204_1.sha204m_nonce(command, response_status, NONCE_MODE_PASSTHROUGH, temp);
  if (ret_code_t != SHA204_SUCCESS) {
  	return ret_code_t;
  }
  	
  //  Send DeriveKey command.
  ret_code_t = sha204_1.sha204m_derive_key(command, response_status, DERIVE_KEY_RANDOM_FLAG, SHA204_KEY_CHILD, NULL);
  
  // --------------------------------------------------------------------- //
  // Send the serial number ---------------------------------------------- //
  // --------------------------------------------------------------------- //
  
  
  //send serial number over serial
 // for(int i = 0; i < NONCE_NUMIN_SIZE_PASSTHROUGH; i++)
 // {
 //   Serial.write(temp[i]);
 // }
  int bytes_sent = Serial.write(temp, NONCE_NUMIN_SIZE_PASSTHROUGH);
  
  if(bytes_sent != NONCE_NUMIN_SIZE_PASSTHROUGH)
  {
    Serial.println("Error sending random number");
    return -1;
  }
  //----------------------------------------------------------------------// 
  // Lock Config Zone : Danger make sure you configure everything before--// 
  // uint8_t ret_code_lock =  sha204.sha204e_lock_config_zone();----------//
  //----------------------------------------------------------------------// 

  // --------------------------------------------------------------------- //
  // Recieve Random Number ----------------------------------------------- //
  // generated a random number on the host ------------------------------- //
  // --------------------------------------------------------------------- //
  // loop and wait untill serial data is available
 // Serial.println("Waiting to recieve random number");
  while(!Serial.available()){} 
  Serial.flush();
  while(!Serial.available()){} 
  memset(randomnumber, 0, sizeof(randomnumber));
  int bytes_read = Serial.readBytes((char *)randomnumber, NONCE_NUMIN_SIZE_PASSTHROUGH);
  
  if(bytes_read != NONCE_NUMIN_SIZE_PASSTHROUGH)
  {
    Serial.println("Error Receiving Serial");
    return -1;
  }
  
  //uint8_t temp_key[NONCE_NUMIN_SIZE_PASSTHROUGH];
  
  /*
  Serial.println("Random Number Recieved:");
  for (int i=0; i<NONCE_NUMIN_SIZE_PASSTHROUGH; i++)
  {
    Serial.print(randomnumber[i], HEX);
    Serial.print(" ");
  }
  Serial.println(); 
  */
  
  // -------------------------------------------------------------- //
  // ------------------- MAC -------------------------------------- //
  // ----- issue a mac command on the client ---------------------- //
  // -------------------------------------------------------------- //
  
  memset(command, 0, sizeof(command));
  memset(response_mac, 0, sizeof(response_mac));
  uint8_t ret_code_mac = sha204_1.sha204m_mac(command, response_mac, MAC_MODE_CHALLENGE, SHA204_KEY_CHILD, randomnumber);
  if (ret_code_mac != SHA204_SUCCESS) {
  	//sha204e_sleep();
  	return ret_code_mac;
  }
  
  
  
  // send the mac to host
   bytes_sent = Serial.write(&response_mac[SHA204_BUFFER_POS_DATA], SHA204_RSP_SIZE_MAX - SHA204_BUFFER_POS_DATA);
  
  if(bytes_sent != SHA204_RSP_SIZE_MAX - SHA204_BUFFER_POS_DATA)
  {
    Serial.println("Error sending response mac");
    return -1;
  }

   //output
  Serial.println("MAC");
  for (int i=SHA204_BUFFER_POS_DATA; i<SHA204_RSP_SIZE_MAX; i++)
  {
    Serial.print(response_mac[i], HEX);
    Serial.print(' ');
  }
  Serial.println(); 
  

  delay(1000);
  return 0;
}
