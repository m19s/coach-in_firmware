#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class Scene6_5Pattern : public BasePattern
	{
	public:
		Scene6_5Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene6_5Pattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			double speed = 1.7;
			int count = dissolve_count * speed;
			double brightness = (count / 120.0);
			if (brightness > 1) {
				brightness -= std::ceil(brightness);
			}
			brightness = std::fabs(brightness);
			for (int i = 0; i < strip->numberOfPixels(); i++) {
				if (brightness < 0.5) {
					BasePattern::updatePixel(strip, i, LED::Color::white(brightness * 100));
				}
				else {
					BasePattern::updatePixel(strip, i, LED::Color::white((1 - brightness) * 100));
				}
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				Scene6_5Pattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
