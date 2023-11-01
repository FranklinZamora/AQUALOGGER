#include "library.h"
// SensorsNAS sensors(O2, PH, EC, ORP);

UINT16_t temp, hum, waterTemp, maxtemp, mintemp, maxhum, minhum, maxwaterTemp, minwaterTemp;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature senW(&oneWire);

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    xbee.begin(9600);
    senW.begin();
    sht.begin(0x44);
    Wire.setClock(100000);

    for (int i = 0; i < 8; i++)
    {
        macGW[i] = EEPROM.read(i);
        Serial.print(EEPROM[i], HEX);
        Serial.print(" ");
    }

    Serial.println();

    if (EEPROM[0] != 0xFF && EEPROM[1] != 0xFF && EEPROM[2] != 0xFF && EEPROM[3] != 0xFF && EEPROM[4] != 0xFF && EEPROM[5] != 0xFF && EEPROM[6] != 0xFF && EEPROM[7] != 0xFF)
    {
        for (int i = 0; i <= 7; i++)
        {
            macGW[i] = EEPROM[i];
            Serial.print(EEPROM[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        macFound = true;
    }
    else
    {
        NodeDiscover();
    }

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
            Serial.println("AQUALOGGER ON");
        }
        else
        {
            setupInitial = true;
            tOnAQUA = false;
            Serial.println("AQUALOGGER OFF");
        }
    }

    byte vSend[] = {0x7E, 0x00, 0x16, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x41, 0x51, 0x55, 0x41, 0x56, 0x30, 0x2E, 0x31, generateChecksum(24, vSend)};
    debug(vSend, sizeof(vSend));
    xbee.write(vSend, sizeof(vSend));
}

void loop()
{

    GetGps();

    sht.read();
    hum.value = sht.getHumidity() * 100;
    temp.value = sht.getTemperature() * 100;
    senW.requestTemperatures();
    waterTemp.value = senW.getTempCByIndex(0) * 100;

    debugSens();

    if (currentGPSday != lastGPSday || hourUTC - lastGPShour >= 24)
    {
        // Ha pasado un nuevo día o más de 24 horas desde la última actualización
        // Restablece los valores máximos y mínimos
        maxhum.value = 0;
        minhum.value = 0xFFFF;
        maxtemp.value = 0;
        mintemp.value = 0xFFFF;
        maxwaterTemp.value = 0;
        minwaterTemp.value = 0xFFFF;

        _MaxHum = 0;
        _MinHum = 9999;

        _MaxTemp = 0;
        _MinTemp = 9999;

        _MaxWaterTemp = 0;
        _MinWaterTemp = 9999;

        // Actualiza la última hora del GPS y fecha de reinicio
        lastGPShour = hourUTC;
        lastGPSday = currentGPSday;
    }

    // Convertir la hora y los minutos en formato decimal ASCII
    char decHour[3];
    char decMinute[3];

    sprintf(decHour, "%02d", hourUTC);
    sprintf(decMinute, "%02d", minuteUTC);

    // Max Hunidity
    if (hum.value > _MaxHum)
    {
        _MaxHum = hum.value;
        maxhum.value = _MaxHum;
        // Almacenar la hora en formato ASCII en maxHumHour y maxHumMinute
        maxHumHour[0] = decHour[0];
        maxHumHour[1] = decHour[1];
        maxHumMinute[0] = decMinute[0];
        maxHumMinute[1] = decMinute[1];

        if (EEPROM[8] == 0x01) // Pendiente las demas variables
        {
            EEPROM.write(9, maxhum.value);
            EEPROM.write(10, maxHumHour[0]);
            EEPROM.write(11, maxHumHour[1]);
            EEPROM.write(12, maxHumMinute[0]);
            EEPROM.write(13, maxHumMinute[1]);
        }
    }

    // Min Humidity
    if (hum.value < _MinHum)
    {
        _MinHum = hum.value;
        minhum.value = _MinHum;

        minHumHour[0] = decHour[0];
        minHumHour[1] = decHour[1];
        minHumMinute[0] = decMinute[0];
        minHumMinute[1] = decMinute[1];

        if (EEPROM[8] == 0x01) // Pendiente las demas variables
        {
            EEPROM.write(14, minhum.value);
            EEPROM.write(15, minHumHour[0]);
            EEPROM.write(16, minHumHour[1]);
            EEPROM.write(17, minHumMinute[0]);
            EEPROM.write(18, minHumMinute[1]);
        }
    }

    // Max temp
    if (temp.value > _MaxTemp)
    {
        _MaxTemp = temp.value;
        maxtemp.value = _MaxTemp;
        // Almacenar la hora en formato ASCII en maxHumHour y maxHumMinute
        maxTempHour[0] = decHour[0];
        maxTempHour[1] = decHour[1];
        maxTempMinute[0] = decMinute[0];
        maxTempMinute[1] = decMinute[1];

        if (EEPROM[8] == 0x01) // Pendiente las demas variables
        {
            EEPROM.write(19, maxtemp.value);
            EEPROM.write(20, maxTempHour[0]);
            EEPROM.write(21, maxTempHour[1]);
            EEPROM.write(22, maxTempMinute[0]);
            EEPROM.write(23, maxTempMinute[1]);
        }
    }

    // Min temp
    if (temp.value < _MaxTemp)
    {
        _MinTemp = temp.value;
        mintemp.value = _MinTemp;
        // Almacenar la hora en formato ASCII en maxHumHour y maxHumMinute
        minTempHour[0] = decHour[0];
        minTempHour[1] = decHour[1];
        minTempMinute[0] = decMinute[0];
        minTempMinute[1] = decMinute[1];

        if (EEPROM[8] == 0x01) // Pendiente las demas variables
        {
            EEPROM.write(24, mintemp.value);
            EEPROM.write(25, minTempHour[0]);
            EEPROM.write(26, minTempHour[1]);
            EEPROM.write(27, minTempMinute[0]);
            EEPROM.write(28, minTempMinute[1]);
        }
    }

    // Max  water temp
    if (waterTemp.value > _MaxWaterTemp)
    {
        _MaxWaterTemp = waterTemp.value;
        maxwaterTemp.value = _MaxWaterTemp;
        // Almacenar la hora en formato ASCII en maxHumHour y maxHumMinute
        maxWaterTempHour[0] = decHour[0];
        maxWaterTempHour[1] = decHour[1];
        maxWaterTempMinute[0] = decMinute[0];
        maxWaterTempMinute[1] = decMinute[1];

        if (EEPROM[8] == 0x01) // Pendiente las demas variables
        {
            EEPROM.write(29, maxwaterTemp.value);
            EEPROM.write(30, maxWaterTempHour[0]);
            EEPROM.write(31, maxWaterTempHour[1]);
            EEPROM.write(32, maxWaterTempMinute[0]);
            EEPROM.write(33, maxWaterTempMinute[1]);
        }
    }

    // Min water temp
    if (waterTemp.value < _MinWaterTemp)
    {
        _MinWaterTemp = waterTemp.value;
        minwaterTemp.value = _MinWaterTemp;
        // Almacenar la hora en formato ASCII en maxHumHour y maxHumMinute
        minWaterTempHour[0] = decHour[0];
        minWaterTempHour[1] = decHour[1];
        minWaterTempMinute[0] = decMinute[0];
        minWaterTempMinute[1] = decMinute[1];

        if (EEPROM[8] == 0x01) // Pendiente las demas variables
        {
            EEPROM.write(34, minwaterTemp.value);
            EEPROM.write(35, minWaterTempHour[0]);
            EEPROM.write(36, minWaterTempHour[1]);
            EEPROM.write(37, minWaterTempMinute[0]);
            EEPROM.write(38, minWaterTempMinute[1]);
        }
    }

    // Histeresis Min Water Temp
    if (alert == false && (waterTemp.value / 100) < minSPtemperature && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[27];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x17;
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
        rAlertSPMin[22] = decHour[0];
        rAlertSPMin[23] = decHour[1];
        rAlertSPMin[24] = decMinute[0];
        rAlertSPMin[25] = decMinute[1];
        rAlertSPMin[26] = generateChecksum(25, rAlertSPMin);

        for (int i = 0; i < sizeof(rAlertSPMin); i++)
        {
            Serial.print(rAlertSPMin[i], HEX);
            if (rAlertSPMin[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(" ");
        }
        Serial.println();

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        Serial.println("[ALERT]: SET POINT MIN WATER TEMP");
        alert = true;

        return;
    }

    // Histeresis Max Water Temp
    if (alert == false && (waterTemp.value / 100) > maxSPtemperature && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[27];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x17;
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
        rAlertSPMin[22] = decHour[0];
        rAlertSPMin[23] = decHour[1];
        rAlertSPMin[24] = decMinute[0];
        rAlertSPMin[25] = decMinute[1];
        rAlertSPMin[26] = generateChecksum(25, rAlertSPMin);

        for (int i = 0; i < sizeof(rAlertSPMin); i++)
        {
            Serial.print(rAlertSPMin[i], HEX);
            if (rAlertSPMin[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(" ");
        }
        Serial.println();

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        Serial.println("[ALERT]: SET POINT MAX WATER TEMP");
        alert = true;

        return;
    }

    // Histeresis Min Enviroment Temp
    if (alert == false && (temp.value / 100) < minSPEnviromentTemp && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[27];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x17;
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
        rAlertSPMin[22] = decHour[0];
        rAlertSPMin[23] = decHour[1];
        rAlertSPMin[24] = decMinute[0];
        rAlertSPMin[25] = decMinute[1];
        rAlertSPMin[26] = generateChecksum(25, rAlertSPMin);

        for (int i = 0; i < sizeof(rAlertSPMin); i++)
        {
            Serial.print(rAlertSPMin[i], HEX);
            if (rAlertSPMin[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(" ");
        }
        Serial.println();

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        Serial.println("[ALERT]: SET POINT MIN ENVIROMENT TEMP");
        alert = true;

        return;
    }

    // Histeresis Max Enviroment Temp
    if (alert == false && (temp.value / 100) > maxSPEnviromentTemp && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[27];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x17;
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
        rAlertSPMin[22] = decHour[0];
        rAlertSPMin[23] = decHour[1];
        rAlertSPMin[24] = decMinute[0];
        rAlertSPMin[25] = decMinute[1];
        rAlertSPMin[26] = generateChecksum(25, rAlertSPMin);

        for (int i = 0; i < sizeof(rAlertSPMin); i++)
        {
            Serial.print(rAlertSPMin[i], HEX);
            if (rAlertSPMin[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(" ");
        }
        Serial.println();

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        Serial.println("[ALERT]: SET POINT MAX ENVIROMENT TEMP");
        alert = true;

        return;
    }

    // Histeresis Min Enviroment Hum
    if (alert == false && (hum.value / 100) < minSPEnviromentHum && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[27];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x17;
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
        rAlertSPMin[20] = hum.bytes[1];
        rAlertSPMin[21] = hum.bytes[0];
        rAlertSPMin[22] = decHour[0];
        rAlertSPMin[23] = decHour[1];
        rAlertSPMin[24] = decMinute[0];
        rAlertSPMin[25] = decMinute[1];
        rAlertSPMin[26] = generateChecksum(25, rAlertSPMin);

        for (int i = 0; i < sizeof(rAlertSPMin); i++)
        {
            Serial.print(rAlertSPMin[i], HEX);
            if (rAlertSPMin[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(" ");
        }
        Serial.println();

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        Serial.println("[ALERT]: SET POINT MIN ENVIROMENT HUM");
        alert = true;

        return;
    }

    // Histeresis Max Enviroment Hum
    if (alert == false && (hum.value / 100) > maxSPEnviromentHum && flagSP == true && setSP == true)
    {
        byte rAlertSPMin[27];
        rAlertSPMin[0] = 0x7E;
        rAlertSPMin[1] = 0x00;
        rAlertSPMin[2] = 0x17;
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
        rAlertSPMin[20] = hum.bytes[1];
        rAlertSPMin[21] = hum.bytes[0];
        rAlertSPMin[22] = decHour[0];
        rAlertSPMin[23] = decHour[1];
        rAlertSPMin[24] = decMinute[0];
        rAlertSPMin[25] = decMinute[1];
        rAlertSPMin[26] = generateChecksum(25, rAlertSPMin);

        for (int i = 0; i < sizeof(rAlertSPMin); i++)
        {
            Serial.print(rAlertSPMin[i], HEX);
            if (rAlertSPMin[i] < 0x10)
            {
                Serial.print("0");
            }
            Serial.print(" ");
        }
        Serial.println();

        xbee.write(rAlertSPMin, sizeof(rAlertSPMin));

        Serial.println("[ALERT]: SET POINT MAX ENVIROMENT HUM");
        alert = true;

        return;
    }

    while (xbee.available() > 0) // Read xbee
    {
        for (int i = 0; i < 35; i++)
        {
            request[i] = xbee.read();
            Serial.print(request[i], HEX);
            Serial.print(" ");
        }

        Serial.println();

        if (request[0] == 0x7E) // Header ?
        {
            if (request[4] == macGW[0] && request[5] == macGW[1] && request[6] == macGW[2] && request[7] == macGW[3] && request[8] == macGW[4] && request[9] == macGW[5] && request[10] == macGW[6] && request[11] == macGW[7]) // Mac coordinator ?
            {
                if (request[2] == 0x0E && request[15] == 0x54) // Gateway
                {
                    if (request[16] == 0x00) // Off Gateway
                    {
                        Serial.println("\n[INFO]: Turn Off Gateway.");
                        byte tOff_GW[] = {0x7E, 0x00, 0x12, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x54, 0x4F, 0x46, 0x46, generateChecksum(20, tOff_GW)};
                        xbee.write(tOff_GW, sizeof(tOff_GW));
                        EEPROM.write(8, 0x00); // 00 Apagado virtual, Evaluar MAC
                        debug(tOff_GW, sizeof(tOff_GW));
                    }

                    if (request[16] == 0x01) // On Gateway
                    {
                        Serial.println("\n[INFO]: Turn ON Gateway.");

                        EEPROM.write(8, 0x01); // 01 encendido virtual

                        byte tON_GW[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x54, 0x4F, 0x4E, generateChecksum(19, tON_GW)};
                        debug(tON_GW, sizeof(tON_GW));
                        xbee.write(tON_GW, sizeof(tON_GW));
                    }
                }

                if (request[2] == 0x0D && request[15] == 0x3F) // Version ?
                {
                    Serial.println("\n[INFO]: Version send.");
                    byte vSend[] = {0x7E, 0x00, 0x16, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x41, 0x51, 0x55, 0x41, 0x56, 0x30, 0x2E, 0x31, generateChecksum(24, vSend)};
                    debug(vSend, sizeof(vSend));
                    xbee.write(vSend, sizeof(vSend));
                }

                if (request[2] == 0x15 && request[15] == 0x47) // Change MAC
                {
                    Serial.println("\n[INFO]: MAC Changed.");

                    macGW[0] = request[16];
                    macGW[1] = request[17];
                    macGW[2] = request[18];
                    macGW[3] = request[19];
                    macGW[4] = request[20];
                    macGW[5] = request[21];
                    macGW[6] = request[22];
                    macGW[7] = request[23];

                    for (int i = 0; i < 8; i++)
                    {
                        EEPROM.write(i, macGW[i]);
                        Serial.print(macGW[i], HEX);
                        Serial.print(" ");
                    }
                    Serial.println();
                }

                if (request[2] == 0x0E && request[15] == 0x73) // First sensors
                {
                    if (request[16] == 0x00) // Off
                    {
                        Serial.println("[INFO]: Sensors 1 turn OFF");
                        byte tOFF_Sensors1[] = {0x7E, 0x00, 0x11, 0x10, 0x01, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x4F, 0x46, 0x46, generateChecksum(19, tOFF_Sensors1)};
                        debug(tOFF_Sensors1, sizeof(tOFF_Sensors1));
                        xbee.write(tOFF_Sensors1, sizeof(tOFF_Sensors1));
                        flagSensors = false;
                    }

                    else if (request[16] == 0x01) // On first sensors
                    {
                        Serial.println("[INFO]: Sensors 1 turn ON.");
                        byte tON_Sensors1[] = {0x7E, 0x00, 0x10, 0x10, 0x01, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x4F, 0x4E, generateChecksum(18, tON_Sensors1)};
                        debug(tON_Sensors1, sizeof(tON_Sensors1));
                        xbee.write(tON_Sensors1, sizeof(tON_Sensors1));

                        flagSensors = true;
                    }

                    else if (request[16] == 0x03 && flagSensors == true) // Request first sensors
                    {

                        if (isTimeSynced)
                        {
                            Serial.println("[INFO]: Send first packet sensors data with clock and max & min");
                            sense1();
                        }
                        else
                        {
                            Serial.println("Send first packet sensors whithout clock");

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

                            debug(requestSens_1, sizeof(requestSens_1));
                            xbee.write(requestSens_1, sizeof(requestSens_1));
                        }
                    }
                }
                if (request[2] == 0x0E && request[15] == 0x53) // Set points ON
                {

                    if (request[16] == 0x00) // Set points OFF
                    {
                        Serial.println("[INFO]: Set Points OFF");
                        byte tOFF_SP[] = {0x7E, 0x00, 0x12, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x53, 0x4F, 0x46, 0x46, generateChecksum(20, tOFF_SP)};
                        debug(tOFF_SP, sizeof(tOFF_SP));
                        xbee.write(tOFF_SP, sizeof(tOFF_SP));
                        flagSP = false;
                    }

                    if (request[16] == 0x01) // Set points ON
                    {
                        Serial.println("[INFO]: Set Points ON");
                        byte tON_SP[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x53, 0x4F, 0x4E, generateChecksum(19, tON_SP)};
                        debug(tON_SP, sizeof(tON_SP));
                        xbee.write(tON_SP, sizeof(tON_SP));
                        flagSP = true;
                    }
                }

                if (flagSP == true && request[2] == 0x1A && request[15] == 0x53 && request[16] == 0x02) // Set SP
                {
                    Serial.println("Set Points SET");
                    min_SPtemp1 = request[17];
                    min_SPtemp2 = request[18];
                    max_SPtemp1 = request[19];
                    max_SPtemp2 = request[20];
                    min_SPEnviromentTemp1 = request[21];
                    min_SPEnviromentTemp2 = request[22];
                    max_SPEnviromentTemp1 = request[23];
                    max_SPEnviromentTemp2 = request[24];
                    min_SPEnviromentHum1 = request[25];
                    min_SPEnviromentHum2 = request[26];
                    max_SPEnviromentHum1 = request[27];
                    max_SPEnviromentHum2 = request[28];

                    minSPtemp1 = min_SPtemp1;
                    minSPtemp2 = min_SPtemp2 / 10.0;
                    minSPtemperature = minSPtemp1 + minSPtemp2;

                    maxSPtemp1 = max_SPtemp1;
                    maxSPtemp2 = max_SPtemp2 / 10.0;
                    maxSPtemperature = maxSPtemp1 + maxSPtemp2;

                    minSPEnvTemp1 = min_SPEnviromentTemp1;
                    minSPEnvTemp2 = min_SPEnviromentTemp2 / 10.0;
                    minSPEnviromentTemp = minSPEnvTemp1 + minSPEnvTemp2;

                    maxSPEnvTemp1 = max_SPEnviromentTemp1;
                    maxSPEnvTemp2 = max_SPEnviromentTemp2 / 10.0;
                    maxSPEnviromentTemp = maxSPEnvTemp1 + maxSPEnvTemp2;

                    minSPEnvHum1 = min_SPEnviromentHum1;
                    minSPEnvHum2 = min_SPEnviromentHum2 / 10.0;
                    minSPEnviromentHum = minSPEnvHum1 + minSPEnvHum2;

                    maxSPEnvHum1 = max_SPEnviromentHum1;
                    maxSPEnvHum2 = max_SPEnviromentHum2 / 10.0;
                    maxSPEnviromentHum = maxSPEnvHum1 + maxSPEnvHum2;

                    Serial.println(maxSPtemperature);
                    Serial.println(minSPtemperature);

                    Serial.println(maxSPEnviromentTemp);
                    Serial.println(minSPEnviromentTemp);

                    Serial.println(maxSPEnviromentHum);
                    Serial.println(minSPEnviromentHum);

                    if (EEPROM[8] == 0x01)
                    {
                        EEPROM[39] = maxSPtemperature;
                        EEPROM[40] = minSPtemperature;
                        EEPROM[41] = maxSPEnviromentTemp;
                        EEPROM[42] = minSPEnviromentTemp;
                        EEPROM[43] = maxSPEnviromentHum;
                        EEPROM[44] = minSPEnviromentHum;
                    }

                    byte tOK_SP[] = {0x7E, 0x00, 0x11, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x53, 0x4F, 0x4B, generateChecksum(19, tOK_SP)};
                    debug(tOK_SP, sizeof(tOK_SP));
                    xbee.write(tOK_SP, sizeof(tOK_SP));
                    setSP = true;
                }

                if (request[2] == 0x0D && request[15] == 0x52) // Alarm reset
                {
                    alert = false;
                    Serial.println("[INFO]: Alarm Reset");
                    byte rAlarm[] = {0x7E, 0x00, 0x10, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x52, 0x52, generateChecksum(18, rAlarm)};
                    debug(rAlarm, sizeof(rAlarm));
                    xbee.write(rAlarm, sizeof(rAlarm));
                }

                if (request[2] == 0x0E && request[15] == 0x79 && request[16] == 0x3F)
                {
                    Serial.println("[INFO]: Configuration auto request");
                }

                if (request[2] == 0x0E && request[15] == 0x79 && request[16] == 0x01)
                {
                    Serial.println("[INFO]: Request 1HR");

                    rHr = true;
                    r3Hrs = false;
                    r5Hrs = false;
                }

                if (request[2] == 0x11 && request[15] == 0x79 && request[16] == 0x03)
                {
                    Serial.println("[SEND]: Request 3HRs");
                    hr1 = request[17];
                    Serial.println(hr1);
                    hr2 = request[18];
                    Serial.println(hr2);
                    hr3 = request[19];
                    Serial.println(hr3);

                    rHr = false;
                    r3Hrs = true;
                    r5Hrs = false;
                }

                if (request[2] == 0x13 && request[15] == 0x79 && request[16] == 0x05)
                {
                    Serial.println("[SEND]: Request  5HRs");

                    hr1 = (int)request[17];
                    Serial.println(hr1);
                    hr2 = (int)request[18];
                    Serial.println(hr2);
                    hr3 = (int)request[19];
                    Serial.println(hr3);
                    hr4 = (int)request[20];
                    Serial.println(hr4);
                    hr5 = (int)request[21];
                    Serial.println(hr5);

                    rHr = false;
                    r3Hrs = false;
                    r5Hrs = true;
                }

                if (request[2] == 0x0E && request[15] == 0x48 && request[16] == 0x01)
                {
                    Serial.println("[INFO]: Sensors 2 turn ON.");
                    byte tON_Sensors2[] = {0x7E, 0x00, 0x10, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x01, generateChecksum(18, tON_Sensors2)};
                    debug(tON_Sensors2, sizeof(tON_Sensors2));
                    xbee.write(tON_Sensors2, sizeof(tON_Sensors2));
                    flagSensors2 = true;
                }

                if (request[2] == 0x0E && request[15] == 0x48 && request[16] == 0x00)
                {
                    Serial.println("[INFO]: Sensors 2 turn OFF.");
                    byte tOFF_Sensors2[] = {0x7E, 0x00, 0x10, 0x10, 0x00, macGW[0], macGW[1], macGW[2], macGW[3], macGW[4], macGW[5], macGW[6], macGW[7], 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x00, generateChecksum(18, tOFF_Sensors2)};
                    debug(tOFF_Sensors2, sizeof(tOFF_Sensors2));
                    xbee.write(tOFF_Sensors2, sizeof(tOFF_Sensors2));
                    flagSensors2 = false;
                }
                if (request[2] == 0x0E && request[15] == 0x48 && request[16] == 0x3F && flagSensors2 == true)
                {
                    Serial.println("[INFO]: Request Real Time.");
                    //GenerateArray
                }
            }
        }
    }

    if (rHr == true && isTimeSynced == true && flagSensors == true)
    {
        if (hourUTC != previousHourUTC)
        {
            sense1();
            previousHourUTC = hourUTC; // Actualiza el valor anterior
        }
    }

    if (r3Hrs == true && isTimeSynced == true && flagSensors == true)
    {
        if (hourUTC == hr1 || hourUTC == hr2 || hourUTC == hr3)
        {
            sense1();
        }
    }

    if (r5Hrs == true && isTimeSynced == true && flagSensors == true)
    {
        if (hourUTC == hr1 || hourUTC == hr2 || hourUTC == hr3 || hourUTC == hr4 || hourUTC == hr5)
        {
            sense1();
        }
    }
}

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

static void smartdelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (Serial.available())
            gps.encode(Serial.read());
    } while (millis() - start < ms);
}

void GetGps(void)
{
    while (Serial.available() > 0)
    {
        if (gps.encode(Serial.read()))
        {
            if (gps.date.isValid())
            {
                int year = gps.date.year();
                int month = gps.date.month();
                int day = gps.date.day();
                Serial.print("Fecha: ");
                Serial.print(year);
                Serial.print("-");
                Serial.print(month);
                Serial.print("-");
                Serial.println(day);
            }

            if (gps.time.isValid())
            {
                int hour = gps.time.hour();
                int minute = gps.time.minute();
                int second = gps.time.second();
                Serial.print("Hora: ");
                Serial.print(hour);
                Serial.print(":");
                Serial.print(minute);
                Serial.print(":");
                Serial.println(second);
            }
        }
    }

    smartdelay(1000);
}

void debugSens()
{
    if (flagSensors == true)
    {
        Serial.print("Temp:");
        Serial.print(temp.value / 100);
        Serial.print(" Hum:");
        Serial.print(hum.value / 100);
        Serial.print(" Water Temp:");
        Serial.println(waterTemp.value / 100);
    }
}

void sense1()
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
    debug(requestSens1_1, sizeof(requestSens1_1));
}

byte generateChecksum(int len, byte arrayBytes[])
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

void NodeDiscover()
{
    while (!macFound)
    {
        Serial.println("Searching coordinator...");
        xbee.write(frameDiscover, sizeof(frameDiscover));

        delay(15000); // Wait for a response
        while (xbee.available() > 0)
        {
            byte xbresp[3];
            xbee.readBytes(xbresp, 3);

            if (xbresp[0] == 0x88)
            {
                // Ignorar los primeros 4 bytes después de 0x88
                for (int i = 0; i < 4; i++)
                {
                    xbee.read(); // Leer y descartar estos bytes
                }
                // Leer los siguientes 8 bytes y guardarlos en macID
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

                if (strstr((char *)NI, "Coordinador"))
                {
                    Serial.println("Coordinador found");
                    Serial.println("Ready ✓");

                    for (int i = 0; i < 8; i++)
                    {
                        Serial.print(macGW[i], HEX);
                        Serial.print(" ");
                    }

                    macFound = true;
                }
            }
        }
    }
}
