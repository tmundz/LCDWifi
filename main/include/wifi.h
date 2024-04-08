#ifndef WIFI_H
#define WIFI_H
// imports
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "netdb.h"
// defines
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define MAX_FAILURES 10
// globals
static EventGroupHandle_t wifi_event_group;
static int s_retry_num = 0;
static const char *TAG = "WIFI";
#endif // !WIFI_H
