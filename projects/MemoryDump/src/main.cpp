#include <Arduino.h>
#include <nfc_dynamic_tag.h>
#include <Wire.h>

NXP_NTAG_I2C ntag(0x55);
char ui_buffer[64];

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    ntag.begin();
}

void loop()
{

    static uint8_t command;
    promptMenu();

    Serial.flush();

    if (!Serial.available())
    {
        command = read_int();
        Serial.println(command);
        Serial.println();

        switch (command)
        {
        case 1:
            ntag.UserMemoryDump();
            break;
        case 2:
            ntag.GetSerialNumber();
            break;
        case 3:
            ntag.GetCapabilityContainer();
            break;
        case 4:
            ntag.GetStaticLockStatus();
            break;
        case 5:
            ntag.GetConfigurationStatus();
            break;
        case 6:
            ntag.GetSessionStatus();
            break;
        case 7:
            ntag.GetNTAGFullReport();
            break;
        default:
            Serial.println("Incorrect Option");
            break;
        }
    }
}

void promptMenu()
{
    Serial.print(F("\n*************************Read Memory Menu*************************\n"));
    Serial.print(F("\n1-Memory Dump\n"));
    Serial.print(F("2-Serial Number\n"));
    Serial.print(F("3-Capability Container\n"));
    Serial.print(F("4-Static Lock Status\n"));
    Serial.print(F("5-Configuration Status\n"));
    Serial.print(F("6-Session Status\n"));
    Serial.print(F("7-Full NTAG Report \n\n"));
    Serial.print(F("Enter a command: "));
}

uint8_t read_data()
{
    uint8_t index = 0;
    int c;
    while (index < 63)
    {
        c = Serial.read();
        if (((char)c == '\r') || ((char)c == '\n'))
            break;
        if (((char)c == '\x7F') || ((char)c == '\x08'))
        {
            if (index > 0)
                index--;
        }
        else if (c >= 0)
        {
            ui_buffer[index++] = (char)c;
        }
    }
    ui_buffer[index] = '\0'; // terminate string with NULL

    if ((char)c == '\r')
    {
        delay(10);
        if (Serial.peek() == '\n')
            Serial.read();
    }

    return index;
}

int32_t read_int()
{
    int32_t data;
    read_data();
    if (ui_buffer[0] == 'm')
        return ('m');
    if ((ui_buffer[0] == 'B') || (ui_buffer[0] == 'b'))
    {
        data = strtol(ui_buffer + 1, NULL, 2);
    }
    else
        data = strtol(ui_buffer, NULL, 0);
    return (data);
}