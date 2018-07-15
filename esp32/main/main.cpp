#include <FreeRTOS-cpp_task.h>
#include <StreamLogger/Logger.h>
#include <coach-in/Board.h>
#include <sdkconfig.h>
#include <string>

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
}
