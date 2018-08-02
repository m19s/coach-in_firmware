#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class LineFingerForwardPattern : public BasePattern
	{
	public:
		LineFingerForwardPattern()
		    : BasePattern()
		{
		}
		virtual ~LineFingerForwardPattern() {}
		static void configureStrip(LED::Strip *strip, int index, int num, int r, int g, int b)
		{
			for (int i = 0; i < num; i++) {
				if (index + i < DeviceConfiguration::LEDStripLength) {
					BasePattern::updatePixel(strip, index + i, r, g, b, 255 - (i * 255 / num));
				}
			}
		}
	};
}
}
