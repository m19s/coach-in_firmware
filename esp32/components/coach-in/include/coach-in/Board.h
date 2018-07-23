#pragma once

#include <cstdlib>
#include <functional>
#include <sdkconfig.h>

#include <BLEDevice.h>
#include <SPIWrapper.h>
#include <StreamLogger/Logger.h>
#include <coach-in/Configuration.h>
#include <coach-in/Packet.h>
#include <parallel_lines.h>

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
			SPIWrapper *spi;

			BLEServer *server;
			BLEService *device_info_service;
			BLEService *device_status_service;
			BLEService *ems_service;

			class EMSDriveCharacteristicHandler : public EventHandledCharacteristicCallbacks
			{
			public:
				void onWrite(BLECharacteristic *c)
				{
					const char *data = c->getValue().c_str();
					char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t *)data, 1);
					logger.debug << pHex << Logger::endl;
					logger.debug << (uint8_t)data[0] << Logger::endl;

					this->write_handler(c->getValue().c_str());
				}
			};

		public:
			typedef enum
			{
				NormalMode = 0,
				AlexaMode = 1
			} Mode;

			Board(std::string name)
			{
				this->spi = new SPIWrapper(1000000, 3, (gpio_num_t)GPIO_NUM_18, (gpio_num_t)GPIO_NUM_23, (gpio_num_t)GPIO_NUM_19, (gpio_num_t)GPIO_NUM_5, VSPI_HOST, (SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST | SPI_DEVICE_NO_DUMMY));

				BLEDevice::init(name);
				this->server = BLEDevice::createServer();

				this->device_info_service = server->createService(UUID::DeviceInfoServiceUUID.c_str());
				BLECharacteristic *device_name_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceNameCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
				device_name_characteristic->setValue(Configuration::DeviceName);
				BLECharacteristic *device_version_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceVersionCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
				device_version_characteristic->setValue(Configuration::DeviceVerson);
				BLECharacteristic *device_firmware_version_characteristic = device_info_service->createCharacteristic(UUID::DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ);
				device_firmware_version_characteristic->setValue(Configuration::FirmareVersion);

				this->ems_service = server->createService(UUID::EMSServiceUUID.c_str());
				ems_service->createCharacteristic(UUID::EMSServiceStatusCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
				BLECharacteristic *drive_characteristic = ems_service->createCharacteristic(UUID::EMSServiceDriveCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				DrivePacket drive_packet(0, 0);
				uint8_t d = drive_packet.to_8bits_data();
				drive_characteristic->setValue(&d, 1);

				auto ems_drive_handler = new EMSDriveCharacteristicHandler();
				ems_drive_handler->write_handler = [&](const char *data) {
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

				static m2d::FreeRTOS::Task task("packet task", 10, 1024 * 5, [&] {
					parallel_lines::scheduler *s = parallel_lines::scheduler::shared_scheduler();
					while (1) {
						s->update();
						vTaskDelay(25 / portTICK_PERIOD_MS);
					}
				});
				task.run();
			}

			void send_packet(coach_in::ESP32::Packet *packet)
			{
				m2d::ESP32::SPITransaction t;
				uint8_t type = packet->type();

				Logger::I << std::bitset<16>(packet->to_16bits_data()).to_string() << Logger::endl;

				t.set_tx_buffer(&type, 1);
				assert(this->spi->transmit(t) == ESP_OK);

				for (uint8_t d : packet->to_byte_vector()) {
					t.set_tx_buffer(&d, 1);
					assert(this->spi->transmit(t) == ESP_OK);
					Logger::I << std::bitset<8>(d).to_string() << ", ";
					vTaskDelay(20 / portTICK_PERIOD_MS);
				}

				Logger::I << Logger::endl;
			}
		};

		class DevKit2 : public Board
		{
		private:
			class ChannelCharacteristicHandler : public EventHandledCharacteristicCallbacks
			{
				void onWrite(BLECharacteristic *c)
				{
					const char *data = c->getValue().c_str();
					char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t *)data, 1);
					logger.debug << pHex << Logger::endl;
					logger.debug << (uint8_t)data[0] << Logger::endl;

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
			bool go = false;
			DevKit2(std::string name)
			    : Board(Configuration::DeviceName + name)
			{
				auto channel_handler = new ChannelCharacteristicHandler();
				channel_handler->write_handler = [this](const char *data) {
					parallel_lines::scheduler *s = parallel_lines::scheduler::shared_scheduler();

					s->push([this, data] {
						uint16_t packet_data = 0;
						packet_data = (uint8_t)data[0];
						packet_data <<= 8;
						packet_data |= (uint8_t)data[1];

						m19s::coach_in::ESP32::ChannelPacket packet(packet_data);
						this->send_packet(&packet);
					});
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
