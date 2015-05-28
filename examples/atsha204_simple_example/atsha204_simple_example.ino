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

const int sha204Pin = 7;

atsha204Class sha204(sha204Pin);

void setup()
{
  Serial.begin(9600);
  Serial.println("Sending a Wakup Command. Response should be:\r\n4 11 33 43:");
  Serial.println("Response is:");
  wakeupExample();
  Serial.println();
  Serial.println("Asking the SHA204's serial number. Response should be:");
  Serial.println("1 23 x x x x x x x EE");
  Serial.println("Response is:");
  serialNumberExample();
  Serial.println();
  Serial.println("Sending a MAC Challenge. Response should be:");
  Serial.println("23 6 67 0 4F 28 4D 6E 98 62 4 F4 60 A3 E8 75 8A 59 85 A6 79 96 C4 8A 88 46 43 4E B3 DB 58 A4 FB E5 73");
  Serial.println("Response is:");
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
  for (int i=0; i<SHA204_RSP_SIZE_MIN; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
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
  
  return returnValue;
}

byte macChallengeExample()
{
  uint8_t command[MAC_COUNT_LONG];
  uint8_t response[35];
  uint8_t randomnumber[35];

  const uint8_t challenge[MAC_CHALLENGE_SIZE] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };
   
  // Lock Config Zone : Danger make sure you configure everything before 
  //uint8_t ret_code_lock =  sha204.sha204e_lock_config_zone();
  // 
 
  //generate a random number
  uint8_t ret_code_random_number = sha204.sha204m_random(&command[0], &response[0], RANDOM_SEED_UPDATE);
  
  //sha204.sha204m_execute(SHA204_RANDOM, RANDOM_SEED_UPDATE, 0, 0, 
   //   NULL, 0, NULL, 0, NULL, sizeof(command), &command[0], 
    //  32*sizeof(uint8_t), &response[0]);
  
  for (int i=1; i<21; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(' ');
    randomnumber[i-1] = response[i];
  }
  Serial.println();
  
  
   for (int i=0; i<SHA204_RSP_SIZE_MAX; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
 
  // generate the nonce 
  uint8_t ret_code_nonce = sha204.sha204m_execute(SHA204_NONCE, NONCE_MODE_SEED_UPDATE, 0, NONCE_NUMIN_SIZE,
      (uint8_t *) randomnumber, 0, NULL, 0, NULL, sizeof(command), &command[0], 
      sizeof(response), &response[0]);
  
  for (int i=0; i<SHA204_RSP_SIZE_MAX; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
 
  // generate the gendig
  

  // get the mac
  uint8_t ret_code = sha204.sha204m_execute(SHA204_MAC, 0x01, 0x0000, 0, 
    NULL, 0, NULL, 0, NULL, sizeof(command), &command[0], 
    sizeof(response), &response[0]);
   
  //
  Serial.print(ret_code);
  Serial.print(' ');
  for (int i=0; i<SHA204_RSP_SIZE_MAX; i++)
  {
    Serial.print(response[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  
  
  //uint8_t returnCode1 = sha204.sha204e_configure_derive_key();
  //try a read
  uint8_t readCommand[READ_COUNT];
  uint8_t readResponse[READ_32_RSP_SIZE];
	
 
  memset(readResponse, 0, sizeof(readResponse));
  uint8_t returnCode = sha204.sha204m_read(readCommand, readResponse, SHA204_ZONE_COUNT_FLAG | SHA204_ZONE_CONFIG , 32);
  Serial.println();
  for (int i=0; i<READ_32_RSP_SIZE; i++)
  {
    Serial.print(readResponse[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  
  uint8_t config_data[88];
  
  sha204.sha204e_read_config_zone(config_data);
  
   for (int i=0; i<88; i++)
  {
    Serial.print(config_data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  
  return 0;
}



 /* read from bytes 0->3 of config zone 
  //uint8_t returnCode = sha204.sha204m_read(readCommand, readResponse, SHA204_ZONE_COUNT_FLAG | SHA204_ZONE_DATA , 0x0140);
  //Serial.println();
  //Serial.println(returnCode);
 // for (int i=0; i<READ_32_RSP_SIZE; i++)
 // {
 //   Serial.print(readResponse[i], HEX);
 //   Serial.print(' ');
 // }
 // Serial.println();
  
  uint8_t data_load[SHA204_ZONE_ACCESS_32];
  
  // Write client configuration.
  memcpy(data_load, &readResponse[SHA204_BUFFER_POS_DATA], sizeof(data_load));
  
  for (int i=0; i<SHA204_ZONE_ACCESS_32; i++)
  {
    Serial.print(data_load[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  
  */
  
  /*
 // data_load[0] = 0x8F;
    
  data_load[1] = 0x8F;
  data_load[2] = 0x8F;
  data_load[3] = 0x8F;
  */
  /*
  //write the config
  uint8_t writeCommand[WRITE_COUNT_LONG];
  uint8_t writeResponse[WRITE_RSP_SIZE];
  
  const uint8_t write_field[4] = {0x8F, 0x8F, 0x9F, 0x8F};
  
  //get the mac
  //uint8_t ret_code_write = sha204.sha204m_execute(SHA204_WRITE, 0x00, 20, 2,
  //  (uint8_t *)write_field, 0, NULL, 0, NULL, sizeof(writeCommand), &writeCommand[0], 
  //  sizeof(writeResponse), &writeResponse[0]);
  
  uint8_t ret_code_write = sha204.sha204m_write(writeCommand, writeResponse,  SHA204_ZONE_CONFIG, 24, (uint8_t *)write_field, NULL);
  Serial.println();  
  Serial.println(ret_code_write);
  Serial.println();
  for (int i=0; i<WRITE_RSP_SIZE; i++)
  {
    Serial.print(writeResponse[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  */
  /* read from bytes 0->3 of config zone */
 // returnCode = sha204.sha204m_read(readCommand, readResponse, SHA204_ZONE_COUNT_FLAG | SHA204_ZONE_CONFIG , 20);
 // Serial.println();
 //  for (int i=0; i<READ_4_RSP_SIZE; i++)
 // {
 //   Serial.print(readResponse[i], HEX);
 //   Serial.print(' ');
 // }
 // Serial.println();
  

