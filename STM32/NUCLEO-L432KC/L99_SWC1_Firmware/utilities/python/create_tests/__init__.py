import argparse
import pathlib
from typing import Union

from .utils import createTestData, createMakefile, createSTPLFiles, createTPLFiles, createIncDir


def create_tests(test_list: list[str], unit_list: list[Union[str, dict]]):
    """"User-facing interface that internally calls function that set up the test folder

    Steps:
    * createTestData: Creates the 'testdata' folder and initializes it
    * createMakefile: Creates the Makefile for the test
    * createSTPLFiles: Creates the 'src' folder and populates it with a .c.stpl-file for each unit
    * createTPLFiles: Copies the necessary .tpl-files into 'src'
    * createIncDir: Creates the include directory

    All functions do not overwrite directories or files if they already exist, but do delete uncessary files from
    unspecified units or unit tests.

    Args:
         test_list: **[in] (list[str])** A list of strings containing the names of all the individual unit tests that
            should be initialized
        unit_list: **[in] (list[Union[str, dict]])** A list of strings and dicts containing the names of all the units
            that should be tested

    Returns:
        None

    Raises:
        ValueError: createTestData can raise a ValueError if not all unit tests could be initialized

    """
    print("[INFO] Starting setup")

    cwd = pathlib.Path.cwd()

    print(f"[INFO] path: {cwd}")

    unit_test_mapping = createTestData(cwd, test_list, unit_list)

    print("[INFO] Setup of test data complete")

    createMakefile(cwd, unit_list)

    print("[INFO] Created Makefile")

    print("[INFO] Setting up src folder ")

    createSTPLFiles(cwd, unit_test_mapping)

    print("[INFO] Created .stpl files")

    createTPLFiles(cwd)

    print("[INFO] Created .tpl files")

    print("[INFO] Setting up inc folder ")

    createIncDir(cwd)

    print("[INFO] Completed setup, ending script")
