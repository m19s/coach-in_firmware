#pragma once

#include <array>
#include <cstring>
#include <driver/adc.h>
#include <esp_gap_ble_api.h>
#include <sdkconfig.h>

namespace m19s
{
namespace coach_in
{
	namespace ESP32
	{
		namespace Configuration
		{
			static const std::string DeviceName = "coach-in";
			static const std::string DeviceVerson = "DEVKIT2";
			static const std::string FirmareVersion = "2018-07-15";
		}

		namespace UUID
		{
			static const std::string DeviceInfoServiceUUID = "6f876815-73fa-49eb-9e5f-052c2b448430";

			static const std::string DeviceInfoServiceDeviceNameCharacteristicUUID = "6f876815-73fa-49eb-9e5f-052c2b448431";
			static const std::string DeviceInfoServiceDeviceVersionCharacteristicUUID = "6f876815-73fa-49eb-9e5f-052c2b448432";
			static const std::string DeviceInfoServiceDeviceFirmwareVersionCharacteristicUUID = "6f876815-73fa-49eb-9e5f-052c2b448433";

			static const std::string EMSServiceUUID = "6f876815-73fa-49eb-9e5f-152c2b448440";
			static const std::string EMSServiceDriveCharacteristicUUID = "6f876815-73fa-49eb-9e5f-152c2b448441";
			static const std::string EMSServiceChannel1CharacteristicUUID = "6f876815-73fa-49eb-9e5f-152c2b448442";
			static const std::string EMSServiceChannel2CharacteristicUUID = "6f876815-73fa-49eb-9e5f-152c2b448443";
			static const std::string EMSServiceChannel3CharacteristicUUID = "6f876815-73fa-49eb-9e5f-152c2b448444";
			static const std::string EMSServiceChannel4CharacteristicUUID = "6f876815-73fa-49eb-9e5f-152c2b448445";

			static const std::string DeviceStatusServiceUUID
			    = "6f876815-73fa-49eb-9e5f-252c2b448430";
			static const std::string DeviceStatusServiceModeCharacteristicUUID
			    = "6f876815-73fa-49eb-9e5f-252c2b448431";
		}
	}
}
}
