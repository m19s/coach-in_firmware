#include <FreeRTOS-cpp_task.h>
#include <StreamLogger/Logger.h>
#include <coach-in/Board.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <string>

#include <bitset>

extern "C" {
void app_main();
}

using namespace m2d::ESP32;
using namespace m19s::coach_in::ESP32;

void app_main()
{
	static Logger::Group logger("coach_in");

	DevKit2 devkit(std::string(CONFIG_COACH_IN_DEVICE_NAME));
	devkit.run();
	logger.debug << "coach-in start" << Logger::endl;

	gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_5, 0);

	while (1) {
		auto drive_packet = DrivePacket(1, 1);
		devkit.send_packet(&drive_packet);
		vTaskDelay(5000 / portTICK_PERIOD_MS);

		auto drive_packet2 = DrivePacket(1, 1, true);
		devkit.send_packet(&drive_packet2);
		vTaskDelay(5000 / portTICK_PERIOD_MS);

		auto channel_packet = ChannelPacket(1, 80, 50, 1500);
		devkit.send_packet(&channel_packet);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}
