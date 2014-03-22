//TODO:
//- Don't create log file for every trip, just keep one that covers all trips?
//- Log file can then contain meta data for each of the trips, used to display prev averages, # trips, etc
//- Add function to log a shutdown messsage when power off is detected
//- Test w/update to SDFat library for improvements in efficiency
//- When power supply is done, have filename change at completion of trip (normal shutdown)


bool initSD()
{ 
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
    //sd.initErrorHalt()?
    #ifdef ENABLE_DEBUG
    Serial.println("SD Card Initialization failed");
    #endif
    return false;
  }
  
  if (sd.exists("logs/")) { 
   Serial.println("found logs directory"); 
  } else { 
    
   sd.mkdir("logs");
  }  
  
  if (sd.exists("trips/")) { 
  } else {
   sd.mkdir("trips");
  }
 
  //Create Log File
  char fExt[] = ".txt";
  char logFilePrefix[] = "logs/LOG";

  for (uint8_t i = 0; i < 100; i++) {    
    char tmpLogPath[18];
    sprintf(tmpLogPath, "%s%d%s",logFilePrefix,i,fExt);

    if(sd.exists(tmpLogPath)) {
      //File already exists, move on to next
      Serial.println(tmpLogPath);
    } else {
      //Filename is available - use it
      memcpy(logName, tmpLogPath, sizeof(tmpLogPath));
      break;
    }
  }
  
  if (!logFile.open(logName, O_RDWR | O_CREAT | O_AT_END)) {
    //error creating the file
    Serial.println("Error creating log file");
    return false;
  } else {
    //Successfully created logfile
    Serial.println("Created logfile");
    logFile.close();      
  }

  //Create Trip File
  char tripFilePrefix[] = "trips/TRIP";
  //char finalFilePath[16];
  for (uint8_t i = 0; i < 100; i++) {    
    char tmpFilePath[16];
    sprintf(tmpFilePath, "%s%d%s",tripFilePrefix,i,fExt);

    if(sd.exists(tmpFilePath)) {
      Serial.println(tmpFilePath);
      //File already exists, move on to next
    } else {
      //Filename is available - use it
      memcpy(tripName, tmpFilePath, sizeof(tmpFilePath));
      break;
    }      
  }
  
  if (!tripFile.open(tripName, O_RDWR | O_CREAT | O_AT_END)) {
    //error creating the file
        //error creating the file
    Serial.println("Error creating trip file");
    return false;
    //logInfo("Trip file initialization failed");
  } else {
    //Successfully created file
    Serial.println("Created tripfile");
    tripFile.close();  
    logInfo("Trip file created: "); 
    logInfo(tripName);   
  }
  
  //Don't need below line, because errors would have returned already
  //if(tripFileInitialized == true && logFileInitialized == true) 
  //{
    //Add header with OBD titles to log file
    
    char titleBuff[260];
    strcpy(titleBuff, "");
    strcat(titleBuff, "NUMREADINGS,");
    strcat(titleBuff, "FUEL(g),");
    strcat(titleBuff, "DIST(ft),");
    strcat(titleBuff, "MPG,"); 
    for (int i = 0; i < (sizeof(pidIndex) / sizeof(pidIndex[0])); i++) {
      strcat(titleBuff,pidName[(pidIndex[i])]);
      strcat(titleBuff,",");
    }
  logData(titleBuff);
  
  #ifdef DEBUG_ENABLE 
  Serial.println(titleBuff);
  #endif
  
  return true;
  //}
}

void logData(char data[])
{ 
  //File file = SD.open(tripName, FILE_WRITE);
  if (!tripFile.open(tripName, O_RDWR | O_CREAT | O_AT_END)) {
    //file.print(time_Stamp());
    tripFile.print(millis());
    tripFile.print(" ");
    //tripFile.print(" ");
    tripFile.println(data);
    tripFile.close();    
 // } else {
  }
  //memset(timeStamp,0,20);
}
/*
void logDataWithTitle(char title[], char data[])
{ 
  File file = SD.open(tripName, FILE_WRITE);
  if (file) {
    //file.print(time_Stamp());
    file.print(millis());
    file.print(" ");
    file.print(title);
    file.print(" ");
    file.println(data);
    file.close();    
  } else {
  }
  //memset(timeStamp,0,20);
}
*/

void logGPS(float gpsCoord)
{
  if(!logFile.open(tripName, O_RDWR | O_CREAT | O_AT_END)) {
    logFile.print(millis());
    logFile.print(" ");
    logFile.println(gpsCoord);
    logFile.close();
  } else {
    #ifdef DEBUG_ENABLE 
    Serial.println("Writing to log file failed");
    #endif
  }
  #ifdef DEBUG_ENABLE 
  Serial.println(gpsCoord);  
  #endif
  //memset(buffer,0,sizeof(buffer));
  
  //memset(timeStamp,0,20);
}

void logGPSData()
{
  if(!tripFile.open(tripName, O_RDWR | O_CREAT | O_AT_END)) {
    tripFile.print(millis());
    tripFile.print(" ");
    tripFile.println(gpsBuf);
    tripFile.close();
  } else {
    #ifdef DEBUG_ENABLE 
    Serial.println("Writing to data file failed");
    #endif
  }
  #ifdef DEBUG_ENABLE 
  Serial.println(gpsCoord);  
  #endif
  //memset(buffer,0,sizeof(buffer));
  
  //memset(timeStamp,0,20);
}

void logInfo(char info[])
{
  //File file = SD.open(tripName, FILE_WRITE);
  if (!logFile.open(logName, O_RDWR | O_CREAT | O_AT_END)) {
    //file.print(time_Stamp());
    logFile.print(millis());
    logFile.print(" ");
    //logFile.print(" ");
    logFile.println(info);
    logFile.close();    
 // } else {
  } 
  //memset(timeStamp,0,20);
}

void logFuel(char info[], int msrmt)
{
  if (!logFile.open(logName, O_RDWR | O_CREAT | O_AT_END)) {
    //file.print(time_Stamp());
    logFile.print(millis());
    logFile.print(",");
    logFile.print(info);
    logFile.print(":");
    logFile.println(msrmt);
    logFile.close();
  } else {
    #ifdef DEBUG_ENABLE 
    Serial.println("Writing to log file failed");
    #endif
  }
  #ifdef DEBUG_ENABLE
  Serial.print(info);
  Serial.print(": ");
  Serial.println(msrmt);
  #endif
}

void logDataBuf()
{
  
  logData(dataBuff);   

  memset(dataBuff, 0, 72);
  memset(gpsBuf, 0, 48);  
  delay(50);  
  
}

void rename()
{
  if (!logFile.open(logName, O_RDWR | O_CREAT | O_AT_END));
  if (!logFile.rename(sd.vwd(),"logs/NAME2.TXT"));
  logFile.close();
}
