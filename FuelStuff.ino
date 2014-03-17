

float cumRPM;
float cumSPD;
float cumMAP;
float cumIAT;

float totalDist = 0;
int totalIdlingTime = 0;
//float totalFuel = 0; //total to include idle fuel or moving only?
//float totalIdleTime = 0;
//float totalIdleFuel = 0;

void mpgCalc()
{
  //IS THIS INCLUDING ALL OF THE 0MPH READINGS INTO AVERAGING AFTER A STOP?
  
  //TODO:
  //- Take timestamp of readings and calculate actual time diff between previous reading and current,
  //  so that total consumption can be calculated, based on averages
  //- Same as above, timestamp to calc total distance, and compare this to other distance msrmts
  //- Add function to monitor idling time, and total fuel consumed during idle
  
  numReadings++;
  
  //add to total collected values
  cumRPM = cumRPM + vehicleData.rpmVal;      
  cumSPD = cumSPD + vehicleData.spdVal;      
  cumMAP = cumMAP + mapVal;     
  cumIAT = cumIAT + iatVal;      
      
  if(((millis() - fuelStartTime) > fuelTimer) && numReadings > 1) {
    //perform averaging functions and report fuel consumption
    fuelStartTime = millis();
    
    char fuelBuff[10];
    char readingsBuff[10];
    char rpmBuff[10];
    char spdBuff[10];
    char mapBuff[10];
    char iatBuff[10];
    
    char airMassBuff[10];
    char fuelMassBuff[10];
    char fuelVolBuff[10];
    char shortTermMPGBuff[10];
    char distEstBuff[10];
     
    float imap;
    float airMass; 
    float fuelMass;
    float fuelVol; 
    float shortTermMPG;
    
    float aveRPM;
    float aveSPD;
    float aveMAP;
    float aveIAT;
    
    aveRPM = cumRPM / numReadings;
    aveSPD = cumSPD / numReadings;
    aveMAP = cumMAP / numReadings;
    aveIAT = cumIAT / numReadings;
  /*  
    #ifdef DEBUG_ENABLE //moved from below
    sprintf(readingsBuff, "%2u", numReadings);
    strcat(readingsBuff,",");
    //logDataWithTitle("# Samples", readingsBuff); 
    logData(readingsBuff);    
    //dtostrf(val, len, prec, buffer);
    //#ifdef DEBUG_ENABLE //move this before readingsBuff log
    dtostrf(aveRPM, 8, 2, rpmBuff);
    dtostrf(aveSPD, 8, 2, spdBuff);
    dtostrf(aveMAP, 8, 2, mapBuff);
    dtostrf(aveIAT, 8, 2, iatBuff);

    logDataWithTitle("Ave RPM", rpmBuff);
    logDataWithTitle("Ave SPD", spdBuff);
    logDataWithTitle("Ave MAP", mapBuff);    
    logDataWithTitle("Ave IAT", iatBuff);  
    #endif
    */
    //reset cumulative values
    cumRPM = 0;
    cumSPD = 0;
    cumMAP = 0;
    cumIAT = 0;  

    //set number of calls to function back to 0
    numReadings = 0;
    
    imap = aveRPM * aveMAP / aveIAT / 2 * (fuelTimer / 1000);    
    airMass = imap / 60 * volEff * engDisp * mMAir / rVal;
    fuelMass = airMass / afRatio;
    fuelVol = fuelMass / gasDensity / convgtolb;    
      
    #ifdef ENABLE_DEBUG 
    dtostrf(airMass, 8, 2, airMassBuff); 
    dtostrf(fuelMass, 8, 2, fuelMassBuff);    
    dtostrf(fuelVol, 8, 5, fuelVolBuff);
      
//    logDataWithTitle("Air Mass (g)", airMassBuff);
//    logDataWithTitle("Fuel Mass (g)", fuelMassBuff);
//    logDataWithTitle("Fuel Vol (gal)", fuelVolBuff);
    //#endif  //move this statement to go after fuelMassBuff log
    
    //75ms between this log and previous logging event (readingsBuff) - needed?
    dtostrf(fuelMass, 8, 2, fuelMassBuff);
 //   logData(fuelMassBuff);
    #endif //moved from above
    
    if (aveSPD > 0) {
      //estimate distance using feet - using miles is too small a number to do accurate float maths with
      float ftPerSec = aveSPD * (5280 / 3600);     
      float distance = ftPerSec * (fuelTimer / 1000); //in feet
      shortTermMPG = distance / fuelVol; //feet per gallon
      shortTermMPG = shortTermMPG / 5280; //convert to mpg

      vehicleData.instantMPG = shortTermMPG;
      vehicleData.totalFuel = vehicleData.totalFuel + fuelVol;
      
      dtostrf(distance, 8, 5, distEstBuff);
      dtostrf(shortTermMPG, 8, 5, shortTermMPGBuff);
 //     logData(distEstBuff); //in feet
 //     logData(shortTermMPGBuff);
      
      strcat(dataBuff, distEstBuff);
      strcat(dataBuff, ",");
      strcat(dataBuff, shortTermMPGBuff);
      strcat(dataBuff, ",");
      //myData.totalIdleTime = myData.totalIdleTime;
      
      //Reset the time used for calculating averages
      prevReadTimeStamp = movingTimeStamp;
      
    } else {
      // add function to account for idling time here
      //myData.totalIdleFuel = myData.totalIdleFuel + fuelVol;
    }
  } 
}

