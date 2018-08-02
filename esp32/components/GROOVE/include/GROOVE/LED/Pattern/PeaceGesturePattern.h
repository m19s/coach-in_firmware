#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <GROOVE/LED/Pattern/LineFingerForwardPattern.h>
#include <GROOVE/LED/Pattern/LineRingPattern.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class PeaceGesturePattern : public BasePattern
	{
	public:
		PeaceGesturePattern()
		    : BasePattern()
		{
		}
		virtual ~PeaceGesturePattern() {}
		static void configureStrip(LED::Strip *strip, EmitCounter counter)
		{
			int index = (counter.indexOfDissolveCounter() / 2) % DeviceConfiguration::LEDStripLength;
			for (int i = 0; i < index; i++) {
				BasePattern::updatePixel(strip, i, 0, 0, 255, 200);
			}
		}

		static std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emitHandler()
		{
			return [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) {
				PeaceGesturePattern::configureStrip(strip, counter);
				if ((counter.indexOfDissolveCounter() / 2) % DeviceConfiguration::LEDStripLength == DeviceConfiguration::LEDStripLength - 1) {
					emitter->suspend();
				}
			};
		}
	};
}
}
