import re
import os
from datetime import datetime

# Path to the gInfo.h file
header_file_path = '../gLib/gCore/gInfo.h'

# Function to format the define line with the value aligned at the 39th character
def format_define(key, value):
    return f"#define {key:<29} {value}\n"  # Align to 39th character

# Function to update the gInfo header
def update_ginfo_header():
    with open(header_file_path, 'r') as file:
        lines = file.readlines()

    # Pattern to find the version defines
    version_pattern = re.compile(r'#define\s+(GINFO_FW_MAJOR|GINFO_FW_MINOR|GINFO_FW_PATCH|GINFO_FW_DEV_BUILD)\s+(\d+)')
    defines = {}

    # Read current values
    for line in lines:
        match = version_pattern.search(line)
        if match:
            defines[match.group(1)] = int(match.group(2))

    # Increase GINFO_FW_DEV_BUILD
    defines['GINFO_FW_DEV_BUILD'] += 1

    # Check if any version numbers have changed
    version_changed = False
    for key in ['GINFO_FW_MAJOR', 'GINFO_FW_MINOR', 'GINFO_FW_PATCH']:
        if defines[key] != int(re.search(r'(\d+)', next(line for line in lines if f'#define {key}' in line)).group(0)):
            version_changed = True
            break

    # Reset dev build if any major version changes
    if version_changed:
        defines['GINFO_FW_DEV_BUILD'] = 0  # Reset to 0

    # Write the updated values back to the header file
    with open(header_file_path, 'w') as file:
        for line in lines:
            for key in defines:
                if f'#define {key}' in line:
                    line = format_define(key, defines[key])
            file.write(line)
    
    return defines  # Return the defines dictionary

# Function to update all .c and .h files in the gLib directory
def update_fw_info_in_files(base_dir, defines):
    version_string = f"{defines['GINFO_FW_MAJOR']}.{defines['GINFO_FW_MINOR']}.{defines['GINFO_FW_PATCH']}-{defines['GINFO_FW_DEV_BUILD']:03d}"
    date_string = datetime.now().strftime("%d-%m-%Y")
    fw_info_block = (
        "/**\n"
        f" * @fw_version:  {version_string}\n"
        f" * @fw_date:     {date_string}\n"
        " */\n"
    )

    for root, _, files in os.walk(base_dir):
        for file in files:
            if file.endswith(('.c', '.h')):
                file_path = os.path.join(root, file)
                with open(file_path, 'r+') as f:
                    content = f.readlines()
                    f.seek(0)  # Move to the start of the file
                    in_version_block = False
                    for line in content:
                        if '/* USER CODE BEGIN fw_version */' in line:
                            f.write(line)  # Write the begin marker
                            f.write(fw_info_block)  # Write the new version info block
                            in_version_block = True  # Start capturing
                        elif '/* USER CODE END fw_version */' in line:
                            f.write(line)  # Write the end marker
                            in_version_block = False  # Exit version block
                        elif not in_version_block:
                            f.write(line)  # Write the original line
                    f.truncate()  # Remove any leftover content

if __name__ == "__main__":
    defines = update_ginfo_header()  # Capture the defines dictionary
    update_fw_info_in_files('../gLib/', defines)  # Pass the defines to update files
