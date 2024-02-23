<h1 align="center"> WaterWise </h1>

![Sin título (1000 x 500 px)](https://github.com/FranklinZamora/SensorsNAS/assets/139190968/dd34b7ae-60c4-4c73-b87e-0187c1f9dfec)

[![NAS - Sensors](https://img.shields.io/badge/NAS-Sensors-blue?logo=c%2B%2B)](https://)
[![NAS - Technology](https://img.shields.io/badge/NAS-Technology-2ea44f)](https://) [![AquaNode](https://img.shields.io/badge/WaterWise-2ea44f)](https://) ![Pre - Release](https://img.shields.io/badge/Pre--Release-v1.0-red)

# Descripcion

WaterWise is a device designed to provide real-time measurements and alerts for various water and environmental parameters
(This code is only compatible with ATMega 2560 and ATMega3a28p at the moment).

## Key Features

- **Water Temperature:** Measures the temperature of the water.
- **Ambient Temperature:** Provides ambient temperature readings.
- **Ambient Humidity:** Offers readings of the surrounding humidity.
- **pH:** Measures the acidity or alkalinity of the water.
- **Dissolved Oxygen:** Indicates the amount of dissolved oxygen in the water.
- **Oxygen Saturation:** Calculates the oxygen saturation in the water.
- **Electrical Conductivity:** Measures the electrical conductivity of the water.
- **TDS (Total Dissolved Solids):** Provides readings of dissolved solids in the water.
- **Salinity:** Indicates the level of salinity in the water.
- **Specific Gravity of Water:** Provides information about the density of water.
- **ORP (Oxidation-Reduction Potential):** Measures the oxidation-reduction potential in the water.

## Usage

To use this library in your project, you should:

1. Clone this repository.
2. Include the `SensorsNAS.h` header file in your Script. Download in [Repository](https://github.com/FranklinZamora/SensorsNAS)

```c++
#include <SensorsNAS.h>
```

## Code Initialization

The following code snippet is part of the setup function in the WaterWise project. It initializes sensors, communication interfaces, and handles EEPROM data.

##### OneWire and DallasTemperature Initialization

```cpp
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature senW(&oneWire);
senW.begin();           // Initialize DS18B20 (Temperature)
senW.setResolution(10); // Set resolution for Water Temperature

```

##### SensorsNAS Initialization

```cpp
#define O2 97
#define ORP 98
#define PH 99
#define EC 100

SensorsNAS sensors(O2, PH, EC, ORP);
sensors.begin(sensors); // Initialize custom sensor class

```

##### GPS, Debug and RF module Initialization

```cpp
Serial3.begin(9600);  // Initialize GPS and DEBUG serial port
#ifdef __AVR_ATmega2560__
Serial2.begin(9600);  // Initialize XBEE serial port (for ATmega2560)
#endif
#ifdef __AVR_ATmega328P__
xbeeSerial.begin(9600);  // Initialize XBEE serial port (for ATmega328P)
#endif
```

##### Watchdog.

```cpp
wdt_disable(); // Disable Watchdog

wdt_enable(WDTO_4S);

wdt_reset(); // Reset watchdog
```

##### Methods.

Debug is to know what is being sent by the XBee module.

```cpp
void debug(byte frame[], int len)
```

Smart delay prevents the code from freezing for gps

```cpp
static void smartdelay(unsigned long ms)
```

Get data clock for GPS

```cpp
void getGps(void)
```

Get sensors 1 data (Temperature and Humidity)

```cpp
void getSensors1()
```

Print in console Temperature and humidity

```cpp
void debugSens()
```

Send (Temperature and Humidity) with clock and max's & min's

```cpp
void sense1()
```

Send (Temperature and Humidity) without clock

```cpp
void senseNotClock()
```

Set points process

```cpp
void conditionalSP()
```

Save Set Points in EEPROM and Global Variables

```cpp
void SetSetPoints()
```

Historical data process

```cpp
void maxandmin()
```

Add checksum in frame for XBee

```cpp
byte generateChecksum(int len, byte arrayBytes[])
```

Search coordinador (AT Command)

```cpp
void NodeDiscover()
```

Meassure volts for Panel and Battery

```cpp
void MeassureVolts()
```

Read microchip volts

```cpp
long readVcc()
```

## Note

This library has been developed to meet the needs of Ochoa Technology and may not be directly compatible with other companies' hardware.
