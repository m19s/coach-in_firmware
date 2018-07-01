#include "coach-in.h"
#include <StandardCplusplus.h>
#include <vector>

using namespace std;

m19s::coach_in::Arduino::MultiEMS_Board *board;

void setup()
{
	board = new m19s::coach_in::Arduino::MultiEMS_Board();
	Serial.begin(115200);
	Serial.println("ready");
}

void loop()
{
	//always discharge
	board->discharge();

	if (Serial.available() > 19) {
		Serial.println("drive");
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
		m19s::coach_in::Arduino::Packet p(board->update());
		auto v = p.to_byte_vector();
		Serial.println("Receive data: ");
		Serial.print(v[0], BIN);
		Serial.print(" ");
		Serial.println(v[1], BIN);

		Serial.println("driveAll");
		board->driveAll();
	}
}
