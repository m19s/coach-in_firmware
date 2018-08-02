#pragma once

#include <GROOVE/LED/Pattern/BasePattern.h>
#include <WS2812-Utility/LED.h>
#include <functional>

using namespace m2d::ESP32;
namespace GROOVE
{
namespace LEDStrip
{
	class EmitCounter
	{
	private:
		int led_index = 0;
		int ring_index = 0;
		int finger_index = DeviceConfiguration::NumberOfLEDsInRingLED;

		int dissolve_counter = 0;
		const static int MaximumDissolveCount = DeviceConfiguration::NumberOfMaximumDissolveCount;
		uint8_t finger_increment_speed = 1;
		uint8_t ring_increment_speed = 1;

	public:
		void setFingerSpeed(uint8_t speed)
		{
			this->finger_increment_speed = std::min(6, (int)speed);
		}

		void setRingSpeed(uint8_t speed)
		{
			this->ring_increment_speed = std::min(7, (int)speed);
		}

		uint8_t fingerSpeed()
		{
			return this->finger_increment_speed;
		}

		uint8_t ringSpeed()
		{
			return this->ring_increment_speed;
		}

		void increment()
		{
			this->dissolve_counter++;
			if (this->dissolve_counter > MaximumDissolveCount - 1) {
				this->dissolve_counter = 0;
			}

			if (this->dissolve_counter % (7 - (this->ring_increment_speed - 1)) == 0) {
				this->ring_index++;
				if (this->ring_index > DeviceConfiguration::NumberOfLEDsInRingLED - 1) {
					this->ring_index = 0;
				}
			}

			if (this->dissolve_counter % (6 - (this->finger_increment_speed - 1)) == 0) {
				this->led_index++;
				if (this->led_index > DeviceConfiguration::LEDStripLength - 1) {
					this->led_index = 0;
				}

				this->finger_index++;
				if (this->finger_index > DeviceConfiguration::LEDStripLength - 1) {
					this->finger_index = DeviceConfiguration::NumberOfLEDsInRingLED;
				}
			}
		}

		void clear()
		{
			this->led_index = 0;
			this->dissolve_counter = 0;
		}

		int indexOfDissolveCounter()
		{
			return this->dissolve_counter;
		}

		int indexOfLED()
		{
			return this->led_index;
		}

		int indexOfRingCounter()
		{
			return this->ring_index;
		}

		int indexOfFingerCounter()
		{
			return this->finger_index;
		}
	};

	class LightEmitter
	{
	protected:
		LED::Strip *strip = nullptr;
		EmitCounter counter;
		bool running = true;
		std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emit_handler = [](LightEmitter *emitter, LED::Strip *strip, EmitCounter counter) { return; };
		std::string pattern_name;

	public:
		bool resume_when_handler_update = true;
		LightEmitter(LED::Strip *strip)
		{
			this->strip = strip;
		}

		virtual ~LightEmitter(){};

		void update()
		{
			if (this->running == false) {
				return;
			}

			this->counter.increment();

			this->emit_handler(this, this->strip, this->counter);
			this->strip->show();
		}

		void clear()
		{
			this->counter.clear();
			BasePattern::clear(this->strip);
		}

		void resume()
		{
			this->running = true;
		}

		void suspend()
		{
			this->running = false;
		}

		void setFingerSpeed(int speed)
		{
			this->counter.setFingerSpeed(speed);
		}

		void setRingSpeed(int speed)
		{
			this->counter.setRingSpeed(speed);
		}

		int fingerSpeed()
		{
			return this->counter.fingerSpeed();
		}

		int ringSpeed()
		{
			return this->counter.ringSpeed();
		}

		void setEmitHandler(std::string pattern_name, std::function<void(LightEmitter *emitter, LED::Strip *strip, EmitCounter counter)> emit_handler, bool fource_update = false)
		{
			if (fource_update == false && this->pattern_name == pattern_name) {
				return;
			}
			this->clear();

			this->pattern_name = pattern_name;
			this->emit_handler = emit_handler;
			if (this->resume_when_handler_update) {
				this->resume();
			}
		}
	};
}
}
