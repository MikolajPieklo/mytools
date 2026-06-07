/**
 ********************************************************************************
 * @file    cc1101_reg.h
 * @author  Mikolaj Pieklo
 * @date    08.02.2026
 * @brief
 ********************************************************************************
 */

#ifndef __CC1101_REG_H__
#define __CC1101_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/
/*
+------+-----------------------+------------------------------+
|      |         Write         |             Read             |
+------+-------------+---------+-------------+----------------+
|      | Single Byte |  Burst  | Single Byte |      Burst     |
+------+-------------+---------+-------------+----------------+
|      |    +0x00    |  +0x40  |    +0x80    |      +0xC0     |
+------+-------------+---------+-------------+----------------+
| 0x00 |                        IOCFG2                        |
+------+------------------------------------------------------+
| 0x01 |                        IOCFG1                        |
+------+------------------------------------------------------+
| 0x02 |                        IOCFG0                        |
+------+------------------------------------------------------+
| 0x03 |                        FIFOTHR                       |
+------+------------------------------------------------------+
| 0x04 |                         SYNC1                        |
+------+------------------------------------------------------+
| 0x05 |                         SYNC0                        |
+------+------------------------------------------------------+
| 0x06 |                        PKTLEN                        |
+------+------------------------------------------------------+
| 0x07 |                       PKTCTRL1                       |
+------+------------------------------------------------------+
| 0x08 |                       PKTCTRL0                       |
+------+------------------------------------------------------+
| 0x09 |                         ADDR                         |
+------+------------------------------------------------------+
| 0x0A |                        CHANNR                        |
+------+------------------------------------------------------+
| 0x0B |                        FSCTRL1                       |
+------+------------------------------------------------------+
| 0x0C |                        FSCTRL0                       |
+------+------------------------------------------------------+
| 0x0D |                         FREQ2                        |
+------+------------------------------------------------------+
| 0x0E |                         FREQ1                        |
+------+------------------------------------------------------+
| 0x0F |                         FREQ0                        |
+------+------------------------------------------------------+
| 0x10 |                        MDMCFG4                       |
+------+------------------------------------------------------+
| 0x11 |                        MDMCFG3                       |
+------+------------------------------------------------------+
| 0x12 |                        MDMCFG2                       |
+------+------------------------------------------------------+
| 0x13 |                        MDMCFG1                       |
+------+------------------------------------------------------+
| 0x14 |                        MDMCFG0                       |
+------+------------------------------------------------------+
| 0x15 |                        DEVIATN                       |
+------+------------------------------------------------------+
| 0x16 |                         MCSM2                        |
+------+------------------------------------------------------+
| 0x17 |                         MCSM1                        |
+------+------------------------------------------------------+
| 0x18 |                         MCSM0                        |
+------+------------------------------------------------------+
| 0x19 |                        FOCCFG                        |
+------+------------------------------------------------------+
| 0x1A |                         BSCFG                        |
+------+------------------------------------------------------+
| 0x1B |                       AGCCTRL2                       |
+------+------------------------------------------------------+
| 0x1C |                       AGCCTRL1                       |
+------+------------------------------------------------------+
| 0x1D |                       AGCCTRL0                       |
+------+------------------------------------------------------+
| 0x1E |                        WOREVT1                       |
+------+------------------------------------------------------+
| 0x1F |                        WOREVT0                       |
+------+------------------------------------------------------+
| 0x20 |                        WORCTRL                       |
+------+------------------------------------------------------+
| 0x21 |                        FREND1                        |
+------+------------------------------------------------------+
| 0x22 |                        FREND0                        |
+------+------------------------------------------------------+
| 0x23 |                        FSCAL3                        |
+------+------------------------------------------------------+
| 0x24 |                        FSCAL2                        |
+------+------------------------------------------------------+
| 0x25 |                        FSCAL1                        |
+------+------------------------------------------------------+
| 0x26 |                        FSCAL0                        |
+------+------------------------------------------------------+
| 0x27 |                        RCCTRL1                       |
+------+------------------------------------------------------+
| 0x28 |                        RCCTRL0                       |
+------+------------------------------------------------------+
| 0x29 |                        FSTEST                        |
+------+------------------------------------------------------+
| 0x2A |                         PTEST                        |
+------+------------------------------------------------------+
| 0x2B |                        AGCTEST                       |
+------+------------------------------------------------------+
| 0x2C |                         TEST2                        |
+------+------------------------------------------------------+
| 0x2D |                         TEST1                        |
+------+------------------------------------------------------+
| 0x2E |                         TEST0                        |
+------+------------------------------------------------------+
| 0x2F |                                                      |
+------+-------------+---------+-------------+----------------+
| 0x30 | SRES        |         | SRES        | PARTNUM        |
+------+-------------+---------+-------------+----------------+
| 0x31 | SFSTXON     |         | SFSTXON     | VERSION        |
+------+-------------+---------+-------------+----------------+
| 0x32 | SXOFF       |         | SXOFF       | FREQEST        |
+------+-------------+---------+-------------+----------------+
| 0x33 | SCAL        |         | SCAL        | LQI            |
+------+-------------+---------+-------------+----------------+
| 0x34 | SRX         |         | SRX         | RSSI           |
+------+-------------+---------+-------------+----------------+
| 0x35 | STX         |         | STX         | MARCSTATE      |
+------+-------------+---------+-------------+----------------+
| 0x36 | SIDLE       |         | SIDLE       | WORTIME1       |
+------+-------------+---------+-------------+----------------+
| 0x37 |             |         |             | WORTIME0       |
+------+-------------+---------+-------------+----------------+
| 0x38 | SWOR        |         | SWOR        | PKTSTATUS      |
+------+-------------+---------+-------------+----------------+
| 0x39 | SPWD        |         | SPWD        | VCO_VC_DAC     |
+------+-------------+---------+-------------+----------------+
| 0x3A | SFRX        |         | SFRX        | TXBYTES        |
+------+-------------+---------+-------------+----------------+
| 0x3B | SFTX        |         | SFTX        | RXBYTES        |
+------+-------------+---------+-------------+----------------+
| 0x3C | SWORRST     |         | SWORRST     | RCCTRL1_STATUS |
+------+-------------+---------+-------------+----------------+
| 0x3D | SNOP        |         | SNOP        | RCCTRL0_STATUS |
+------+-------------+---------+-------------+----------------+
| 0x3E | PATABLE     | PATABLE | PATABLE     | PATABLE        |
+------+-------------+---------+-------------+----------------+
| 0x3F | TX FIFO     | TX FIFO | RX FIFO     | RX FIFO        |
+------+-------------+---------+-------------+----------------+
 */
#define CC1101_WRITE_SINGLE_BYTE 0x00u
#define CC1101_WRITE_BURST       0x40u
#define CC1101_READ_SINGLE_BYTE  0x80u
#define CC1101_READ_BURST        0xC0u

#define CC1101_BYTES_IN_RXFIFO 0x7F
#define CC1101_CRC_OK          0x80

/*
 * ================================================================================
 * OPIS REJESTRÓW CC1101 W JĘZYKU POLSKIM
 * ================================================================================
 *
 * REJESTRY KONFIGURACYJNE (0x00-0x2E): Czasami zapisywalne na życzenie
 * ================================================================================
 *
 * IOCFG2 (0x00) - Konfiguracja pinu wyjściowego GDO2
 *   Konfiguruje funkcję i zachowanie pinu wyjścia danych GDO2 (GPIO).
 *   Wybiera sygnał, który będzie wysyłany na ten pin w zależności od stanu urządzenia.
 *
 * IOCFG1 (0x01) - Konfiguracja pinu wyjściowego GDO1
 *   Konfiguruje funkcję i zachowanie pinu wyjścia danych GDO1 (GPIO).
 *   Zazwyczaj nieużywany, ale może być skonfigurowany do wysyłania różnych sygnałów.
 *
 * IOCFG0 (0x02) - Konfiguracja pinu wyjściowego GDO0
 *   Konfiguruje funkcję i zachowanie pinu wyjścia danych GDO0 (GPIO).
 *   Powszechnie używany jako przerwanie do mikrokontrolera (np. koniec odboru).
 *
 * FIFOTHR (0x03) - Progi FIFO dla RX i TX
 *   Setting: ADC_RES[2:0][2:0] FIFO_THR[3:0]
 *   Definiuje próg, przy którym pobierana jest próbka z ADC i dostępne dane w FIFO.
 *
 * SYNC1 (0x04) - Słowo synchronizacji (bajt wysoki)
 *   Górne 8 bitów słowa synchronizacji (16-bitowe).
 *   Receiver szuka tego sekwencji aby zsynchronizować się ze strumienem danych.
 *   Typowa wartość: 0x57 (razem z SYNC0 = 0x5743).
 *
 * SYNC0 (0x05) - Słowo synchronizacji (bajt niski)
 *   Dolne 8 bitów słowa synchronizacji (16-bitowe).
 *
 * PKTLEN (0x06) - Długość pakietu
 *   Określa oczekiwaną długość pakietu danych (0-255 bajtów).
 *   W trybie stałej długości: dokładna długość pakietu.
 *   W trybie zmiennej długości: maksymalna długość pakietu.
 *
 * PKTCTRL1 (0x07) - Sterowanie automatyzacją pakietów
 *   Bit[6:4]: ADDR_CHECK[1:0] - Sprawdzanie adresu (brak, własny, broadcast, broadcast+własny)
 *   Bit[3]: FEC_EN - Włączenie korekcji błędów (Hamming code)
 *   Bit[2]: APPEND_STATUS - Dołącza status (RSSI, LQI) do każdego pakietu
 *   Bit[1:0]: CRC_AUTOFLUSH - Automatyczne wyczyszczenie FIFO przy złej CRC
 *
 * PKTCTRL0 (0x08) - Sterowanie automatyzacją pakietów
 *   Bit[6]: LENGTH_CONFIG[1:0] - Konfiguracja długości (stała, zmienna z 1. bajtem, itd.)
 *   Bit[5:4]: PKT_FORMAT - Format pakietu (normal, synchronous serial, itd.)
 *   Bit[3]: CRC_EN - Włączenie obliczania i sprawdzania CRC
 *   Bit[2]: WHITE_DATA - Włączenie scramblingu danych (whitening)
 *
 * ADDR (0x09) - Adres urządzenia
 *   Adres tego urządzenia (8-bitowy). Używany jeśli włączone sprawdzanie adresu w PKTCTRL1.
 *
 * CHANNR (0x0A) - Numer kanału
 *   Wybiera kanał frekvencji (0-255). Each channel offset = (CHANNR × Channel Spacing).
 *   Umożliwia przełączanie między kanałami bez zmiany częstotliwości podstawowej.
 *
 * FSCTRL1 (0x0B) - Sterowanie syntezatorem częstotliwości (IF)
 *   Bit[5:0]: FREQ_IF - Pożądana częstotliwość IF (Intermediate Frequency).
 *   Typowe ustawienie: 0x06-0x08 (dla IF = 152.3 kHz przy XOSC = 26 MHz).
 *
 * FSCTRL0 (0x0C) - Sterowanie syntezatorem częstotliwości (przepustka)
 *   Bit[7:0]: FREQOFF - Offset częstotliwości nośnej (±100 kHz w krokach 99.5 Hz).
 *   Korekta pasma pracującego syntezatora.
 *
 * FREQ2 (0x0D) - Słowo sterujące częstotliwością (bajt wysoki)
 *   Górne 8 bitów 24-bitowej wartości kontroli częstotliwości.
 *   FCHIP = (FREQ / 2^16) × XOSC (dla 26 MHz: ~433.92 MHz = 0x10B071).
 *
 * FREQ1 (0x0E) - Słowo sterujące częstotliwością (bajt środkowy)
 *   Środkowe 8 bitów wartości kontroli częstotliwości.
 *
 * FREQ0 (0x0F) - Słowo sterujące częstotliwością (bajt niski)
 *   Dolne 8 bity wartości kontroli częstotliwości.
 *
 * MDMCFG4 (0x10) - Konfiguracja modemu
 *   Bit[7:6]: BW_E[1:0] - Wykładnik szerokości pasma RX.
 *   Bit[5:4]: BW_M[1:0] - Mantysa szerokości pasma RX.
 *   Bit[3:0]: DRATE_E[3:0] - Wykładnik prędkości danych.
 *   Większe wartości = większa szerokość pasma = mniej czułości ale szybciej.
 *
 * MDMCFG3 (0x11) - Konfiguracja modemu (prędkość danych)
 *   Bit[7:0]: DRATE_M[7:0] - Mantysa prędkości danych (8 bitów).
 *   Razem z MDMCFG4[3:0] określa prędkość transmisji w bps.
 *
 * MDMCFG2 (0x12) - Konfiguracja modemu
 *   Bit[6:4]: MOD_FORMAT[2:0] - Format modulacji (2-FSK, GFSK, ASK, 4-FSK, MSK).
 *   Bit[3]: MANCHESTER - Kodowanie Manchester (jeśli włączone).
 *   Bit[2]: SYNC_MODE[1:0] - Tryb synchronizacji (15/16 bity słowa sync, itd.).
 *
 * MDMCFG1 (0x13) - Konfiguracja modemu
 *   Bit[7:6]: FEC_EN - Włączenie korekcji błędów forward error correction.
 *   Bit[5:4]: NUM_PREAMBLE[1:0] - Liczba bajtów preambuły (1, 2, 4, 8).
 *   Bit[3:0]: CHANSPC_E[3:0] - Wykładnik rozstawy kanałów.
 *
 * MDMCFG0 (0x14) - Konfiguracja modemu (rozstaw kanałów)
 *   Bit[7:0]: CHANSPC_M[7:0] - Mantysa rozstawy kanałów (8 bitów).
 *   Przesunięcie kanału = CHANSPC_M × 2^CHANSPC_E × XOSC / 2^18.
 *
 * DEVIATN (0x15) - Ustawienie dewiacji modemu (odchylenie częstotliwości)
 *   Bit[6:4]: DEVIATION_E[2:0] - Wykładnik dewiacji częstotliwości.
 *   Bit[2:0]: DEVIATION_M[2:0] - Mantysa dewiacji.
 *   Dewiacja = (DEVIATION_M + 8) × 2^DEVIATION_E × XOSC / 2^17.
 *
 * MCSM2 (0x16) - Główne sterowanie stanem maszyny radiowej (Wait Time)
 *   Bit[7]: RX_TIME_RSSI - Włączenie pomiaru RSSI.
 *   Bit[6:4]: RX_TIME_QUAL[2:0] - Czas oczekiwania na zmianę jakości.
 *   Bit[3:0]: RX_TIME[3:0] - Czas oczekiwania RX przed powrotem do IDLE.
 *
 * MCSM1 (0x17) - Główne sterowanie stanem maszyny radiowej (stan końcowy)
 *   Bit[5:4]: LPM_ORDER - Kolejność wejścia do PLL.
 *   Bit[3:2]: CCA_MODE[1:0] - Tryb Clear Channel Assessment (CCA).
 *   Bit[1:0]: RXOFF_MODE[1:0] - Stan wejścia po RX (IDLE, FSTXON, TX, RX).
 *   Bit[1:0]: TXOFF_MODE[1:0] - Stan wejścia po TX (IDLE, FSTXON, TX, RX).
 *
 * MCSM0 (0x18) - Główne sterowanie stanem maszyny radiowej (kalibracja)
 *   Bit[5:4]: FS_AUTOCAL[1:0] - Automatyczna kalibracja (nigdy, IDLE, RX->TX, 4x na sec).
 *   Bit[3:2]: PO_TIMEOUT - Czas dla bit timeout power down (XOSC ÷ 64 do 2 sekundy).
 *   Bit[0]: PIN_CTRL_EN - Włączenie pinu XOSC output (pin 19).
 *
 * FOCCFG (0x19) - Konfiguracja kompensacji przesunięcia częstotliwości
 *   Bit[6:5]: FOC_BS_CS_GATE - Gating dla czujnika przesunięcia bitów.
 *   Bit[4]: FOC_PRE_K[1:0] - Przedział przed kalibracja.p
 *   Bit[3:0]: FOC_K[1:0] - Współczynnik kompensacji przesunięcia (0-3, wyżej = bardziej agresywna).
 *
 * BSCFG (0x1A) - Konfiguracja synchronizacji bitów
 *   Bit[7:6]: BS_PRE_KI[1:0] - Okres wstępnego pomiaru (PRE_KI).
 *   Bit[5:4]: BS_PRE_KP[1:0] - Okres wstępnego pomiaru (PRE_KP).
 *   Bit[3:2]: BS_PH_OFFSET[3:0] - Przesunięcie fazy (0-3, 0 = brak).
 *   Bit[1:0]: BS_LIMIT[1:0] - Limit synchronizacji bitów.
 *
 * AGCTRL2 (0x1B) - Sterowanie AGC (Automatic Gain Control) - górne limity
 *   Bit[7]: MAX_DVGA_GAIN[1:0] - Maksymalny wzmacniacz DVGA (wzmacniacz cyfrowy).
 *   Bit[6:4]: MAX_LNA_GAIN[2:0] - Maksymalny wzmacniacz LNA (niskoszumny wzmacniacz).
 *   Bit[3:0]: MAGN_TARGET[3:0] - Docelowa wielkość (MAGN) dla AGC (0-7).
 *
 * AGCTRL1 (0x1C) - Sterowanie AGC (współczynniki wzmacniania)
 *   Bit[7]: AGC_LNA_PRIORITY[1:0] - Priorytet LNA w stosunku do DVGA.
 *   Bit[6:4]: CARRIER_SENSE_REL_THR[2:0] - Względny próg detekcji nośnej.
 *   Bit[3:0]: CARRIER_SENSE_ABS_THR[3:0] - Bezwzględny próg detekcji nośnej.
 *
 * AGCTRL0 (0x1D) - Sterowanie AGC (filtry)
 *   Bit[7:6]: HYST_LEVEL[1:0] - Poziom histerezy.
 *   Bit[5:4]: WAIT_TIME[1:0] - Czas oczekiwania (0-3, liczba symboli).
 *   Bit[3:0]: AGC_FREEZE[3:0] - Zamrożenie AGC (normalnie, czasami, zawsze, nigdy).
 *
 * WOREVT1 (0x1E) - Wysoki bajt limitu czasu zdarzenia 0 (Wake-On-Radio)
 *   Górna część 16-bitowego limitu czasu dla WOR.
 *   Czasowy wyzwalacz dla budowania się odbiornika (Wake-On-Radio).
 *
 * WOREVT0 (0x1F) - Niski bajt limitu czasu zdarzenia 0 (Wake-On-Radio)
 *   Dolna część 16-bitowego limitu czasu dla WOR.
 *
 * WORCTRL (0x20) - Sterowanie Wake-On-Radio
 *   Bit[7]: EVENT0_MASK - Maskowanie zdarzenia 0.
 *   Bit[6]: RC_PD - Wyłączenie oscylatora RC.
 *   Bit[5:4]: EVENT0_RES[1:0] - Rozdzielczość zegara dla Event0.
 *   Bit[3:0]: WOR_RES[3:0] - Rozdzielczość WOR (oscylator RC).
 *
 * FREND1 (0x21) - Konfiguracja Front-End RX
 *   Bit[6:4]: LNA_CURRENT[2:0] - Prąd zasilania LNA (wzmacniacz szumów nisko).
 *   Bit[3:0]: MIX_CURRENT[3:0] - Prąd zasilania MIXera.
 *   Wpływa na pobór mocy i czułość odbiornika.
 *
 * FREND0 (0x22) - Konfiguracja Front-End TX
 *   Bit[6:4]: PA_POWER[2:0] - Moc wyjściowa wzmacniacza mocy (PA) (0-7 indeks w tabeli).
 *   Bit[0]: LODIV_BW - Przepustowość dzielnika wyjścia oscylatora lokalnego.
 *   Wpływa na moc i pobór prądu w trybie transmisji.
 *
 * FSCAL3 (0x23) - Kalibracja syntezatora częstotliwości
 *   Bit[7:6]: CHP_CURR_CAL_EN - Włączenie kalibracji prądu pompy ładunku.
 *   Bit[5:0]: FSCAL3[5:0] - Parametr kalibracji (zapisywany podczas kalibracji).
 *
 * FSCAL2 (0x24) - Kalibracja syntezatora częstotliwości
 *   Bit[7:0]: FSCAL2[7:0] - Parametr kalibracji (zapisywany podczas kalibracji).
 *   Wpływa na dokładność osiąganej częstotliwości.
 *
 * FSCAL1 (0x25) - Kalibracja syntezatora częstotliwości
 *   Bit[7:0]: FSCAL1[7:0] - Parametr kalibracji (zapisywany podczas kalibracji).
 *
 * FSCAL0 (0x26) - Kalibracja syntezatora częstotliwości
 *   Bit[7:0]: FSCAL0[7:0] - Parametr kalibracji (zapisywany podczas kalibracji).
 *
 * RCCTRL1 (0x27) - Konfiguracja oscylatora RC
 *   Bit[6:0]: RCCTRL1[6:0] - Sterowanie oscylatorem RC.
 *   Dostrojenie częstotliwości oscylatora RC (dla WOR/Sleep).
 *
 * RCCTRL0 (0x28) - Konfiguracja oscylatora RC
 *   Bit[6:0]: RCCTRL0[6:0] - Sterowanie oscylatorem RC.
 *   Dostrojenie częstotliwości oscylatora RC.
 *
 * FSTEST (0x29) - Sterowanie test-em syntezatora częstotliwości
 *   Testowy rejestr do weryfikacji funkcji syntezatora.
 *   Zazwyczaj ustawiony na 0x59 (wartość domyślna).
 *
 * PTEST (0x2A) - Test produkcyjny
 *   Bit[7:0]: PTEST[7:0] - Ustawienie testowe dla produkcji.
 *   Zazwyczaj 0x7F. Zmiana na 0xBF włącza czujnik temperatury.
 *
 * AGCTEST (0x2B) - Test AGC
 *   Testowy rejestr do weryfikacji funkcji AGC.
 *   Zazwyczaj 0x3F.
 *
 * TEST2 (0x2C) - Różne ustawienia testowe
 *   Testowy rejestr zawierający różne ustawienia dla testowania.
 *   Zazwyczaj 0x81.
 *
 * TEST1 (0x2D) - Różne ustawienia testowe
 *   Testowy rejestr. W kontekście czujnika temperatury: bramka do odczytu temperatury.
 *   Zazwyczaj 0x3F. Może zawierać wartość temperatury.
 *
 * TEST0 (0x2E) - Różne ustawienia testowe
 *   Bit[3:0]: TEST0[3:0] - Ustawienia testowe.
 *   Zazwyczaj 0x0B. Zmiana na 0x09 włącza pomiary czujnika temperatury.
 *
 * ================================================================================
 * REJESTRY STATUSU (0x30-0x3D): Tylko do odczytu (oprócz komend striobe)
 * ================================================================================
 *
 * PARTNUM (0x30) - Numer części
 *   0x00 = CC1100, 0x01 = CC2500 + inne, zwykle czyta się 0x00. W CC1101 to jednak identyfikator.
 *
 * VERSION (0x31) - Numer wersji
 *   Numer wersji krzemnika. CC1101 zwraca 0x04 (wersja 4).
 *
 * FREQEST (0x32) - Estymata przesunięcia częstotliwości
 *   Szacunkowe przesunięcie częstotliwości odboru (w stosunku do oczekiwanej).
 *   Wartość ze znakiem (-128 do +127), w jednostkach Fxosc/(2^18).
 *
 * LQI (0x33) - Estymata jakości linku (Link Quality Indicator)
 *   Bit[6:0]: LQI[6:0] - Jakość linku (0 = słaba, 127 = doskonała).
 *   Bit[7]: CRC_OK - Flaga CRC (1 = pakiet przeszedł CRC).
 *   Informuje o jakości odbieranego sygnału i poprawności pakietu.
 *
 * RSSI (0x34) - Wskaźnik siły odbieranego sygnału (Received Signal Strength Indicator)
 *   Wartość bez znaku (0-255). Konwersja: P[dBm] = (RSSI - 256) / 2 - 74 lub RSSI / 2 - 74.
 *   Im wyższa wartość, tym silniejszy sygnał.
 *
 * MARCSTATE (0x35) - Stan głównego automatu stanów radiowego
 *   Bit[7]: CHIP_RDYn - Flaga gotowości krzemnika (0 = gotów).
 *   Bit[6:4]: STATE[2:0] - Aktualny stan automatu (0-7).
 *     0=IDLE, 1=RX, 2=TX, 3=FSTXON, 4=CALIBRATE, 5=SETTLING, 6=RXFIFO_OVERFLOW, 7=TXFIFO_UNDERFLOW
 *   Bit[3:0]: FIFO_BYTES[3:0] - Liczba bajtów dostępnych w RX FIFO lub wolnych w TX FIFO.
 *
 * WORTIME1 (0x36) - Wysoki bajt licznika WOR (Wake-On-Radio)
 *   Górna część 16-bitowego timera Event0 dla WOR.
 *
 * WORTIME0 (0x37) - Niski bajt licznika WOR
 *   Dolna część 16-bitowego timera Event0 dla WOR.
 *
 * PKTSTATUS (0x38) - Status pakietu i stan GDOx
 *   Bit[7]: CRC_OK - Flaga poprawności CRC pakietu.
 *   Bit[6]: LENGTH_OK - Flaga długości pakietu.
 *   Bit[5:3]: GDOx_STATUS[2:0] - Status logiczny na pinach GDO0/GDO1/GDO2.
 *
 * VCO_VC_DAC (0x39) - Aktualne ustawienie z modułu kalibracji PLL
 *   Bit[7:0]: VCO_VC_DAC[7:0] - Wartość DAC oscylatora kontrolowanego napięciem VCO.
 *   Odbicie ze stanu PLL podczas ostatniej kalibracji.
 *
 * TXBYTES (0x3A) - Niedopełnienie i liczba bajtów w TX FIFO
 *   Bit[7]: TXFIFO_UNDERFLOW - Flaga niedopełnienia TX FIFO.
 *   Bit[6:0]: NUM_TXBYTES[6:0] - Liczba bajtów w TX FIFO (0-127).
 *
 * RXBYTES (0x3B) - Przepełnienie i liczba bajtów w RX FIFO
 *   Bit[7]: RXFIFO_OVERFLOW - Flaga przepełnienia RX FIFO.
 *   Bit[6:0]: NUM_RXBYTES[6:0] - Liczba bajtów w RX FIFO (0-127).
 *   Jeśli bit[7] = 1, FIFO została przepełniona i utracono dane.
 *
 * RCCTRL1_STATUS (0x3C) - Wynik ostatniej kalibracji RC (1)
 *   Bit[6:0]: RCCTRL1_STATUS[6:0] - Parametr RC z ostatniej kalibracji.
 *
 * RCCTRL0_STATUS (0x3D) - Wynik ostatniej kalibracji RC (0)
 *   Bit[6:0]: RCCTRL0_STATUS[6:0] - Parametr RC z ostatniej kalibracji.
 *
 * ================================================================================
 * ADRESY SPECJALNE
 * ================================================================================
 *
 * PATABLE (0x3E) - Tabela mocy wzmacniacza (PA)
 *   8-bajtowy bufor konfiguracji mocy. Zapisz 8 wartości dla 8 poziomów mocy.
 *   Każdy bit = ~1 dBm wzrostu mocy. Wartości: 0x03-0xC0 (od minimalnej do maksymalnej).
 *
 * TX FIFO (0x3F) - Bufor FIFO transmisji
 *   Bufor danych do transmisji. Wpisz dane, które mają być wysłane.
 *   Maksimum 64 bajty. Komplement rozmiaru pisze się automatycznie.
 *
 * RX FIFO (0x3F) - Bufor FIFO odbioru
 *   Bufor danych odebranych. Odczytaj dane odbierane z radioodbiornika.
 *   Maksimum 64 bajty. Automatycznie dodana CRC i flagi statusu.
 *
 * ================================================================================
 * KOMENDY STRIOBE (Strobe Commands) - Sterowanie stanem maszyny radiowej
 * ================================================================================
 *
 * SRES (0x30)   - Reset Software - Resetuje układ do stanu początkowego.
 * SFSTXON (0x31) - Szybka TX ON - Przejście do trybu TX z wstępnie załadowanym FIFO.
 * SXOFF (0x32)  - Wyłączenie krystalicznego oscylatora (pobudka).
 * SCAL (0x33)   - Kalibracja - Wymusza zrekalibrowanie PLL.
 * SRX (0x34)    - Odbiór - Wejście w tryb odbioru radiowego.
 * STX (0x35)    - Transmisja - Wejście w tryb transmisji radiowej.
 * SIDLE (0x36)  - Stan bezczynności - Powrót do stanu bezczynności (IDLE).
 * SWOR (0x38)   - Wake-On-Radio - Aktywacja trybu Wake-On-Radio.
 * SPWD (0x39)   - Power Down - Przejście do trybu uśpienia (minimalny pobór).
 * SFRX (0x3A)   - Flush RX - Czyszczenie bufora RX FIFO.
 * SFTX (0x3B)   - Flush TX - Czyszczenie bufora TX FIFO.
 * SWORRST (0x3C) - Reset WOR - Reset timera Wake-On-Radio.
 * SNOP (0x3D)   - No Operation - Brak operacji (sprawdzenie statusu).
 *
 * ================================================================================
 * FORMATY MODULACJI
 * ================================================================================
 *
 * 2-FSK (0x00)   - Binary Frequency Shift Keying (domyślnie)
 * GFSK (0x10)    - Gaussian Frequency Shift Keying (bardziej wąski)
 * ASK/OOK (0x30) - Amplitude Shift Keying / On-Off Keying
 * 4-FSK (0x40)   - Quaternary FSK (przebiera 4 symbole)
 * MSK (0x70)     - Minimum Shift Keying
 *
 * ================================================================================
 */

#define CC1101_R_IOCFG2   0x00 // Konfiguracja pinu GDO2
#define CC1101_R_IOCFG1   0x01 // Konfiguracja pinu GDO1
#define CC1101_R_IOCFG0   0x02 // Konfiguracja pinu GDO0
#define CC1101_R_FIFOTHR  0x03 // Progi FIFO dla RX i TX
#define CC1101_R_SYNC1    0x04 // Słowo sync (bajt wysoki)
#define CC1101_R_SYNC0    0x05 // Słowo sync (bajt niski)
#define CC1101_R_PKTLEN   0x06 // Długość pakietu
#define CC1101_R_PKTCTRL1 0x07 // Kontrola pakietu 1
#define CC1101_R_PKTCTRL0 0x08 // Kontrola pakietu 2
#define CC1101_R_ADDR     0x09 // Adres urządzenia
#define CC1101_R_CHANNR   0x0A // Numer kanału
#define CC1101_R_FSCTRL1  0x0B // Syntezator częstotliwości (IF)
#define CC1101_R_FSCTRL0  0x0C // Syntezator (offset)
#define CC1101_R_FREQ2    0x0D // Słowo kontroli częstotliwości (wysoki)
#define CC1101_R_FREQ1    0x0E // Słowo kontroli częstotliwości (środkowy)
#define CC1101_R_FREQ0    0x0F // Słowo kontroli częstotliwości (niski)
#define CC1101_R_MDMCFG4  0x10 // Konfiguracja modemu 1
#define CC1101_R_MDMCFG3  0x11 // Konfiguracja modemu 2
#define CC1101_R_MDMCFG2  0x12 // Konfiguracja modemu 3
#define CC1101_R_MDMCFG1  0x13 // Konfiguracja modemu 4
#define CC1101_R_MDMCFG0  0x14 // Konfiguracja modemu 5
#define CC1101_R_DEVIATN  0x15 // Dewiacja modemu
#define CC1101_R_MCSM2    0x16 // Maszyna stanów 1
#define CC1101_R_MCSM1    0x17 // Maszyna stanów 2
#define CC1101_R_MCSM0    0x18 // Maszyna stanów 3
#define CC1101_R_FOCCFG   0x19 // FOC - Kompensacja przesunięcia
#define CC1101_R_BSCFG    0x1A // Synchronizacja bitów
#define CC1101_R_AGCTRL2  0x1B // AGC - Sterowanie wzmacniacza 1
#define CC1101_R_AGCTRL1  0x1C // AGC - Sterowanie wzmacniacza 2
#define CC1101_R_AGCTRL0  0x1D // AGC - Sterowanie wzmacniacza 3
#define CC1101_R_WOREVT1  0x1E // WOR Event 0 (bajt wysoki)
#define CC1101_R_WOREVT0  0x1F // WOR Event 0 (bajt niski)
#define CC1101_R_WORCTRL  0x20 // Sterowanie Wake-On-Radio
#define CC1101_R_FREND1   0x21 // Front-End RX
#define CC1101_R_FREND0   0x22 // Front-End TX
#define CC1101_R_FSCAL3   0x23 // Kalibracja syntezatora 1
#define CC1101_R_FSCAL2   0x24 // Kalibracja syntezatora 2
#define CC1101_R_FSCAL1   0x25 // Kalibracja syntezatora 3
#define CC1101_R_FSCAL0   0x26 // Kalibracja syntezatora 4
#define CC1101_R_RCCTRL1  0x27 // Konfiguracja RC oscylatora 1
#define CC1101_R_RCCTRL0  0x28 // Konfiguracja RC oscylatora 2
#define CC1101_R_FSTEST   0x29 // Test syntezatora
#define CC1101_R_PTEST    0x2A // Test produkcyjny
#define CC1101_R_AGCTEST  0x2B // Test AGC
#define CC1101_R_TEST2    0x2C // Ustawienia testowe 1
#define CC1101_R_TEST1    0x2D // Ustawienia testowe 2
#define CC1101_R_TEST0    0x2E // Ustawienia testowe 3

// Rejestry statusu (od strony 70 datasheetu)
#define CC1101_R_PARTNUM        0x30 // Numer części
#define CC1101_R_VERSION        0x31 // Numer wersji
#define CC1101_R_FREQEST        0x32 // Estymata przesunięcia
#define CC1101_R_LQI            0x33 // Link Quality Indicator
#define CC1101_R_RSSI           0x34 // Siła sygnału
#define CC1101_R_MARCSTATE      0x35 // Stan maszyny radiowej
#define CC1101_R_WORTIME1       0x36 // Timer WOR (wysoki)
#define CC1101_R_WORTIME0       0x37 // Timer WOR (niski)
#define CC1101_R_PKTSTATUS      0x38 // Status pakietu i GDOx
#define CC1101_R_VCO_VC_DAC     0x39 // VCO z kalibracji PLL
#define CC1101_R_TXBYTES        0x3A // Bajtów w TX FIFO
#define CC1101_R_RXBYTES        0x3B // Bajtów w RX FIFO
#define CC1101_R_RCCTRL1_STATUS 0x3C // Status kalibracji RC 1
#define CC1101_R_RCCTRL0_STATUS 0x3D // Status kalibracji RC 2

#define CC1101_R_PATABLE 0x3Eu // Tabela mocy wzmacniacza
#define CC1101_R_TX_FIFO 0x3Fu // Bufor TX
#define CC1101_R_RX_FIFO 0x3Fu // Bufor RX

// Komendy (Strobe commands) - sterowanie stanem
#define CC1101_CMD_SRES    0x30u // Reset Software
#define CC1101_CMD_SFSTXON 0x31u // Szybkie włączenie TX
#define CC1101_CMD_SXOFF   0x32u // Wyłączenie oscylatora
#define CC1101_CMD_SCAL    0x33u // Kalibracja
#define CC1101_CMD_SRX     0x34u // Tryb RX
#define CC1101_CMD_STX     0x35u // Tryb TX
#define CC1101_CMD_SIDLE   0x36u // Bezczynność IDLE
#define CC1101_CMD_SWOR    0x38u // Wake-On-Radio
#define CC1101_CMD_SPWD    0x39u // Power Down
#define CC1101_CMD_SFRX    0x3Au // Czyszczenie RX FIFO
#define CC1101_CMD_SFTX    0x3Bu // Czyszczenie TX FIFO
#define CC1101_CMD_SWORRST 0x3Cu // Reset WOR
#define CC1101_CMD_SNOP    0x3Du // No Operation
#define CC1101_CMD_PATABLE 0x3Eu // Tabela mocy
#define CC1101_CMD_TX_FIFO 0x3Fu // TX FIFO

// Formaty modulacji
#define CC1101_F_2FSK_MODULATION 0x00u // 2-FSK (domyślnie)
#define CC1101_F_GFSK_MODULATION 0x10u // GFSK
#define CC1101_F_ASK_MODULATION  0x30u // ASK/OOK
#define CC1101_F_4FSK_MODULATION 0x40u // 4-FSK
#define CC1101_F_MSK_MODULATION  0x70u // MSK
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/


#ifdef __cplusplus
}
#endif

#endif
