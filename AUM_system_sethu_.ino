#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <String.h>
ModbusMaster node;
SoftwareSerial MOD_Serial(13, 12); // RX, TX
SoftwareSerial GSM_Serial(3, 4); // RX, TX
#define set_reg 2
#define Analog0 A2
#define Analog1 A3
#define Analog2 A4
#define Analog3 A5
#define Analog4 A6
char temp_char[82];
int reg_status, data_length = 0;
String data, config_check;
int reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10, reg11, reg12, reg13, reg14, reg15, reg16, reg17, reg18, reg19;
String floating, integer, decimal, calender;
unsigned long currentTime = 0;
unsigned long tmr_20s = 0; // Millis Calculation
String analog, full_data1, full_data2, full_data3, full_data4,full_data5;
String ful_data1,ful_data2,ful_data3,ful_data4,ful_data5;
void setup()
{
  pinMode(set_reg, INPUT_PULLUP);
  Serial.begin(9600);
  delay(1000);
  //  MOD_Serial.begin(9600);
  //  GSM_Serial.begin(9600);
  // Modbus slave ID 1
  node.begin(1, MOD_Serial);  /////////////////////////node id//////////////////////////////////////////////
  pinMode(Analog0, INPUT);
  pinMode(Analog1, INPUT);
  pinMode(Analog2, INPUT);
  pinMode(Analog3, INPUT);
  pinMode(Analog4, INPUT);
  delay(1000);
  read_config();
  delay(1000);
}
void data_process()
{
  String config_check, config_reg1, config_reg2, config_reg3, config_reg4, config_reg5, config_reg6, config_reg7, config_reg8, config_reg9, config_reg10, config_reg11, config_reg12, config_reg13, config_reg14, config_reg15, config_reg16, config_reg17, config_reg18, config_reg19;
  config_reg1 = data.substring(4, 8);  config_reg2 = data.substring(8, 12);  config_reg3 = data.substring(12, 16);  config_reg4 = data.substring(16, 20);  config_reg5 = data.substring(20, 24);  config_reg6 = data.substring(24, 28);
  config_reg7 = data.substring(28, 32);  config_reg8 = data.substring(32, 36);  config_reg9 = data.substring(36, 40);  config_reg10 = data.substring(40, 44);  config_reg11 = data.substring(44, 48);  config_reg12 = data.substring(48, 52);
  config_reg13 = data.substring(52, 56);  config_reg14 = data.substring(56, 60);  config_reg15 = data.substring(60, 64);  config_reg16 = data.substring(64, 68);  config_reg17 = data.substring(68, 72);  config_reg18 = data.substring(72, 76);
  config_reg19 = data.substring(76, 80);
  reg1 = config_reg1.toInt();  reg2 = config_reg2.toInt();  reg3 = config_reg3.toInt();  reg4 = config_reg4.toInt();  reg5 = config_reg5.toInt();  reg6 = config_reg6.toInt();
  reg7 = config_reg7.toInt();  reg8 = config_reg8.toInt();  reg9 = config_reg9.toInt();  reg10 = config_reg10.toInt();  reg11 = config_reg11.toInt();  reg12 = config_reg12.toInt();
  reg13 = config_reg13.toInt();  reg14 = config_reg14.toInt();  reg15 = config_reg15.toInt();  reg16 = config_reg16.toInt();  reg17 = config_reg17.toInt();  reg18 = config_reg18.toInt();
  reg19 = config_reg19.toInt();
  Serial.print("Config Register:");
  Serial.print(reg1);  Serial.print(",");  Serial.print(reg2);  Serial.print(",");  Serial.print(reg3);  Serial.print(",");  Serial.print(reg4);  Serial.print(",");  Serial.print(reg5);  Serial.print(",");  Serial.print(reg6);  Serial.print(",");
  Serial.print(reg7);   Serial.print(",");  Serial.print(reg8);  Serial.print(",");  Serial.print(reg9);  Serial.print(",");  Serial.print(reg10);  Serial.print(",");  Serial.print(reg11);  Serial.print(",");  Serial.print(reg12);  Serial.print(",");
  Serial.print(reg13);  Serial.print(",");  Serial.print(reg14);  Serial.print(",");  Serial.print(reg15);  Serial.print(",");  Serial.print(reg16);  Serial.print(",");  Serial.print(reg17);  Serial.print(",");  Serial.print(reg18);  Serial.print(",");
  Serial.print(reg19);  Serial.println();
}
void read_config()
{
  data = EEPROM_Read();
  data_process();
}
void set_register()
{
  char temp;
  while (Serial.available())
  {
    temp = Serial.read();
    data += String(temp);
  }
  config_check = data.substring(0, 3);
  if (config_check == "CON")
  {
    config_check = "";
    data_length = data.length();
    data_process();
    String EEPROM_Write_data = data;
    EEPROM_Write(EEPROM_Write_data);
  }
  data = "";
}
String read_floating(int address)
{
  MOD_Serial.listen();
  uint8_t result;
  result = node.readHoldingRegisters(address, 2);
  if (result == node.ku8MBSuccess)
  {
    float data_final;
    int32_t Buffer[0];
    Buffer[0] = node.getResponseBuffer(0);
    Buffer[1] = node.getResponseBuffer(1);
    long int Result = ((Buffer[1] & 0xFFFFUL) << 16) | (Buffer[0] & 0x00FF);
    data_final = *(float*)&Result;
    floating = String(data_final);
   // Serial.print("data_final: ");
  //  Serial.println(data_final, 4);
  }
   delay(1000);
  return floating;
}
String read_integer(int address)
{
  MOD_Serial.listen();
  uint8_t result;
  result = node.readHoldingRegisters(address, 1);
  if (result == node.ku8MBSuccess)
  {
    int Buffer;
    Buffer = node.getResponseBuffer(0);
    integer = String(Buffer);
   // Serial.print("data_final: ");
   // Serial.println(Buffer);
  }
   delay(1000);
  return integer;
}
String read_decimal(int address)
{
  MOD_Serial.listen();
  uint8_t result;
  result = node.readHoldingRegisters(address, 1);
  if (result == node.ku8MBSuccess)
  {
    float Buffer;
    Buffer = node.getResponseBuffer(0);
    decimal = String(Buffer);
  //  Serial.print("data_final: ");
 //   Serial.println(Buffer, 4);
  }
  delay(1000);
  return decimal;
}
String read_calander(int address)
{
  MOD_Serial.listen();
  int result;
  result = node.readHoldingRegisters(address, 3);
  if (result == node.ku8MBSuccess)
  {
    int Buffer[3];
    int secs, mins, hrs, days, months, years;
    Buffer[0] = node.getResponseBuffer(0);
    Buffer[1] = node.getResponseBuffer(1);
    Buffer[2] = node.getResponseBuffer(2);
    secs =  ((Buffer[0] & 0xFFFFUL) << 16) | (Buffer[0] & 0x00FF);
    mins =  Buffer[0] >> 8;
    hrs = ((Buffer[1] & 0xFFFFUL) << 16) | (Buffer[1] & 0x00FF);
    days =  Buffer[1] >> 8;
    months = ((Buffer[2] & 0xFFFFUL) << 16) | (Buffer[2] & 0x00FF) ;
    years =  Buffer[2] >> 8;
    calender = String(hrs, HEX) + ":" + String(mins, HEX) + ":" + String(secs, HEX) + ":" + String(days, HEX) + "/" + String(months, HEX) + "/" + String(years, HEX);
//    Serial.print(hrs, HEX);
//    Serial.print(":");
//    Serial.print(mins, HEX);
//    Serial.print(":");
//    Serial.println(secs, HEX);
//    Serial.print(days, HEX);
//    Serial.print("/");
//    Serial.print(months, HEX);
//    Serial.print("/");
//    Serial.println(years, HEX);
  }
  delay(1000);
  return calender;
}
void read_analog()
{
  String a1, a2, a3, a4, a5;
  a1 = String(analogRead(Analog0));
  a2 = String(analogRead(Analog1));
  a3 = String(analogRead(Analog2));
  a4 = String(analogRead(Analog3));
  a5 = String(analogRead(Analog4));
  analog = a1 + "," + a2 + "," + a3 + "," + a4 + "," + a5;
 // Serial.print("Analog:  ");
 // Serial.println(analog);
}
void EEPROM_Write(String Value)
{
  Value.toCharArray(temp_char, 81);
  Serial.print("Data Write to Memory: ");
  for (int i = 0; i < 81; i++)
  {
    Serial.print(temp_char[i]);
    EEPROM.write(i, temp_char[i]);
  }
  Serial.println();
  delay(1000);
}
String EEPROM_Read()
{
  char EEPROM_char[81];
  String EEPROM_Result;
  for (int j = 0; j < 81; j++)
  {
    EEPROM_char[j] = EEPROM.read(j);
    EEPROM_Result += String(EEPROM_char[j]);
    //Serial.print(EEPROM_char[j]);
    delay(50);
  }
  Serial.print("Data Read from Memory: ");
  Serial.println(EEPROM_Result);
  delay(1000);
  return EEPROM_Result;
}
void loop()
{
  reg_status = digitalRead(set_reg);
  if (reg_status == 0)
  {
    set_register();
  }
  else
  {
    if (tmr_20s == 0 || currentTime - tmr_20s >= 3600000)
    {
      String str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16, str17, str18, str19;
      MOD_Serial.begin(9600);
      delay(1000);
      str1 = read_floating(reg1 - 1); str2 = read_floating(reg2 - 1); str3 = read_floating(reg3- 1); str4 = read_floating(reg4 - 1); str5 = read_floating(reg5 - 1); str6 = read_floating(reg6 - 1);
      str7 = read_integer(reg7 - 1); str8 = read_integer(reg8 - 1); str9 = read_integer(reg9 - 1); str10 = read_integer(reg10 - 1); str11 = read_integer(reg11 - 1); str12 = read_integer(reg12 - 1);
      str13 = read_decimal(reg13 - 1); str14 = read_decimal(reg14 - 1); str15 = read_decimal(reg15 - 1); str16 = read_decimal(reg16 - 1); str17 = read_decimal(reg17- 1); str18 = read_decimal(reg18 - 1);
      str19 = read_calander(reg19 - 1);
      read_analog();
      delay(1000);
      analog ="A:"+ analog;
      full_data1 = "F1:" + str1 + "," + str2 + "," + str3;
      ful_data1= "F2:"+str4 + "," + str5 + "," + str6  ;
      str1 = ""; str2 = ""; str3 = ""; str4 = ""; str5 = ""; str6 = "";
      full_data2 = "I1:" + str7 + "," + str8 + "," + str9;
      ful_data2="I2:"+str10 + "," + str11 + "," + str12 ;
      str7 = ""; str8 = ""; str9 = ""; str10 = ""; str11 = ""; str12 = "";
      full_data3 = "D1:" + str13 + "," + str14 + "," + str15;
      ful_data3="D2:"+ str16 + "," + str17 + "," + str18 ;
      full_data4="B:"+str19;
      str13 = ""; str14 = ""; str15 = ""; str16 = ""; str17 = ""; str18 = ""; str19 = "";
      Serial.println(analog); //floating
      Serial.println(full_data1);  // interger
      Serial.println(ful_data1);   // decimal
      Serial.println(full_data2);   // decimal
      Serial.println(ful_data2);
      Serial.println(full_data3);
      Serial.println(ful_data3);
      Serial.println(full_data4);
      MOD_Serial.end();
      GSM_Serial.begin(9600);
      delay(5000);
      Send_Sms1();
      delay(5000);
      Send_Sms1();
      delay(5000);
      Send_Sms1();
      delay(5000);
      GSM_Serial.end();
      tmr_20s = millis();
      full_data1 = "";
      full_data2 = "";
      full_data3 = "";
      full_data4 = "";
      ful_data1= "";
      ful_data2= "";
      ful_data3= "";
      analog = "";
    }
    currentTime = millis();
  }
  delay(1000);
}
void Send_Sms1()
{
  parseCmd("AT"); //Check AT
  parseCmd("ATE0");
  parseCmd("AT+CMGD=1");    // Delete the sms
  parseCmd("AT+CMGF=1");    // Text Mode
  parseCmd("AT+CMGS=\"+917871449671\"");  ////////////////////////////////mobile number///////////////////////////////////////
  delay(500);
  parseCmd(full_data1);
  delay(50);
  parseCmd(ful_data1);
  delay(50);
  parseCmd(full_data2);
  delay(50);
  parseCmd(ful_data2);
  delay(50);
  parseCmd(full_data3);
  delay(50);
  parseCmd(ful_data3);
  delay(50);
  parseCmd(full_data4);
  delay(50);
  GSM_Serial.write((char)26);
  delay(500);
  Serial.println("end of sms");
  GSM_Serial.flush();
}
void Send_Sms2()
{
  parseCmd("AT"); //Check AT
  parseCmd("ATE0");
  parseCmd("AT+CMGD=1");    // Delete the sms
  parseCmd("AT+CMGF=1");    // Text Mode
  parseCmd("AT+CMGS=\"+917871449671\"");
  delay(500);
  GSM_Serial.write((char)26);
  delay(500);
  Serial.println("end of sms");
}
void parseCmd(String command)
{
  GSM_Serial.println(command);
  delay(200);
  while (GSM_Serial.available() > 0)
  {
    Serial.print(char(GSM_Serial.read()));
  }
}
