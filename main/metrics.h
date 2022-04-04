#include "prom.h"

prom_counter_t *request_counter;

void request_counter_init(void) {
  request_counter = prom_collector_registry_must_register_metric(
    prom_counter_new("req_counter", "counts requests to /metrics", 0, NULL));
}