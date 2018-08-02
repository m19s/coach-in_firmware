#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>

namespace GROOVE
{
namespace LEDStrip
{
	class UniqueDevicePattern : public BasePattern
	{
	public:
		UniqueDevicePattern()
		    : BasePattern()
		{
		}
		virtual ~UniqueDevicePattern() {}
		static void configureStrip(LED::Strip *strip, uint16_t udid)
		{
			int max = udid & 0x0f;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < max; j++) {
					BasePattern::updatePixel(strip, j * 2, 255, 0, 0, 255);
				}
				strip->show();
				vTaskDelay(500 / portTICK_PERIOD_MS);
				for (int j = 0; j < max; j++) {
					BasePattern::updatePixel(strip, j * 2, 255, 0, 0, 0);
				}
				strip->show();
				vTaskDelay(500 / portTICK_PERIOD_MS);
			}
		}
	};
}
}
