
bool obdInit() {
  int attemptCount = 0;
  obd.begin();
  while (attemptCount < 3 && !obd.init()) {
    attemptCount++;
    #ifdef ENABLE_DEBUG
    Serial.print("attempts: ");
    Serial.println(attemptCount);
    #endif
  }
  //return true should be in above loop
  if (attemptCount >= 2) return false;
  else return true;
}

void readOBD()
{
  strcpy(dataBuff," "); //just to initialize the buffer for strcat
  
  for (int i = 0; i < (sizeof(pidIndex) / sizeof(pidIndex[0])); i++) {  //SIZEOF HERE IS INCORRECTLY USED
    //read each item listed in pidIndex array
    readPID(pidIndex[i]);   
  }
  
  //Should really check if speed is zero here, to only need to use one write
  
  //logData(dataBuff);
  //delay(50); //do I need this delay? or is logData a blocking function?
 
  //memset(dataBuff, 0, 64);

}

void readPID(int pidIndex)
{
  uint8_t pid = modePids[pidIndex];
  int value;
  static char buf[8];
  
  if(!obd.read(pid, value)) {
    logInfo("F");
    //ADD SOMETHING HERE TO SET VALUES TO 0 OR ERROR OF SOME KIND
    return;
  } else {
      switch (pidIndex) {    
        case 0:  //PID_RPM, 
          sprintf(buf, modeFmts[pidIndex], value);
          vehicleData.rpmVal = value;    
        break;
        
        case 1:  //PID_SPEED,
          value = value * .62137;
          sprintf(buf, modeFmts[pidIndex], (unsigned int)(value));  
          vehicleData.spdVal = value;
   
          if (vehicleData.spdVal > 0 && movingTimeStamp > stoppedTimeStamp) {
          //You have been and still are moving          
          //add time to movingTimer
            vehicleData.totalMovingTime = vehicleData.totalMovingTime + (millis() - movingTimeStamp);
          } else if (vehicleData.spdVal > 0 && stoppedTimeStamp > movingTimeStamp) {
          //You were stopped but you just now started moving
          //could further refine within this statement to see if you are still moving very slowly, i.e. taking
          //turns at a stop sign or something
        
          //stop stoppedTimer and start movingTimer
            vehicleData.totalIdleTime = vehicleData.totalIdleTime + (millis() - stoppedTimeStamp);
          
          } else if (vehicleData.spdVal == 0 && stoppedTimeStamp > movingTimeStamp) {
          //You are stopped and have been stopped 
          
          //add time to stoppedTimer - this method currently skips the first cycle, but could
          //set stoppedTimeStamp to millis() immediately upon engine start, before entering this fn
            vehicleData.totalIdleTime = vehicleData.totalIdleTime + (millis() - stoppedTimeStamp);
          
          } else if (vehicleData.spdVal == 0 && movingTimeStamp > stoppedTimeStamp) {
          //You were moving, but you just now stopped        
          //stop movingTimer and start stoppedTimer
            vehicleData.totalMovingTime = vehicleData.totalMovingTime + (millis() - movingTimeStamp);
          }
          
          if (vehicleData.spdVal > 0) movingTimeStamp = millis();
          else stoppedTimeStamp = millis();
          
        break;  
        
        case 2:  //PID_THROTTLE,
          sprintf(buf, modeFmts[pidIndex], value);    
        break;
        
        case 3:  //PID_ENGINE_LOAD, 
          sprintf(buf, modeFmts[pidIndex], value);    
        break;   
        
        case 4:  //PID_COOLANT_TEMP,
          sprintf(buf, modeFmts[pidIndex], (unsigned int)((value*1.8)+32));      
        break;   
        
        case 5:  //PID_INTAKE_TEMP
          sprintf(buf, modeFmts[pidIndex], (unsigned int)((value*1.8)+32));  
          iatVal = value + 273.15;      
        break;
        
        case 6:  //PID_AMBIENT_TEMP 
          sprintf(buf, modeFmts[pidIndex], (unsigned int)((value*1.8)+32));      
        break;  
        
        case 7:   //PID_MAF_FLOW
          sprintf(buf, modeFmts[pidIndex], value);    
        break;  
        
        case 8:  //PID_ABS_ENGINE_LOAD
          sprintf(buf, modeFmts[pidIndex], value);    
        break;
        
        case 9:  //PID_FUEL_PRESSURE 
          sprintf(buf, modeFmts[pidIndex], value);     
        break;  
        
        case 10:  //PID_INTAKE_PRESSURE
          sprintf(buf, modeFmts[pidIndex], value); 
          mapVal = value;   
        break;  
        
        case 11:  //PID_BAROMETRIC
          sprintf(buf, modeFmts[pidIndex], value);    
        break;
        
        case 12:  //PID_TIMING_ADVANCE 
          sprintf(buf, modeFmts[pidIndex], value);    
        break; 
        
        case 13:  //PID_FUEL_LEVEL
          sprintf(buf, modeFmts[pidIndex], value);    
        break;  
        
        case 14:  //PID_RUNTIME   
          sprintf(buf, modeFmts[pidIndex], (unsigned int)value / 60, (unsigned int)value % 60); 
        break;  
        
        case 15:  //PID_DISTANCE
          sprintf(buf, modeFmts[pidIndex], (unsigned int)(value*.622));
        break;
        
        default:    
          sprintf(buf, modeFmts[pidIndex], value);
        break;
      }
    #ifdef ENABLE_LOG
    strcat(dataBuff, buf);  
    strcat(dataBuff,","); 
    #endif    
  }
}

void vehicleStatus()
{
  //check to see if obd is active
  //just check for engine running for now, but add obd status check also
  if (vehicleData.rpmVal > 0) {
    vehicleData.engineStatus = true;
    vehicleData.runTime = (millis() / 1000);
  }
  else vehicleData.engineStatus = false;
  
  
}
