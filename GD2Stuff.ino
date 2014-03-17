
void displayInit() {
  GD.begin();
  delay(100);
  displaySetup();
}

void displaySetup()
{
  GD.ClearColorRGB(BLACK);
  GD.Clear(); 
  GD.cmd_text(300, 175, 29, OPT_CENTERY, " SD Stat:");
  GD.cmd_text(300, 200, 29, OPT_CENTERY, " PO Stat:");
  GD.cmd_text(300, 225, 29, OPT_CENTERY, " EN Stat:");
  GD.cmd_text(300, 250, 29, OPT_CENTERY, "Time (s):");

  GD.cmd_text(410, 176, 29, OPT_CENTERY, "0");
  GD.cmd_text(410, 201, 29, OPT_CENTERY, "0");
  GD.cmd_text(410, 226, 29, OPT_CENTERY, "0");
  GD.cmd_text(400, 251, 29, OPT_CENTERY, "0000");
    
  GD.cmd_text(20, 20, 30, OPT_CENTERY, "RPM:");
  GD.cmd_text(225, 20, 30, OPT_CENTERY, "MPG:");
  GD.cmd_text(20, 80, 30, OPT_CENTERY, "SPD:");
  GD.cmd_text(20, 140, 30, OPT_CENTERY, "Move:");
  //GD.cmd_text(20, 200, 30, OPT_CENTERY, " Move:");


  GD.cmd_text(120, 20, 30, OPT_CENTERY, "0000");
  GD.cmd_text(320, 20, 31, OPT_CENTERY, "000.00");  
  GD.cmd_text(120, 80, 30, OPT_CENTERY, "000");
  GD.cmd_text(120, 140, 30, OPT_CENTERY, "0000");
  //GD.cmd_text(120, 200, 30, OPT_CENTERY, "0000");
  
  GD.cmd_text(225, 80, 30, OPT_CENTERY, " Idle:");
  GD.cmd_text(225, 140, 30, OPT_CENTERY, " Fuel:");
  
  GD.cmd_text(320, 80, 31, OPT_CENTERY, "0000");
  GD.cmd_text(320, 140, 31, OPT_CENTERY, "000.00");
 
  GD.swap();
}

void handleData()
{
  vehicleData.totalMovingTime = vehicleData.totalMovingTime/1000;    
  vehicleData.totalIdleTime = vehicleData.totalIdleTime/1000; 
}

void updateScreen() { 
  
  if (vehicleData.powerStatus == true) {
    char powerStatusBuf[8];
    
    char engineStatusBuf[8]; 
    char sdStatusBuf[8];
    char runTimeBuf[8]; 
    char rpmBuf[8];
    char spdBuf[8];
    //char idleBuf[8];
    char moveBuf[8];
    char instantMPGBuf[8];
    char totalIdleTimeBuf[8];
    char totalFuelBuf[8];
    
    //Should check all of the below for validity too
    sprintf(powerStatusBuf, "%u", vehicleData.powerStatus);
    
    sprintf(engineStatusBuf, "%u", vehicleData.engineStatus);
    sprintf(sdStatusBuf, "%u", vehicleData.sdStatus);
    sprintf(runTimeBuf, "%04u", vehicleData.runTime);
    sprintf(rpmBuf, "%04u", vehicleData.rpmVal);
    sprintf(spdBuf, "%03u", vehicleData.spdVal);
    //sprintf(mapBuf, "%03u", vehicleData.mapVal);
    sprintf(moveBuf, "%5u", vehicleData.totalMovingTime); 
    dtostrf(vehicleData.instantMPG, 5, 2, instantMPGBuf);
    sprintf(totalIdleTimeBuf, "%5u", vehicleData.totalIdleTime);
    dtostrf(vehicleData.totalFuel, 5, 2, totalFuelBuf);
   
    GD.ClearColorRGB(BLACK);
    GD.Clear();
   
    drawStaticText();

    GD.cmd_text(410, 201, 29, OPT_CENTERY, powerStatusBuf);
    GD.cmd_text(410, 176, 29, OPT_CENTERY, sdStatusBuf);  
    GD.cmd_text(410, 226, 29, OPT_CENTERY, engineStatusBuf);
    GD.cmd_text(400, 251, 29, OPT_CENTERY, runTimeBuf);
    
    GD.cmd_text(120, 20, 30, OPT_CENTERY, rpmBuf);
    GD.cmd_text(320, 20, 31, OPT_CENTERY, instantMPGBuf);
    GD.cmd_text(120, 80, 30, OPT_CENTERY, spdBuf);
    GD.cmd_text(120, 140, 30, OPT_CENTERY, moveBuf);
    //GD.cmd_text(120, 200, 30, OPT_CENTERY, iatBuf);
    
    GD.cmd_text(320, 80, 31, OPT_CENTERY, totalIdleTimeBuf);
    GD.cmd_text(320, 140, 31, OPT_CENTERY, totalFuelBuf);  
    
    GD.swap();   
  }
}
void drawStaticText()
{
 
  GD.cmd_text(300, 175, 29, OPT_CENTERY, " SD Stat:");
  GD.cmd_text(300, 200, 29, OPT_CENTERY, " PO Stat:");
  GD.cmd_text(300, 225, 29, OPT_CENTERY, " EN Stat:");
  GD.cmd_text(300, 250, 29, OPT_CENTERY, "Time (s):");
  
  GD.cmd_text(20, 20, 30, OPT_CENTERY, "RPM:");
  GD.cmd_text(225, 20, 30, OPT_CENTERY, "MPG:");
  GD.cmd_text(20, 80, 30, OPT_CENTERY, "SPD:");
  GD.cmd_text(20, 140, 30, OPT_CENTERY, "Move:");
  //GD.cmd_text(20, 200, 30, OPT_CENTERY, " IAT:");
  
  GD.cmd_text(225, 80, 30, OPT_CENTERY, " Idle:");
  GD.cmd_text(225, 140, 30, OPT_CENTERY, " Fuel:");

}
