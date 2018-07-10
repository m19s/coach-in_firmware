#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <Arduino_SPI-Stack.h>
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
			bool invalid = false;

			Packet()
			{
				this->invalid = true;
			}

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
			m2d::Arduino::SPI::Stack spi_stack = m2d::Arduino::SPI::Stack(2);

		public:
			uint16_t last_packet_data = 0;

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

			bool set_pwm_frequency(int frequency)
			{
				if (frequency >= 33 | frequency <= 720) {
					pwm.setPWMFreq(frequency);
					return true;
				}

				return false;
			}

			void process_data(char data)
			{
				this->spi_stack.process_data(data);
			}

			bool update()
			{
				if (this->spi_stack.available()) {
					uint16_t data = this->spi_stack.buffer[0] << 8;
					data |= this->spi_stack.buffer[1];

					this->last_packet_data = data;

					this->spi_stack.flush();
					return this->update_channel(Packet(data));
				}
				return false;
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
