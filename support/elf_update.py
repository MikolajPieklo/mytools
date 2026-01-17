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
        "bin_data", help="binary file used to calculate crc32"
    )
    parser.add_argument(
        "elf_data", help="elf file to update"
    )

    args = parser.parse_args()
def check_magic_string(data):
    magic = data[0:4]
    if magic != b'\xAB\xEF\xCD\xAB':
        raise ValueError("Invalid magic string in header_app_section")

def detect_header_version(version):
    if version == 1:
        offset = 5
    elif version == 2:
        offset = 0
    else:
        raise ValueError(f"Unsupported header version: {version}")
    print(f"Header version: {version}")

    return offset

def patch_elf(data, crc, all_size):
    elf = elf_reader.ELFObject.from_bytes(data)
    temp_section_size = 0
    for info_section in elf.sections:
        if info_section.name == b'.header_app_section':
            header_app_section_offset_in_data = info_section.sh_offset
            header_app_section_size = info_section.sh_size

    bytes_little = struct.pack('<I', crc)
    size_bytes = struct.pack('<I',4 * all_size)
    check_magic_string(data[header_app_section_offset_in_data:header_app_section_offset_in_data + 4])
    version_offset = detect_header_version(data[header_app_section_offset_in_data + 4])

    #magic
        # data[header_app_section_offset_in_data + 0]
        # data[header_app_section_offset_in_data + 1]
        # data[header_app_section_offset_in_data + 2]
        # data[header_app_section_offset_in_data + 3]
    #header_version
        # data[header_app_section_offset_in_data + 4]
    # Update CRC32 in header_app_section
    data[header_app_section_offset_in_data + version_offset + 0] = bytes_little[0]
    data[header_app_section_offset_in_data + version_offset + 1] = bytes_little[1]
    data[header_app_section_offset_in_data + version_offset + 2] = bytes_little[2]
    data[header_app_section_offset_in_data + version_offset + 3] = bytes_little[3]
    #app size
    data[header_app_section_offset_in_data + version_offset + 4] = size_bytes[0]
    data[header_app_section_offset_in_data + version_offset + 5] = size_bytes[1]
    data[header_app_section_offset_in_data + version_offset + 6] = size_bytes[2]
    data[header_app_section_offset_in_data + version_offset + 7] = size_bytes[3]

    print("############################################")
    print("Section INFO offset:" + hex(header_app_section_size) +" size:"+ hex(header_app_section_size))
    print("Program CRC32: " + hex(crc))
    print("Program size:  " + hex(all_size))
    print("############################################")

if __name__ == '__main__':
    parse_args()

    bin_data = args.bin_data
    elf_data = args.elf_data

    with open(bin_data, "rb") as f:
        bin_bytes = f.read()
    num_integers = len(bin_bytes) // 4
    numbers = list(struct.unpack(f'{num_integers}I', bin_bytes))
    crc = crc32_stm32(numbers)
    print(f"CRC32 of {bin_data}: {hex(crc)}")

    with open(args.elf_data, "rb") as f:
        data = bytearray(f.read())

    patch_elf(data, crc, num_integers)

    with open(args.elf_data, "wb") as f:
        f.write(data)
