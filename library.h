#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"
#include "TinyGPSPlus.h"
#include "SensorsNAS.h"
#include "EEPROM.h"
#include "SHT31.h"
#include "OneWire.h"
#include "DallasTemperature.h"

// Address I2C Atlas Sensors
#define O2 97
#define PH 99
#define EC 100
#define ORP 98
#define ONE_WIRE_BUS 2

// Alert
bool tOnAQUA = false;
bool setupInitial = false;
bool alertWaterMin = false;
bool alertWaterMax = false;
bool alertEnviormentTempMin = false;
bool alertEnviormentTempMax = false;
bool alertEnviormentHumMin = false;
bool alertEnviormentHumMax = false;
bool isTimeSynced = false;

// Measurment PV and BATT
float Batt = 0.0;
float Pv = 0.0;
float R1 = 1000000;
float R2 = 100000;

// Sensors and GPS
TinyGPSPlus gps;
SHT31 sht;

bool flagSensors = false;
bool flagSensors2 = false;
bool stateHour = false;

char decHour[3];
char decMinute[3];

// Set points
bool flagSP = false;
bool setSP = false;

float minSPtemperature = 0; // SP

float maxSPtemperature = 0; // SP

float minSPEnviromentTemp = 0; // SP

float maxSPEnviromentTemp = 0; // SP

float minSPEnviromentHum = 0; // SP

float maxSPEnviromentHum = 0; // SP

byte BandSP = 0xFF;

// XBee
SoftwareSerial xbee(12, 11); // RX TX
byte dato;
byte macGW[8];                                                                                                                               // Mac ID EEPROM
boolean macFound = false;                                                                                                                    // Bool Search MAC
byte request[255];                                                                                                                           // Frame Gateway
byte frameDiscover[19] = {0x7E, 0x00, 0x0F, 0x08, 0x01, 0x4E, 0x44, 0x43, 0x6F, 0x6F, 0x72, 0x64, 0x69, 0x6E, 0x61, 0x64, 0x6F, 0x72, 0xF0}; // Frame AT Node Discover

// Max and Min
float _MaxHum = 0;
float _MinHum = 9999;

float _MaxTemp = 0;
float _MinTemp = 9999;

float _MaxWaterTemp = 0;
float _MinWaterTemp = 9999;

byte maxHumHour[2];
byte maxHumMinute[2];
byte minHumHour[2];
byte minHumMinute[2];

byte maxTempHour[2];
byte maxTempMinute[2];
byte minTempHour[2];
byte minTempMinute[2];

byte maxWaterTempHour[2];
byte maxWaterTempMinute[2];
byte minWaterTempHour[2];
byte minWaterTempMinute[2];

int lastGPShour = -1;
int lastGPSday = -1;
bool state = false;
int lastHour = -1;
int previousHour = -1;
int previousMin = -1;

// Auto Request
bool rMin = false;
bool rHrs = true;
bool r3Hrs = false;
bool r5Hrs = false;

bool rHrs_2 = true;
bool r3Hrs_2 = false;
bool r5Hrs_2 = false;

int hr1 = 0;
int hr2 = 0;
int hr3 = 0;
int hr4 = 0;
int hr5 = 0;

int hr1_2 = 0;
int hr2_2 = 0;
int hr3_2 = 0;
int hr4_2 = 0;
int hr5_2 = 0;

int hourUTC = 0;
int minuteUTC = 0;
int secondsUTC = 0;
int currentGPSday = 0;
int currentGPSyear = 0;

int previousHourUTC = -1;
int previousHourUTC3 = -1;
int previousHourUTC5 = -1;

int previousHourUTC_2 = -1;
int previousHourUTC3_2 = -1;
int previousHourUTC5_2 = -1;

// Struct
typedef union
{
    uint16_t value;
    uint8_t bytes[2];
} UINT16_t; // Sensors Temp and Hum
