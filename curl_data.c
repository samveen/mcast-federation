#include <string.h>
#include <stdio.h>

# include "curl_data.h"

size_t update_mycurl_data(void *buffer, size_t size, size_t nmemb, rawdata_t *data)
{
    /* Init data->end on first run*/
    if (data->end == NULL) 
        data->end=data->buffer;

    memcpy(data->end, buffer, size*nmemb);
    data->end += size*nmemb;
    *(data->end) = '\0';
    return size*nmemb;
}
