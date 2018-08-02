#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <GROOVE/LED/Pattern/LineFingerForwardPattern.h>
#include <GROOVE/LED/Pattern/LineRingPattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class RockGesturePattern : public BasePattern
	{
	public:
		RockGesturePattern()
		    : BasePattern()
		{
		}
		virtual ~RockGesturePattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			int ic = dissolve_count * 4 / 256;
			int brightness = dissolve_count * 4 % 200;
			if (ic == 1 || ic == 3) {
				for (int i = 0; i < strip->numberOfPixels(); i++) {
					BasePattern::updatePixel(strip, i, 255, 0, 0, brightness);
				}
			}
			else {
				for (int i = 0; i < strip->numberOfPixels(); i++) {
					BasePattern::updatePixel(strip, i, 255, 0, 0, 255 - brightness);
				}
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				RockGesturePattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
