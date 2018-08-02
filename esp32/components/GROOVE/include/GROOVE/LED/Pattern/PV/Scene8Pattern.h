#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class Scene8Pattern : public BasePattern
	{
	public:
		Scene8Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene8Pattern() {}
		static void configureStrip(LED::Strip *strip, int dissolve_count)
		{
			float divider = DeviceConfiguration::NumberOfMaximumDissolveCount / 2.0;
			int max_index = DeviceConfiguration::NumberOfMaximumDissolveCount / 2.0;

			LED::Color source = LED::Color::purple();
			LED::Color target = LED::Color::blue();

			if (dissolve_count < DeviceConfiguration::NumberOfMaximumDissolveCount / 2) {
				source = LED::Color::blue();
				target = LED::Color::purple();
			}
			else if (dissolve_count == DeviceConfiguration::NumberOfMaximumDissolveCount / 2) {
				return;
			}
			float dr = (target.redValue() - source.redValue()) / divider;
			float dg = (target.greenValue() - source.greenValue()) / divider;
			float db = (target.blueValue() - source.blueValue()) / divider;

			float power = (dissolve_count % max_index);
			int r = source.redValue() + (dr * power);
			int g = source.greenValue() + (dg * power);
			int b = source.blueValue() + (db * power);

			LED::Color current_color(r, g, b);
			for (int i = 0; i < strip->numberOfPixels(); i++) {
				BasePattern::updatePixel(strip, i, current_color);
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				Scene8Pattern::configureStrip(strip, counter.indexOfDissolveCounter());
			};
		}
	};
}
}
