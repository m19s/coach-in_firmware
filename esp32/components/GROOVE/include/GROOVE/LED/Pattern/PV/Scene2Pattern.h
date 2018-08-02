#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class Scene2Pattern : public BasePattern
	{
	public:
		Scene2Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene2Pattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			double speed = 2.8;
			int count = dissolve_count * speed;
			double brightness = (count / 255.0);
			if (brightness > 1) {
				brightness -= std::ceil(brightness);
			}
			brightness = std::fabs(brightness);
			for (int i = 0; i < strip->numberOfPixels(); i++) {
				if (brightness < 0.5) {
					BasePattern::updatePixel(strip, i, LED::Color::yellow(brightness * 100));
				}
				else {
					BasePattern::updatePixel(strip, i, LED::Color::yellow((1 - brightness) * 100));
				}
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				Scene2Pattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
