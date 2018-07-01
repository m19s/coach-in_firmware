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
				uint16_t bytes = 0;
				uint16_t identifier = this->channel_identifier;
				bytes |= identifier;
				bytes <<= 5;

				uint16_t pulse = this->pulse;
				pulse -= 40;
				pulse /= 10;
				bytes |= pulse;
				bytes <<= 4;

				uint16_t frequency = this->frequency;
				frequency -= 50;
				frequency /= 10;
				bytes |= frequency;
				bytes <<= 4;

				uint16_t duration = this->duration;
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
