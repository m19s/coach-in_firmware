#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <GROOVE/LED/Pattern/LineRingPattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class LineForwardPattern : public BasePattern
	{
	public:
		LineForwardPattern()
		    : BasePattern()
		{
		}
		virtual ~LineForwardPattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count, int index, int num, int r, int g, int b)
		{
			for (int i = 0; i < num; i++) {
				if ((index - i) < GROOVE::DeviceConfiguration::NumberOfLEDsInRingLED) {
					if ((index + i - 4) >= 0) {
						LineRingPattern line_ring_pattern;
						line_ring_pattern.configureStrip(strip, index - i - 4, num, r, g, b);
						line_ring_pattern.configureStrip(strip, index + i - 4, num, r, g, b);
					}
				}
				else {
					BasePattern::updatePixel(strip, index - i, r, g, b, 255 - (i * 255 / num));
				}
			}
		}
	};
}
}
