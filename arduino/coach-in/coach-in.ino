#include "coach-in.h"
#include <SoftwareSerial.h>
#include <StandardCplusplus.h>
#include <vector>
SoftwareSerial esp_serial(10, 11); // RX, TX

using namespace std;
using namespace m19s::coach_in::Arduino;

volatile m19s::coach_in::Arduino::DevKit2 *board;

void setup()
{
	board = new m19s::coach_in::Arduino::DevKit2();
	Serial.begin(115200);
	esp_serial.begin(4800);
	Serial.println("ready");
}

void loop()
{
	//always discharge
	board->discharge();

	if (Serial.available() > 19) {
		Serial.println("serial drive");
		for (rkmtlab::MultiEMS::Channel *c : board->channels()) {
			c->pulse = Serial.read();
			c->frequency = Serial.read();
			c->voltage = Serial.read();
			c->state = Serial.read();
			c->duration = Serial.read() * 100; // converted for msec
		}

		board->driveAll();
	}
	else if (esp_serial.available()) {
		char c = esp_serial.read();
		board->process_data(c);
	}
	else {
		DevKit2::UpdateType type = board->update();
		switch (type) {
			case DevKit2::UpdateType::Invalid:
				Serial.println("invalid");
				break;
			case DevKit2::UpdateType::Drive:
				Serial.println("drive");
				break;
			case DevKit2::UpdateType::DriveAll:
				Serial.println("drive all");
				break;
			case DevKit2::UpdateType::Channel:
				Serial.println("channel");
				break;
		}
	}
}