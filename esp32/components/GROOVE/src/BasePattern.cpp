#include <GROOVE/LED/Pattern/BasePattern.h>
#include <StreamLogger/Logger.h>

namespace GROOVE
{
void LEDStrip::BasePattern::updatePixel(LED::Strip *strip, int index, int r, int g, int b, int brightness)
{
	LEDStrip::BasePattern::updatePixel(strip, index, LED::Color(r, g, b, brightness));
}

void LEDStrip::BasePattern::updatePixel(LED::Strip *strip, int index, m2d::ESP32::LED::Color color)
{
	if (index >= strip->numberOfPixels()) {
		m2d::ESP32::Logger::E << "An index of pixel to update is out of bounds! (" << index << ")" << m2d::ESP32::Logger::endl;
		return;
	}

	strip->setPixel(index, color.pixel_color());
}

void LEDStrip::BasePattern::clear(LED::Strip *strip)
{
	for (int i = 0; i < strip->numberOfPixels(); i++) {
		LEDStrip::BasePattern::updatePixel(strip, i, LED::Color::off());
	}
}
}
