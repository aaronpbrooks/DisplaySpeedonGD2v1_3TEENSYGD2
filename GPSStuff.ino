

void readGPS(boolean saveData)
{ 
  unsigned long startTime = millis();
  
  while (Serial3.available() > 0 && millis() - startTime < 500) { //changed from (Serial3.available() > 0 && millis() - start < 100)
  
    if (gps.encode(Serial3.read())) {
      if (saveData == true) {
        Serial.println("Going to handle GPS data");
        handleGPSData();
        break;  //<-- Added break statement to break after successful read is this in the right place?
        Serial.println("Went past the break line - fix it!");
      }
      else {
        #ifdef ENABLE_DEBUG
        Serial.println("GPS Read (false)");
        #endif
        break;
      }
    } else {
      #ifdef ENABLE_DEBUG
      Serial.println("Didn't encode GPS");
      #endif
    } 
    
  }
  
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS detected: check wiring.");
    //while(true);
  }
  
    
}

void handleGPSData()
{
 //strcpy(gpsBuf," "); //just to initialize the buffer for strcat
  if (gps.location.isUpdated()) { 
    static char latBuf[12];   
    static char lonBuf[12];
    if (gps.location.isValid()) {
    
      dtostrf(gps.location.lat(), 10, 6, latBuf);
      dtostrf(gps.location.lng(), 10, 6, lonBuf);
      //sprintf(gpsBuf, "%5.5f", gps.location.lat());
      //sprintf(lonBuf, "%5.5f", gps.location.lng());
      
      strcat(dataBuff, ",");
      
      #ifdef ENABLE_DEBUG
      Serial.print(millis());
      #endif
      
      strcat(dataBuff, latBuf);
      strcat(dataBuff, ",");
      strcat(dataBuff, lonBuf);
    } else {
      #ifdef ENABLE_DEBUG
      Serial.println("GPS INVALID");
      #endif
  }
  
  #ifdef ENABLE_DEBUG
  Serial.println(dataBuff);
  #endif
  //delay(5);
  //logGPSData(); 
  //logData(dataBuff);
  //delay(50); //do I need this delay? or is logData a blocking function?
 } else {  
  //logData(dataBuff); 
   strcat(gpsBuf, "Not shit from GPS");  
 }
 
  //memset(dataBuff, 0, 72);
  //memset(gpsBuf, 0, 48);  

}

static void feedGPS(unsigned long ms)
{ //This section could update lat and lon variables whenever something is available, or lat and long can
  //call this function only when they are ready to be set
  unsigned long start = millis();
  do 
  {
    while (Serial3.available())
      gps.encode(Serial3.read());
  } while (millis() - start < ms); //change this to "if" and have it retire early with break?
}

void getLocation()
{
  if (gps.location.isValid()) {
    vehicleData.latLocation = gps.location.lat();
    vehicleData.lonLocation = gps.location.lng();  
  } else {
      
  }
  feedGPS(0);
}
/*
(gps.location.lat()); // Latitude in degrees (double)
(gps.location.lng()); // Longitude in degrees (double)
(gps.location.rawLatDegrees()); // Raw latitude in whole degrees
(gps.location.rawLatBillionths()); // ... and billionths (i16/u32)
(gps.location.rawLngDegrees()); // Raw longitude in whole degrees
(gps.location.rawLngBillionths()); // ... and billionths (i16/u32)
(gps.date.value()); // Raw date in DDMMYY format (u32)
(gps.date.year()); // Year (2000+) (u16)
(gps.date.month()); // Month (1-12) (u8)
(gps.date.day()); // Day (1-31) (u8)
(gps.time.value()); // Raw time in HHMMSSCC format (u32)
(gps.time.hour()); // Hour (0-23) (u8)
(gps.time.minute()); // Minute (0-59) (u8)
(gps.time.second()); // Second (0-59) (u8)
(gps.time.centisecond()); // 100ths of a second (0-99) (u8)
(gps.speed.value()); // Raw speed in 100ths of a knot (i32)
(gps.speed.knots()); // Speed in knots (double)
(gps.speed.mph()); // Speed in miles per hour (double)
(gps.speed.mps()); // Speed in meters per second (double)
(gps.speed.kmph()); // Speed in kilometers per hour (double)
(gps.course.value()); // Raw course in 100ths of a degree (i32)
(gps.course.deg()); // Course in degrees (double)
(gps.altitude.value()); // Raw altitude in centimeters (i32)
(gps.altitude.meters()); // Altitude in meters (double)
(gps.altitude.miles()); // Altitude in miles (double)
(gps.altitude.kilometers()); // Altitude in kilometers (double)
(gps.altitude.feet()); // Altitude in feet (double)
(gps.satellites.value()); // Number of satellites in use (u32)
(gps.hdop.value()); // Horizontal Dim. of Precision (100ths-i32)
*/
