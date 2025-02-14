#!/usr/bin/python3
import sys, array, os, struct

def crc32_stm32(data, init_crc=0xFFFFFFFF, polynomial=0x04C11DB7):

    crc = init_crc

    for num in data:
        for i in range(32):  # Przejdź przez każdy bit
            if (crc ^ (num & 0x80000000)) & 0x80000000:
                crc = (crc << 1) ^ polynomial
            else:
                crc <<= 1
            crc &= 0xFFFFFFFF  # Zachowaj 32 bity
            num <<= 1

    return crc

filename = sys.argv[1]
if len(sys.argv) > 2 and sys.argv[2].strip():  # Czy istnieje i nie jest pusty?
    datalen = sys.argv[2]

with open(filename, "rb") as file:
    # Utwórz tablicę, określając typ danych ('i' dla integer, 'f' dla float)
    buf = array.array('I')
        # Oblicz liczbę elementów na podstawie rozmiaru pliku
    file_size = os.path.getsize(filename)
    element_size = buf.itemsize  # Rozmiar pojedynczego elementu w bajtach
    num_elements = file_size // element_size
    if len(sys.argv) > 2 and sys.argv[2].strip():
        buflen = int(datalen)
    else:
        buflen = num_elements

    # Wczytaj dane
    buf.fromfile(file, buflen)

# for i in range(0, len(buf), 16):  # 16 bajtów na wiersz
#     line = buf[i:i+16]
#     print(' '.join(f'{byte:08x}' for byte in line))

# Obliczanie CRC32 zgodnie z domyślną konfiguracją STM32
crc_stm32 = crc32_stm32(buf)
print(f"CRC32 (STM32): {crc_stm32:#010x}")

file.close()
