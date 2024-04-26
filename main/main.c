#include "http.h"
#include "wifi.h"
#define PORT 80

void app_main(void) {
  wifi_connection();
  led_init();
  start_webserver();
}
