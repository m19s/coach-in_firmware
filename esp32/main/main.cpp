#include <FreeRTOS-cpp_task.h>
#include <SPIWrapper.h>
#include <StreamLogger/Logger.h>
#include <coach-in/Packet.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <bitset>

extern "C" {
void app_main();
}

using namespace m2d::ESP32;
void app_main()
{
	static Logger::Group logger("coach_in");

	static m2d::FreeRTOS::Task task("EMS task", 10, 1024 * 3, [&] {
		SPIWrapper spi(4000000, 0, (gpio_num_t)GPIO_NUM_18, (gpio_num_t)GPIO_NUM_23, (gpio_num_t)GPIO_NUM_19, (gpio_num_t)GPIO_NUM_5, VSPI_HOST, (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_TXBIT_LSBFIRST));
		while (1) {
			// 0b000 00001 0001 0001
			m19s::coach_in::ESP32::Packet packet;
			packet.channel_identifier = 0;
			packet.pulse = 50;
			packet.frequency = 60;
			packet.duration = 600;

			auto v = packet.to_byte_vector();
			uint8_t *data = &v[0];

			logger.debug << std::bitset<8>(v[0]).to_string() << std::bitset<8>(v[1]).to_string() << Logger::endl;

			m2d::ESP32::SPITransaction t;
			t.set_tx_buffer(data, 2);
			assert(spi.transmit(t) == ESP_OK);

			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
	});
	logger.debug << "Start ems task" << Logger::endl;
	task.run();
}
