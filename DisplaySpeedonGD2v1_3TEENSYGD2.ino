#include <Arduino.h>
#include <OBD.h>
#include <Wire.h>
#include <SdFat.h>
#include <SPI.h>
//#include <RTClib.h>
#include <TinyGPS++.h>
#include <EEPROM.h>
#include <GD2.h>
#define CS 20 //GPUSel
#define SD_SEL 21 //GD2 SD

#define BLACK  0x000000


//TODO:
//- Add readme file in github
//- Experiment with shortening time between fuel averaging (1.5-2secs?) or change to monitor num readings


//3/18 Tests:
//Change baud on OBD
//Add GPS
//Test different buffer sizes



/*************************************************************************
SETTINGS
*************************************************************************/
#define ENABLE_DEBUG //Serial
//#define ENABLE_FUEL  //Serial
#define ENABLE_GPS
#define ENABLE_LOG
#define ENABLE_OBD
//#define ENABLE_DISPLAY
//#define ENABLE_NET


//#ifdef ENABLE_OBD
COBD obd;
//#endif

//#ifdef ENABLE_DISPLAY
struct SEND_DATA_STRUCTURE {
  //OBD Variables
  bool powerStatus = false;
  bool engineStatus = false;
  bool sdStatus = false;
  int runTime = 0; //change to unsigned long
  int rpmVal = 0;
  int spdVal = 0;
  //int mapVal;
  //int iatVal;
  float instantMPG = 0;
  
  unsigned long totalIdleTime = 0;
  unsigned long totalMovingTime = 0;
  float totalFuel = 0;
  
  float latLocation;
  float lonLocation;
  
} vehicleData;

//#endif

//#ifdef ENABLE_LOG

const int chipSelect = 4;//Logging SD

SdFat sd;

SdFile tripDirectory;
SdFile logDirectory;

SdFile tripFile;
SdFile logFile;

char tripName[18];
char logName[18];

char dataBuff[72];  //Update to multiple of 128 for improved speed?
char tempbuf[18];   //Could also make much bigger, add carriage return and line feed
char gpsBuf[48];    //Could also put in flash instead of RAM - look into this
//char timeStamp[20];

//  RTC_DS1307 RTC;
//ArduinoOutStream cout(Serial);

/*
// call back for file timestamps
void dateTime(uint16_t* date, uint16_t* time) {
    DateTime now = RTC.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

ostream& operator << (ostream& os, DateTime& dt) {
  os << dt.year() << '/' << int(dt.month()) << '/' << int(dt.day()) << ',';
  os << int(dt.hour()) << ':' << setfill('0') << setw(2) << int(dt.minute());
  os << ':' << setw(2) << int(dt.second()) << setfill(' ');
  return os;
}
*/

//#endif

TinyGPSPlus gps;

const char modePids[] = {
  PID_RPM,             //0
  PID_SPEED,           //1
  PID_THROTTLE,        //2
  PID_ENGINE_LOAD,     //3
  PID_COOLANT_TEMP,    //4
  PID_INTAKE_TEMP,     //5
  PID_AMBIENT_TEMP,    //6
  PID_MAF_FLOW,        //7
  PID_ABSOLUTE_ENGINE_LOAD, //8 
  PID_FUEL_PRESSURE,   //9
  PID_INTAKE_MAP,      //10
  PID_BAROMETRIC,      //11
  PID_TIMING_ADVANCE,  //12
  PID_FUEL_LEVEL,      //13
  PID_RUNTIME,         //14
  PID_DISTANCE };      //15
  
const int pidIndex[] = {
  0,  //PID_RPM,             
  1,  //PID_SPEED,           
  2,  //PID_THROTTLE,        
  3,  //PID_ENGINE_LOAD,     
  //4,  //PID_COOLANT_TEMP,    
  5,  //PID_INTAKE_TEMP,     
  6,  //PID_AMBIENT_TEMP,    
  //7,  //PID_MAF_FLOW,        
  //8,  //PID_ABS_ENGINE_LOAD,  
  //9,  //PID_FUEL_PRESSURE,   
  10,  //PID_INTAKE_PRESSURE, 
  //11,  //PID_BAROMETRIC,      
  12,  //PID_TIMING_ADVANCE,  
  //13,  //PID_FUEL_LEVEL,      
  //14,  //PID_RUNTIME,         
  15,  //PID_DISTANCE };      
};  
  
const char *pidName[] = {
  "RPM", 
  "SPEED", 
  "THROTTLE", 
  "ENGINE_LOAD",
  "COOLANT_TEMP",
  "INTAKE_TEMP", 
  "AMBIENT_TEMP", 
  "MAF_FLOW",
  "ABS_ENGINE_LOAD", 
  "FUEL_PRESSURE", 
  "INTAKE_PRESSURE", 
  "BAROMETRIC",
  "TIMING_ADVANCE", 
  "FUEL_LEVEL", 
  "RUNTIME", 
  "DISTANCE" };

const char* modeFmts[] = {
  "%04u",   //0
  "%03u",   //1
  "%03u",   //2
  "%02u",   //3
  "%03d",   //4
  "%03d",   //5
  "%03d",   //6
  "%03u",   //7
  "%03u",   //8
  "%03u",   //9
  "%03u",   //10
  "%u",     //11
  "%03d",   //12
  "%03u",   //13
  "%05u",   //14
  "%04u" }; //15

static float volEff = 0.89;
static float engDisp = 1.9844; //L
static float mMAir = 28.97; //g/mol
static float rVal = 8.314; //J/K/mol
static float afRatio = 14.7;
static float gasDensity = 6.17; //lb/gal
static float convgtolb = 454; //g/lb

unsigned long fuelTimer = 3000;
unsigned long fuelStartTime = 0;
unsigned long stoppedTimeStamp = 0;
unsigned long movingTimeStamp = 0;
unsigned long prevReadTimeStamp = 0;
int numReadings = 0;

int mapVal;
int iatVal;

void setup() {
  delay(2000);
  #ifdef ENABLE_DEBUG
  Serial.begin(115200);
  #endif  

  #ifdef ENABLE_DISPLAY
  GD.begin();
  delay(100);
  displaySetup();  
  
  #endif


  /*
  Wire.begin();
  if (!RTC.begin()) Serial.println("RTC failed");
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  // set date time callback function
  SdFile::dateTimeCallback(dateTime);
  DateTime now = RTC.now();
  cout  << now << endl;
  */
  vehicleData.sdStatus = initSD();
  
  #ifdef ENABLE_GPS
  Serial3.begin(57600);
  #endif  
  
  #ifdef ENABLE_DEBUG
  Serial.print("SD Card Status: ");
  Serial.println(vehicleData.sdStatus);
  #endif

  #ifdef ENABLE_OBD
  vehicleData.powerStatus = obdInit();
  if (vehicleData.powerStatus == true) logData("OBD Connection Success");
  
  fuelStartTime = millis() + 3000;
  #endif
  

  //delay(1000);
    
}

void loop() {
  
  if(vehicleData.powerStatus == true) { 
    readOBD();
    feedGPS(0);
    mpgCalc();      
  }
  feedGPS(1000);
  getLocation();
  vehicleStatus();

  feedGPS(0);
  
  //handleData();
  //updateScreen();  
  
}


