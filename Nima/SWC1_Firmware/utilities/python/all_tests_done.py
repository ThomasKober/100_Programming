import re

def check_test_list():
    from test_list import test_list

    results_file = open('./test/doc/test_reports/testrunner_result.txt', "r+")
    test_results = results_file.read()
    add_text = ("###########################################################################################################################\n")
    add_text = add_text+("############################################# ATTENTION - Tests are missing! ##############################################\n")
    add_text = add_text+("###########################################################################################################################\n")
    not_all_tested = 0
    for test in test_list:
      if test not in test_results:
        add_text = add_text+("  "+test+ " is missing \n")
        not_all_tested = 1
    if (not_all_tested == 1):
      add_text = add_text+("###########################################################################################################################\n")
      add_text = add_text+("############################################# ATTENTION - Tests are missing! ##############################################\n")
      add_text = add_text+("###########################################################################################################################\n\n\n")
      results_file.seek(0, 0)
      results_file.write(add_text+test_results)
    else:
      add_text = ("###########################################################################################################################\n")
      add_text = add_text+("################################################### All Tests done! #######################################################\n")
      add_text = add_text+("###########################################################################################################################\n")
      results_file.seek(0, 0)
      results_file.write(add_text+test_results)
      results_file.close()
    results_file.close()
	
def add_fw_version():
    with open('./source/gLib/gCore/gInfo.h', "r") as global_header_file:
      header_file = global_header_file.read()
      # Define regex patterns to extract version numbers
      patterns = {
          'major': r'#define\s+GINFO_FW_MAJOR\s+(\d+)',
          'minor': r'#define\s+GINFO_FW_MINOR\s+(\d+)',
          'patch': r'#define\s+GINFO_FW_PATCH\s+(\d+)',
          'dev_build': r'#define\s+GINFO_FW_DEV_BUILD\s+(\d+)'
      }

      # Initialize variables
      version_info = {}

      # Extract version numbers using regex
      for key, pattern in patterns.items():
          match = re.search(pattern, header_file)
          if match:
              version_info[key] = int(match.group(1))
          else:
              version_info[key] = None  # Or handle the case where the pattern is not found

      # Assign extracted values to variables
      ginfo_fw_major = version_info['major']
      ginfo_fw_minor = version_info['minor']
      ginfo_fw_patch = version_info['patch']
      ginfo_fw_dev_build = version_info['dev_build']

    FW_version = f"{ginfo_fw_major}.{ginfo_fw_minor}.{ginfo_fw_patch}-{ginfo_fw_dev_build:03d}"
    results_file = open('./test/doc/test_reports/testrunner_result.txt', "r+")
    test_results = results_file.read()
    results_file.seek(0, 0)
    add_text = ("###########################################################################################################################\n")
    add_text = add_text + "uC Firmware Version:   " + FW_version + "\n";
    results_file.write(add_text+test_results)

