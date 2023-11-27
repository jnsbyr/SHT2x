//
//    FILE: SHT2x_demo_async_fast_HT.ino
//  AUTHOR: JensB
// PURPOSE: demo asynchronous interface
//     URL: https://github.com/RobTillaart/SHT2x
//
// - needs version 0.4.2 or higher
//
// - saves separate conversion cycle for the temperature
//   by reading back the temperature that was established
//   with the last humidity measurement
// 


#include "SHT2x.h"

SHT2x sht;


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("SHT2x_LIB_VERSION: \t");
  Serial.println(SHT2x_LIB_VERSION);

  // connect to sensor
  Wire.begin();
  while (!sht.isConnected()) 
  {
    Serial.print("*");
    delay(1);
  }
  Serial.println();

  // soft reset sensor
  sht.reset();
  delay(5); // ~5 ms for soft reset to complete
  while (!sht.isConnected()) 
  {
    Serial.print("*");
    delay(1);
  }
  Serial.println();

  // change resolution to 11 bits
  if (!sht.setResolution(3)) // 3: 11 bits / 0.08 °C / 0.05 % / ~18 ms - see datasheet
  {
    Serial.print("set resolution error:\t");
    Serial.println(sht.getError());      
  }
}


void loop()
{
  uint32_t from = millis();
  
  if (sht.isConnected())
  {
    // async request humidity and wait for completion
    sht.requestHumidity();  
    int available = 20; // [ms] choose a value that is at least 10 % higher that the measurement time from the datasheet
    while (!sht.reqHumReady() && (available-- > 0))
    {
      delay(1); // or do something else here
    }

    // retrieve humidity and temperature values
    if (available)
    {
      if (!sht.readHumidity())
      {
        Serial.print("read humidity error:\t");
        Serial.println(sht.getError());  
      }
      else
      {
        if (!sht.readTemperatureForHumidity())
        {
          Serial.print("read temp error:\t");
          Serial.println(sht.getError());  
        }   
      }
    
      Serial.print("TEMP:\t");
      Serial.println(sht.getTemperature(), 1);
      Serial.print("HUMI:\t");
      Serial.println(sht.getHumidity(), 1);
    }
    else
    {
      Serial.println("requesting humidity failed");    
    }
    
    uint32_t now = millis();
    Serial.print("TIME:\t");
    Serial.println(now - from);  
    Serial.println();  
  }
  else
  {
    Serial.println("sensor not connected");    
  }
  
  // do other things here
  delay(1000);
}


//  -- END OF FILE --
