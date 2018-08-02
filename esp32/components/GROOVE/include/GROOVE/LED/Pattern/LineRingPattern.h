#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class LineRingPattern : public BasePattern
	{
	public:
		LineRingPattern()
		    : BasePattern()
		{
		}
		virtual ~LineRingPattern() {}
		static void configureStrip(LED::Strip *strip, int index, int num, int r, int g, int b)
		{
			for (int i = 0; i < num; i++) {
				int written_index = index - i;
				if (written_index < 0) {
					written_index += GROOVE::DeviceConfiguration::NumberOfLEDsInRingLED;
				}
				if (written_index < DeviceConfiguration::LEDStripLength) {
					BasePattern::updatePixel(strip, written_index, r, g, b, 255 - (i * 255 / num));
				}
			}
		}
	};
}
}
