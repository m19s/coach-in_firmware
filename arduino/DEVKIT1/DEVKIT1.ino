#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <MultiEMS.h>
#include <StandardCplusplus.h>
#include <vector>
using namespace std;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
std::vector<rkmtlab::MultiEMS::Channel *> channels;
int channelNum = 4;

void setup()
{
    Serial.begin(115200);

    pwm.begin();
    pwm.setPWMFreq(350);
    pwm.setPWM(0, 0, 4096 / 2);

    // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
    // some i2c devices dont like this so much so if you're sharing the bus, watch
    // out for this!
    Wire.setClock(400000);

    for (int i = 0; i < channelNum; i++)
    {
        channels.push_back(new rkmtlab::MultiEMS::Channel(i, 100, 100, 5));
    }
    pinMode(13, OUTPUT);
}

void loop()
{
    //always discharge
    for (rkmtlab::MultiEMS::Channel *c : channels)
    {
        c->discharge();
    }

    if (Serial.available() > 19)
    {
        digitalWrite(13, HIGH);
        for (rkmtlab::MultiEMS::Channel *c : channels)
        {
            c->pulse = Serial.read();
            c->frequency = Serial.read();
            c->voltage = Serial.read();
            c->state = Serial.read();
            c->duration = Serial.read() * 100; // converted for msec
        }
        channelWrite();
        digitalWrite(13, LOW);
    }
}

//write for each channel
void channelWrite()
{
    //duration * 10 for debug
    //this must be updated for individual controls of durations
    for (int i = 0; i < ((channels[0]->duration)) / (1000 / channels[0]->frequency); i++)
    {
        for (rkmtlab::MultiEMS::Channel *c : channels)
        {
            c->drive();
        }
        //one pulse relevant for the freq
        //microsec calculation
        delayMicroseconds((1000000 / channels[0]->frequency) - (rkmtlab::MultiEMS::Channel::Delay * channels.size()));
    }
}