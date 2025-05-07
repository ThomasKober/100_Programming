###########################################################################################
#   testrunner.py   V 1.0.0
#
#   Used to call tests for host
#   This can be defined via command line options
#
#   History:
#
#    Version  |    Date    |  Notes
#  ---------------------------------------------------------------------------
#   V1.0.0    | 24.03.2017 | Initial Draft
#
#
#   AKN, 03.09.2024                                         (C) g.tec
###########################################################################################

import os  # used to search files in path;
import getpass  # used to get user name
import datetime  # used to log time and date in file
import shutil
import subprocess  # used to call programs
import argparse  # used to define command line options
import re  # used to parse output strings (rexular expression module)
import bottle  # used to generate c files from stpl template_extension
import codecs

import sys

sys.path.append("./utilities/python")
import svn_tools

##########################################################################################
#   Config Section
#   ==============
#
#
##########################################################################################
root_path = (os.getcwd())
print("Root_Path: ", root_path)

test_source_path = os.path.abspath("./test/src/")
test_testdata_path = os.path.abspath("./test/testdata/")
test_template_extension = ".stpl"

host_path_test_files = os.path.abspath("./dist/")
host_extension_type = ".exe"
host_path_test_obj = os.path.abspath("./build/")
host_gcov = os.path.abspath("C:/MinGW/bin/gcov.exe")

testrunner_log_file = "testrunner_log.txt"
testrunner_result_file = os.path.abspath("./test/doc/test_reports/testrunner_result.txt")
coverage_report_path = os.path.abspath("./test/doc/test_reports/coverage")
html_report_file = os.path.abspath("./test/doc/test_reports/coverage_report.html")


##########################################################################################
#   Program Section
#   ===============
#
#   Please don't change anything here
#
#
##########################################################################################

# -----------------------------------------------------------------------------------------
#   RunnerMessage
#   =============
#
#   param:  text : Message which should be shown on console and logged into textfile
#
#   return: none
#
#   Prints message on the console and stores it into a log file
# -----------------------------------------------------------------------------------------
def RunnerMessage(text):
    # write on console
    print(text)
    with open(testrunner_log_file, "a") as file:
        file.write(text + "\n")


# -----------------------------------------------------------------------------------------
#   RunnerResult
#   =============
#
#   param:  text : Message which should be written in result text file
#
#   return: none
#
#   Writes text into result textfile
# -----------------------------------------------------------------------------------------
def RunnerResult(text):
    with open(testrunner_result_file, "a") as file:
        file.write(text)


# -----------------------------------------------------------------------------------------
#   parseUnityOutput
#   ================
#
#   param:  text : Unity output text
#
#   return: parsedOutput : string included fail and pass info
#
#   Parses parameter text for unity fail and pass info and brings it into a table format
#   e.g.:   "./src/testfile.c:17 |PASS|                     " or
#           "./src/testfile.c:17 |FAIL| Expected 100 was 126"
# -----------------------------------------------------------------------------------------
def parseUnityOutput(text):
    parsedOutput = ""

    pass_string = ":PASS"  # Match string for pass information
    fail_string = ":FAIL:"  # Match string for fail information

    prog = re.compile(r"%s|%s" % (pass_string, fail_string))  # regular expression to find pass/fail info

    for line in text.split("\n"):
        match_obj = prog.search(line)  # find pass and fail info in line
        if match_obj != None:  # if fail and pass is found

            match_string_pre = line[0:match_obj.start()]
            match_string_post = line[match_obj.end():]

            if match_obj.group(0) == pass_string:
                parsedOutput += "%-120s|%4s|%4s|%s\n" % (match_string_pre, "PASS", "", match_string_post)
            elif match_obj.group(0) == fail_string:
                parsedOutput += "%-120s|%4s|%4s|%s\n" % (match_string_pre, "", "FAIL", match_string_post)

    return parsedOutput


# -----------------------------------------------------------------------------------------
#   getFilesList
#   ===============
#
#   param:  path        : Path to testfiles
#           extension   : Type of file extension
#
#   return: list of tuples e.g ("testfile.exe", "path/testfile.exe")
#
#   returns a list of tuples. Tuple includes the testfile with extension and a second strings
#   with where the testfile is extended with the path.
#   On host the testfile usually end with .exe and on target they end with .elf.
#
# -----------------------------------------------------------------------------------------

def getFilesList(path, extension):
    dir_file_list = []  # All files in directory
    test_file_list = []  # All files with the appropriate extension

    # Get files from path
    for (dirpath, dirnames, filenames) in os.walk(path):
        dir_file_list.extend(filenames)
        break

    # Get file list with specified extension
    for testfile in dir_file_list:
        if (testfile.endswith(extension)):
            test_file_list.append((testfile, os.path.join(path, testfile)))

    print(test_file_list)
    return test_file_list


# -----------------------------------------------------------------------------------------
#   createTestfiles
#   ===============
#
#   param:  template_path       : Path to stpl templates
#           template_extension  : Type of file extension
#
#   return: none
#
#   converts all .stpl templates in the given folder into c files
#
# -----------------------------------------------------------------------------------------
def createTestfiles(template_path, template_extension):
    template_file_list = getFilesList(template_path, template_extension)

    for template_file in template_file_list:
        fn = os.path.abspath(template_file[1])
        lookup, name = os.path.split(fn)

        st = bottle.template(name, template_lookup=[lookup], path_to_root=root_path,
                             path_to_spec_testdata=test_testdata_path)
        with codecs.open(os.path.splitext(fn)[0], mode='w', encoding="utf-8") as f: f.write(st)

    return


# -----------------------------------------------------------------------------------------
def getSvnStatus(svn_dir_path):
    svn_check_return = svn_tools.svnCheckDirectory(svn_dir_path)
    if svn_check_return == '####':
        return ('Tests ran on local working copy!')
    else:
        return (svn_check_return)


# Function to generate HTML report
def generate_html_report(data: list):
    # Start creating the HTML content
    html_content = """
    <html>
    <head>
        <title>Test Coverage Report</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            h1 { color: #2E4053; }
            table { width: 100%; border-collapse: collapse; margin-bottom: 20px; }
            th, td { padding: 10px; border: 1px solid #ddd; text-align: left; }
            th { background-color: #2E4053; color: white; }
            tr:nth-child(even) { background-color: #f2f2f2; }
            tr:nth-child(odd) { background-color: #ffffff; }
        </style>
    </head>
    <body>
        <h1>Test Coverage Report</h1>
        <table>
            <thead>
                <tr>
                    <th>C-Module</th>
                    <th>Function</th>
                    <th>Line Coverage</th>
                    <th>Branch Executed</th>
                    <th>Branch Taken</th>
                </tr>
            </thead>
            <tbody>
    """

    # Loop through each line in the result_row_list
    for line in data:
        # Split the line into columns based on the pipe character "|"
        columns = line.split("|")
        if len(columns) > 4:
            html_content += "<tr>"
            for col in columns[:-1]:
                # Strip any leading or trailing whitespace from each column and wrap it in a <td>
                html_content += f"<td>{col.strip()}</td>"
            html_content += "</tr>"

    # Close the HTML content
    html_content += """
            </tbody>
        </table>
    </body>
    </html>
    """

    # Save the HTML content to a file
    with open(html_report_file, "w") as f:
        f.write(html_content)

    print(f"HTML report generated successfully: {os.path.abspath(html_report_file)}")


# -----------------------------------------------------------------------------------------
#   gcovControl
#   =============
#
#   param:  c_file_names : list of c file names
#   return: none
#
#   Controls the gnu coverage tool and provides additional info about line and branch
#   coverage. Steps which are done:
#
#   1) Get testfiles (test_<module>.exe) and build a list of <module> names
#   2) Call gcov tool in the build directory and parse the output for the information
#   3) Build table with results in result log file - format is as follows
#
#       Module | Function | Line | Branch Exectuted | Branch Taken
#
#
#       GCOV command: gcov ./bld/<module>.c -b -f -n
#
#       -b Include branch probablities in output
#       -f output summaries for each function
#       -n Do not create an output file
#
#
#   Example Outputs on console:
#
#       Function 'calcCuboidVolumne'
#       Lines executed:100.00% of 5
#       No branches
#       Calls executed:100.00% of 2
#
#       Function 'multiply'
#       Lines executed:83.33% of 6
#       Branches executed:100.00% of 4
#       Taken at least once:75.00% of 4
#       No calls
#
#   Different regex are compiled to match the Function, Lines executed, Branches executed
#   and Taken information (as shown in the examples).
#
#
# -----------------------------------------------------------------------------------------
def gcovTestControl():
    # generate gcovr html report
    if not os.path.isdir(os.path.relpath(coverage_report_path.replace("/", "\\"))):
        os.mkdir(os.path.relpath(coverage_report_path.replace("/", "\\")))

    # Construct the exclusion path
    # unity_path = os.path.join(os.getcwd(), 'utilities', 'unity', 'src', '*').replace('\\', '/')
    test_source_path = os.path.join(os.getcwd(), 'test', 'src').replace('\\', '/')
    source_path = os.path.join(os.getcwd(), 'source').replace('\\', '/')
    # subprocess.call(
    #     f"gcovr --html --html-details -o {coverage_report_path}/coverage.html --root .  --object-directory . ")
    # subprocess.call(
    #     f"gcovr --html --html-details -o {coverage_report_path}/coverage.html --root . --exclude {unity_path} --object-directory . ")
    subprocess.call(
        f"gcovr --html --html-details -o {coverage_report_path}/coverage.html --root . --filter {test_source_path} --filter {source_path} --object-directory . ")
    result_row_list = []

    function_re = re.compile("^Function '.*'")
    function_name_re = re.compile("'.*'")
    lines_re = re.compile("^Lines executed:")
    lines_percent_re = re.compile("[1]?[0-9]?[0-9]\.[0-9][0-9]%")
    branch_no_re = re.compile("No branches")
    branch_yes_re = re.compile("^Branches executed:")
    branch_yes_percent_re = re.compile("[1]?[0-9]?[0-9]\.[0-9][0-9]%")
    branch_taken_percent_re = re.compile("[1]?[0-9]?[0-9]\.[0-9][0-9]%")

    RunnerMessage('Get Test Files ...')
    host_test_files_list = getFilesList(host_path_test_files, host_extension_type)
    RunnerMessage('Get C Files ...')
    c_file_names = list(map(lambda x: x[0].replace(host_extension_type, ""), host_test_files_list))
    c_file_names = list(map(lambda x: x.replace("test_", ""), c_file_names))

    for item in c_file_names:
        try:
            proc = subprocess.Popen(host_gcov + f" {host_path_test_obj}\\{item}.c -b -f -n",
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            terminal_output, errors = proc.communicate()
            terminal_output = terminal_output.decode("utf-8")
            if proc.returncode != 0:
                raise RuntimeError(f"gcov failed with errors: {errors.decode('utf-8')}")
        except Exception as e:
            RunnerMessage(f"Error while running gcov: {e}")
            continue

        terminal_output_lines = terminal_output.split("\n")

        for i in range(len(terminal_output_lines)):
            match_obj_dummy = function_re.search(terminal_output_lines[i])
            if match_obj_dummy:
                result_row = f'{item:20}|'
                match_obj_dummy = function_name_re.search(terminal_output_lines[i])
                if match_obj_dummy:
                    result_row += f'{terminal_output_lines[i][match_obj_dummy.start() + 1:match_obj_dummy.end() - 1]:20}|'
                i += 1
                match_obj_dummy = lines_re.search(terminal_output_lines[i])
                if match_obj_dummy:
                    match_obj_dummy = lines_percent_re.search(terminal_output_lines[i])
                    if match_obj_dummy:
                        result_row += f'{terminal_output_lines[i][match_obj_dummy.start():match_obj_dummy.end()]:^10}|'
                i += 1
                match_obj_dummy = branch_no_re.search(terminal_output_lines[i])
                if match_obj_dummy:
                    result_row += "{:20}|{:20}|".format("", "")
                match_obj_dummy = branch_yes_re.search(terminal_output_lines[i])
                if match_obj_dummy:
                    match_obj_dummy = branch_yes_percent_re.search(terminal_output_lines[i])
                    if match_obj_dummy:
                        result_row += f'{terminal_output_lines[i][match_obj_dummy.start():match_obj_dummy.end()]:^20}|'
                        i += 1
                        match_obj_dummy = branch_taken_percent_re.search(terminal_output_lines[i])
                        if match_obj_dummy:
                            result_row += f'{terminal_output_lines[i][match_obj_dummy.start():match_obj_dummy.end()]:^20}|'
                result_row += "\n"
                result_row_list.append(result_row)

    RunnerMessage('Save gcov results... ')
    generate_html_report(result_row_list)


# -----------------------------------------------------------------------------------------
#   hostTestControl
#   ===============
#
#   param:  none
#   return: none
#
#   Controls test run on host machine
#
#   Steps are as follows:
#
#   1) Invoke Makefile
#   2) Get list of testfiles from bin directory (test_<module>.exe)
#   3) Remove GCOV (.gcda) files from build directory
#   4) Call each test file, parse unity output and save in result and log file
#
# -----------------------------------------------------------------------------------------
def hostTestControl():
    RunnerMessage('Start Testrunner - Host')

    RunnerMessage('Generate Testfiles...')
    print(test_source_path, test_template_extension)
    createTestfiles(test_source_path, test_template_extension)

    RunnerMessage('Build Testfiles...')
    subprocess.call("make host ")

    RunnerMessage('Get Test Files ...')
    host_test_executable_list = getFilesList(host_path_test_files, host_extension_type)
    host_test_files_list = getFilesList(test_source_path, ".c")

    RunnerMessage('Get C Files ...')
    c_file_names = list(map(lambda x: x[0].replace(host_extension_type, ""), host_test_files_list))  # Get Test C files
    # os.makedirs(host_path_test_obj + "src", exist_ok=True)
    # for file in c_file_names:
    #    shutil.copy("./src/" + file, os.path.relpath(host_path_test_obj + "src"))
    c_file_names = list(map(lambda x: x.replace("test_", ""), c_file_names))  # Remove "test_"

    RunnerMessage('Remove old gcov files from build - test preparation... ')
    for item in c_file_names:
        if os.path.isfile(host_path_test_obj + "\\" + item + ".gcda"):
            subprocess.call("rm %s" % (host_path_test_obj + "\\" + item + ".gcda"))

    RunnerMessage('Run Tests... \n')
    parsed_output = ""
    for test_name, test_executable in host_test_executable_list:
        proc = subprocess.Popen(test_executable, stdout=subprocess.PIPE)
        terminal_output = proc.stdout.read().decode("utf-8")
        RunnerMessage(terminal_output)

        parsed_output = parsed_output + parseUnityOutput(terminal_output)

    tests_passed = parsed_output.count("PASS")
    tests_failed = parsed_output.count("FAIL")

    RunnerResult("Summary: \n")
    RunnerResult("Tests Ran: " + str(tests_passed + tests_failed) + "\n")
    RunnerResult("Tests Passed: " + str(tests_passed) + "\n")
    RunnerResult("Tests Failed: " + str(tests_failed) + "\n")
    RunnerResult("\n\n")

    RunnerResult(parsed_output)
    RunnerMessage('\n')

    RunnerMessage('End Testrunner - Host')


# -----------------------------------------------------------------------------------------
#   MAIN function in project - starting point
#
#   Arguments for testrunner are defined. If a commandline option is selected the
#   appropriate controller is invoked which provides information about the unit test.
#   In addition, The headers for result log file are written.
# -----------------------------------------------------------------------------------------
def main():
    # Define arguments for command line parser
    parser = argparse.ArgumentParser(description='Testrunner options')
    parser.add_argument('--host', action='store_true', help='run tests on host')
    parser.add_argument('--cov', action='store_true', help='run coverage test')
    parser.add_argument('--create', action='store_true', help='create testfiles')
    args = parser.parse_args()

    revision = ''
    os.makedirs('./test/doc/test_reports', exist_ok=True)

    # Write header of log file
    with open(testrunner_log_file, "w") as f:
        f.write(
            "###########################################################################################################################\n")
        f.write(" TEST RUNNER LOG FILE\n")
        f.write(
            "###########################################################################################################################\n")
        f.write("  Settings:\n\n")
        f.write("  - Run host: %s\n" % args.host)
        f.write("\n")
        f.write("\t%s                                                                                  %s\n" % (
            getpass.getuser(), datetime.datetime.now().strftime('%d-%m-%Y %H:%M:%S')))
        f.write(
            "###########################################################################################################################\n")
        f.write("\n")
        f.write("\n")
        f.write(
            "###########################################################################################################################\n")
        f.write("  Test Revision Number:\n\n")
        f.write("  -  Test Revision: %s\n" % revision)
        f.write(
            "###########################################################################################################################\n")
        f.write("\n")

    # Write header of result file
    with open(testrunner_result_file, "w") as f:
        f.write(
            "###########################################################################################################################\n")
        f.write(" TEST RUNNER RESULT FILE\n")
        f.write(
            "###########################################################################################################################\n")
        f.write("  Settings:\n\n")
        f.write("  - Run host: %s\n" % args.host)
        f.write("\n")
        f.write("\t%s                                                                                  %s\n" % (
            getpass.getuser(), datetime.datetime.now().strftime('%d-%m-%Y %H:%M:%S')))
        f.write(
            "###########################################################################################################################\n")
        f.write("\n")
        f.write("\n")
        f.write(
            "###########################################################################################################################\n")
        f.write("  Test Revision Number:\n\n")
        f.write("  -  Test Revision: %s\n" % revision)
        f.write(
            "###########################################################################################################################\n")
        f.write("\n")

    # Run tests on Host
    if args.host:
        with open(testrunner_result_file, "a") as f:
            f.write(
                "###########################################################################################################################\n")
            f.write(" HOST TESTS\n")
            f.write(
                "###########################################################################################################################\n")
        hostTestControl()

    # Run Coverage tests on host
    if args.cov:
        with open(testrunner_result_file, "a") as f:
            f.write(
                "###########################################################################################################################\n")
            f.write(" COVERAGE TEST ON HOST\n")
            f.write(
                "###########################################################################################################################\n")
        gcovTestControl()

    if args.create:
        createTestfiles(test_source_path, test_template_extension)

    # Print help if nothing should be done
    if (args.host != True) and (args.cov != True) and (args.create != True):
        parser.print_help()
    # Remove log file
    try:
        os.remove(testrunner_log_file)
    except FileNotFoundError:
        pass

# Main
if __name__ == "__main__":
    main()
