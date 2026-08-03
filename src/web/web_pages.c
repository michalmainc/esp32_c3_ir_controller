#include "web_pages.h"

#include "generated_assets.h"

#include "esp_http_server.h"


static esp_err_t send_embedded_file(
    httpd_req_t *request,
    const char *content_type,
    const unsigned char *content,
    size_t content_length
)
{
    if (
        request == NULL ||
        content_type == NULL ||
        content == NULL
    )
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = httpd_resp_set_type(
        request,
        content_type
    );

    if (result != ESP_OK)
    {
        return result;
    }

    result = httpd_resp_set_hdr(
        request,
        "Cache-Control",
        "no-cache, no-store, must-revalidate"
    );

    if (result != ESP_OK)
    {
        return result;
    }

    return httpd_resp_send(
        request,
        (const char *)content,
        content_length
    );
}


static esp_err_t index_handler(
    httpd_req_t *request
)
{
    return send_embedded_file(
        request,
        "text/html; charset=utf-8",
        web_index_html,
        web_index_html_length
    );
}


static esp_err_t style_handler(
    httpd_req_t *request
)
{
    return send_embedded_file(
        request,
        "text/css; charset=utf-8",
        web_style_css,
        web_style_css_length
    );
}


static esp_err_t script_handler(
    httpd_req_t *request
)
{
    return send_embedded_file(
        request,
        "application/javascript; charset=utf-8",
        web_app_js,
        web_app_js_length
    );
}


esp_err_t web_pages_register(
    httpd_handle_t server
)
{
    if (server == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    const httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL
    };

    esp_err_t result = httpd_register_uri_handler(
        server,
        &index_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t style_uri = {
        .uri = "/style.css",
        .method = HTTP_GET,
        .handler = style_handler,
        .user_ctx = NULL
    };

    result = httpd_register_uri_handler(
        server,
        &style_uri
    );

    if (result != ESP_OK)
    {
        return result;
    }

    const httpd_uri_t script_uri = {
        .uri = "/app.js",
        .method = HTTP_GET,
        .handler = script_handler,
        .user_ctx = NULL
    };

    return httpd_register_uri_handler(
        server,
        &script_uri
    );
}