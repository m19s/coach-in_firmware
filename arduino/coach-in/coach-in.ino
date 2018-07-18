#include "coach-in.h"
#include <StandardCplusplus.h>
#include <vector>

using namespace std;
using namespace m19s::coach_in::Arduino;

m19s::coach_in::Arduino::DevKit2 *board;

volatile uint8_t counter = 0;
ISR(SPI_STC_vect)
{
	byte c = SPDR; // grab byte from SPI Data Register
	Serial.print("receive: ");
	Serial.println(c, BIN);

	board->process_data(c);
}

void setup()
{
	board = new m19s::coach_in::Arduino::DevKit2();
	Serial.begin(115200);
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
	else {
		DevKit2::UpdateType type = board->update();
		switch (type) {
			case DevKit2::UpdateType::None:
				break;
			case DevKit2::UpdateType::Drive:
				Serial.println("drive");
				break;
			case DevKit2::UpdateType::Channel:
				Serial.println("channel");
				break;
		}
	}
}
