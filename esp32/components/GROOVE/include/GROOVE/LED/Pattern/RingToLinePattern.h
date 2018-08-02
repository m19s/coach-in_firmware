#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class RingToLinePattern : public BasePattern
	{
	public:
		RingToLinePattern()
		    : BasePattern()
		{
		}
		virtual ~RingToLinePattern() {}
		void configureStrip(LED::Strip *strip, int dissolve_count, int index, int num, int r, int g, int b, int brightness)
		{
			if (index == 0) {
				BasePattern::updatePixel(strip, 1, r, g, b, brightness);
			}
			else if (index == 1) {
				BasePattern::updatePixel(strip, 0, r, g, b, brightness);
				BasePattern::updatePixel(strip, 2, r, g, b, brightness);
			}
			else if (index == 2) {
				BasePattern::updatePixel(strip, 11, r, g, b, brightness);
				BasePattern::updatePixel(strip, 3, r, g, b, brightness);
			}
			else if (index == 3) {
				BasePattern::updatePixel(strip, 10, r, g, b, brightness);
				BasePattern::updatePixel(strip, 4, r, g, b, brightness);
			}
			else if (index == 4) {
				BasePattern::updatePixel(strip, 9, r, g, b, brightness);
				BasePattern::updatePixel(strip, 5, r, g, b, brightness);
			}
			else if (index == 5) {
				BasePattern::updatePixel(strip, 8, r, g, b, brightness);
				BasePattern::updatePixel(strip, 6, r, g, b, brightness);
			}
			else if (index == 6) {
				BasePattern::updatePixel(strip, 7, r, g, b, brightness);
				BasePattern::updatePixel(strip, 7, r, g, b, brightness);
			}
		}
	};
}
}
