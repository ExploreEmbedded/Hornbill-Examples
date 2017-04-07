#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "certs.h"
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "currentSensor.h"
#include "max6675.h"
#include "driver/gpio.h"
#include "rom\gpio.h"
#include "driver/adc.h"

#define WIFI_SSID "my_ssid"
#define WIFI_PASS "my_ssid_pwd"

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
double tempCelcius, tempFahr, currentIrms;

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			IOT_WARN("Manual Reconnect Successful");
		} else {
			IOT_WARN("Manual Reconnect Failed - %d", rc);
		}
	}
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    fprintf(stderr, "Setting WiFi configuration SSID %s...\n", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void record_temp_task(void *pvParameters) 
{
		xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
		fprintf(stderr, "Connected to AP\n");

    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;

    IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

    mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = AWS_IOT_MQTT_HOST;
    mqttInitParams.port = AWS_IOT_MQTT_PORT;
    mqttInitParams.pRootCALocation = "root-CA.crt";
    mqttInitParams.pDeviceCertLocation = "certificate.pem.crt";
    mqttInitParams.pDevicePrivateKeyLocation = "private.pem.key";
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 20000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;

    IoT_Error_t rc = FAILURE;
    AWS_IoT_Client client;
    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(SUCCESS != rc) {
      IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
      abort();
    }

    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
    connectParams.isWillMsgPresent = false;

    IOT_INFO("Connecting...");
    rc = aws_iot_mqtt_connect(&client, &connectParams);
    if(SUCCESS != rc) {
      IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
      abort();
    }

    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if(SUCCESS != rc) {
      IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
      abort();
    }

    
    const char *topicName = "$aws/things/c0ee87d2bd955a0cf217bbc263/shadow/update";

    char cPayload[150];
    uint32_t payloadCount = 0;
    sprintf(cPayload, "%s : %d ", "hello from SDK", payloadCount);

    IoT_Publish_Message_Params paramsQOS0;
    paramsQOS0.qos = QOS0;
    paramsQOS0.payload = (void *) cPayload;
    paramsQOS0.isRetained = 0;

    Timer sendit;
    countdown_ms(&sendit, 1500);

    uint32_t reconnectAttempts = 0;
    uint32_t reconnectedCount = 0;

    while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)) {
      IOT_DEBUG("Top of loop: payloadCount=%d, reconnectAttempts=%d, reconnectedCount=%d\n", payloadCount, reconnectAttempts, reconnectedCount);

      // Max time the yield function will wait for read messages
      rc = aws_iot_mqtt_yield(&client, 1000);
      if(NETWORK_ATTEMPTING_RECONNECT == rc) {
        reconnectAttempts++;
        IOT_DEBUG("Reconnecting...\n");
        // If the client is attempting to reconnect we will skip the rest of the loop.
        continue;
      }

      if(NETWORK_RECONNECTED == rc) {
        reconnectedCount++;
        IOT_DEBUG(stderr, "Reconnected...\n");
      }

      if(!has_timer_expired(&sendit)) {
        IOT_INFO("--> sleeping it off");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        continue;
      } 
       
      tempCelcius = max6675_readCelsius();
      currentIrms = currentSensor_calcIrms(2000);
   
    sprintf(cPayload,"{\"state\": { \"desired\" : {\"temperature\" : %f, \"power\" : %f }}}",tempCelcius,currentIrms*230);  
    printf("\n{\"state\": { \"desired\" : {\"temperature\" : %f, \"power\" : %f }}}",tempCelcius,currentIrms*230);
 
      
      
      
      paramsQOS0.payloadLen = strlen(cPayload);
      rc = aws_iot_mqtt_publish(&client, topicName, strlen(topicName), &paramsQOS0);
      if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
        IOT_DEBUG("QOS0 publish ack not received.\n");
        rc = SUCCESS;
      }

      if(SUCCESS != rc) {
        IOT_ERROR("An error occurred in the loop.\n");
      } else {
        IOT_INFO("Publish done\n");
      }

      countdown_ms(&sendit, 150);
    }

    IOT_ERROR("Escaped loop...\n");
    abort();
}

void app_main(void)
{
  nvs_flash_init();
  initialise_wifi();
  max6675_init(12,14,27); // SCK:6   CS:15   MISO:7
  currentSensor_currentPin(ADC1_CHANNEL_0,111.1);
  xTaskCreate(&record_temp_task, "record_temp_task", 8192*2, NULL, 5, NULL);
}
