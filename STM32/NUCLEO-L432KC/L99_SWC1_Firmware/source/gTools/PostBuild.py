import os
import struct
import subprocess
import shutil
from intelhex import IntelHex

# ANSI escape codes for colors
BLUE = "\033[94m"
GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

# Path to the OpenSSL binary
openssl_exe = "../gTools/openssl/openssl.exe"

# Define the AES key (64 bytes hexadecimal)
AES_KEY = "15777fcd56a1c33b224fde44adc5699dc321d4c5d6d4c8d9b446789A00DEF011"
# Define the IV key (32 bytes hexadecimal)
AES_IV = "845646daddaadac482c0c44abcfffcba"
# Define the GBOOT key
HASH_KEY = "A9780012981DB012AC1AC411CC7BC354DF903C3A454A3C2AA41F631AA8B14699"
# Start address (constant value as defined)
START_ADDRESS = 0x8020000

def read_version_info(version_file_path):
    version_pattern = "@fw_version:"
    try:
        with open(version_file_path, 'r') as file:
            for line in file:
                if version_pattern in line:
                    return line.split(version_pattern)[-1].strip()  # Return version string
    except FileNotFoundError:
        print(RED + f"File '{version_file_path}' not found." + RESET)
    except Exception as e:
        print(RED + f"Error reading version info: {e}" + RESET)
    return None

def generate_hmac_sha256(input_file_path, openssl_bin):
    try:
        with open(input_file_path, 'rb') as file:
            file.seek(0x380)
            input_data = file.read()
    except FileNotFoundError:
        raise Exception(f"File {input_file_path} not found.")
    except IOError as e:
        raise Exception(f"Error reading file: {e}")

    openssl_hmac_cmd = [
        openssl_bin, 'dgst', '-sha256', '-mac', 'hmac', '-macopt', 'hexkey:' + HASH_KEY, '-binary'
    ]

    try:
        hmac_hash_output = subprocess.Popen(openssl_hmac_cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        hmac_hash_bytes, _ = hmac_hash_output.communicate(input_data)

        if hmac_hash_output.returncode != 0:
            raise Exception("OpenSSL command failed.")
    except Exception as e:
        raise Exception(f"Error running OpenSSL command: {e}")

    return hmac_hash_bytes

def write_hash_and_length_to_address(hash_bytes, length, output_file_path):
    length_position = 0x300 + 12
    hash_position = 0x300 + 16
    with open(output_file_path, 'r+b') as output_file:
        output_file.seek(hash_position)
        output_file.write(hash_bytes)
        output_file.seek(length_position)
        output_file.write(struct.pack('<I', length - 0x380))

def encrypt_aes_ctr_256(input_file_path, output_file_path, openssl_bin, AES_KEY, AES_IV):
    openssl_aes_cmd = [
        openssl_bin, 'enc', '-aes-256-ctr', '-e', '-in', input_file_path, '-out', output_file_path, '-K', AES_KEY, '-iv', AES_IV
    ]
    subprocess.run(openssl_aes_cmd)
    print(GREEN + f"File encrypted." + RESET)
    print(BLUE + "-------------------------------------------------------------------------------------" + RESET)

def pad_file_with_ff(input_file_path):
    bytes_padded = 0
    with open(input_file_path, 'ab+') as f:
        initial_file_length = f.tell()
        if initial_file_length % 1024 != 0:
            remaining_bytes = 1024 - (initial_file_length % 1024)
            f.write(b'\xff' * remaining_bytes)
            bytes_padded = remaining_bytes
    return bytes_padded

def convert_to_intel_hex(file_path, start_address):
    try:
        ih = IntelHex()
        with open(file_path, 'rb') as f:
            binary_data = f.read()
        ih.frombytes(binary_data)

        offset = start_address - ih.minaddr()
        for addr in ih.addresses():
            ih[addr + offset] = ih[addr]
            del ih[addr]

        output_file_path = os.path.splitext(file_path)[0] + '.hex'
        ih.tofile(output_file_path, format='hex')
        return output_file_path  # Return the path of the HEX file
    except FileNotFoundError:
        print(RED + f"File '{file_path}' not found. Skipping." + RESET)
        return None
    except Exception as e:
        print(f"Error converting {file_path}: {str(e)}")
        return None

def copy_generated_files(output_gbin_file_path, output_hex_file_path, version_string):
    destination_folder = "../../release/"
    
    if not os.path.exists(destination_folder):
        os.makedirs(destination_folder)
    else:
        # Delete all files in the destination folder
        for filename in os.listdir(destination_folder):
            file_path = os.path.join(destination_folder, filename)
            try:
                if os.path.isfile(file_path):
                    os.unlink(file_path)  # Remove the file
            except Exception as e:
                print(RED + f"Error deleting file {filename}: {e}" + RESET)

    try:
        # Rename and copy the .gBin file
        gbin_file_name = f"{os.path.splitext(os.path.basename(output_gbin_file_path))[0]}_{version_string}.gbin"
        gbin_destination = os.path.join(destination_folder, gbin_file_name)
        shutil.move(output_gbin_file_path, gbin_destination)
        print(f"Moved " + BLUE + f"{gbin_file_name}" + RESET + f" to " + BLUE + f"{destination_folder}" + RESET)

        # Rename and copy the .hex file
        hex_file_name = f"{os.path.splitext(os.path.basename(output_hex_file_path))[0]}_{version_string}.hex"
        hex_destination = os.path.join(destination_folder, hex_file_name)
        shutil.move(output_hex_file_path, hex_destination)
        print(f"Moved " + BLUE + f"{hex_file_name}" + RESET + f" to " + BLUE + f"{destination_folder}" + RESET)

        print(BLUE + "-------------------------------------------------------------------------------------" + RESET)

    except Exception as e:
        print(RED + f"Error copying files: {e}" + RESET)

def process_files():
    search_folders = ["../_Release"]
    version_file_path = "../gLib/gCore/gInfo.h"
    version_string = read_version_info(version_file_path)  # Get version info from gInfo.h

    print(BLUE + "-------------------------------------------------------------------------------------" + RESET)
    print(BLUE + "     Binary Encryption (AES CTR 256)       Version: 1.0.0      gtec Medical GmbH" + RESET)
    print(BLUE + "-------------------------------------------------------------------------------------" + RESET)
    
    for search_folder in search_folders:
        try:
            folder_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), search_folder)
            openssl_bin = os.path.abspath(os.path.join(folder_path, openssl_exe))

            for filename in os.listdir(folder_path):
                if filename.endswith('.bin'):
                    print("Processing file:", BLUE + os.path.relpath(os.path.join(folder_path, filename), "..") + RESET)

                    input_file_path = os.path.join(folder_path, filename)
                    output_tmp_file_path = os.path.join(folder_path, os.path.splitext(filename)[0] + '.tmp')
                    output_gbin_file_path = os.path.join(folder_path, os.path.splitext(filename)[0] + '.gbin')

                    shutil.copyfile(input_file_path, output_tmp_file_path)
                    hmac_hash = generate_hmac_sha256(output_tmp_file_path, openssl_bin)

                    if hmac_hash:
                        file_length = os.path.getsize(output_tmp_file_path)
                        write_hash_and_length_to_address(hmac_hash, file_length, output_tmp_file_path)
                        pad_file_with_ff(output_tmp_file_path)

                        # Convert the temporary file to Intel HEX format
                        output_hex_file_path = convert_to_intel_hex(output_tmp_file_path, START_ADDRESS)

                        # Encrypt the temporary file
                        encrypt_aes_ctr_256(output_tmp_file_path, output_gbin_file_path, openssl_bin, AES_KEY, AES_IV)

                        # If both files are successfully created, copy them to the release folder
                        if output_hex_file_path:
                            copy_generated_files(output_gbin_file_path, output_hex_file_path, version_string)
        except FileNotFoundError:
            continue

if __name__ == "__main__":
    process_files()
