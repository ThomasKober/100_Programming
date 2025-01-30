###########################################################################################
#   testrun.py   V 1.0.1
#
#   Used to call testrunner.py
#
#   History:
#
#    Version  |    Date    |  Notes
#  ---------------------------------------------------------------------------
#   V0.1.0    | 27.09.2017 | Initial Draft
#   V1.0.0    | 04.06.2019 | Added check if all desired tests performed
#   V1.0.1    | 01.07.2019 | Added check of test list and add FW Version to result file
#
#
#   Thomas Kerbl, 27.09.2017                                     (C) GUGER TECHNOLOGIES OG
###########################################################################################


import sys
from testrunner import *
import all_tests_done as all_tests_done

#Main
if __name__ == "__main__":
    main()

all_tests_done.check_test_list()
all_tests_done.add_fw_version()
