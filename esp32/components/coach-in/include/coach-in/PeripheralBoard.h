#include <FreeRTOS-cpp_task.h>
#include <I2Cbus.hpp>

#include <MPU.hpp>
#include <mpu/math.hpp> // math helper for dealing with MPU data
#include <mpu/types.hpp> // MPU data types and definitions

namespace m19s
{
namespace coach_in
{
	namespace Peripheral
	{
		class Board
		{
		private:
			MPU_t MPU;

			static constexpr gpio_num_t SDA = GPIO_NUM_27;
			static constexpr gpio_num_t SCL = GPIO_NUM_15;
			static constexpr uint32_t CLOCK_SPEED = 400000; // range from 100 KHz ~ 400Hz

		public:
			Board()
			{
				i2c0.setTimeout(10000);
				i2c0.begin(SDA, SCL, CLOCK_SPEED);
				MPU.setBus(i2c0);
				MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);

				while (esp_err_t err = MPU.testConnection()) {
					uint8_t wai = MPU.whoAmI();
					ESP_LOGE("MPU", "Failed to connect to the MPU, error=%#X, %x", err, wai);
					vTaskDelay(1000 / portTICK_PERIOD_MS);
				}
				ESP_LOGI("MPU", "MPU connection successful!");

				// Initialize
				ESP_ERROR_CHECK(MPU.initialize()); // initialize the chip and set initial configurations
				// Setup with your configurations
				// ESP_ERROR_CHECK(MPU.setSampleRate(50)); // set sample rate to 50 Hz
				// ESP_ERROR_CHECK(MPU.setGyroFullScale(mpud::GYRO_FS_500DPS));
				// ESP_ERROR_CHECK(MPU.setAccelFullScale(mpud::ACCEL_FS_4G));
			}

			void run()
			{
				static m2d::FreeRTOS::Task task("MPU task", 11, 1024 * 7, [&] {
					printf("Reading sensor data:\n");
					mpud::raw_axes_t accelRaw; // x, y, z axes as int16
					mpud::raw_axes_t gyroRaw; // x, y, z axes as int16
					mpud::float_axes_t accelG; // accel axes in (g) gravity format
					mpud::float_axes_t gyroDPS; // gyro axes in (DPS) º/s format
					while (true) {
						// Read
						MPU.acceleration(&accelRaw); // fetch raw data from the registers
						MPU.rotation(&gyroRaw); // fetch raw data from the registers
						// MPU.motion(&accelRaw, &gyroRaw);  // read both in one shot
						// Convert
						accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);
						gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
						// Debug
						printf("accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
						printf("gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\n", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
						vTaskDelay(20 / portTICK_RATE_MS);
					}
				});
				task.run(1);
			}
		};
	}
}
}