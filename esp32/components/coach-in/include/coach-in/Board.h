#pragma once

#include "./Configuration.h"
#include "./Packet.h"
#include <BLEDevice.h>

namespace m19s
{
namespace coach_in
{
	namespace ESP32
	{
		class Board
		{
		protected:
			BLEServer *server;
			BLEService *device_info_service;
			BLEService *device_status_service;
			BLEService *ems_service;

		public:
			typedef enum
			{
				NormalMode = 0,
				AlexaMode = 1
			} Mode;

			Board(std::string name)
			{
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
				BLECharacteristic *drive_characteristic = ems_service->createCharacteristic(UUID::EMSServiceDriveCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				DrivePacket drive_packet(0, 0);
				uint8_t d = drive_packet.to_8bits_data();
				drive_characteristic->setValue(&d, 1);

				this->device_status_service = server->createService(UUID::DeviceStatusServiceUUID.c_str());
			}

			void run()
			{
				device_info_service->start();
				ems_service->start();
				device_status_service->start();
				server->startAdvertising();
			}
		};

		class DevKit2 : public Board
		{
		public:
			DevKit2()
			    : Board("DevKit2")
			{
				ChannelPacket channel_packet(0, 0, 0, 0);
				uint16_t data = channel_packet.to_16bits_data();
				BLECharacteristic *channel1_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel1CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel1_characteristic->setValue(data);

				BLECharacteristic *channel2_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel2CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel2_characteristic->setValue(data);

				BLECharacteristic *channel3_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel3CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel3_characteristic->setValue(data);

				BLECharacteristic *channel4_characteristic = this->ems_service->createCharacteristic(UUID::EMSServiceChannel4CharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE);
				channel4_characteristic->setValue(data);

				BLECharacteristic *device_mode_characteristic = this->device_status_service->createCharacteristic(UUID::DeviceStatusServiceModeCharacteristicUUID.c_str(), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
				device_mode_characteristic->setValue((int)NormalMode);
			}
		};
	}
}
}
