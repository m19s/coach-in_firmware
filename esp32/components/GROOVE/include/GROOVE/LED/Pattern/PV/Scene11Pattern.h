#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class Scene11Pattern : public BasePattern
	{
	public:
		Scene11Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene11Pattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			for (int i = 0; i < strip->numberOfPixels() + 1; i++) {
				LED::Color c;
				int pos = GROOVE::Util::mapping((i + dissolve_count) % 26, 0, 26, 0, 252);
				if (pos < 85) {
					c = LED::Color(255 - pos * 3, 0, pos * 3);
				}
				else if (pos < 170) {
					pos -= 85;
					c = LED::Color(0, pos * 3, 255 - pos * 3);
				}
				else {
					pos -= 170;
					c = LED::Color(pos * 3, 255 - pos * 3, 0);
				}

				BasePattern::updatePixel(strip, (strip->numberOfPixels() - i), c);
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				Scene11Pattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
