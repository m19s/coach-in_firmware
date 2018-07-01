#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <MultiEMS.h>
#include <SPI.h>
#include <vector>

namespace m19s
{
namespace coach_in
{
	namespace Arduino
	{
		class Packet
		{
		public:
			int channel_identifier;
			int duration;
			int frequency;
			int pulse;

			Packet() {}
			Packet(uint16_t data)
			{
				// 合計2byte。下記データはMSBから順番に並んでる。
				// | param     | size | note             |
				// |-----------|------|------------------|
				// | channel   | 3bit |                  |
				// | pulse     | 5bit | 40~240, 10刻み    |
				// | frequency | 4bit | 50~150, 10刻み    |
				// | duration  | 4bit | 500~2000, 100刻み |

				this->duration = data & 0b00001111;
				this->duration *= 100;
				this->duration += 500;
				data >>= 4;

				this->frequency = data & 0b00001111;
				this->frequency *= 10;
				this->frequency += 50;
				data >>= 4;

				this->pulse = data & 0b00011111;
				this->pulse *= 10;
				this->pulse += 40;
				data >>= 5;

				this->channel_identifier = data & 0b00000111;
			}

			uint16_t to_16bits_data()
			{
				uint16_t bytes;
				int channel_identifier = this->channel_identifier;
				bytes |= channel_identifier;
				bytes <<= 3;

				int pulse = this->pulse;
				this->pulse -= 40;
				this->pulse /= 10;
				bytes |= pulse;
				bytes <<= 5;

				int frequency = this->frequency;
				frequency -= 50;
				frequency /= 10;
				bytes |= frequency;
				bytes <<= 4;

				int duration = this->duration;
				duration -= 500;
				duration /= 100;
				bytes |= duration;

				return bytes;
			}

			std::vector<uint8_t> to_byte_vector()
			{
				std::vector<uint8_t> v;
				uint16_t bit_data = this->to_16bits_data();
				v.push_back((uint8_t)(bit_data >> 8));
				v.push_back((uint8_t)((bit_data << 8) >> 8));

				return v;
			}
		};

		class MultiEMS_Board : public rkmtlab::MultiEMS::Board
		{
		private:
			Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
			SPISettings spi_settings = SPISettings(80000, LSBFIRST, SPI_MODE0);

		public:
			MultiEMS_Board()
			    : rkmtlab::MultiEMS::Board(4)
			{
				pwm.begin();
				// Supported only 33~720Hz
				pwm.setPWMFreq(350);
				Wire.setClock(400000);
				for (int i = 0; i < 4; i++) {
					pwm.setPWM(i, 0, 4096 / 2.0);
				}
			}

			bool setPWMFrequency(int frequency)
			{
				if (frequency >= 33 | frequency <= 720) {
					pwm.setPWMFreq(frequency);
					return true;
				}

				return false;
			}

			uint16_t update()
			{
				SPI.beginTransaction(spi_settings);
				uint16_t dummy = 0xff;
				uint16_t received_data = SPI.transfer16(dummy);
				if (received_data > 0) {
					this->update_channel(Packet(received_data));
				}
				SPI.endTransaction();

				return received_data;
			}

			bool update_channel(Packet p)
			{
				if (p.channel_identifier < this->channels().size()) {
					auto c = this->channelForIndex(p.channel_identifier);
					c->frequency = p.frequency;
					c->pulse = p.pulse;
					c->duration = p.duration;
					return true;
				}

				return false;
			}
		};
	}
}
}
