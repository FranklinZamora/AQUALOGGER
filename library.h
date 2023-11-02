#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"
#include "TinyGPSPlus.h"
// #include "SensorsNAS.h"
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
bool alert = false;
bool isTimeSynced = false;

// Sensors and GPS
TinyGPSPlus gps;
SHT31 sht;


bool flagSensors = false;
bool flagSensors2 = false;

char decHour[3];
char decMinute[3];

// Set points
bool flagSP = false;
bool setSP = false;

byte min_SPtemp1 = 0x00;
byte min_SPtemp2 = 0x00;
int minSPtemp1 = 0;
float minSPtemp2 = 0;
float minSPtemperature = 0; // SP

byte max_SPtemp1 = 0x00;
byte max_SPtemp2 = 0x00;
int maxSPtemp1 = 0;
float maxSPtemp2 = 0;
float maxSPtemperature = 0; // SP

byte min_SPEnviromentTemp1 = 0x00;
byte min_SPEnviromentTemp2 = 0x00;
int minSPEnvTemp1 = 1;
float minSPEnvTemp2 = 0;
float minSPEnviromentTemp = 0; // SP

byte max_SPEnviromentTemp1 = 0x00;
byte max_SPEnviromentTemp2 = 0x00;
int maxSPEnvTemp1 = 0;
float maxSPEnvTemp2 = 0;
float maxSPEnviromentTemp = 0; // SP

byte min_SPEnviromentHum1 = 0x00;
byte min_SPEnviromentHum2 = 0x00;
int minSPEnvHum1 = 0;
float minSPEnvHum2 = 0;
float minSPEnviromentHum = 0; // SP

byte max_SPEnviromentHum1 = 0x00;
byte max_SPEnviromentHum2 = 0x00;
int maxSPEnvHum1 = 0;
float maxSPEnvHum2 = 0;
float maxSPEnviromentHum = 0; // SP

// XBee
SoftwareSerial xbee(12, 11); // RX TX
byte dato;
byte macGW[8];            // Mac ID EEPROM
boolean macFound = false; // Bool Search MAC
byte request[255];        // Frame Gateway

byte frameDiscover[] = {0x7E, 0x00, 0x04, 0x08, 0x01, 0x4E, 0x44, 0x64}; // Frame AT Node Discover

// Max and Min
// Cambiar a flotate
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

// Auto Request
bool rHr = false;
bool r3Hrs = false;
bool r5Hrs = false;
bool hasExecutedSense1 = false;

int hr1 = 0;
int hr2 = 0;
int hr3 = 0;
int hr4 = 0;
int hr5 = 0;
int hourUTC = 0;
int minuteUTC = 0;
int secondsUTC = 0;
int currentGPSday = 0;
int currentGPSyear = 0;

int previousHourUTC = -1;
unsigned long previousMillis = 0;
const long interval = 1000; // Intervalo de 1 segundo (en milisegundos)

// Struct
typedef union
{
    uint16_t value;
    uint8_t bytes[2];
} UINT16_t; // Sensors Temp and Hum


void getGps(void);