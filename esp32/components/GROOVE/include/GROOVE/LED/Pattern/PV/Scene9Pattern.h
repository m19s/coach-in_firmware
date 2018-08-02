#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class Scene9Pattern : public BasePattern
	{
	public:
		Scene9Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene9Pattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			for (int i = 0; i < strip->numberOfPixels(); i++) {
				if (dissolve_count < 8) {
					BasePattern::updatePixel(strip, i, LED::Color::yellow(100));
				}
				else {
					BasePattern::updatePixel(strip, i, LED::Color::off());
				}
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				Scene9Pattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
