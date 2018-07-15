#include <FreeRTOS-cpp_task.h>
#include <StreamLogger/Logger.h>
#include <coach-in/Board.h>

extern "C" {
void app_main();
}

using namespace m2d::ESP32;
using namespace m19s::coach_in::ESP32;

void app_main()
{
	static Logger::Group logger("coach_in");

	DevKit2 devkit("a");
	devkit.run();

	logger.debug << "coach-in start" << Logger::endl;
	// static m2d::FreeRTOS::Task task("EMS task", 10, 1024 * 3, [&] {
	// 	while (1) {
	// 		m19s::coach_in::ESP32::ChannelPacket packet(0, 50, 60, 600);

	// 		auto v = packet.to_byte_vector();
	// 		uint8_t *data = &v[0];
	// 		logger.debug << std::bitset<8>(data[0]).to_string() << ", " << std::bitset<8>(data[1]).to_string() << Logger::endl;

	// 		m2d::ESP32::SPITransaction t;
	// 		t.set_tx_buffer(&data[0], 1);
	// 		assert(spi.transmit(t) == ESP_OK);
	// 		vTaskDelay(30 / portTICK_PERIOD_MS);
	// 		t.set_tx_buffer(&data[1], 1);
	// 		assert(spi.transmit(t) == ESP_OK);

	// 		vTaskDelay(500 / portTICK_PERIOD_MS);
	// 		m19s::coach_in::ESP32::DrivePacket drive_packet(0, 0);
	// 		vTaskDelay(3000 / portTICK_PERIOD_MS);
	// 	}
	// });
	// logger.debug << "Start ems task" << Logger::endl;
	// task.run();
}
