#include <DHT.h>        // You have to download DHT  library
#define DHTPIN 10                        // PIN №10 подключения датчика DTH22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
int sensorDhtTempBoiler;       // температура в бойлерной
int sensorDhtHumBoiler;        // влажность в бойлерной

//-----------------
#include <OneWire.h>                    // Библиотека протокола 1-Wire
#include <DallasTemperature.h>          // Библиотека для работы с датчиками DS*
#define ONE_WIRE_BUS 12                 // Шина данных на 12 пине для датчиков DS18B20 (ардуино ProMini)
#define TEMPERATURE_PRECISION 9         // Точность измерений (0,5град)  в битах (по умолчанию 12)
OneWire oneWire(ONE_WIRE_BUS);            // Создаем экземпляр объекта протокола 1-WIRE - OneWire
DallasTemperature sensorsDS(&oneWire);    // На базе ссылки OneWire создаем экземпляр объекта, работающего с датчиками DS*

// адрес датчика DS18B20 на выходном патрубке котла:
DeviceAddress sensorDsBoiler = { 0x28, 0xFF, 0x72, 0x7C, 0x01, 0x17, 0x05, 0x0A };
// адрес датчика DS18B20 на стенке бака внизу:
DeviceAddress sensorDsTankLow = { 0x28, 0xFF, 0x3D, 0x1C, 0xB3, 0x16, 0x04, 0x75 };
// адрес датчика DS18B20 на стенке бака посередине:
DeviceAddress sensorDsTankMiddle = { 0x28, 0xFF, 0xCF, 0x04, 0xB3, 0x16, 0x04, 0x74 };
// адрес датчика DS18B20 на стенке бака вверху:
DeviceAddress sensorDsTankHigh = { 0x28, 0xFF, 0xCF, 0x09, 0xB3, 0x16, 0x03, 0x6A };
// адрес датчика DS18B20 внутри бака:
DeviceAddress sensorDsTankInside = { 0x28, 0xFF, 0x51, 0xBD, 0xA1, 0x16, 0x05, 0x38 };
// адрес датчика DS18B20 на трубе в бак:
DeviceAddress sensorDsTankIn = { 0x28, 0xFF, 0x9F, 0x65, 0x01, 0x17, 0x04, 0xC9 };
// адрес датчика DS18B20 на трубе из бака:
DeviceAddress sensorDsTankOut = { 0x28, 0xFF, 0xAE, 0x35, 0xC1, 0x16, 0x04, 0xCF };
//-----------------

int pPressPin = A0;   //  датчик давления в трубе от бака подключен 0-му аналоговому входу
int aTrans1Pin = A1;   //  трансформатор тока тэна №1 бойлера подключен 1-му аналоговому входу
int aTrans2Pin = A2;   //  трансформатор тока тэна №2 бойлера подключен 2-му аналоговому входу
int aTrans3Pin = A3;   //  трансформатор тока тэна №3 бойлера подключен 3-му аналоговому входу
//------------------
int sensorDsTempBoiler;        // температура от датчика DS18B20 на выходном патрубке котла
int sensorDsTempTankLow;       // температура от датчика DS18B20 на стенке бака внизу
int sensorDsTempTankMiddle;    // температура от датчика DS18B20 на стенке бака посередине
int sensorDsTempTankHigh;      // температура от датчика DS18B20 на стенке бака вверху
int sensorDsTempTankInside;    // температура от датчика DS18B20 внутри бака
int sensorDsTempTankIn;    // температура от датчика DS18B20 на трубе в бак
int sensorDsTempTankOut;   // температура от датчика DS18B20 на трубе из бака
int sensorPressTankFrom;    // давление от датчика давления в трубе от бака
int aTrans1Boiler;      //  эл.ток от тр-ра тока ТЭНа №1
int aTrans2Boiler;      //  эл.ток от тр-ра тока ТЭНа №2
int aTrans3Boiler;      //  эл.ток от тр-ра тока ТЭНа №3
int statePin;      //  статус Pin

//-------------------------------------------------
#include <EasyTransfer.h>
EasyTransfer ETin, ETout;  //create two objects
#define DIR 13            // переключатель прием\передача на Pin13

int ID = 21;              // номер этой ардуины в летней кухне

struct RECEIVE_DATA_STRUCTURE {         // структура, которую будем принимать
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int ID;
  int action;
  int targetPin;
  int levelPin;
};
int action = 0;    //  0/1 ("get"/"set") - запрос выдачи данных / команда на исполнение
int levelPin = 0;       //  0/1 - установка уровня на Pin (LOW/HIGH)
int targetPin = 9;          //  Pin управления

struct SEND_DATA_STRUCTURE {                  // структура, которую будем передавать
  int ID;
  int sensorDsTankLow;       // температура от датчика DS18B20 на стенке бака внизу
  int sensorDsTankMiddle;    // температура от датчика DS18B20 на стенке бака посередине
  int sensorDsTankHigh;      // температура от датчика DS18B20 на стенке бака вверху
  int sensorDsTankInside;    // температура от датчика DS18B20 внутри бака
  int sensorDsBoiler;        // температура от датчика DS18B20 на выходном патрубке котла
  int sensorDhtTBoiler;      // температура датчика DTH22 в бойлерной
  int sensorDhtHBoiler;      // влажность датчика DTH22 в бойлерной
  int sensorDsTankIn;    // температура от датчика DS18B20 на трубе в бак
  int sensorDsTankOut;   // температура от датчика DS18B20 на трубе из бака
  int sensorPressTankFrom;    // температура от датчика давления в трубе от бака
  int aTrans1Boiler;      //  эл.ток от тр-ра тока ТЭНа №1
  int aTrans2Boiler;      //  эл.ток от тр-ра тока ТЭНа №2
  int aTrans3Boiler;      //  эл.ток от тр-ра тока ТЭНа №3
  int statePin;      //  статус Pin
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE rxdata;
SEND_DATA_STRUCTURE txdata;
//////////////////////////

int maxValue;

//-----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  //----------
  ETin.begin(details(rxdata), &Serial); //  //start the library, pass in the data details and the name of the serial port
  ETout.begin(details(txdata), &Serial);

  pinMode(DIR, OUTPUT);
  delay(50);
  digitalWrite(DIR, LOW);     // включаем прием
  //----------
  dht.begin();
  sensorsDS.begin(); // Запускаем поиск и запуск всех датчиков DS18B20 на шине
  //------------
}
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

void loop() {

  sensorsDS.requestTemperatures(); // Запускаем процесс измерение температуры на всех датчиках DS18B20
  delay(50);  // небольшая задержка, иначе неуспевают

  sensorDhtTempBoiler = dht.readTemperature(); //  считываем показания температуры
  sensorDhtHumBoiler = dht.readHumidity();     //  считываем показания влажности

  sensorPressTankFrom = analogRead(pPressPin);     //  считываем показания давления

  aTrans1Boiler = maxValueData(aTrans1Pin);     //  считываем показания эл.тока
  aTrans2Boiler = maxValueData(aTrans2Pin);     //  считываем показания эл.тока
  aTrans3Boiler = maxValueData(aTrans3Pin);     //  считываем показания эл.тока

  sensorDsTempBoiler = sensorsDS.getTempC(sensorDsBoiler);
  sensorDsTempTankIn = sensorsDS.getTempC(sensorDsTankIn);
  sensorDsTempTankOut = sensorsDS.getTempC(sensorDsTankOut);
  sensorDsTempTankHigh = sensorsDS.getTempC(sensorDsTankHigh);
  sensorDsTempTankMiddle = sensorsDS.getTempC(sensorDsTankMiddle);
  sensorDsTempTankLow = sensorsDS.getTempC(sensorDsTankLow);
  sensorDsTempTankInside = sensorsDS.getTempC(sensorDsTankInside);

  statePin = digitalRead(targetPin);	//	считываем состояние PINa

  //-------------
  if (ETin.receiveData()) {                         // если пришли данные
    byte id = rxdata.ID; // читаем байт, в нем для кого этот пакет

    if (id == ID) { // и если пакет пришел нам

      if (rxdata.action > 0) {   //  и если команда управления
        targetPin = rxdata.targetPin;
        levelPin = rxdata.levelPin;
        pinMode(targetPin, OUTPUT);

        if (rxdata.levelPin > 0) {   //  то управляем реле

          delay(50);
          //       analogWrite(targetPin, levelPin);
          digitalWrite(targetPin, LOW);   // включаем реле
        }
        else
        {
          digitalWrite(targetPin, HIGH);    // выключаем реле
        }

      }
      //   включаем передачу данных
      //      ID = 21;
      txdata.ID = ID;

      txdata.sensorDsTankLow = sensorDsTempTankLow;       // температура от датчика DS18B20 на стенке бака внизу
      txdata.sensorDsTankMiddle = sensorDsTempTankMiddle;    // температура от датчика DS18B20 на стенке бака посередине
      txdata.sensorDsTankHigh = sensorDsTempTankHigh;      // температура от датчика DS18B20 на стенке бака вверху
      txdata.sensorDsTankInside = sensorDsTempTankInside;    // температура от датчика DS18B20 внутри бака
      txdata.sensorDsBoiler = sensorDsTempBoiler;        // температура от датчика DS18B20 на выходном патрубке котла

      txdata.sensorDhtTBoiler = sensorDhtTempBoiler;      // температура в бойлерной
      txdata.sensorDhtHBoiler = sensorDhtHumBoiler;        // влажность в бойлерной

      txdata.sensorDsTankIn = sensorDsTempTankIn;    // температура от датчика DS18B20 на трубе в бак
      txdata.sensorDsTankOut = sensorDsTempTankOut;   // температура от датчика DS18B20 на трубе из бака

      txdata.sensorPressTankFrom = sensorPressTankFrom;    // давление от датчика давления в трубе от бака

      txdata.aTrans1Boiler = aTrans1Boiler;     //  эл.ток от тр-ра тока ТЭНа №1
      txdata.aTrans2Boiler = aTrans2Boiler;     //  эл.ток от тр-ра тока ТЭНа №2
      txdata.aTrans3Boiler = aTrans3Boiler;     //  эл.ток от тр-ра тока ТЭНа №3

      txdata.statePin = statePin;			//	состояние PINa

      //////

      Serial.print(" ID = ");
      Serial.print(txdata.ID);                 // и отправляем в Serial
      Serial.print("  ");

      Serial.println();
      delay(10);                                     // небольшая задержка, иначе неуспевает

      digitalWrite(DIR, HIGH);                      // включаем передачу
      delay(50);

      ETout.sendData();                              // отправляем
      delay(50);
      // небольшая задержка, иначе неуспевает
      digitalWrite(DIR, LOW);                       // включаем прием
    }
  }
  //----------
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//			Дополнительные
//				функции
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////   Максимальное значение из массива потока данных   ///////
int maxValueData(int xPin) {

  int tmpValue = -8; // измеряемое значение
  maxValue = -9; // полученное максимальное значение

  for (int i = 0; i < 100; i++)
  {
    tmpValue = analogRead(xPin); // измеряем
    if (tmpValue > maxValue) {
      maxValue = tmpValue;
    }
  }
  return maxValue;
}

///////////
