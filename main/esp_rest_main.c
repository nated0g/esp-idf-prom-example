/* HTTP Restful API Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_vfs_semihost.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mdns.h"
#include "lwip/apps/netbiosns.h"
#include "protocol_examples_common.h"
#include "prom.h"
#include "rest_server.h"
#include "metrics.h"

static const char *TAG = "esp-idf-prom-test";

prom_histogram_t *test_histogram;

static void init(void) {
  // Initialize the Default registry
  prom_collector_registry_default_init();

  test_histogram = prom_collector_registry_must_register_metric(
    prom_histogram_new(
      "test_histogram",
      "histogram under test",
      prom_histogram_buckets_linear(5.0, 5.0, 2),
      0,
      NULL
    )
  );
}

void app_main(void)
{
    init();
    request_counter_init();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(start_rest_server(CONFIG_EXAMPLE_WEB_MOUNT_POINT));

}
