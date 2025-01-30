import os
from pathlib import Path
import shutil
from datetime import datetime
from typing import Union

import numpy as np
import pandas as pd
import jinja2


def createTestData(cwd: Path, test_list: list, unit_list: list[Union[str, dict]]) -> dict:
    """Creates the 'testdata' folder and populates it with .csv files. Also creates the 'unit_test_mapping' dict

    If the directory already exists, it is not replaced. Inside, all irrelevant files and folders will be deleted. If
    files for units tests and directories for units already exist, they are kept.

    In general, for each unit, a folder is created within 'testdata', named 'test_<unit name>'. For each unit test, a
    .csv-file is created in 'testdata', that contains the file paths to the input data .csv-file and reference data
    .csv-file of this test. These files are located in the sub-folder of the unit that is being tested. They are
    initiated as empty files, for the user to fill out manually.

    The dict is later used in createSTPLFiles().

    Args:
        cwd: **[in] (pathlib.Path)** The current active working directory. This is where the 'testdata' folder will be
            created
        test_list: **[in] (list[str])** A list of strings containing the names of all the individual unit tests that
            should be initialized
        unit_list: **[in] (list[Union[str, dict]])** A list of strings and dicts containing the names of all the units
            that should be tested

    Returns:
        The dict 'unit_test_mapping' that maps units to their unit tests

    Raises:
        ValueError: If not all the unit tests named in 'test_list' could be inititialized

    """

    test_data_folder = cwd.parent.parent.joinpath(Path("test")).joinpath("testdata")

    os.makedirs(test_data_folder, exist_ok=True)

    existing_files = test_data_folder.glob("*.csv")

    unit_test_mapping = {(current_unit["unit"] if isinstance(current_unit, dict) else current_unit): []
                         for current_unit in unit_list}

    for old_csv_file in existing_files:

        filename = old_csv_file.stem
        if any(test in filename for test in test_list):
            continue
        else:
            if filename.exists():
                filename.unlink()
    for current_unit in unit_list:

        if isinstance(current_unit, dict):
            current_unit = current_unit["unit"]

        for current_test in test_list:
            current_path = test_data_folder.joinpath(Path(current_test)).with_suffix(".csv")

            *test_rest, test_nr = current_test.split("_", maxsplit=4)
            test_nr = test_nr.split("_")[0]
            test_name = test_rest[-1]

            test_nr = int(test_nr)
            test_name = test_name.lower()

            # moved forward to fill up 'unit_test_mapping'
            if test_name != current_unit.lower():
                continue

            # first condition is redundant, but I kept it so its explicit
            elif test_name == current_unit.lower() and current_test not in unit_test_mapping[current_unit]:

                unit_test_mapping[current_unit].append(current_test)

            if current_path.exists():
                continue

            else:
                test_csv_contents = {"description": []}

                csv_df = pd.DataFrame(test_csv_contents)

                # write to file system
                csv_df.to_csv(current_path, index=False, sep=";")

    new_files = [element.stem for element in test_data_folder.glob("*.csv")]
    tests_completed = np.isin(test_list, new_files)

    if all(tests_completed):
        return unit_test_mapping

    else:
        tests_not_completed = test_list[np.logical_not(tests_completed)]
        raise ValueError(f"The following tests were not completed: {', '.join(tests_not_completed)}")


def createMakefile(cwd: Path, unit_list: list[str]) -> None:
    """Function that creates the Makefile from a template

    Args:
        cwd: **[in] (cwd)** The current active working directory. This is where the Makefile will be saved to
        unit_list: **[in] (list[str])** A list of strings containing the names of the units that should be tested

    Returns:
         None

    """

    relative_template_path = __getRelativePath(cwd, "makefile_template.txt")
    output_path = cwd.parent.parent.joinpath(Path("Makefile"))

    unit_names = [current_unit["unit"] if isinstance(current_unit, dict)
                  else current_unit for current_unit in unit_list]

    unit_files = [current_unit["file"] if isinstance(current_unit, dict)
                  else current_unit for current_unit in unit_list]

    template_variables = {"unit_names": unit_names,
                          "unit_files": unit_files,
                          "zip": zip}

    environment = jinja2.Environment(block_start_string='\BLOCK{',
                                     block_end_string='}',
                                     variable_start_string='\VAR{',
                                     variable_end_string='}',
                                     comment_start_string='\#{',
                                     comment_end_string='}',
                                     line_statement_prefix='%%',
                                     line_comment_prefix='%#',
                                     trim_blocks=True,
                                     autoescape=False,
                                     loader=jinja2.FileSystemLoader("."))

    template = environment.get_template(relative_template_path)

    rendered_template = template.render(template_variables)

    with open(output_path, "w") as f:
        f.write(rendered_template)


def createSTPLFiles(cwd: Path, unit_test_mapping: dict[list]) -> None:
    """Creates the 'src' folder if it doesn't already exist and populates it with .stpl files

    If the folder already exists, it is not replaced. Existing .stpl files will be checked to make sure their units
    are still being tested. If this is the case, they are left unchanged, otherwise they are deleted. For units with no
    preexisting .stpl file, a new one is created based on the 'stpl_template.txt'- template.

    Args:
        cwd: **[in] (pathlib.Path)** The current active working directory. This is where the 'src' folder will be
            created
        unit_test_mapping: **[in] (dict[list])** A dictionary of lists that matches units to the individual unit tests.
            The keys are the unit names and the values are lists of strings of the corresponding unit test names.

    Returns:
        None

    """

    source_folder = cwd.parent.parent.joinpath(Path("test")).joinpath(Path("src"))

    os.makedirs(source_folder, exist_ok=True)

    relative_template_path = __getRelativePath(cwd, "stpl_template.txt")

    for existing_stpl_file in source_folder.glob("*.stpl"):
        base_name, _ = existing_stpl_file.stem.split(".")
        _, tested_unit = base_name.split("_", maxsplit=1)

        if tested_unit not in unit_test_mapping.keys():
            existing_stpl_file.unlink()

    today = datetime.now()
    today = today.strftime("%d.%m.%Y")

    for current_unit_name, current_unit_tests in unit_test_mapping.items():

        output_path = source_folder.joinpath(Path(f"test_{current_unit_name}")).with_suffix(".c.stpl")

        if output_path.exists():
            # to avoid overwriting existing code
            continue

        template_variables = {"unit_name": current_unit_name,
                              "unit_tests": current_unit_tests,
                              "today": today,
                              "zip": zip}

        environment = jinja2.Environment(block_start_string='\BLOCK{',
                                         block_end_string='}',
                                         variable_start_string='\VAR{',
                                         variable_end_string='}',
                                         comment_start_string='\#{',
                                         comment_end_string='}',
                                         line_statement_prefix='%%',
                                         line_comment_prefix='%#',
                                         trim_blocks=True,
                                         autoescape=False,
                                         loader=jinja2.FileSystemLoader("."))

        template = environment.get_template(relative_template_path)

        rendered_template = template.render(template_variables)

        with open(output_path, "w") as f:
            f.write(rendered_template)


def createTPLFiles(cwd: Path) -> None:
    """Creates the 'src' folder if it doesn't already exist. Also copies the .tpl files into the folder

    This copy only happens if the files aren't already located in 'src'

    Args:
        cwd: **[in] (pathlib.Path)** The current active working directory. This is where the 'src' folder will be
            created

    Returns:
        None

    """

    source_folder = cwd.parent.parent.joinpath(Path("test")).joinpath(Path("src"))

    os.makedirs(source_folder, exist_ok=True)

    test_caller_template_path = __getRelativePath(cwd, "test_caller.tpl")
    test_types_template_path = __getRelativePath(cwd, "test_types.tpl")

    test_caller_output_path = source_folder.joinpath(Path("test_caller.tpl"))
    test_types_output_path = source_folder.joinpath(Path("test_types.tpl"))

    if not test_caller_output_path.exists():
        shutil.copy(test_caller_template_path, test_caller_output_path)

    if not test_types_output_path.exists():
        shutil.copy(test_types_template_path, test_types_output_path)


def createIncDir(cwd: Path) -> None:
    """Creates a directory named 'inc' if it doesn't already exist

    Args:
        cwd: **[in] (pathlib.Path)** The current active working directory. This is where the 'inc' folder will be
            created

    Returns:
        None

    """

    include_path = cwd.parent.parent.joinpath(Path("test")).joinpath(Path("inc"))

    os.makedirs(include_path, exist_ok=True)


def __getRelativePath(cwd: Path, template_file_name: str) -> str:
    """Helper function to get the relative paths of the template files

    Args:
        cwd: **[in] (pathlib.Path)** The current active directory
        template_file_name: **[in] (str)** the file name of the template

    Returns:
        A string of the relative path between cwd and the template in question

    """

    package_directory = os.path.dirname(os.path.realpath(__file__))
    package_directory = Path(package_directory)
    template_directory = package_directory.joinpath(Path("templates"))

    template_file_path = template_directory.joinpath(template_file_name)

    relative_template_path = template_file_path.relative_to(cwd.absolute())

    parsed_template_path = str(relative_template_path).replace("\\", "/")

    return parsed_template_path
