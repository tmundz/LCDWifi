#include "../include/wifi.h"
// heel
// event handler wifi events
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    // connecting
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Connecting to AP..."); // msg
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENt && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        //trying to connect
        if (s_retry_num < MAX_FAILURES) {
            ESP_LOGI(TAG, "Reconnecting to AP...");
			esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupBits(wifi_event_group, WIFI_FAILURE);
        }
    }
}
// event handler for ip events
// connect to wifi and return the result
esp_err_t connect_wifi() {
  int status = WIFI_FAILURE;

    // init esp network

  return status;
}
