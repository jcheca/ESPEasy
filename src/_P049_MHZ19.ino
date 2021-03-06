/*

  This plug in is written by Dmitry (rel22 ___ inbox.ru)
  Plugin is based upon SenseAir plugin by Daniel Tedenljung info__AT__tedenljungconsulting.com
  Additional features based on https://geektimes.ru/post/285572/ by Gerben (infernix__AT__gmail.com)

  This plugin reads the CO2 value from MH-Z19 NDIR Sensor
  DevicePin1 - is RX for ESP
  DevicePin2 - is TX for ESP
*/

#ifdef PLUGIN_BUILD_TESTING

#define PLUGIN_049
#define PLUGIN_ID_049         49
#define PLUGIN_NAME_049       "NDIR CO2 Sensor MH-Z19 [TESTING]"
#define PLUGIN_VALUENAME1_049 "PPM"
#define PLUGIN_VALUENAME2_049 "Temperature" // Temperature in C
#define PLUGIN_VALUENAME3_049 "U" // Undocumented, minimum measurement per time period?
#define PLUGIN_READ_TIMEOUT   3000

boolean Plugin_049_init = false;

#include <SoftwareSerial.h>
SoftwareSerial *Plugin_049_S8;

// 9-bytes CMD PPM read command
byte mhzCmdReadPPM[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
byte mhzResp[9];    // 9 bytes bytes response
byte mhzCmdCalibrateZero[9] = {0xFF,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78};
byte mhzCmdABCEnable[9] = {0xFF,0x01,0x79,0xA0,0x00,0x00,0x00,0x00,0xE6};
byte mhzCmdABCDisable[9] = {0xFF,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86};
byte mhzCmdReset[9] = {0xFF,0x01,0x8d,0x00,0x00,0x00,0x00,0x00,0x72};
byte mhzCmdMeasurementRange1000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x03,0xE8,0x7B};
byte mhzCmdMeasurementRange2000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x07,0xD0,0x8F};
byte mhzCmdMeasurementRange3000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x0B,0xB8,0xA3};
byte mhzCmdMeasurementRange5000[9] = {0xFF,0x01,0x99,0x00,0x00,0x00,0x13,0x88,0xCB};

boolean Plugin_049(byte function, struct EventStruct *event, String& string)
{
  bool success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_049;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;
        Device[deviceCount].VType = SENSOR_TYPE_TRIPLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 3;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_049);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_049));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_049));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_049));
        break;
      }

    case PLUGIN_INIT:
      {
        Plugin_049_S8 = new SoftwareSerial(Settings.TaskDevicePin1[event->TaskIndex], Settings.TaskDevicePin2[event->TaskIndex]);
        Plugin_049_S8->begin(9600);
        String log = F("MHZ19: Init OK ");
        addLog(LOG_LEVEL_INFO, log);

        //delay first read, because hardware needs to initialize on cold boot
        //otherwise we get a weird value or read error
        timerSensor[event->TaskIndex] = millis() + 15000;

        Plugin_049_init = true;
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
        String log = "";
        String command = parseString(string, 1);

        if (command == F("mhzcalibratezero"))
        {
          Plugin_049_S8->write(mhzCmdCalibrateZero, 9);
          log = String(F("MHZ19: Calibrated zero point!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzreset"))
        {
          Plugin_049_S8->write(mhzCmdReset, 9);
          log = String(F("MHZ19: Sent sensor reset!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzabcenable"))
        {
          Plugin_049_S8->write(mhzCmdABCEnable, 9);
          log = String(F("MHZ19: Sent sensor ABC Enable!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzabcdisable"))
        {
          Plugin_049_S8->write(mhzCmdABCDisable, 9);
          log = String(F("MHZ19: Sent sensor ABC Disable!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzmeasurementrange1000"))
        {
          Plugin_049_S8->write(mhzCmdMeasurementRange1000, 9);
          log = String(F("MHZ19: Sent measurement range 0-1000PPM!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzmeasurementrange2000"))
        {
          Plugin_049_S8->write(mhzCmdMeasurementRange2000, 9);
          log = String(F("MHZ19: Sent measurement range 0-2000PPM!"));
          success = true;
          addLog(LOG_LEVEL_INFO, log);
        }

        if (command == F("mhzmeasurementrange3000"))
        {
          Plugin_049_S8->write(mhzCmdMeasurementRange3000, 9);
          log = String(F("MHZ19: Sent measurement range 0-3000PPM!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }

        if (command == F("mhzmeasurementrange5000"))
        {
          Plugin_049_S8->write(mhzCmdMeasurementRange5000, 9);
          log = String(F("MHZ19: Sent measurement range 0-5000PPM!"));
          addLog(LOG_LEVEL_INFO, log);
          success = true;
        }
        break;

      }

    case PLUGIN_READ:
      {

        if (Plugin_049_init)
        {
          //send read PPM command
          int nbBytesSent = Plugin_049_S8->write(mhzCmdReadPPM, 9);
          if (nbBytesSent != 9) {
            String log = F("MHZ19: Error, nb bytes sent != 9 : ");
              log += nbBytesSent;
              addLog(LOG_LEVEL_INFO, log);
          }

          // get response
          memset(mhzResp, 0, sizeof(mhzResp));

          long start = millis();
          int counter = 0;
          while (((millis() - start) < PLUGIN_READ_TIMEOUT) && (counter < 9)) {
            if (Plugin_049_S8->available() > 0) {
              mhzResp[counter++] = Plugin_049_S8->read();
            } else {
              delay(10);
            }
          }
          if (counter < 9){
              String log = F("MHZ19: Error, timeout while trying to read");
              addLog(LOG_LEVEL_INFO, log);
          }

          unsigned int ppm = 0;
          int i;
          signed int temp = 0;
          unsigned int s = 0;
          float u = 0;
          byte crc = 0;
          for (i = 1; i < 8; i++) crc+=mhzResp[i];
              crc = 255 - crc;
              crc++;

          if ( !(mhzResp[8] == crc) ) {
             String log = F("MHZ19: Read error : CRC = ");
             log += String(crc); log += " / "; log += String(mhzResp[8]);
             log += " bytes read  => ";for (i = 0; i < 9; i++) {log += mhzResp[i];log += "/" ;}
             addLog(LOG_LEVEL_ERROR, log);

             // Sometimes there is a misalignment in the serial read
             // and the starting byte 0xFF isn't the first read byte.
             // This goes on forever.
             // There must be a better way to handle this, but here
             // we're trying to shift it so that 0xFF is the next byte
             byte crcshift;
             for (i = 1; i < 8; i++) {
                crcshift = Plugin_049_S8->peek();
                if (crcshift == 0xFF) {
                  String log = F("MHZ19: Shifted ");
                  log += i;
                  log += F(" bytes to attempt to fix buffer alignment");
                  addLog(LOG_LEVEL_ERROR, log);
                  break;
                } else {
                 crcshift = Plugin_049_S8->read();
                }
             }
             success = false;
             break;

          // Process responses to 0x86
          } else if (mhzResp[0] == 0xFF && mhzResp[1] == 0x86 && mhzResp[8] == crc)  {

              //calculate CO2 PPM
              unsigned int mhzRespHigh = (unsigned int) mhzResp[2];
              unsigned int mhzRespLow = (unsigned int) mhzResp[3];
              ppm = (256*mhzRespHigh) + mhzRespLow;

              // set temperature (offset 40)
              unsigned int mhzRespTemp = (unsigned int) mhzResp[4];
              temp = mhzRespTemp - 40;

              // set 's' (stability) value
              unsigned int mhzRespS = (unsigned int) mhzResp[5];
              s = mhzRespS;

              // calculate 'u' value
              unsigned int mhzRespUHigh = (unsigned int) mhzResp[6];
              unsigned int mhzRespULow = (unsigned int) mhzResp[7];
              u = (256*mhzRespUHigh) + mhzRespULow;

              String log = F("MHZ19: ");

              // During (and only ever at) sensor boot, 'u' is reported as 15000
              // We log but don't process readings during that time
              if (u == 15000) {

                log += F("Bootup detected! ");
                success = false;

              // If s = 0x40 the reading is stable; anything else should be ignored
              } else if (s < 64) {

                log += F("Unstable reading, ignoring! ");
                success = false;

              // Finally, stable readings are used for variables
              } else {

                success = true;

                UserVar[event->BaseVarIndex] = (float)ppm;
                UserVar[event->BaseVarIndex + 1] = (float)temp;
                UserVar[event->BaseVarIndex + 2] = (float)u;
              }

              // Log values in all cases
              log += F("PPM value: ");
              log += ppm;
              log += F(" Temp/S/U values: ");
              log += temp;
              log += F("/");
              log += s;
              log += F("/");
              log += u;
              addLog(LOG_LEVEL_INFO, log);
              break;

          // Sensor responds with 0x99 whenever we send it a measurement range adjustment
          } else if (mhzResp[0] == 0xFF && mhzResp[1] == 0x99 && mhzResp[8] == crc)  {

            String log = F("MHZ19: Received measurement range acknowledgment! ");
            log += F("Expecting sensor reset...");
            addLog(LOG_LEVEL_INFO, log);
            success = false;
            break;

          // log verbosely anything else that the sensor reports
          } else {

              String log = F("MHZ19: Unknown response: ");
              log += String(mhzResp[0], HEX);
              log += F(" ");
              log += String(mhzResp[1], HEX);
              log += F(" ");
              log += String(mhzResp[2], HEX);
              log += F(" ");
              log += String(mhzResp[3], HEX);
              log += F(" ");
              log += String(mhzResp[4], HEX);
              log += F(" ");
              log += String(mhzResp[5], HEX);
              log += F(" ");
              log += String(mhzResp[6], HEX);
              log += F(" ");
              log += String(mhzResp[7], HEX);
              log += F(" ");
              log += String(mhzResp[8], HEX);
              addLog(LOG_LEVEL_INFO, log);
              success = false;
              break;

          }

        }
        break;
      }
  }
  return success;
}

#endif
