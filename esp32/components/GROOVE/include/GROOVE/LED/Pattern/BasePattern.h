#pragma once

#include <WS2812-Utility/LED.h>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class BasePattern
	{
	public:
		BasePattern(){};
		virtual ~BasePattern(){};
		static void updatePixel(LED::Strip *strip, int index, int r, int g, int b, int brightness);
		static void updatePixel(LED::Strip *strip, int index, LED::Color color);
		static void clear(LED::Strip *strip);
	};
}
}
