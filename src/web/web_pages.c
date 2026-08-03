#include "web_pages.h"


static const char index_html[] =
    "<!DOCTYPE html>"
    "<html lang=\"pl\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<title>ESP32-C3 Controller</title>"

    "<style>"
    "*{box-sizing:border-box;}"

    "body{"
        "margin:0;"
        "min-height:100vh;"
        "display:flex;"
        "align-items:center;"
        "justify-content:center;"
        "padding:20px;"
        "background:#101216;"
        "color:#eeeeee;"
        "font-family:Arial,sans-serif;"
    "}"

    ".panel{"
        "width:100%;"
        "max-width:560px;"
        "padding:24px;"
        "background:#1b1e24;"
        "border:1px solid #343943;"
        "border-radius:14px;"
        "box-shadow:0 10px 35px rgba(0,0,0,.4);"
    "}"

    "h1{"
        "margin:0 0 24px;"
        "text-align:center;"
        "font-size:25px;"
    "}"

    ".status{"
        "padding:16px;"
        "margin-bottom:20px;"
        "background:#252932;"
        "border-radius:10px;"
        "line-height:1.7;"
    "}"

    ".online{"
        "color:#58d68d;"
        "font-weight:bold;"
    "}"

    ".pwm-card{"
        "padding:16px;"
        "margin-bottom:12px;"
        "background:#252932;"
        "border-radius:10px;"
    "}"

    ".pwm-header{"
        "display:flex;"
        "justify-content:space-between;"
        "align-items:center;"
        "margin-bottom:10px;"
        "font-size:17px;"
    "}"

    ".pwm-value{"
        "font-size:20px;"
        "font-weight:bold;"
        "min-width:55px;"
        "text-align:right;"
    "}"

    "input[type=range]{"
        "width:100%;"
        "height:32px;"
        "cursor:pointer;"
    "}"

    ".pwm-info{"
        "margin-top:6px;"
        "font-size:13px;"
        "color:#aeb6c2;"
    "}"

        ".temperature-section{"
        "padding:16px;"
        "margin:20px 0 12px;"
        "background:#252932;"
        "border-radius:10px;"
    "}"

    ".temperature-title{"
        "margin:0 0 12px;"
        "font-size:18px;"
    "}"

    ".temperature-sensor{"
        "padding:12px;"
        "margin-top:10px;"
        "background:#1b1e24;"
        "border:1px solid #343943;"
        "border-radius:8px;"
    "}"

    ".temperature-row{"
        "display:flex;"
        "justify-content:space-between;"
        "align-items:center;"
        "gap:12px;"
    "}"

    ".temperature-address{"
        "font-family:monospace;"
        "font-size:13px;"
        "color:#aeb6c2;"
        "overflow-wrap:anywhere;"
    "}"

    ".temperature-value{"
        "font-size:22px;"
        "font-weight:bold;"
        "white-space:nowrap;"
    "}"

    ".temperature-missing{"
        "color:#e57373;"
    "}"

    ".temperature-empty{"
        "color:#aeb6c2;"
    "}"

    "button{"
        "width:100%;"
        "padding:15px;"
        "margin-top:10px;"
        "border:0;"
        "border-radius:10px;"
        "background:#c94b4b;"
        "color:white;"
        "font-size:17px;"
        "font-weight:bold;"
        "cursor:pointer;"
    "}"

    "button:disabled{"
        "opacity:.6;"
        "cursor:default;"
    "}"

    "#message{"
        "min-height:24px;"
        "margin-top:16px;"
        "text-align:center;"
        "color:#b8c0cc;"
    "}"
    "</style>"
    "</head>"

    "<body>"
    "<main class=\"panel\">"

    "<h1>ESP32-C3 Controller</h1>"

    "<div class=\"status\">"
    "Stan urządzenia: <span class=\"online\">ONLINE</span><br>"
    "PWM: 400 Hz"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM1</span>"
            "<span class=\"pwm-value\" id=\"pwmValue0\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider0\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO2 / D0</div>"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM2</span>"
            "<span class=\"pwm-value\" id=\"pwmValue1\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider1\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO3 / D1</div>"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM3</span>"
            "<span class=\"pwm-value\" id=\"pwmValue2\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider2\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO4 / D2</div>"
    "</div>"

    "<div class=\"pwm-card\">"
        "<div class=\"pwm-header\">"
            "<span>PWM4</span>"
            "<span class=\"pwm-value\" id=\"pwmValue3\">0%</span>"
        "</div>"
        "<input id=\"pwmSlider3\" type=\"range\" "
        "min=\"0\" max=\"100\" step=\"1\" value=\"0\">"
        "<div class=\"pwm-info\">GPIO5 / D3</div>"
    "</div>"

    "<section class=\"temperature-section\">"
        "<h2 class=\"temperature-title\">Temperatura</h2>"
        "<div id=\"temperatureSensors\">"
            "<div class=\"temperature-empty\">Ładowanie danych...</div>"
        "</div>"
    "</section>"

    "<button id=\"restartButton\" onclick=\"restartDevice()\">"
    "Restart ESP32"
    "</button>"

    "<div id=\"message\"></div>"

    "</main>"

    "<script>"
    "const message=document.getElementById('message');"
    "const pwmTimers=[null,null,null,null];"

    "function renderTemperatures(temperature){"
        "const container=document.getElementById('temperatureSensors');"

        "if(!temperature||!Array.isArray(temperature.sensors)){"
            "container.innerHTML="
                "'<div class=\"temperature-empty\">Brak danych temperatury</div>';"
            "return;"
        "}"

        "if(temperature.sensors.length===0){"
            "container.innerHTML="
                "'<div class=\"temperature-empty\">Nie wykryto czujników DS18B20</div>';"
            "return;"
        "}"

        "container.innerHTML='';"

        "for(const sensor of temperature.sensors){"
            "const card=document.createElement('div');"
            "card.className='temperature-sensor';"

            "const row=document.createElement('div');"
            "row.className='temperature-row';"

            "const address=document.createElement('div');"
            "address.className='temperature-address';"
            "address.textContent=sensor.address||'Nieznany adres';"

            "const value=document.createElement('div');"

            "if(sensor.present){"
                "value.className='temperature-value';"
                "value.textContent=Number(sensor.value).toFixed(2)+' °C';"
            "}else{"
                "value.className='temperature-value temperature-missing';"
                "value.textContent='Brak odczytu';"
            "}"

            "row.appendChild(address);"
            "row.appendChild(value);"
            "card.appendChild(row);"
            "container.appendChild(card);"
        "}"
    "}"

    "async function loadStatus(){"
        "try{"
            "const response=await fetch('/api/status');"

            "if(!response.ok){"
                "throw new Error('HTTP '+response.status);"
            "}"

            "const data=await response.json();"

            "if(!Array.isArray(data.outputs.pwm)||data.outputs.pwm.length<4){"
                "throw new Error('Nieprawidlowe dane PWM');"
            "}"

            "for(let channel=0;channel<4;channel++){"
                "const value=data.outputs.pwm[channel];"
                "const slider=document.getElementById('pwmSlider'+channel);"
                "const valueText=document.getElementById('pwmValue'+channel);"

                "slider.value=value;"
                "valueText.textContent=value+'%';"
            "}"
            "renderTemperatures(data.temperature);"
        "}catch(error){"
            "message.textContent='Nie można pobrać aktualnego stanu PWM';"
        "}"
    "}"

    "for(let channel=0;channel<4;channel++){"
        "const slider=document.getElementById('pwmSlider'+channel);"
        "const valueText=document.getElementById('pwmValue'+channel);"

        "slider.addEventListener('input',function(){"
            "const value=slider.value;"
            "valueText.textContent=value+'%';"

            "clearTimeout(pwmTimers[channel]);"

            "pwmTimers[channel]=setTimeout(function(){"
                "setPwm(channel,value);"
            "},100);"
        "});"
    "}"

    "async function setPwm(channel,value){"
        "try{"
            "const response=await fetch("
                "'/api/pwm?channel='+channel+'&value='+value,"
                "{method:'POST'}"
            ");"

            "if(!response.ok){"
                "throw new Error('HTTP '+response.status);"
            "}"

            "message.textContent="
                "'PWM'+(channel+1)+' ustawiono na '+value+'%';"
        "}catch(error){"
            "message.textContent="
                "'Błąd ustawiania PWM'+(channel+1);"
        "}"
    "}"

    "async function restartDevice(){"
        "const button=document.getElementById('restartButton');"

        "button.disabled=true;"
        "message.textContent='Restartowanie urządzenia...';"

        "try{"
            "await fetch('/api/restart',{method:'POST'});"
            "message.textContent='ESP32 uruchamia się ponownie.';"
        "}catch(error){"
            "message.textContent='Połączenie przerwane — trwa restart.';"
        "}"

        "setTimeout(function(){"
            "window.location.reload();"
        "},6000);"
    "}"

    "window.addEventListener('load',loadStatus);"
        "loadStatus();"
        "setInterval(loadStatus,5000);"
    "</script>"

    "</body>"
    "</html>";


static esp_err_t index_handler(httpd_req_t *request)
{
    httpd_resp_set_type(
        request,
        "text/html; charset=utf-8"
    );

    return httpd_resp_send(
        request,
        index_html,
        HTTPD_RESP_USE_STRLEN
    );
}


esp_err_t web_pages_register(httpd_handle_t server)
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

    return httpd_register_uri_handler(
        server,
        &index_uri
    );
}