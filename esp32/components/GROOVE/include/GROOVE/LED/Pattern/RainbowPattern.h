#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class RainbowPattern : public BasePattern
	{
	public:
		RainbowPattern()
		    : BasePattern()
		{
		}
		virtual ~RainbowPattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			for (int i = 0; i < strip->numberOfPixels(); i++) {
				int ic = (i + dissolve_count) % 256;
				if (ic < 85) {
					BasePattern::updatePixel(strip, i, ic * 3, 255 - ic * 3, 0, 100);
				}
				else if (ic < 170) {
					ic -= 85;
					BasePattern::updatePixel(strip, i, 255 - ic * 3, 0, ic * 3, 100);
				}
				else {
					ic -= 170;
					BasePattern::updatePixel(strip, i, 0, ic * 3, 255 - ic * 3, 100);
				}
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				RainbowPattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
