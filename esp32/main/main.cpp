#include <FreeRTOS-cpp_task.h>
#include <StreamLogger/Logger.h>
#include <coach-in/ControlBoard.h>
#include <coach-in/PeripheralBoard.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <string>

#include <bitset>

extern "C" {
void app_main();
}

using namespace m2d::ESP32;
using namespace m19s::coach_in;

void app_main()
{
	static Logger::Group logger("coach_in");

	static auto devkit = ESP32::DevKit2(std::string(CONFIG_COACH_IN_DEVICE_NAME));
	devkit.run();
	logger.debug << "coach-in start" << Logger::endl;

	static auto sensor_board = Peripheral::Board();
	sensor_board.run();

	gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_5, 0);
}
