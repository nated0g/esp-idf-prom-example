/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "rest_server.h"
#include <string.h>
#include <fcntl.h>
#include "esp_chip_info.h"
#include "esp_random.h"
#include "esp_log.h"
#include "prom.h"

prom_collector_registry_t *PROM_ACTIVE_REGISTRY;

static const char *REST_TAG = "esp-rest";

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* Handler for Prometheus monitoring endpoint */
static esp_err_t prom_metrics_handler(httpd_req_t *req) {
    prom_counter_inc(request_counter, NULL);
    const char *buf = prom_collector_registry_bridge(PROM_ACTIVE_REGISTRY);
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, buf);
    free((void *)buf);
    return ESP_OK;
}

esp_err_t start_rest_server(const char *base_path)
{
    PROM_ACTIVE_REGISTRY = PROM_COLLECTOR_REGISTRY_DEFAULT;

    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(REST_TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    /* URI handler Prometheus monitoring endpoint */
    httpd_uri_t prom_get_uri = {
        .uri = "/metrics",
        .method = HTTP_GET,
        .handler = prom_metrics_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &prom_get_uri);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}
