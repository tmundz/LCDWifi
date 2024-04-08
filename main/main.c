#include "include/wifi.h"
#include <stdio.h>

void app_main(void) {
    esp_err_t status = WIFI_FAILURE;

    // init storage
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    // connect to wireless AP
    status = connect_wifi();
    if (WIFI_SUCCESS != status) {
        ESP_LOGI(TAG, "Failed to associate to AP, dying...");
        return;
    }

    status = connect_tcp_server();
    if (TCP_SUCCESS != status) {
        ESP_LOGI(TAG, "Failed to connect to remote server, dying...");
		return;
    }
}
