#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <GROOVE/LED/Pattern/LineFingerForwardPattern.h>
#include <GROOVE/LED/Pattern/LineRingPattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class PointGesturePattern : public BasePattern
	{
	public:
		PointGesturePattern()
		    : BasePattern()
		{
		}
		virtual ~PointGesturePattern() {}
		static void configureStrip(LED::Strip *strip, EmitCounter counter, int num)
		{
			BasePattern::clear(strip);
			LineRingPattern::configureStrip(strip, counter.indexOfRingCounter(), 3, 255, 0, 255);

			int timing_offset = 40;
			int led_fing_length = (DeviceConfiguration::NumberOfLEDsInRingLED + DeviceConfiguration::NumberOfLEDsInFingerLED + timing_offset);
			LineFingerForwardPattern::configureStrip(strip, counter.indexOfFingerCounter(), 30, 255 - (counter.indexOfFingerCounter() * 255 / led_fing_length), 0, (counter.indexOfFingerCounter() * 255 / led_fing_length));
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				PointGesturePattern::configureStrip(strip, counter, 30);
			};
		}
	};
}
}
