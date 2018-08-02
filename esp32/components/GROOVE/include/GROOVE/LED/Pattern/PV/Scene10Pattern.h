#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <cmath>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class Scene10Pattern : public BasePattern
	{
	public:
		Scene10Pattern()
		    : BasePattern()
		{
		}
		virtual ~Scene10Pattern() {}
		static void configureStrip(LED::Strip *strip, int finger_index, int ring_index)
		{
			BasePattern::clear(strip);

			int grow_length = 8;
			float divider = M_PI / (float)grow_length;
			for (int i = 0; i < grow_length; i++) {
				int index = (finger_index + i) % DeviceConfiguration::LEDStripLength;
				if (index < DeviceConfiguration::NumberOfLEDsInRingLED) {
					index += DeviceConfiguration::NumberOfLEDsInRingLED;
				}
				float brightness = std::sin(divider * i);
				auto color = LED::Color::white(std::pow(8 * brightness, 2.6));
				BasePattern::updatePixel(strip, index, color);
			}

			int ring_grow_length = 8;
			divider = M_PI / (float)ring_grow_length;
			for (int i = 0; i < ring_grow_length; i++) {
				int index = (ring_index + i) % DeviceConfiguration::NumberOfLEDsInRingLED;
				float brightness = std::sin(divider * i);
				auto color = LED::Color::white(std::pow(8 * brightness, 2.6));
				BasePattern::updatePixel(strip, index, color);
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)>
		emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				Scene10Pattern::configureStrip(strip, counter.indexOfFingerCounter(), counter.indexOfRingCounter());
			};
		}
	};
}
}
