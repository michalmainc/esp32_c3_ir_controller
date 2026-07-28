# ESP32-C3 Controller

Projekt sterownika bazującego na płytce **Seeed Studio XIAO ESP32-C3**.

Firmware jest tworzony w języku **C** z wykorzystaniem frameworka **ESP-IDF** oraz środowiska **PlatformIO**.

## Hardware

[Schemat elektryczny XIAO ESP32-C3 v1.3](https://files.seeedstudio.com/wiki/XIAO_WiFi/Resources/XIAO_ESP32C3_v1.3_SCH_260116.pdf)

[Link do mapy Google z domyślną lokalizacją](https://www.google.com/maps/place/Ignacego+Krasickiego+3,+64-731+Drawski+M%C5%82yn/@52.8597701,16.0823327,19.66z/data=!4m6!3m5!1s0x4706b1ab5519d49f:0x373f7f458c295c7f!8m2!3d52.8596415!4d16.0816679!16s%2Fg%2F11qrvnc_xr?entry=ttu&g_ep=EgoyMDI2MDcyMi4wIKXMDSoASAFQAw%3D%3D)

Platforma:

* MCU: ESP32-C3 (RISC-V, 160 MHz)
* Pamięć Flash: 4 MB
* Framework: ESP-IDF
* Build system: PlatformIO

Docelowe funkcje sprzętowe:

| Funkcja                   |  GPIO |
| ------------------------- | ----: |
| PWM kanał 1               | GPIO2 |
| PWM kanał 2               | GPIO3 |
| PWM kanał 3               | GPIO4 |
| PWM kanał 4               | GPIO5 |
| Wyjście ON/OFF            | GPIO6 |
| Magistrala 1-Wire DS18B20 | GPIO7 |

## Funkcje planowane

### PWM

* 4 niezależne kanały PWM
* częstotliwość pracy: 400 Hz
* regulacja 0-100%
* nazwy kanałów konfigurowane przez użytkownika

### Web Interface

Interfejs WWW dostępny przez sieć LAN:

* konfiguracja adresu IP
* konfiguracja maski i bramy
* konfiguracja MQTT
* ustawienia współrzędnych geograficznych
* konfiguracja nazw kanałów PWM
* odczyt temperatur DS18B20
* aktualizacja firmware OTA

## Konfiguracja sieci

Domyślne ustawienia:

IP:

```
172.16.5.5
```

Konfiguracja zapisywana jest w pamięci nieulotnej ESP32.

Zmiany konfiguracji są zapisywane automatycznie po czasie opóźnienia.

## MQTT

Wbudowany klient MQTT.

Domyślny broker:

```
192.168.1.182:1883
```

Format tematów:

```
telemetria/{SN_ESP32}/pwm0
telemetria/{SN_ESP32}/pwm1
telemetria/{SN_ESP32}/pwm2
telemetria/{SN_ESP32}/pwm3
```

Planowane dane telemetryczne:

* stan PWM
* temperatura DS18B20
* stan wyjścia ON/OFF
* czas systemowy
* status urządzenia

## Zegar astronomiczny

Urządzenie będzie posiadało zegar astronomiczny:

* obliczanie wschodu i zachodu słońca
* wykorzystanie współrzędnych GPS
* automatyczne sterowanie wyjściami PWM

Domyślne współrzędne:

```
Latitude:  52.8597009
Longitude: 16.0817148
```

Obliczenia astronomiczne wykonywane okresowo i przechowywane w pamięci operacyjnej.

## Synchronizacja czasu

Czas systemowy będzie synchronizowany przez NTP.

Serwery:

```
0.pl.pool.ntp.org
1.pl.pool.ntp.org
2.pl.pool.ntp.org
3.pl.pool.ntp.org
```

Aktualizacja czasu:

```
co 12 godzin
```

## FreeRTOS

Projekt wykorzystuje architekturę zadaniową ESP-IDF:

Planowane taski:

* `wifi_task` - obsługa WiFi
* `mqtt_task` - komunikacja MQTT
* `web_task` - serwer WWW
* `sensor_task` - obsługa DS18B20
* `astro_task` - obliczenia astronomiczne
* `ntp_task` - synchronizacja czasu
* `control_task` - sterowanie wyjściami

## Struktura projektu

```
esp32_c3_ir_controller/

├── platformio.ini
├── sdkconfig.seeed_xiao_esp32c3
├── README.md
│
├── src/
│   ├── main.c
│   ├── config/
│   │   ├── config.c
│   │   └── config.h
│   ├── drivers/
│   └── services/
│
├── include/
├── lib/
└── test/
```

## Historia zmian

### Initial version

* utworzenie projektu ESP-IDF
* konfiguracja PlatformIO
* obsługa konfiguracji NVS
* przygotowanie architektury FreeRTOS

---

Projekt jest rozwijany etapami.