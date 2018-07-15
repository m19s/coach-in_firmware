#pragma once

#include <vector>

namespace m19s
{
namespace coach_in
{
	namespace ESP32
	{
		class Packet
		{
		public:
			Packet() {}
			Packet(uint8_t data){};
			Packet(uint16_t data){};

			virtual uint8_t type() = 0;
			virtual uint8_t to_8bits_data() = 0;
			virtual uint16_t to_16bits_data() = 0;
			virtual std::vector<uint8_t> to_byte_vector() = 0;
		};

		class DrivePacket : public Packet
		{
		public:
			uint8_t _channel_identifier;
			uint8_t _delay_ms;

			DrivePacket(uint8_t identifier, uint8_t delay_ms)
			{
				this->_channel_identifier = identifier;
				this->_delay_ms = delay_ms;
			}

			DrivePacket(uint8_t data)
			{
				this->_channel_identifier = data & 0b11100000;
				this->_delay_ms = data & 0b00011111;
			}

			uint8_t type()
			{
				return 0;
			}

			uint8_t to_8bits_data()
			{
				uint8_t data = 0;
				data = this->_channel_identifier;
				data <<= 5;
				data |= this->_delay_ms;

				return data;
			}

			uint16_t to_16bits_data()
			{
				uint16_t data = (uint16_t)this->to_8bits_data();
				return data;
			}

			std::vector<uint8_t> to_byte_vector()
			{
				std::vector<uint8_t> v;
				v.push_back(this->to_8bits_data());

				return v;
			}
		};

		class ChannelPacket : public Packet
		{
		private:
			uint8_t _channel_identifier;
			int _duration;
			int _frequency;
			int _pulse;

		public:
			ChannelPacket(uint8_t identifier, int pulse, int frequency, int duration)
			{
				this->_channel_identifier = identifier;
				this->_pulse = pulse;
				this->_frequency = frequency;
				this->_duration = duration;
			}

			ChannelPacket(uint16_t data)
			{
				// 合計2byte。下記データはMSBから順番に並んでる。
				// | param     | size | note             |
				// |-----------|------|------------------|
				// | channel   | 3bit |                  |
				// | pulse     | 5bit | 40~240, 10刻み    |
				// | frequency | 4bit | 50~150, 10刻み    |
				// | duration  | 4bit | 500~2000, 100刻み |

				this->_duration = data & 0b00001111;
				this->_duration *= 100;
				this->_duration += 500;
				data >>= 4;

				this->_frequency = data & 0b00001111;
				this->_frequency *= 10;
				this->_frequency += 50;
				data >>= 4;

				this->_pulse = data & 0b00011111;
				this->_pulse *= 10;
				this->_pulse += 40;
				data >>= 5;

				this->_channel_identifier = data & 0b00000111;
			}

			uint8_t type()
			{
				return 1;
			}

			uint8_t channel_identifier()
			{
				return this->_channel_identifier;
			}

			int duration()
			{
				return this->_duration;
			}

			int frequency()
			{
				return this->_frequency;
			}

			int pulse()
			{
				return this->_pulse;
			}

			uint8_t to_8bits_data()
			{
				assert(true && "No implementation.");
				return 0;
			}

			uint16_t to_16bits_data()
			{
				uint16_t bytes = 0;
				uint16_t identifier = this->_channel_identifier;
				bytes |= identifier;
				bytes <<= 5;

				uint16_t pulse = this->_pulse;
				pulse -= 40;
				pulse /= 10;
				bytes |= pulse;
				bytes <<= 4;

				uint16_t frequency = this->_frequency;
				frequency -= 50;
				frequency /= 10;
				bytes |= frequency;
				bytes <<= 4;

				uint16_t duration = this->_duration;
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
	}
}
}
