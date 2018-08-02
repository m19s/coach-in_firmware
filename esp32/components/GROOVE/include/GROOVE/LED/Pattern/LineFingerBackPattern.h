#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class LineFingerBackPattern : public BasePattern
	{
	public:
		LineFingerBackPattern()
		    : BasePattern()
		{
		}
		virtual ~LineFingerBackPattern() {}
		static void configureStrip(LED::Strip *strip, int index, int num, int r, int g, int b)
		{
			for (int i = 0; i < num; i++) {
				if (index - i < GROOVE::DeviceConfiguration::NumberOfLEDsInRingLED) {
					return;
				}
				BasePattern::updatePixel(strip, index - i, r, g, b, 255 - (i * 255 / num));
			}
		}
	};
}
}
