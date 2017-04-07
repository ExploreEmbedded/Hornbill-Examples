deps_config := \
	/f/GitHub/Wireless/esp-idf/components/aws_iot/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/bt/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/esp32/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/ethernet/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/fatfs/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/freertos/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/log/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/lwip/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/mbedtls/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/openssl/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/spi_flash/Kconfig \
	/f/GitHub/Wireless/esp-idf/components/bootloader/Kconfig.projbuild \
	/f/GitHub/Wireless/esp-idf/components/esptool_py/Kconfig.projbuild \
	/f/GitHub/Wireless/esp-idf/components/partition_table/Kconfig.projbuild \
	/f/GitHub/Wireless/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
