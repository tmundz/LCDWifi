#include "http.h"
#include "esp_http_server.h"
#include "hal/gpio_types.h"

#define LED_PIN 27

static httpd_uri_t uri_on = {
    .uri = "/on", .method = HTTP_POST, .handler = on_handler, .user_ctx = NULL};

static httpd_uri_t uri_off = {.uri = "/off",
                              .method = HTTP_POST,
                              .handler = off_handler,
                              .user_ctx = NULL};

void led_init() {
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

esp_err_t on_handler(httpd_req_t *req) {
  ESP_LOGI("LED_CTRL", "LED ON");
  gpio_set_level(LED_PIN, 1);
  const char *resp = "LED turned ON";

  httpd_resp_send(req, resp, strlen(resp));
  return ESP_OK;
}

esp_err_t off_handler(httpd_req_t *req) {
  ESP_LOGI("LED_CTRL", "LED OFF");
  gpio_set_level(LED_PIN, 0);
  const char *resp = "LED turned off";

  httpd_resp_send(req, resp, strlen(resp));
  return ESP_OK;
}

httpd_handle_t start_webserver(void) {
  // gen default config
  httpd_config_t conf = HTTPD_DEFAULT_CONFIG();

  // empty handle
  httpd_handle_t server = NULL;

  // start esp32  webserver
  if (httpd_start(&server, &conf) == ESP_OK) {
    // set up URI
    httpd_register_uri_handler(server, &uri_on);
    httpd_register_uri_handler(server, &uri_off);
  }
  return server;
}

void stop_webserver(httpd_handle_t server) {
  if (server) {
    // stop http server
    httpd_stop(server);
  }
}
