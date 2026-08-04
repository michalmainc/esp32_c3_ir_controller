#include "onewire_utils.h"

#include <stdio.h>


esp_err_t onewire_address_to_string(
    uint64_t address,
    char *buffer,
    size_t buffer_size
)
{
    if (
        buffer == NULL ||
        buffer_size < ONEWIRE_ADDRESS_STRING_SIZE
    )
    {
        return ESP_ERR_INVALID_ARG;
    }

    /*
     * Biblioteka przechowuje adres jako uint64_t.
     * Najmłodszy bajt zawiera kod rodziny, np. 0x28.
     *
     * Wypisujemy bajty od najmłodszego do najstarszego,
     * aby otrzymać standardowy zapis adresu 1-Wire:
     *
     * 28 FF B4 A5 31 17 03 25
     */
    int written = snprintf(
        buffer,
        buffer_size,
        "%02X%02X%02X%02X%02X%02X%02X",
        (unsigned int)((address >> 0)  & 0xFF),
        (unsigned int)((address >> 8)  & 0xFF),
        (unsigned int)((address >> 16) & 0xFF),
        (unsigned int)((address >> 24) & 0xFF),
        (unsigned int)((address >> 32) & 0xFF),
        (unsigned int)((address >> 40) & 0xFF),
        (unsigned int)((address >> 48) & 0xFF)
        // (unsigned int)((address >> 56) & 0xFF)
    );

    if (
        written < 0 ||
        (size_t)written >= buffer_size
    )
    {
        buffer[0] = '\0';

        return ESP_FAIL;
    }

    return ESP_OK;
}