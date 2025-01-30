% ###-----------------------------------------------------------
% ### \brief Function skeleton for a test of "test_type_1"
% ###
% ### This template provides parsing of a csv mapping file. It then
% ### calls the do_details_fnc and delivers the mapped data.
% ###
% ### Mapped data are:
% ### OrderedDict of input paths
% ### OrderedDict of reference paths
% ###
% ### This template also provides generation of the c function header
% ### and adding the function to a list of testfunctions.
% ###
%
% def test_type_1(test_id, test_description, do_details_fnc):
%   test_list, input_paths = csv_tools.csvGetTestDataPathsAndNames(test_id, test_description, path_to_mapping_files)
%
%   for test_cnt,test_function_name in enumerate(test_list, start=0):
%     tests_to_run.append(test_function_name)
%

///-------------------------------------------------------------
///
void {{test_function_name}}(void)
{
  % do_details_fnc(input_paths[test_cnt])
}
%   end
% end



% ###-----------------------------------------------------------
% ### \brief Function skeleton for a test of "test_type_2"
% ###
% ### test_type_2 does support input files with multiple lines.
% ### it generates a test for each of the lines in the first input
% ### file and passes the input and reference data to the do_details_fnc
% ### in form of a list of dictionaries. every list item is a orderedDict
% ### containing the input or reference data from one input file.
% ###
% ### \note (at the moment only the first input and reference file is
% ###       allowed to have multiple lines. in addition, if the first
% ###       input file has multiple lines, the first reference data
% ###       must contain exactly the same amount of lines.
% ###
% def test_type_2(test_id, test_description, do_details_fnc):
%   test_list, input_path = csv_tools.csvGetTestDataPathsAndNames(test_id, test_description, path_to_mapping_files)
%
%   # loop through all lines of a mapping file
%   for test_cnt,test_function_name in enumerate(test_list, start=0):
%     # generate a list of all input data from all input files of one line of a mapping file.
%     # each list element is a list of dictionaries representing the content of one input file of the actual mapping.
%     csv_input_data = [csv_tools.csvFetchCsvData(test_id)]
%

%     # loop through all input data lines of the first input data file
%     for input_cnt,input_data_0_line in enumerate(csv_input_data[0], start = 0):
%       # prepare input_data
%       # if there is more than one input file in the mapping, add this content to the input_data list
%       input_data = [input_data_0_line]
%       if len(csv_input_data) > 1:
%         input_data = input_data + [in_data_list[0] for in_data_list in csv_input_data[1:]]
%       end
%
%       # generate test name and append it to the test list
%       test_function_name_ext = test_function_name + '_' + str(input_cnt)
%       tests_to_run.append(test_function_name_ext)
%

///-------------------------------------------------------------
///
void {{test_function_name_ext}}(void)
{
  % do_details_fnc(input_data)
}
%     end
%   end
% end
