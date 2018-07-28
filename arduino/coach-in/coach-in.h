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
		class DrivePacket
		{
		public:
			bool drive_all;
			int channel_identifier;
			int delay_ms;

			DrivePacket(uint8_t data)
			{
				this->drive_all = (data & 0b10000000) > 0;
				this->channel_identifier = ((data & 0b01110000) >> 4);
				this->delay_ms = (data & 0b00001111) * 100;
			}
		};

		class ChannelPacket
		{
		public:
			int channel_identifier;
			int duration;
			int frequency;
			int pulse;

			ChannelPacket(uint16_t data)
			{
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
				pulse -= 40;
				pulse /= 10;
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

		class DevKit2 : public rkmtlab::MultiEMS::Board
		{
		private:
			Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
			m2d::Arduino::SPI::Stack spi_stack = m2d::Arduino::SPI::Stack(3);
			static const int kNumberOfChannels = 4;

		public:
			typedef enum
			{
				Invalid = -1,
				None = 0,
				Drive = 1,
				DriveAll = 2,
				Channel = 3
			} UpdateType;

			DevKit2()
			    : rkmtlab::MultiEMS::Board(kNumberOfChannels)
			{
				spi_stack.setup(m2d::Arduino::SPI::Stack::Mode3, m2d::Arduino::SPI::Stack::LSBFirst, SPI_CLOCK_DIV8);
				spi_stack.set_flush_command_setting(0xff, 1);
				spi_stack.debug_mode = true;

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

			UpdateType update()
			{
				if (this->spi_stack.available()) {
					uint8_t type = this->spi_stack.buffer[0];
					if (type == 0) {
						// drive packet
						uint16_t data = this->spi_stack.buffer[1];
						auto packet = DrivePacket(data);
						if (this->drive(packet)) {
							if (packet.drive_all) {
								Serial.println("packet type is drive all");
								this->stack_flush();
								return DriveAll;
							}
							else {
								Serial.println("packet type is drive");
								this->stack_flush();
								return Drive;
							}
						}
					}
					else if (type == 1) {
						// channel packet
						uint16_t data = this->spi_stack.buffer[1] << 8;
						data |= this->spi_stack.buffer[2];
						if (this->update_channel(ChannelPacket(data))) {
							Serial.println("packet type is channel");
							this->stack_flush();
							return Channel;
						}
					}
					else {
						return Invalid;
					}
				}
				return None;
			}

			void stack_flush()
			{
				this->spi_stack.flush();
			}

			bool update_channel(ChannelPacket p)
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

			bool drive(DrivePacket p)
			{
				if (p.drive_all) {
					this->driveAll();
					return true;
				}
				else if (p.channel_identifier < this->channels().size()) {
					auto c = this->channelForIndex(p.channel_identifier);
					if (p.delay_ms > 0) {
						delayMicroseconds(p.delay_ms);
					}
					c->drive();
					delayMicroseconds((1000000 / c->frequency) - (rkmtlab::MultiEMS::Channel::Delay * _channels.size()));

					return true;
				}

				return false;
			}
		};
	}
}
}
