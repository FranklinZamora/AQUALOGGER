// Ochoa Technology - NAS
// Aqualogger v0.2
// 08-22-2023

#include "library.h"

UINT16_t temp, hum, waterTemp, maxtemp, mintemp, maxhum, minhum, maxwaterTemp, minwaterTemp, batteryLevel, panelLevel;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature senW(&oneWire);
SensorsNAS sensors(O2, PH, EC, ORP);

void setup()
{
    // Initialize Sensors, Communication Radio Frecuyency and GPS
    Wire.begin();           // SHT31
    Serial.begin(115200);   // GPS and DEBUG
    xbee.begin(9600);       // Xbee
    senW.begin();           // DSB18 (Temperature)
    senW.setResolution(10); // Steps for Water Temperature
    sht.begin(0x44);        // Address SHT31
    Wire.setClock(100000);  // Clock for SHT31
    sensors.begin(sensors); // Sensors NAS

    for (int i = 0; i <= 75; i++) // Write 0xFF in EEPROM
    {
        // EEPROM.write(i, 255);
    }

    if (EEPROM[0] != 0xFF && EEPROM[1] != 0xFF && EEPROM[2] != 0xFF && EEPROM[3] != 0xFF && EEPROM[4] != 0xFF && EEPROM[5] != 0xFF && EEPROM[6] != 0xFF && EEPROM[7] != 0xFF) // EEPROM is clean?
    {
        for (int i = 0; i <= 7; i++) // Update MAC ID for EEPROm
        {
            macGW[i] = EEPROM[i];
        }

        macFound = true; // Flag to not enter the loop NodeDiscover()
    }
    else
    {
        NodeDiscover(); // Search "Coordinador" (AT Command)
    }

    // Read EEPROM and Initialize Variables
    if (EEPROM[8] == 0xFF)
    {
        setupInitial = false;
    }
    else
    {
        if (EEPROM[8] == 0x01)
        {
            setupInitial = true;

            tOnAQUA = true;
        }

        else
        {
            setupInitial = true;
            tOnAQUA = false;
        }
    }

    for (int i = 9; i <= 75; i++)
    {
        if (EEPROM[i] != 255)
        {
            switch (i)
            {
            case 9:
                minwaterTemp.bytes[1] = EEPROM[i];
                break;
            case 10:
                minwaterTemp.bytes[0] = EEPROM[i];
                break;
            case 11:
                minWaterTempHour[0] = EEPROM[i];
                break;
            case 12:
                minWaterTempHour[1] = EEPROM[i];
                break;
            case 13:
                minWaterTempMinute[0] = EEPROM[i];
                break;
            case 14:
                minWaterTempMinute[1] = EEPROM[i];
                break;
            case 15:
                maxwaterTemp.bytes[1] = EEPROM[i];
                break;
            case 16:
                maxwaterTemp.bytes[0] = EEPROM[i];
                break;
            case 17:
                maxWaterTempHour[0] = EEPROM[i];
                break;
            case 18:
                maxWaterTempHour[1] = EEPROM[i];
                break;
            case 19:
                maxWaterTempMinute[0] = EEPROM[i];
                break;
            case 20:
                maxWaterTempMinute[1] = EEPROM[i];
                break;
            case 21:
                mintemp.bytes[1] = EEPROM[i];
                break;
            case 22:
                mintemp.bytes[0] = EEPROM[i];
                break;
            case 23:
                minTempHour[0] = EEPROM[i];
                break;
            case 24:
                minTempHour[1] = EEPROM[i];
                break;
            case 25:
                minTempMinute[0] = EEPROM[i];
                break;
            case 26:
                minTempMinute[1] = EEPROM[i];
                break;
            case 27:
                maxtemp.bytes[1] = EEPROM[i];
                break;
            case 28:
                maxtemp.bytes[0] = EEPROM[i];
                break;
            case 29:
                maxTempHour[0] = EEPROM[i];
                break;
            case 30:
                maxTempHour[1] = EEPROM[i];
                break;
            case 31:
                maxTempMinute[0] = EEPROM[i];
                break;
            case 32:
                maxTempMinute[1] = EEPROM[i];
                break;
            case 33:
                minhum.bytes[1] = EEPROM[i];
                break;
            case 34:
                minhum.bytes[0] = EEPROM[i];
                break;
            case 35:
                minHumHour[0] = EEPROM[i];
                break;
            case 36:
                minHumHour[1] = EEPROM[i];
                break;
            case 37:
                minHumMinute[0] = EEPROM[i];
                break;
            case 38:
                minHumMinute[1] = EEPROM[i];
                break;
            case 39:
                maxhum.bytes[1] = EEPROM[i];
                break;
            case 40:
                maxhum.bytes[0] = EEPROM[i];
                break;
            case 41:
                maxHumHour[0] = EEPROM[i];
                break;
            case 42:
                maxHumHour[1] = EEPROM[i];
                break;
            case 43:
                maxHumMinute[0] = EEPROM[i];
                break;
            case 44:
                maxHumMinute[1] = EEPROM[i];
                break;
            case 45:
                minSPtemperature = EEPROM[i];
                // Serial.println(minSPtemperature);
                break;
            case 46:
                maxSPtemperature = EEPROM[i];
                // Serial.println(maxSPtemperature);
                break;
            case 47:
                minSPEnviromentTemp = EEPROM[i];
                // Serial.println(minSPEnviromentHum);
                break;
            case 48:
                maxSPEnviromentTemp = EEPROM[i];
                // Serial.println(maxSPEnviromentHum);
                break;
            case 49:
                minSPEnviromentHum = EEPROM[i];
                // Serial.println(minSPEnviromentHum);
                break;
            case 50:
                maxSPEnviromentHum = EEPROM[i];
                // Serial.println(maxSPEnviromentHum);
                break;

            case 57:
                if (EEPROM[57] == 0x01)
                {
                    rHrs = true;
                    r3Hrs = false;
                    r5Hrs = false;
                }

                if (EEPROM[57] == 0x03)
                {
                    rHrs = false;
                    r3Hrs = true;
                    r5Hrs = false;
                    hr1 = EEPROM[58];
                    hr2 = EEPROM[59];
                    hr3 = EEPROM[60];
                }

                if (EEPROM[57] == 0x05)
                {
                    rHrs = false;
                    r3Hrs = false;
                    r5Hrs = true;
                    hr1 = EEPROM[58];
                    hr2 = EEPROM[59];
                    hr3 = EEPROM[60];
                    hr4 = EEPROM[61];
                    hr5 = EEPROM[62];
                }
                break;
            case 58:
                break;
            case 59:
                break;
            case 60:
                break;
            case 61:
                break;
            case 62:
                break;
            case 63:
                if (EEPROM[63] == 0x01)
                {
                    rHrs_2 = true;
                    r3Hrs_2 = false;
                    r5Hrs_2 = false;
                }

                if (EEPROM[63] == 0x03)
                {
                    rHrs_2 = false;
                    r3Hrs_2 = true;
                    r5Hrs_2 = false;
                    hr1_2 = EEPROM[64];
                    hr2_2 = EEPROM[65];
                    hr3_2 = EEPROM[66];
                }

                if (EEPROM[63] == 0x05)
                {
                    rHrs_2 = false;
                    r3Hrs_2 = false;
                    r5Hrs_2 = true;
                    hr1_2 = EEPROM[64];
                    hr2_2 = EEPROM[65];
                    hr3_2 = EEPROM[66];
                    hr4_2 = EEPROM[67];
                    hr5_2 = EEPROM[68];
                }
                break;
            case 64:
                break;
            case 65:
                break;
            case 66:
                break;
            case 67:
                break;
            case 68:
                break;
            case 69:
                _MinWaterTemp = EEPROM[i] * 100;
                break;
            case 70:
                _MaxWaterTemp = EEPROM[i] * 100;
                break;
            case 71:
                _MinTemp = EEPROM[i] * 100;
                break;
            case 72:
                _MaxTemp = EEPROM[i] * 100;
                break;
            case 73:
                _MinHum = EEPROM[i] * 100;
                break;
            case 74:
                _MinHum = EEPROM[i] * 100;
                break;

            default:
                break;
            } // switch

            if (EEPROM[75] == 0x01)
            {
                flagSP = true;
                setSP = true;
            }

            if (EEPROM[76] == 0x01)
            {
                flagSensors = true;
            }

            if (EEPROM[77] == 0x01)
            {
                flagSensors2 = true;
            }

            if (EEPROM[78] == 0x01)
            {
                rMin = true;
            }

        } // EEPROM i !=ff
    }     // for cycle

    byte vSend[] = {0x7E, 0x00, 0x14, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x4E, 0x41, 0x53, 0x30, 0x2E, 0x37, generateChecksum(22, vSend)};
    // debug(vSend, sizeof(vSend));
    xbee.write(vSend, sizeof(vSend)); // Send Version
}

void loop()
{
    getGps(); // Get data GPS

    getSensors1(); // Get data Sensors 1

    MeassureVolts(); // Read voltage divider

    maxandmin();

    /*
    if (flagSensors == true)
    {
        debugSens();
    }
    */
    // Histeresis Min Water Temp
    if (alertWaterMin == false && (waterTemp.value / 100) < minSPtemperature && flagSP == true)
    {
        byte rAlertSPMin[25];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x15;
        rAlertSPMin[3] = 0x10;
        rAlertSPMin[4] = 0x01;
        rAlertSPMin[5] = macGW[0];
        rAlertSPMin[6] = macGW[1];
        rAlertSPMin[7] = macGW[2];
        rAlertSPMin[8] = macGW[3];
        rAlertSPMin[9] = macGW[4];
        rAlertSPMin[10] = macGW[5];
        rAlertSPMin[11] = macGW[6];
        rAlertSPMin[12] = macGW[7];
        rAlertSPMin[13] = 0xFF;
        rAlertSPMin[14] = 0xFE;
        rAlertSPMin[15] = 0x00;
        rAlertSPMin[16] = 0x00;
        rAlertSPMin[17] = 0x53; // H
        rAlertSPMin[18] = 0x57; // W
        rAlertSPMin[19] = 0x6D; // m
        rAlertSPMin[20] = waterTemp.bytes[1];
        rAlertSPMin[21] = waterTemp.bytes[0];
        rAlertSPMin[22] = hourUTC;
        rAlertSPMin[23] = minuteUTC;
        rAlertSPMin[24] = generateChecksum(23, rAlertSPMin);

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        alertWaterMin = true;
    }

    // Histeresis Max Water Temp
    if (alertWaterMax == false && (waterTemp.value / 100) > maxSPtemperature && flagSP == true)
    {
        byte rAlertSPMin[25];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x15;
        rAlertSPMin[3] = 0x10;
        rAlertSPMin[4] = 0x01;
        rAlertSPMin[5] = macGW[0];
        rAlertSPMin[6] = macGW[1];
        rAlertSPMin[7] = macGW[2];
        rAlertSPMin[8] = macGW[3];
        rAlertSPMin[9] = macGW[4];
        rAlertSPMin[10] = macGW[5];
        rAlertSPMin[11] = macGW[6];
        rAlertSPMin[12] = macGW[7];
        rAlertSPMin[13] = 0xFF;
        rAlertSPMin[14] = 0xFE;
        rAlertSPMin[15] = 0x00;
        rAlertSPMin[16] = 0x00;
        rAlertSPMin[17] = 0x53; // H
        rAlertSPMin[18] = 0x57; // w
        rAlertSPMin[19] = 0x4D; // M
        rAlertSPMin[20] = waterTemp.bytes[1];
        rAlertSPMin[21] = waterTemp.bytes[0];
        rAlertSPMin[22] = hourUTC;
        rAlertSPMin[23] = minuteUTC;
        rAlertSPMin[24] = generateChecksum(23, rAlertSPMin);

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        alertWaterMax = true;
    }

    // Histeresis Min Environment Temp
    if (alertEnviormentTempMin == false && (temp.value / 100) < minSPEnviromentTemp && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[25];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x15;
        rAlertSPMin[3] = 0x10;
        rAlertSPMin[4] = 0x01;
        rAlertSPMin[5] = macGW[0];
        rAlertSPMin[6] = macGW[1];
        rAlertSPMin[7] = macGW[2];
        rAlertSPMin[8] = macGW[3];
        rAlertSPMin[9] = macGW[4];
        rAlertSPMin[10] = macGW[5];
        rAlertSPMin[11] = macGW[6];
        rAlertSPMin[12] = macGW[7];
        rAlertSPMin[13] = 0xFF;
        rAlertSPMin[14] = 0xFE;
        rAlertSPMin[15] = 0x00;
        rAlertSPMin[16] = 0x00;
        rAlertSPMin[17] = 0x53; // H
        rAlertSPMin[18] = 0x45; // E
        rAlertSPMin[19] = 0x6D; // m
        rAlertSPMin[20] = temp.bytes[1];
        rAlertSPMin[21] = temp.bytes[0];
        rAlertSPMin[22] = hourUTC;
        rAlertSPMin[23] = minuteUTC;
        rAlertSPMin[24] = generateChecksum(23, rAlertSPMin);

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));
        alertEnviormentTempMin = true;
    }

    // Histeresis Max Environment Temp
    if (alertEnviormentTempMax == false && (temp.value / 100) > maxSPEnviromentTemp && flagSP == true)
    {
        byte rAlertSPMin[25];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x15;
        rAlertSPMin[3] = 0x10;
        rAlertSPMin[4] = 0x01;
        rAlertSPMin[5] = macGW[0];
        rAlertSPMin[6] = macGW[1];
        rAlertSPMin[7] = macGW[2];
        rAlertSPMin[8] = macGW[3];
        rAlertSPMin[9] = macGW[4];
        rAlertSPMin[10] = macGW[5];
        rAlertSPMin[11] = macGW[6];
        rAlertSPMin[12] = macGW[7];
        rAlertSPMin[13] = 0xFF;
        rAlertSPMin[14] = 0xFE;
        rAlertSPMin[15] = 0x00;
        rAlertSPMin[16] = 0x00;
        rAlertSPMin[17] = 0x53; // H
        rAlertSPMin[18] = 0x45; // E
        rAlertSPMin[19] = 0x4D; // M
        rAlertSPMin[20] = temp.bytes[1];
        rAlertSPMin[21] = temp.bytes[0];
        rAlertSPMin[22] = hourUTC;
        rAlertSPMin[23] = minuteUTC;
        rAlertSPMin[24] = generateChecksum(23, rAlertSPMin);

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        // debug(rAlertSPMin, sizeof(rAlertSPMin));

        alertEnviormentTempMax = true;
    }

    // Histeresis Min Environment Hum
    if (alertEnviormentHumMin == false && (hum.value / 100) < minSPEnviromentHum && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[25];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x15;
        rAlertSPMin[3] = 0x10;
        rAlertSPMin[4] = 0x01;
        rAlertSPMin[5] = macGW[0];
        rAlertSPMin[6] = macGW[1];
        rAlertSPMin[7] = macGW[2];
        rAlertSPMin[8] = macGW[3];
        rAlertSPMin[9] = macGW[4];
        rAlertSPMin[10] = macGW[5];
        rAlertSPMin[11] = macGW[6];
        rAlertSPMin[12] = macGW[7];
        rAlertSPMin[13] = 0xFF;
        rAlertSPMin[14] = 0xFE;
        rAlertSPMin[15] = 0x00;
        rAlertSPMin[16] = 0x00;
        rAlertSPMin[17] = 0x53;
        rAlertSPMin[18] = 0x48; // H
        rAlertSPMin[19] = 0x6D; // m
        rAlertSPMin[20] = hum.bytes[1];
        rAlertSPMin[21] = hum.bytes[0];
        rAlertSPMin[22] = hourUTC;
        rAlertSPMin[23] = minuteUTC;
        rAlertSPMin[24] = generateChecksum(23, rAlertSPMin);

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        alertEnviormentHumMin = true;
    }

    // Histeresis Max Environment Hum
    if (alertEnviormentHumMax == false && (hum.value / 100) > maxSPEnviromentHum && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[25];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x15;
        rAlertSPMin[3] = 0x10;
        rAlertSPMin[4] = 0x01;
        rAlertSPMin[5] = macGW[0];
        rAlertSPMin[6] = macGW[1];
        rAlertSPMin[7] = macGW[2];
        rAlertSPMin[8] = macGW[3];
        rAlertSPMin[9] = macGW[4];
        rAlertSPMin[10] = macGW[5];
        rAlertSPMin[11] = macGW[6];
        rAlertSPMin[12] = macGW[7];
        rAlertSPMin[13] = 0xFF;
        rAlertSPMin[14] = 0xFE;
        rAlertSPMin[15] = 0x00;
        rAlertSPMin[16] = 0x00;
        rAlertSPMin[17] = 0x53;
        rAlertSPMin[18] = 0x48; // H
        rAlertSPMin[19] = 0x4D; // M
        rAlertSPMin[20] = hum.bytes[1];
        rAlertSPMin[21] = hum.bytes[0];
        rAlertSPMin[22] = hourUTC;
        rAlertSPMin[23] = minuteUTC;
        rAlertSPMin[24] = generateChecksum(23, rAlertSPMin);

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        alertEnviormentHumMax = true;
    }

    if (isTimeSynced == true && rMin == true && flagSensors == true && flagSensors2 == true) // Send default sensor data 1 hr
    {
        if (minuteUTC != previousMin)
        {
            sensors.generateArray(macGW, sensors);
            sense1();
            previousMin = minuteUTC; // Update last value
        }
    }

    if (isTimeSynced == true && rHrs == true && flagSensors == true) // Send default sensor data 1 hr
    {
        if (hourUTC != previousHourUTC)
        {
            sense1();
            previousHourUTC = hourUTC; // Update last value
        }
    }
    if (isTimeSynced == true && r3Hrs == true && flagSensors == true)
    {
        if (hourUTC == hr1 || hourUTC == hr2 || hourUTC == hr3)
        {
            if (hourUTC != previousHourUTC3)
            {
                sense1();
                previousHourUTC3 = hourUTC; // Update last value
            }
        }
    }

    if (isTimeSynced == true && r5Hrs == true && flagSensors == true)
    {
        if (hourUTC == hr1 || hourUTC == hr2 || hourUTC == hr3 || hourUTC == hr4 || hourUTC == hr5)
        {
            if (hourUTC != previousHourUTC5)
            {
                sense1();
                previousHourUTC5 = hourUTC; // Update last value
            }
        }
    }

    if (isTimeSynced == true && rHrs_2 == true && flagSensors2 == true) // Send default sensor data 2 hr
    {
        if (hourUTC != previousHourUTC_2)
        {
            sensors.generateArray(macGW, sensors);
            previousHourUTC_2 = hourUTC; // Update last value
        }
    }

    if (isTimeSynced == true && r3Hrs_2 == true && flagSensors2 == true)
    {
        if (hourUTC == hr1_2 || hourUTC == hr2_2 || hourUTC == hr3_2)
        {
            if (hourUTC != previousHourUTC3_2)
            {
                sensors.generateArray(macGW, sensors);
                previousHourUTC3_2 = hourUTC; // Update last value
            }
        }
    }

    if (r5Hrs_2 == true && isTimeSynced == true && flagSensors2 == true)
    {
        if (hourUTC == hr1_2 || hourUTC == hr2_2 || hourUTC == hr3_2 || hourUTC == hr4_2 || hourUTC == hr5_2)
        {
            if (hourUTC != previousHourUTC5_2)
            {
                sensors.generateArray(macGW, sensors);
                previousHourUTC5_2 = hourUTC; // Update last value
            }
        }
    }

    if (isTimeSynced == true)
    {
        if (hourUTC != lastHour) // Change band 7 hours for reset.
        {
            lastHour = hourUTC;
            state = true;
        }
    }

    if (hourUTC == 7) // Reset max and min 24 hrs
    {
        if (state == true)
        {
            _MaxHum = 0;
            _MinHum = 9999;

            _MaxTemp = 0;
            _MinTemp = 9999;

            _MaxWaterTemp = 0;
            _MinWaterTemp = 9999;

            alertWaterMin = false;
            alertWaterMax = false;
            alertEnviormentTempMin = false;
            alertEnviormentTempMax = false;
            alertEnviormentHumMin = false;
            alertEnviormentHumMax = false;

            if (tOnAQUA == true || setupInitial == true)
            {

                EEPROM[69] = 0xFF;
                EEPROM[70] = 0xFF;
                EEPROM[71] = 0xFF;
                EEPROM[72] = 0xFF;
                EEPROM[73] = 0xFF;
                EEPROM[74] = 0xFF;
            }
        }

        state = false;
    }

    while (xbee.available() > 0) // Read xbee
    {

        for (int i = 0; i < 32; i++)
        {
            request[i] = xbee.read();
            // Serial.print(request[i], HEX);
            // Serial.print(" ");
        }

        // Serial.println();

        if (request[0] == 0x7E) // Header
        {
            if (request[2] == 0x15 && request[15] == 0x47) // Change MAC
            {
                macGW[0] = request[16];
                macGW[1] = request[17];
                macGW[2] = request[18];
                macGW[3] = request[19];
                macGW[4] = request[20];
                macGW[5] = request[21];
                macGW[6] = request[22];
                macGW[7] = request[23];
                if (setupInitial == true || tOnAQUA == true)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        EEPROM.write(i, macGW[i]);
                    }
                }
            }

            if (request[4] == macGW[0] && request[5] == macGW[1] && request[6] == macGW[2] && request[7] == macGW[3] && request[8] == macGW[4] && request[9] == macGW[5] && request[10] == macGW[6] && request[11] == macGW[7]) // Mac coordinator ?
            {
                if (request[2] == 0x0E && request[15] == 0x54) // Gateway
                {
                    if (request[16] == 0x00) // Off Gateway
                    {
                        tOnAQUA = false;
                        byte tOff_GW[] = {0x7E, 0x00, 0x12, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x54, 0x4F, 0x46, 0x46, generateChecksum(20, tOff_GW)};
                        xbee.write(tOff_GW, sizeof(tOff_GW));
                        EEPROM.write(8, 0x00);
                        // debug(tOff_GW, sizeof(tOff_GW));
                    }

                    if (request[16] == 0x01) // On Gateway
                    {

                        EEPROM.write(8, 0x01);
                        tOnAQUA = true;

                        for (int i = 0; i <= 7; i++)
                        {
                            EEPROM.write(i, macGW[i]);
                        }

                        byte tON_GW[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x54, 0x4F, 0x4E, generateChecksum(19, tON_GW)};
                        // debug(tON_GW, sizeof(tON_GW));
                        xbee.write(tON_GW, sizeof(tON_GW));
                    }
                }

                if (request[2] == 0x0D && request[15] == 0x62) // BATTERY
                {
                    byte rBATT[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x62, batteryLevel.bytes[1], batteryLevel.bytes[0], generateChecksum(19, rBATT)};
                    // debug(rBATT, sizeof(rBATT));
                    xbee.write(rBATT, sizeof(rBATT));
                }

                if (request[2] == 0x0D && request[15] == 0x70) // PANEL
                {
                    byte rBATT[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x70, panelLevel.bytes[1], panelLevel.bytes[0], generateChecksum(19, rBATT)};
                    // debug(rBATT, sizeof(rBATT));
                    xbee.write(rBATT, sizeof(rBATT));
                }

                if (request[2] == 0x0D && request[15] == 0x3F) // Version
                {
                    byte vSend[] = {0x7E, 0x00, 0x14, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x4E, 0x41, 0x53, 0x30, 0x2E, 0x37, generateChecksum(22, vSend)};
                    // debug(vSend, sizeof(vSend));
                    xbee.write(vSend, sizeof(vSend));
                }

                if (request[2] == 0x0E && request[15] == 0x73) // First sensors
                {
                    if (request[16] == 0x00) // Off
                    {
                        byte tOFF_Sensors1[] = {0x7E, 0x00, 0x12, 0x10, 0x01, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x73, 0x4F, 0x46, 0x46, generateChecksum(20, tOFF_Sensors1)};
                        // debug(tOFF_Sensors1, sizeof(tOFF_Sensors1));
                        EEPROM[76] = 0x00;
                        xbee.write(tOFF_Sensors1, sizeof(tOFF_Sensors1));
                        flagSensors = false;
                    }

                    else if (request[16] == 0x01) // On first sensors
                    {

                        byte tON_Sensors1[] = {0x7E, 0x00, 0x11, 0x10, 0x01, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x73, 0x4F, 0x4E, generateChecksum(19, tON_Sensors1)};
                        // debug(tON_Sensors1, sizeof(tON_Sensors1));
                        EEPROM[76] = 0x01;
                        xbee.write(tON_Sensors1, sizeof(tON_Sensors1));

                        flagSensors = true;
                    }

                    else if (request[16] == 0x03 && flagSensors == true) // Request first sensors
                    {
                        if (isTimeSynced)
                        {
                            sense1();
                        }
                        else
                        {
                            senseNotClock();
                        }
                    }
                }
                if (request[2] == 0x0E && request[15] == 0x53) // Sensors 1 Set points
                {

                    if (request[16] == 0x00) // Set points OFF
                    {

                        byte tOFF_SP[] = {0x7E, 0x00, 0x12, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x53, 0x4F, 0x46, 0x46, generateChecksum(20, tOFF_SP)};
                        // debug(tOFF_SP, sizeof(tOFF_SP));
                        xbee.write(tOFF_SP, sizeof(tOFF_SP));
                        flagSP = false;
                        BandSP = 0x00;
                        if (tOnAQUA == true || setupInitial == true)
                        {

                            EEPROM[75] == 0x00;
                        }
                    }

                    if (request[16] == 0x01) // Set points ON
                    {

                        byte tON_SP[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x53, 0x4F, 0x4E, generateChecksum(19, tON_SP)};
                        // debug(tON_SP, sizeof(tON_SP));
                        xbee.write(tON_SP, sizeof(tON_SP));
                        flagSP = true;
                        BandSP = 0x01;
                        if (tOnAQUA == true || setupInitial == true)
                        {

                            EEPROM[75] = 0x01;
                        }
                    }
                }

                if (flagSP == true && request[2] == 0x1A && request[15] == 0x53 && request[16] == 0x02) // Set SP
                {
                    SetSetPoints();

                    byte tOK_SP[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x53, 0x4F, 0x4B, generateChecksum(19, tOK_SP)};
                    // debug(tOK_SP, sizeof(tOK_SP));
                    xbee.write(tOK_SP, sizeof(tOK_SP));
                    setSP = true;
                }

                if (request[2] == 0x0D && request[15] == 0x52) // Alarm reset sens 1
                {
                    alertWaterMin = false;
                    alertWaterMax = false;
                    alertEnviormentTempMin = false;
                    alertEnviormentTempMax = false;
                    alertEnviormentHumMin = false;
                    alertEnviormentHumMax = false;

                    byte rAlarm[] = {0x7E, 0x00, 0x10, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x52, 0x52, generateChecksum(18, rAlarm)};
                    // debug(rAlarm, sizeof(rAlarm));
                    xbee.write(rAlarm, sizeof(rAlarm));
                }

                if (request[2] == 0x0E && request[15] == 0x79 && request[16] == 0x01) // Sens 1, 1 hrs
                {
                    rHrs = true;
                    r3Hrs = false;
                    r5Hrs = false;
                    rMin = false;

                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[57] = 0x01;
                    }
                    byte rClient_1[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x79, 0x4F, 0x4B, generateChecksum(19, rClient_1)};
                    // debug(rClient_1, sizeof(rClient_1));
                    xbee.write(rClient_1, sizeof(rClient_1));
                }

                if (request[2] == 0x11 && request[15] == 0x79 && request[16] == 0x03) // Request Client 3hrs SENS 1
                {

                    hr1 = (int)request[17];

                    hr2 = (int)request[18];

                    hr3 = (int)request[19];

                    rHrs = false;
                    r3Hrs = true;
                    r5Hrs = false;
                    rMin = false;

                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[57] = 0x03;
                        EEPROM[58] = hr1;
                        EEPROM[59] = hr2;
                        EEPROM[60] = hr3;
                    }

                    byte rClient_1[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x79, 0x4F, 0x4B, generateChecksum(19, rClient_1)};
                    // debug(rClient_1, sizeof(rClient_1));
                    xbee.write(rClient_1, sizeof(rClient_1));
                }

                if (request[2] == 0x13 && request[15] == 0x79 && request[16] == 0x05) // Request Client 5hrs SENS 1
                {

                    hr1 = (int)request[17];

                    hr2 = (int)request[18];

                    hr3 = (int)request[19];

                    hr4 = (int)request[20];

                    hr5 = (int)request[21];

                    rHrs = false;
                    r3Hrs = false;
                    r5Hrs = true;
                    rMin = false;

                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[57] = 0x05;
                        EEPROM[58] = hr1;
                        EEPROM[59] = hr2;
                        EEPROM[60] = hr3;
                        EEPROM[61] = hr4;
                        EEPROM[62] = hr5;
                    }
                    byte rClient_1[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x79, 0x4F, 0x4B, generateChecksum(19, rClient_1)};
                    // debug(rClient_1, sizeof(rClient_1));
                    xbee.write(rClient_1, sizeof(rClient_1));
                }

                if (request[2] == 0x0E && request[15] == 0x79 & request[16] == 0x3F) //  Request conf sens 1
                {
                    byte rClient_1[] = {0x7E, 0x00, 0x16, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x79, 0x02, 0x05, hr1, hr2, hr3, hr4, hr5, generateChecksum(24, rClient_1)};
                    // debug(rClient_1, sizeof(rClient_1));
                    xbee.write(rClient_1, sizeof(rClient_1));
                }

                if (request[2] == 0x0F && request[15] == 0x48 && request[16] == 0x02 && request[17] == 0x01) // Sens 2, 1 Hrs
                {
                    rHrs_2 = true;
                    r3Hrs_2 = false;
                    r5Hrs_2 = false;
                    rMin = false;

                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[63] = 0x01;
                    }
                    byte rClient_2[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x02, 0x01, generateChecksum(19, rClient_2)};
                    // debug(rClient_2, sizeof(rClient_2));
                    xbee.write(rClient_2, sizeof(rClient_2));
                }

                if (request[2] == 0x12 && request[15] == 0x48 && request[16] == 0x02 && request[17] == 0x03) // Request Client 3hrs SENS 2
                {
                    hr1_2 = (int)request[18];
                    hr2_2 = (int)request[19];
                    hr3_2 = (int)request[20];

                    rHrs_2 = false;
                    r3Hrs_2 = true;
                    r5Hrs_2 = false;
                    rMin = false;

                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[63] = 0x03;
                        EEPROM[64] = hr1_2;
                        EEPROM[65] = hr2_2;
                        EEPROM[66] = hr3_2;
                    }

                    byte rClient_2[] = {0x7E, 0x00, 0x14, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x02, 0x03, hr1_2, hr2_2, hr3_2, generateChecksum(22, rClient_2)};
                    // debug(rClient_2, sizeof(rClient_2));
                    xbee.write(rClient_2, sizeof(rClient_2));
                }

                if (request[2] == 0x14 && request[15] == 0x48 && request[16] == 0x02 && request[17] == 0x05) // Request Client 5hrs SENS 2
                {
                    hr1_2 = (int)request[18];

                    hr2_2 = (int)request[19];

                    hr3_2 = (int)request[20];

                    hr4_2 = (int)request[21];

                    hr5_2 = (int)request[22];

                    rHrs_2 = false;
                    r3Hrs_2 = false;
                    r5Hrs_2 = true;
                    rMin = false;

                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[63] = 0x05;
                        EEPROM[64] = hr1_2;
                        EEPROM[65] = hr2_2;
                        EEPROM[66] = hr3_2;
                        EEPROM[67] = hr4_2;
                        EEPROM[68] = hr5_2;
                    }

                    byte rClient_2[] = {0x7E, 0x00, 0x16, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x02, 0x05, hr1_2, hr2_2, hr3_2, hr4_2, hr5_2, generateChecksum(24, rClient_2)};
                    // debug(rClient_2, sizeof(rClient_2));
                    xbee.write(rClient_2, sizeof(rClient_2));
                }

                if (request[2] == 0x0F && request[15] == 0x48 && request[16] == 0x02 && request[17] == 0x04)
                {
                    rHrs_2 = false;
                    r3Hrs_2 = false;
                    r5Hrs_2 = false;
                    rMin = true;
                    byte rMin[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x02, 0x04, generateChecksum(19, rMin)};
                    // debug(rMin, sizeof(rMin));
                    xbee.write(rMin, sizeof(rMin));              // Send configuration sensors 2 (Time)
                    if (setupInitial == true || tOnAQUA == true) // Save eeprom conf if AQUA is turn on
                    {
                        EEPROM[78] = 0x01;
                    }
                }

                if (request[2] == 0x0F && request[15] == 0x48 && request[16] == 0x02 && request[17] == 0x3F)
                {
                    byte rClient_2[] = {0x7E, 0x00, 0x16, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x02, 0x3F, hr1_2, hr2_2, hr3_2, hr4_2, hr5_2, generateChecksum(24, rClient_2)};
                    //   debug(rClient_2, sizeof(rClient_2));
                    xbee.write(rClient_2, sizeof(rClient_2)); // Send configuration sensors 2 (Time)
                }

                if (request[2] == 0x0E && request[15] == 0x48 && request[16] == 0x01) // Turn ON Sensors 2
                {

                    byte tON_Sensors2[] = {0x7E, 0x00, 0x10, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x01, generateChecksum(18, tON_Sensors2)};
                    //  debug(tON_Sensors2, sizeof(tON_Sensors2));
                    xbee.write(tON_Sensors2, sizeof(tON_Sensors2));
                    EEPROM[77] = 0x01;
                    flagSensors2 = true;
                }

                if (request[2] == 0x0E && request[15] == 0x48 && request[16] == 0x00) // Turn OFF Sensors 2
                {

                    byte tOFF_Sensors2[] = {0x7E, 0x00, 0x10, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x00, generateChecksum(18, tOFF_Sensors2)};
                    //   debug(tOFF_Sensors2, sizeof(tOFF_Sensors2));
                    xbee.write(tOFF_Sensors2, sizeof(tOFF_Sensors2));
                    EEPROM[77] = 0x00;
                    flagSensors2 = false;
                }
                if (request[2] == 0x0E && request[15] == 0x48 && request[16] == 0x3F && flagSensors2 == true) // Send Sensors 2 in Real Time.
                {
                    sensors.generateArray(macGW, sensors);
                }
            }
        }
    }
}

// Uncomment on debug() to make it work
/*
void debug(byte frame[], int len)
{
    Serial.print("[SEND]: ");
    for (int i = 0; i < len; i++)
    {
        Serial.print(frame[i], HEX);
        if (frame[i] < 0x10)
        {
            Serial.print("0");
        }
        Serial.print(" ");
    }
    Serial.println();
}
*/

static void smartdelay(unsigned long ms) // Smartdelay for GPS
{
    unsigned long start = millis();
    do
    {
        while (Serial.available())
            gps.encode(Serial.read());
    } while (millis() - start < ms);
}

void getGps(void) // Get data clock for GPS
{

    gps.encode(Serial.read());

    while (Serial.available() > 0)

        if (gps.encode(Serial.read()))

            if (gps.date.isValid() == 1)
            {
                currentGPSyear = gps.date.year();
                currentGPSday = gps.date.day();

                if (currentGPSyear != 2080)
                {
                    isTimeSynced = true;
                }
                else
                {
                    isTimeSynced = false;
                }
            }

    if (gps.time.isValid() == 1)
    {
        hourUTC = gps.time.hour();
        minuteUTC = gps.time.minute();
        secondsUTC = gps.time.second();
        // Serial.print(hourUTC);
        // Serial.print(":");
        // Serial.print(minuteUTC);
        // Serial.print(":");
        // Serial.println(secondsUTC);
    }

    sprintf(decHour, "%02d", hourUTC);
    sprintf(decMinute, "%02d", minuteUTC);

    smartdelay(1000);
}

void getSensors1() // Get sensors 1 data (Temperature and Humidity)
{
    sht.read();
    hum.value = (int16_t)(sht.getHumidity() * 100);
    temp.value = (int16_t)(sht.getTemperature() * 100);
    senW.requestTemperatures();
    waterTemp.value = (int16_t)(senW.getTempCByIndex(0) * 100);
}

void debugSens()
{
    Serial.println(temp.value / 100);
    Serial.print(" ");
    Serial.print(hum.value / 100);
    Serial.print(" ");
    Serial.println(waterTemp.value / 100);
}

void sense1() // Send sense 1 with clock and max's & min's
{
    uint8_t requestSens1_1[61];
    requestSens1_1[0] = 0x7E;
    requestSens1_1[1] = 0x00;
    requestSens1_1[2] = 0x39;
    requestSens1_1[3] = 0x10;
    requestSens1_1[4] = 0x00;
    requestSens1_1[5] = macGW[0];
    requestSens1_1[6] = macGW[1];
    requestSens1_1[7] = macGW[2];
    requestSens1_1[8] = macGW[3];
    requestSens1_1[9] = macGW[4];
    requestSens1_1[10] = macGW[5];
    requestSens1_1[11] = macGW[6];
    requestSens1_1[12] = macGW[7];
    requestSens1_1[13] = 0xFF;
    requestSens1_1[14] = 0xFE;
    requestSens1_1[15] = 0x00;
    requestSens1_1[16] = 0x00; // add 0x73
    requestSens1_1[17] = 0x73;
    requestSens1_1[18] = waterTemp.bytes[1]; // Water Temp
    requestSens1_1[19] = waterTemp.bytes[0];
    requestSens1_1[20] = minwaterTemp.bytes[1];
    requestSens1_1[21] = minwaterTemp.bytes[0];
    requestSens1_1[22] = minWaterTempHour[0];
    requestSens1_1[23] = minWaterTempHour[1];
    requestSens1_1[24] = minWaterTempMinute[0];
    requestSens1_1[25] = minWaterTempMinute[1];
    requestSens1_1[26] = maxwaterTemp.bytes[1];
    requestSens1_1[27] = maxwaterTemp.bytes[0];
    requestSens1_1[28] = maxWaterTempHour[0];
    requestSens1_1[29] = maxWaterTempHour[1];
    requestSens1_1[30] = maxWaterTempMinute[0];
    requestSens1_1[31] = maxWaterTempMinute[1];
    requestSens1_1[32] = temp.bytes[1]; // Temp
    requestSens1_1[33] = temp.bytes[0];
    requestSens1_1[34] = mintemp.bytes[1];
    requestSens1_1[35] = mintemp.bytes[0];
    requestSens1_1[36] = minTempHour[0];
    requestSens1_1[37] = minTempHour[1];
    requestSens1_1[38] = minTempMinute[0];
    requestSens1_1[39] = minTempMinute[1];
    requestSens1_1[40] = maxtemp.bytes[1];
    requestSens1_1[41] = maxtemp.bytes[0];
    requestSens1_1[42] = maxTempHour[0];
    requestSens1_1[43] = maxTempHour[1];
    requestSens1_1[44] = maxTempMinute[0];
    requestSens1_1[45] = maxTempMinute[1];
    requestSens1_1[46] = hum.bytes[1]; // Hum
    requestSens1_1[47] = hum.bytes[0];
    requestSens1_1[48] = minhum.bytes[1];
    requestSens1_1[49] = minhum.bytes[0];
    requestSens1_1[50] = minHumHour[0];
    requestSens1_1[51] = minHumHour[1];
    requestSens1_1[52] = minHumMinute[0];
    requestSens1_1[53] = minHumMinute[1];
    requestSens1_1[54] = maxhum.bytes[1];
    requestSens1_1[55] = maxhum.bytes[0];
    requestSens1_1[56] = maxHumHour[0];
    requestSens1_1[57] = maxHumHour[1];
    requestSens1_1[58] = maxHumMinute[0];
    requestSens1_1[59] = maxHumMinute[1];
    requestSens1_1[60] = generateChecksum(59, requestSens1_1);

    xbee.write(requestSens1_1, sizeof(requestSens1_1));
    // debug(requestSens1_1, sizeof(requestSens1_1));
}

void SetSetPoints() // Save Set Points in EEPROM and Global Variables
{

    // Set Point Water Min
    minSPtemperature = request[17];
    minSPtemperature += request[18] / 100;
    // Set Point Water Max
    maxSPtemperature = request[19];
    maxSPtemperature += request[20] / 100;
    // Set Point Enviroment Temp Min
    minSPEnviromentTemp = request[21];
    minSPEnviromentTemp += request[22] / 100;
    // Set Point Enviroment Temp Max
    maxSPEnviromentTemp = request[23];
    maxSPEnviromentTemp += request[24] / 100;
    // Set Point Enviroment Hum Min
    minSPEnviromentHum = request[25];
    minSPEnviromentHum += request[26] / 100;
    // Set Point Enviroment Hum Max
    maxSPEnviromentHum = request[27];
    maxSPEnviromentHum += request[28] / 100;

    if (setupInitial == true || tOnAQUA == true || EEPROM[75] == 0x01)
    {
        EEPROM[45] = minSPtemperature;
        EEPROM[46] = maxSPtemperature;
        EEPROM[47] = minSPEnviromentTemp;
        EEPROM[48] = maxSPEnviromentTemp;
        EEPROM[49] = minSPEnviromentHum;
        EEPROM[50] = maxSPEnviromentHum;
    }
}

void senseNotClock() // Send sensors 1 without clock
{
    byte requestSens_1[25];
    requestSens_1[0] = 0x7E;
    requestSens_1[1] = 0x00;
    requestSens_1[2] = 0x15;
    requestSens_1[3] = 0x10;
    requestSens_1[4] = 0x00;
    requestSens_1[5] = macGW[0];
    requestSens_1[6] = macGW[1];
    requestSens_1[7] = macGW[2];
    requestSens_1[8] = macGW[3];
    requestSens_1[9] = macGW[4];
    requestSens_1[10] = macGW[5];
    requestSens_1[11] = macGW[6];
    requestSens_1[12] = macGW[7];
    requestSens_1[13] = 0xFF;
    requestSens_1[14] = 0xFE;
    requestSens_1[15] = 0x00;
    requestSens_1[16] = 0x00;
    requestSens_1[17] = 0x73;
    requestSens_1[18] = waterTemp.bytes[1];
    requestSens_1[19] = waterTemp.bytes[0];
    requestSens_1[20] = temp.bytes[1];
    requestSens_1[21] = temp.bytes[0];
    requestSens_1[22] = hum.bytes[1];
    requestSens_1[23] = hum.bytes[0];
    requestSens_1[24] = generateChecksum(23, requestSens_1);

    // debug(requestSens_1, sizeof(requestSens_1));
    xbee.write(requestSens_1, sizeof(requestSens_1));
}

void maxandmin() // Histeresis and save in EEPROM alerts
{
    if (isTimeSynced)
    {
        if (waterTemp.value < _MinWaterTemp) // Min water Temperature
        {
            _MinWaterTemp = waterTemp.value;
            minwaterTemp.value = _MinWaterTemp;

            minWaterTempHour[0] = decHour[0];
            minWaterTempHour[1] = decHour[1];
            minWaterTempMinute[0] = decMinute[0];
            minWaterTempMinute[1] = decMinute[1];

            if (setupInitial == true || tOnAQUA == true)
            {

                EEPROM[69] = _MinWaterTemp / 100;
                EEPROM[9] = minwaterTemp.bytes[1];
                EEPROM[10] = minwaterTemp.bytes[0];
                EEPROM[11] = minWaterTempHour[0];
                EEPROM[12] = minWaterTempHour[1];
                EEPROM[13] = minWaterTempMinute[0];
                EEPROM[14] = minWaterTempMinute[1];
            }
        }

        if (waterTemp.value > _MaxWaterTemp) // Max Water Temperature
        {
            _MaxWaterTemp = waterTemp.value;
            maxwaterTemp.value = _MaxWaterTemp;

            maxWaterTempHour[0] = decHour[0];
            maxWaterTempHour[1] = decHour[1];
            maxWaterTempMinute[0] = decMinute[0];
            maxWaterTempMinute[1] = decMinute[1];

            if (setupInitial == true || tOnAQUA == true)
            {

                EEPROM[70] = _MaxWaterTemp / 100;
                EEPROM[15] = maxwaterTemp.bytes[1];
                EEPROM[16] = maxwaterTemp.bytes[0];
                EEPROM[17] = maxWaterTempHour[0];
                EEPROM[18] = maxWaterTempHour[1];
                EEPROM[19] = maxWaterTempMinute[0];
                EEPROM[20] = maxWaterTempMinute[1];
            }
        }

        if (temp.value < _MinTemp) // Min Environment Temperature
        {
            _MinTemp = temp.value;
            mintemp.value = _MinTemp;

            minTempHour[0] = decHour[0];
            minTempHour[1] = decHour[1];
            minTempMinute[0] = decMinute[0];
            minTempMinute[1] = decMinute[1];

            if (setupInitial == true || tOnAQUA == true)
            {
                EEPROM[71] = _MinTemp / 100;
                EEPROM[21] = mintemp.bytes[1];
                EEPROM[22] = mintemp.bytes[0];
                EEPROM[23] = minTempHour[0];
                EEPROM[24] = minTempHour[1];
                EEPROM[25] = minTempMinute[0];
                EEPROM[26] = minTempMinute[1];
            }
        }

        // Max temp
        if (temp.value > _MaxTemp)
        {
            _MaxTemp = temp.value;
            maxtemp.value = _MaxTemp;

            maxTempHour[0] = decHour[0];
            maxTempHour[1] = decHour[1];
            maxTempMinute[0] = decMinute[0];
            maxTempMinute[1] = decMinute[1];

            if (setupInitial == true || tOnAQUA == true)
            {

                EEPROM[72] = _MaxTemp / 100;
                EEPROM[27] = maxtemp.bytes[1];
                EEPROM[28] = maxtemp.bytes[0];
                EEPROM[29] = maxTempHour[0];
                EEPROM[30] = maxTempHour[1];
                EEPROM[31] = maxTempMinute[0];
                EEPROM[32] = maxTempMinute[1];
            }
        }

        // Min humidity
        if (hum.value < _MinHum)
        {
            _MinHum = hum.value;
            minhum.value = _MinHum;

            minHumHour[0] = decHour[0];
            minHumHour[1] = decHour[1];
            minHumMinute[0] = decMinute[0];
            minHumMinute[1] = decMinute[1];
            if (setupInitial == true || tOnAQUA == true)
            {
                EEPROM[73] = _MinHum / 100;
                EEPROM[33] = minhum.bytes[1];
                EEPROM[34] = minhum.bytes[0];
                EEPROM[35] = minHumHour[0];
                EEPROM[36] = minHumHour[1];
                EEPROM[37] = minHumMinute[0];
                EEPROM[38] = minHumMinute[1];
            }
        }

        // Max Hunidity
        if (hum.value > _MaxHum)
        {
            _MaxHum = hum.value;
            maxhum.value = _MaxHum;

            maxHumHour[0] = decHour[0];
            maxHumHour[1] = decHour[1];
            maxHumMinute[0] = decMinute[0];
            maxHumMinute[1] = decMinute[1];
            if (setupInitial == true || tOnAQUA == true)
            {
                EEPROM[74] = _MaxHum / 100;
                EEPROM[39] = maxhum.bytes[1];
                EEPROM[40] = maxhum.bytes[0];
                EEPROM[41] = maxHumHour[0];
                EEPROM[42] = maxHumHour[1];
                EEPROM[43] = maxHumMinute[0];
                EEPROM[44] = maxHumMinute[1];
            }
        }
    }
}

byte generateChecksum(int len, byte arrayBytes[]) // Add checksum in frame for XBee
{
    uint16_t checksum = 0;

    for (int i = 3; i <= len; i++)
    {
        checksum += arrayBytes[i];
    }

    checksum = checksum & 0xFF;
    checksum = (0xFF - checksum);

    return checksum;
}

void NodeDiscover() // Search coordinador (AT Command)
{
    unsigned long startTime = millis();

    while (!macFound)
    {

        xbee.write(frameDiscover, sizeof(frameDiscover));

        while (millis() - startTime < 7000)
        {
        }

        while (xbee.available() > 0)
        {
            byte xbresp[3];
            xbee.readBytes(xbresp, 3);
            if (xbresp[0] == 0x88)
            {
                for (int i = 0; i < 4; i++)
                {
                    xbee.read();
                }

                for (int i = 0; i < 8; i++)
                {
                    macGW[i] = xbee.read();
                }
                int count = 0;
                byte NI[32];
                while (count < 32)
                {
                    byte dataByte = xbee.read();
                    if (dataByte == 0)
                    {
                        break;
                    }
                    NI[count] = dataByte;
                    count++;
                }
                NI[count] = '\0';
                if (strcmp((char *)NI, "Coordinador") == 0)
                {
                    /*
                    for (int i = 0; i < 8; i++)
                    {
                        Serial.print(macGW[i], HEX);
                        Serial.print(" ");
                    }
                    */
                    macFound = true;
                }
            }
        }
        startTime = millis();
    }
}

void MeassureVolts() // Meassure volts for Panel and Battery
{

    // UINT16_t batteryLevel, panelLevel;
    Batt = analogRead(A6);

    Pv = analogRead(A7);

    // Testing
    Batt = (Batt * readVcc() / 1024);
    Pv = (Pv * readVcc() / 1024);

    Batt = (Batt / (R2 / (R1 + R2))) / 1000;
    Pv = (Pv / (R2 / (R1 + R2))) / 1000;

    batteryLevel.value = (int16_t)(Batt * 100);
    panelLevel.value = (int16_t)(Pv * 100);

} // FIN MEASSUREVOLTS

long readVcc() // Read microchip volts
{
    long result;
#if defined(_AVR_ATmega32U4) || defined(__AVR_ATmega1280) || defined(__AVR_ATmega2560_)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(_AVR_ATtiny24) || defined(__AVR_ATtiny44) || defined(__AVR_ATtiny84_)
    ADMUX = _BV(MUX5) | _BV(MUX0);
#else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
    ADCSRA |= _BV(ADSC); // convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result; // back-calculate AVcc in mV
    return result;
}
