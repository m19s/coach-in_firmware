#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <GROOVE/LED/Pattern/RingToLinePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class LineBackPattern : public BasePattern
	{
	public:
		LineBackPattern()
		    : BasePattern()
		{
		}
		virtual ~LineBackPattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count, int index, int num, int r, int g, int b)
		{
			for (int i = 0; i < num; i++) {
				if (index + i > GROOVE::DeviceConfiguration::NumberOfLEDsInRingLED + GROOVE::DeviceConfiguration::NumberOfLEDsInFingerLED) {
					return;
				}
				if ((index + i) < GROOVE::DeviceConfiguration::NumberOfLEDsInRingLED) {
					if ((index + i - 4) >= 0) {
						RingToLinePattern ring_to_line_pattern;
						ring_to_line_pattern.configureStrip(strip, dissolve_count, index + i - 4, num, r, g, b, 255 - (i * 255 / num));
					}
				}
				else {
					BasePattern::updatePixel(strip, index + i, r, g, b, 255 - (i * 255 / num));
				}
			}
		}
	};
}
}
