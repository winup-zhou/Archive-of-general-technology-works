#include <LiquidCrystal_I2C.h> 
#include <dht11.h>
#include <Wire.h>
double settemp = 26.0;
int lcdst(0), lcdbl1(0), lcdbl2(0), rlydelay(0), STATUS_(3);
double Fahrenheit(double celsius)
{
    return 1.8 * celsius + 32;
} //摄氏温度度转化为华氏温度

double Kelvin(double celsius)
{
    return celsius + 273.15;
} //摄氏温度转化为开氏温度

// 露点（点在此温度时，空气饱和并产生露珠）
// 参考: http://wahiduddin.net/calc/density_algorithms.htm
double dewPoint(double celsius, double humidity)
{
    double A0 = 373.15 / (273.15 + celsius);
    double SUM = -7.90298 * (A0 - 1);
    SUM += 5.02808 * log10(A0);
    SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1);
    SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1);
    SUM += log10(1013.246);
    double VP = pow(10, SUM - 3) * humidity;
    double T = log(VP / 0.61078); // temp var
    return (241.88 * T) / (17.558 - T);
}

// 快速计算露点，速度是5倍dewPoint()
// 参考: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
    double a = 17.271;
    double b = 237.7;
    double temp = (a * celsius) / (b + celsius) + log(humidity / 100);
    double Td = (b * temp) / (a - temp);
    return Td;
}

dht11 DHT11;

#define DHT11PIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD屏
void setup()
{
    Serial.begin(115200);
    Serial.println("DHT11 TEST PROGRAM ");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT11LIB_VERSION);
    Serial.println();
    // initialize the pins
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, OUTPUT);
    // initialize the LCD1602-IIC
    lcd.init();
    lcd.clear();
    lcd.setBacklight(1);
    lcd.setCursor(0, 0);
    lcd.print("System Init.....");
    lcd.setCursor(1, 1);
    lcd.print("Basic Test OK.");
    tone(3, 500, 500);
    delay(500);
    tone(3, 600, 500);
    delay(500);
    tone(3, 800, 500);
    delay(500);
}

void loop()
{
    Serial.println("\n");

    int chk = DHT11.read(DHT11PIN);

    Serial.print("Read sensor: ");
    switch (chk)
    {
    case DHTLIB_OK:
        Serial.println("OK");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        lcd.setBacklight(1);
        lcd.setCursor(0, 0);
        lcd.print("  Sensor Error  ");
        lcd.setCursor(0, 1);
        lcd.print(" Checksum error ");
        digitalWrite(4, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        tone(3, 900, 500);
        Serial.println("Checksum error");
        delay(2000);
        loop();
        break;
    case DHTLIB_ERROR_TIMEOUT:
        lcd.setBacklight(1);
        lcd.setCursor(0, 0);
        lcd.print("  Sensor Error  ");
        lcd.setCursor(0, 1);
        lcd.print(" Time out error ");
        digitalWrite(4, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        tone(3, 900, 500);
        Serial.println("Time out error");
        delay(2000);
        loop();
        break;
    default:
        lcd.setBacklight(1);
        lcd.setCursor(0, 0);
        lcd.print("  Sensor Error  ");
        lcd.setCursor(0, 1);
        lcd.print(" Unknown  error ");
        digitalWrite(4, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        tone(3, 900, 500);
        Serial.println("Unknown error");
        delay(2000);
        loop();
        break;
    }

    Serial.print("Humidity (%): ");
    Serial.println((double)DHT11.humidity, 2);

    Serial.print("Temperature (oC): ");
    Serial.println((double)DHT11.temperature, 2);

    Serial.print("Temperature (oF): ");
    Serial.println(Fahrenheit(DHT11.temperature), 2);

    Serial.print("Temperature (K): ");
    Serial.println(Kelvin(DHT11.temperature), 2);

    Serial.print("Dew Point (oC): ");
    Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

    Serial.print("Dew PointFast (oC): ");
    Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));

    Serial.print("Set Temp(oC): ");
    Serial.println(settemp);
    if (!digitalRead(7))
    {
        if (!digitalRead(8))
        {
            tone(3, 800, 150);
            lcdbl2 ^= 1;
            if (lcdbl2 == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print("LCD Backlight NO");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("LCD Backlight NC");
            }
            delay(500);
            loop();
        }
        else if (!digitalRead(9))
        {
            lcd.setBacklight(1);
            tone(3, 800, 150);
            STATUS_++;
            STATUS_ %= 4;
            if (STATUS_ == 0)
            {
                digitalWrite(6, LOW);
                digitalWrite(5, LOW);
                lcd.setCursor(0, 1);
                lcd.print("RLY Control: OFF");
            }
            else if (STATUS_ == 1)
            {
                digitalWrite(5, LOW);
                digitalWrite(6, HIGH);
                lcd.setCursor(0, 1);
                lcd.print("RLY Control:COOL");
            }
            else if (STATUS_ == 2)
            {
                digitalWrite(6, LOW);
                digitalWrite(5, HIGH);
                lcd.setCursor(0, 1);
                lcd.print("RLY Control:HEAT");
            }
            else
            {
                digitalWrite(6, LOW);
                digitalWrite(5, LOW);
                lcd.setCursor(0, 1);
                lcd.print("RLY Control:  NO");
            }
            delay(500);
            loop();
        }
    }
    else
    {
        if (!digitalRead(8))
        {
            tone(3, 900, 100);
            delay(100);
            tone(3, 700, 100);
            tempminus();
        }
        else if (!digitalRead(9))
        {
            tone(3, 700, 100);
            delay(100);
            tone(3, 900, 100);
            tempplus();
        }
    }
    if (lcdbl1)
    {
        lcd.setCursor(0, 0);
        lcd.print("Room Temp:   ");
        lcd.setCursor(12, 0);
        lcd.print((double)DHT11.temperature, 1);
        lcd.setCursor(0, 1);
        lcd.print("Set Temp:    ");
        lcd.setCursor(12, 1);
        lcd.print((double)settemp, 1);
        lcdbl1--;
    }
    else
    {
        if (lcdst > 25)
        {
            lcd.setCursor(0, 0);
            lcd.print("Hum(%):     ");
            lcd.setCursor(12, 0);
            lcd.print((double)DHT11.humidity, 1);
            lcd.setCursor(0, 1);
            lcd.print("Set Temp:   ");
            lcd.setCursor(12, 1);
            lcd.print((double)settemp, 1);
        }
        else
        {
            lcd.setCursor(0, 0);
            lcd.print("Hum(%):     ");
            lcd.setCursor(12, 0);
            lcd.print((double)DHT11.humidity, 1);
            lcd.setCursor(0, 1);
            lcd.print("Room Temp:   ");
            lcd.setCursor(12, 1);
            lcd.print((double)DHT11.temperature, 1);
        }
    }
    if (rlydelay)
        rlydelay--;
    if (lcdst == 0)
        lcdst = 51;
    lcd.setBacklight(lcdbl1 > 0 || lcdbl2);
    lcdst--;
    delay(200);
    if (STATUS_ == 0)
    {
        rlydelay = 0;
        digitalWrite(10, LOW);
        digitalWrite(4, LOW);
        Serial.print("RLY1 Status: ");
        Serial.println("OFF");
    }
    else if (STATUS_ == 1 && rlydelay == 0)
    {
        if (DHT11.temperature > settemp)
        {
            digitalWrite(10, HIGH);
            digitalWrite(4, HIGH);
            Serial.print("RLY1 Status: ");
            Serial.println("ON");
            rlydelay = 300;
        }
        else
        {
            digitalWrite(10, LOW);
            digitalWrite(4, LOW);
            Serial.print("RLY1 Status: ");
            Serial.println("OFF");
            rlydelay = 300;
        }
    }
    else if (STATUS_ == 2 && rlydelay == 0)
    {
        if (DHT11.temperature < settemp)
        {
            digitalWrite(10, HIGH);
            digitalWrite(4, HIGH);
            Serial.print("RLY1 Status: ");
            Serial.println("ON");
            rlydelay = 300;
        }
        else
        {
            digitalWrite(10, LOW);
            digitalWrite(4, LOW);
            Serial.print("RLY1 Status: ");
            Serial.println("OFF");
            rlydelay = 300;
        }
    }
    else if (STATUS_ == 3)
    {
        rlydelay = 0;
        digitalWrite(10, HIGH);
        digitalWrite(4, HIGH);
        Serial.print("RLY1 Status: ");
        Serial.println("ON");
    }
}
void tempplus()
{
    if (lcdbl1 < 25)
        lcdbl1 = 25;
    if (settemp < 30.0)
        settemp += 0.5;
}
void tempminus()
{
    if (lcdbl1 < 25)
        lcdbl1 = 25;
    if (settemp > 16.0)
        settemp -= 0.5;
}