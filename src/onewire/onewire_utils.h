#ifndef ONEWIRE_UTILS_H
#define ONEWIRE_UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"


#define ONEWIRE_ADDRESS_STRING_SIZE 15


esp_err_t onewire_address_to_string(
    uint64_t address,
    char *buffer,
    size_t buffer_size
);


#endif