# Tools

Ten katalog zawiera narzędzia, biblioteki pomocnicze, moduły systemowe i komponenty wspólne wykorzystywane przez projekt STM32F401CCU6 Development Sandbox.

Nie jest to samodzielny projekt produkcyjny, lecz zbiór elementów wspierających kompilację, bootloader, FreeRTOS, sterowniki peryferiów i skrypty pomocnicze dla mikrokontrolera STM32F401CCU6.

## Cel katalogu

Katalog `tools/` integruje:

- własne sterowniki i biblioteki urządzeń,
- moduły RTOS,
- kernel FreeRTOS,
- bootloader,
- skrypty budowania i wsparcia,
- dokumentację techniczną i narzędzia pomocnicze,
- gotowe szablony i struktury projektowe.

## Struktura katalogów

```text
tools/
├── README.md                     # dokumentacja katalogu tools
├── init_freertos.sh             # inicjalizacja/konfiguracja FreeRTOS
├── STM32F103C8TX_FLASH_APP.ld    # linker dla targetu STM32F103
├── STM32F103C8TX_FLASH_SBL.ld    # linker bootloadera dla STM32F103
├── STM32F401CCU6_Hello_World.ioc # projekt CubeMX / konfiguracja peryferiów
├── STM32F401CCUX_FLASH_APP.ld    # linker dla aplikacji STM32F401
├── STM32F401CCUX_FLASH_SBL.ld    # linker bootloadera STM32F401
├── TEMPLATE/                    # szablon nowego modułu/projektu
├── Reuse/                       # wspólne sterowniki i biblioteki peryferiów
├── RTOS_MODULES/                # moduły i zadania FreeRTOS
├── bootloader/                  # kod bootloadera
├── cpu_utils/                   # funkcje pomocnicze CPU / diagnostyka
├── docs/                        # dokumentacja, datasheety, notatki techniczne
├── dump/                        # pliki dump/artefakty pomocnicze
├── freertos/                    # źródła FreeRTOS i konfiguracja portu
├── makefiles/                   # fragmenty Makefile dla budowania
├── support/                     # skrypty pomocnicze, SVD, programowanie flash
├── tinyusb/                     # biblioteka TinyUSB (jeśli zaimportowana)
└── ...
```

## Najważniejsze podkatalogi

### Reuse

Katalog `Reuse/` zawiera biblioteki i moduły wspólne do komunikacji i obsługi urządzeń, np.:

- radiowe moduły: `CC1101`, `LoRa`, `nRF24L01`, `SI4432`, `nRF905`,
- wyświetlacze: `SH1106`, `LCD12864`, `ST7565R`,
- pamięci: `WS25Qxx`,
- czujniki: `DS18B20`, `SHT40`, `SCD41`,
- warstwy komunikacji: `UART`, `SPI`, `I2C`, `1-Wire`,
- narzędzia wspomagające: `delay`, `beep`, `log`, `CRC`, `circular buffer`, `hw_monitor`.

To tutaj znajdują się gotowe elementy wielokrotnego użytku dla aplikacji na STM32.

### RTOS_MODULES

Katalog `RTOS_MODULES/` zawiera moduły zadań i komponentów zintegrowanych z FreeRTOS, takie jak:

- zadania monitorujące stan systemu,
- zadania komunikacji z peryferiami,
- hooki systemowe,
- statystyki czasu wykonania,
- konfigurację `FreeRTOSConfig.h`.

### freertos

Katalog `freertos/` zawiera źródła jądra FreeRTOS wraz z portem dla ARM Cortex-M4F (`ARM_CM4F`). Jest to główna biblioteka RTOS wykorzystywana w projekcie.

### bootloader

Katalog `bootloader/` zawiera implementację bootloadera dla platformy STM32. W projekcie bootloader jest aktywny jako osobny obraz z oddzielnym linkowaniem i pamięcią flash.

### makefiles

`makefiles/` zawiera fragmenty systemu budowania, m.in.:

- reguły kompilacji,
- konfigurację flag kompilatora,
- definicje celu i zależności,
- parametry dla aplikacji i bootloadera.

### support

Katalog `support/` jest przeznaczony dla narzędzi pomocniczych, m.in.:

- skryptów programowania przez ST-Link/OpenOCD,
- plików SVD,
- narzędzi odczytu ELF,
- helperów do aktualizacji obrazów i diagnostyki sprzętu.

### docs

`docs/` zawiera dokumentację projektową, opisy układów, schematy logiczne, notatki dla modułów, materiały referencyjne i cytowane datasheety.

### tinyusb

Katalog `tinyusb/` zawiera bibliotekę TinyUSB, która może być wykorzystywana do komunikacji USB w projekcie lub jako moduł rozszerzający funkcjonalność mikrokontrolera.

## Typowe użycie

W praktyce katalog `tools/` jest używany jako baza narzędzi i modułów wspólnych dla projektu głównego:

1. kompilacja i linkowanie aplikacji oraz bootloadera,
2. integracja FreeRTOS z zadaniami aplikacyjnymi,
3. podłączenie sterowników urządzeń,
4. konfiguracja peryferiów i debugowania,
5. przygotowanie obrazów i programowanie mikrokontrolera.
---

Ten katalog jest fundamentem rozwoju firmware dla STM32F401CCU6 i powinien być traktowany jako miejsce wspólnych narzędzi, bibliotek i zasobów dla całego repozytorium.
