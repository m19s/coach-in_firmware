#pragma once

#include <sdkconfig.h>

#include <cstdlib>
#include <functional>

#include <StreamLogger/Logger.h>
#include <WS2812-Utility/LED.h>

#include <BLEDevice.h>

#include <coach-in/Configuration.h>
#include <coach-in/Packet.h>
#include <driver/uart.h>

#include <bitset>

using namespace m2d::ESP32;
static Logger::Group logger("coach_in");

namespace m19s
{
namespace coach_in
{
	namespace ESP32
	{
		class EventHandledCharacteristicCallbacks : public BLECharacteristicCallbacks
		{
		public:
			std::function<void(const char *)> connect_handler = [](const char *) {};
			std::function<void(const char *)> disconnect_handler = [](const char *) {};
			std::function<void(const char *)> write_handler = [](const char *) {};
			std::function<void(const char *)> read_handler = [](const char *) {};
		};

		class Board
		{
		protected:
			m2d::ESP32::LED::Strip *led_strip;

			BLEServer *server;
			BLEService *device_direction_service;
			BLEService *device_info_service;
			BLEService *device_status_service;
			BLEService *ems_service;

			class EMSDriveCharacteristicHandler : public EventHandledCharacteristicCallbacks
			{
			public:
				void onWrite(BLECharacteristic *c)
				{
					this->write_handler(c->getValue().c_str());
				}
			};

		public:
			typedef enum
			{
				NormalMode = 0,
				AlexaMode = 1
			} Mode;

			std::function<void()> channel_drive_handler;

			Board(std::string name)
			{
				auto tx = (gpio_num_t)GPIO_NUM_5; // D11
				auto rx = (gpio_num_t)GPIO_NUM_19; // D10

				uart_config_t uart_config = {
					.baud_rate = 4800,
					.data_bits = UART_DATA_8_BITS,
					.parity = UART_PARITY_DISABLE,
					.stop_bits = UART_STOP_BITS_1,
					.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
				};
				ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
				ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, tx, rx, (gpio_num_t)UART_PIN_NO_CHANGE, (gpio_num_t)UART_PIN_NO_CHANGE));
				ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0));

				this->led_strip = new LED::Strip(GPIO_NUM_17, Configuration::LEDStripLength, RMT_CHANNEL_1);

				BLEDevice::init(name);
				this->server = BLEDevice::createServer();

				this->device_info_service = server->createService(UUID::DeviceInfoServiceUUID.c_str());
				BLECharacteristic *device_name_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceNameCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
				device_name_characteristic->setValue(name);
				BLECharacteristic *device_version_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceVersionCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
				device_version_characteristic->setValue(Configuration::DeviceVerson);
				BLECharacteristic *device_firmware_version_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
				device_firmware_version_characteristic->setValue(Configuration::FirmareVersion);
				BLECharacteristic *device_direction_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceDirectionCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
#ifdef CONFIG_COACH_IN_HAND_RIGHT
				device_direction_characteristic->setValue("LEFT");
#else
				device_direction_characteristic->setValue("RIGHT");
#endif

				this->ems_service = server->createService(UUID::EMSServiceUUID.c_str());
				ems_service->createCharacteristic(UUID::EMSServiceStatusCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
				BLECharacteristic *drive_characteristic = ems_service->createCharacteristic(UUID::EMSServiceDriveCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				DrivePacket drive_packet(0, 0);
				uint8_t d = drive_packet.to_8bits_data();
				drive_characteristic->setValue(&d, 1);

				auto ems_drive_handler = new EMSDriveCharacteristicHandler();
				ems_drive_handler->write_handler = [this](const char *data) {
					m19s::coach_in::ESP32::DrivePacket packet((uint8_t)data[0]);
					this->send_packet(&packet);
				};
				drive_characteristic->setCallbacks(ems_drive_handler);
				this->device_status_service = server->createService(UUID::DeviceStatusServiceUUID.c_str());
			}

			void run()
			{
				device_info_service->start();
				ems_service->start();
				device_status_service->start();

				BLEAdvertising *advertising = this->server->getAdvertising();
				BLEAdvertisementData advertising_data;
				advertising_data.setCompleteServices(BLEUUID(UUID::DeviceInfoServiceUUID));
				advertising_data.setManufacturerData(Configuration::FirmareVersion);
				advertising->setAdvertisementData(advertising_data);
				server->startAdvertising();

				static m2d::FreeRTOS::Task led_task("LED task", 10, 1024 * 5, [&] {
					while (1) {
						for (int i = 0; i < 16; i++) {
							auto color = LED::Color::green(80);
							this->led_strip->setPixel(i, color.pixel_color());
						}
						this->led_strip->show();
						vTaskDelay(1000 / portTICK_RATE_MS);
						for (int i = 0; i < 16; i++) {
							auto color = LED::Color::red(80);
							this->led_strip->setPixel(i, color.pixel_color());
						}
						this->led_strip->show();
						vTaskDelay(1000 / portTICK_RATE_MS);
					}
				});
				led_task.run();
			}

			void send_packet(coach_in::ESP32::Packet *packet)
			{
				Logger::I << "send packet" << Logger::endl;

				this->send_flush_command();

				uint8_t type = packet->type();
				uart_write_bytes(UART_NUM_1, (const char *)&type, 1);
				Logger::I << std::bitset<8>(type).to_string() << ", ";
				vTaskDelay(50 / portTICK_PERIOD_MS);

				for (uint8_t d : packet->to_byte_vector()) {
					Logger::I << std::bitset<8>(d).to_string() << ", ";
					uart_write_bytes(UART_NUM_1, (const char *)&d, 1);
					vTaskDelay(50 / portTICK_PERIOD_MS);
				}
				Logger::I << Logger::endl;
			}

			void send_flush_command()
			{
				uint8_t data = 0xff;
				uart_write_bytes(UART_NUM_1, (const char *)&data, 1);
				vTaskDelay(50 / portTICK_PERIOD_MS);
			}
		};

		class DevKit2 : public Board
		{
		private:
			class ChannelCharacteristicHandler : public EventHandledCharacteristicCallbacks
			{
				void onWrite(BLECharacteristic *c)
				{
					this->write_handler(c->getValue().c_str());
				}
			};

			class DeviceModeCharacteristicHandler : public EventHandledCharacteristicCallbacks
			{
				void onWrite(BLECharacteristic *c)
				{
					// Do something because a new value was written.
				}
			};

		public:
			DevKit2(std::string name)
			    : Board(Configuration::DeviceName + "_" + name)
			{
				auto channel_handler = new ChannelCharacteristicHandler();
				channel_handler->write_handler = [this](const char *data) {
					uint16_t packet_data = 0;
					packet_data = (uint8_t)data[0];
					packet_data <<= 8;
					packet_data |= (uint8_t)data[1];

					m19s::coach_in::ESP32::ChannelPacket packet(packet_data);
					this->send_packet(&packet);
				};

				ChannelPacket channel_packet(0, 0, 0, 0);
				uint16_t data = channel_packet.to_16bits_data();
				BLECharacteristic *channel1_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel1CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel1_characteristic->setValue(data);
				channel1_characteristic->setCallbacks(channel_handler);

				BLECharacteristic *channel2_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel2CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel2_characteristic->setValue(data);
				channel2_characteristic->setCallbacks(channel_handler);

				BLECharacteristic *channel3_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel3CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel3_characteristic->setValue(data);
				channel3_characteristic->setCallbacks(channel_handler);

				BLECharacteristic *channel4_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel4CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel4_characteristic->setValue(data);
				channel4_characteristic->setCallbacks(channel_handler);

				BLECharacteristic *device_mode_characteristic = this->device_status_service->createCharacteristic(UUID::DeviceStatusServiceModeCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);

				int mode = NormalMode;
				device_mode_characteristic->setValue(mode);
				device_mode_characteristic->setCallbacks(new DeviceModeCharacteristicHandler());
			}
		};
	}
}
}
