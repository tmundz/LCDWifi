#ifndef HTTP_H
#define HTTP_H
#include "driver/gpio.h"
#include "esp_http_server.h"
#include "esp_log.h"
esp_err_t on_handler(httpd_req_t *req);
esp_err_t off_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);
void led_init();

#endif // !HTTP_H
