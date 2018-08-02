#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class Scene5Pattern : public BasePattern
	{
	public:
		Scene5Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene5Pattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			int count = dissolve_count * 2.8;
			for (int i = 0; i < strip->numberOfPixels(); i++) {
				int ic = (i + count) % 256;
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
				Scene5Pattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
