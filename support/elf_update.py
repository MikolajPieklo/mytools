import argparse
import struct

import elf_reader

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

def parse_args():
    global args

    parser = argparse.ArgumentParser(
        description="Add CRC checksum and version information to an ELF or binary file",
        formatter_class=argparse.RawTextHelpFormatter
    )

    parser.add_argument(
        "source", help="source file"
    )

    args = parser.parse_args()

def patch_elf(data):
    elf = elf_reader.ELFObject.from_bytes(data)

    for info_section in elf.sections:
        if info_section.name == b'.program_info_section':
            program_info_section_offset_in_data = info_section.sh_offset
            program_info_section_size = info_section.sh_size
        if info_section.name == b'.text':
            text_section_offset_in_data = info_section.sh_offset
            text_section_size = info_section.sh_size

    selected_bytes = data[text_section_offset_in_data:text_section_offset_in_data + text_section_size]
    num_integers = len(selected_bytes) // 4
    numbers = list(struct.unpack(f'{num_integers}I', selected_bytes))

    crc = crc32_stm32(numbers)

    bytes_little = struct.pack('<I', crc)
    data[program_info_section_offset_in_data] = bytes_little[0]
    data[program_info_section_offset_in_data+1] = bytes_little[1]
    data[program_info_section_offset_in_data+2] = bytes_little[2]
    data[program_info_section_offset_in_data+3] = bytes_little[3]

    print("############################################")
    print("Section TEXT offset:" + hex(text_section_offset_in_data) +" size:"+ hex(text_section_size))
    print("Section INFO offset:" + hex(program_info_section_offset_in_data) +" size:"+ hex(program_info_section_size))
    print("Result: " + hex(crc))
    print("############################################")

if __name__ == '__main__':
    parse_args()

    with open(args.source, "rb") as f:
        data = bytearray(f.read())

    patch_elf(data)

    with open(args.source, "wb") as f:
        f.write(data)
