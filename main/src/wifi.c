#include "include/wifi.h"
// event handler wifi events
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  // connecting
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    ESP_LOGI(TAG, "Connecting to AP..."); // msg
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    // trying to connect
    if (s_retry_num < MAX_FAILURES) {
      ESP_LOGI(TAG, "Reconnecting to AP...");
      esp_wifi_connect();
      s_retry_num++;
    } else {
      xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
    }
  }
}

// event handler for ip events
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data) {
  if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
  }
}

// connect to wifi and return the result
esp_err_t connect_wifi() {
  int status = WIFI_FAILURE;

  // init esp network
  ESP_ERROR_CHECK(esp_netif_init());
  // create wifi station in the wifi driver
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  // setup wifi station with the default wifi conf
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // EVENT LOOP
  wifi_event_group = xEventGroupCreate();

  esp_event_handler_instance_t wifi_handler_event_instance;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL,
      &wifi_handler_event_instance));

  esp_event_handler_instance_t got_ip_event_instance;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL,
      &got_ip_event_instance));
  // START WIFI DRIVER
  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = "ssid",         // replace with your own
              .password = "password", // replace with your own
              .threshold.authmode = WIFI_AUTH_WPA2_PSK,
              .pmf_cfg = {.capable = true, .required = false},

          },
  };

  // set the wifi controller to be a station
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  // set up wifi config
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  // start wifi
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "STA initialization complete");

  EventBits_t bits =
      xEventGroupWaitBits(wifi_event_group, WIFI_SUCCESS | WIFI_FAILURE,
                          pdFALSE, pdFALSE, portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
   * can test which event actually happened. */
  if (bits & WIFI_SUCCESS) {
    ESP_LOGI(TAG, "Connected to ap");
    status = WIFI_SUCCESS;
  } else if (bits & WIFI_FAILURE) {
    ESP_LOGI(TAG, "Failed to connect to ap");
    status = WIFI_FAILURE;
  } else {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
    status = WIFI_FAILURE;
  }

  /* The event will not be processed after unregister */
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
      IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
      WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
  vEventGroupDelete(wifi_event_group);
  return status;
}

// connect to the server and return the result
esp_err_t connect_tcp_server(void) {
  struct sockaddr_in serverInfo = {0};
  char readBuffer[1024] = {0};

  serverInfo.sin_family = AF_INET;
  serverInfo.sin_addr.s_addr = 0x0100007f;
  serverInfo.sin_port = htons(12345);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    ESP_LOGE(TAG, "Failed to create a socket..?");
    return TCP_FAILURE;
  }

  if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0) {
    ESP_LOGE(TAG, "Failed to connect to %s!",
             inet_ntoa(serverInfo.sin_addr.s_addr));
    close(sock);
    return TCP_FAILURE;
  }

  ESP_LOGI(TAG, "Connected to TCP server.");
  bzero(readBuffer, sizeof(readBuffer));
  int r = read(sock, readBuffer, sizeof(readBuffer) - 1);
  for (int i = 0; i < r; i++) {
    putchar(readBuffer[i]);
  }

  if (strcmp(readBuffer, "HELLO") == 0) {
    ESP_LOGI(TAG, "WE DID IT!");
  }

  return TCP_SUCCESS;
}
