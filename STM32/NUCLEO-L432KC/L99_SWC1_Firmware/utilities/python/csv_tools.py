import csv
import sys
import collections
import os
import re
import glob

CSV_DELIMITER = ';'

def getFileMatchingPattern(pattern):
    matching_files = glob.glob(pattern)

    if len(matching_files) == 1:
        return matching_files[0]
    elif len(matching_files) == 0:
        raise FileNotFoundError(f"No matching files found for pattern {pattern}")
    else:
        raise ValueError(f"More than one matching files found for pattern {pattern}")



###-------------------------------------------------------------
### \brief Parses a csv file with the given file path and returns a
###        list of OrderedDict (one list entry for each line in the
###        csv file).
###
### \param[in]  file_to_open   The file which should be parsed.
###
### \return   A list of OrderedDict objects (one OrderedDict for each
###           line in the csv file).
###
def csvParseFile(test_id):
    # Define the pattern to search for files containing 'UT_FW_GAPP_001'
    pattern = f'./test\\testdata\\*{test_id}*.csv'
    file_path = getFileMatchingPattern(pattern)
    reader = csv.DictReader(open(file_path, newline=''), delimiter=CSV_DELIMITER, quotechar='"')
    dict_list = list(reader)
    return dict_list


###-------------------------------------------------------------
### \brief   Parses a csv file named the same as test_id. Two lists
###          of OrderedDict objects are returned. One containing all
###          input_data paths and one containing all reference_data
###          paths.
###
### \param[in]   test_id   The ID of the test (i.e. test_UT_FW2_RB_001)
### \param[in]   test_description   The short description of the test (i.e. EmptyAfterInit)
### \param[in]   path_to_mapping_files   The path containing the mapping csv files
###
### \return   input_paths   A list of OrderedDict objects containing all
###                         input_paths of the mapping csv file (one entry
###                         per line in csv file).
### \return   reference_paths  A list of OrderedDict objects containing all
###                            reference_paths of the mapping csv file (one
###                            entry per line in csv file).
### \return   descriptions      A list of OrderedDict objects containing all
###                            descriptions of the mapping csv file (one
###                            entry per line in csv file).
###
def csvFetchTestDataPaths(test_id, test_description, path_to_mapping_files):
    pattern = f'./test\\testdata\\*{test_id}*.csv'
    testdata_path = getFileMatchingPattern(pattern)

    return testdata_path


  #mappings = csvParseFile(path_to_mapping_files + test_id + '.csv')

  # input_paths = [collections.OrderedDict([(k,v) for k,v in d.items() if 'input_path_' in k]) for d in mappings]
  # reference_paths = [collections.OrderedDict([(k,v) for k,v in d.items() if 'reference_path_' in k]) for d in mappings]
  # descriptions = [collections.OrderedDict([(k,v) for k,v in d.items() if 'description' in k]) for d in mappings]

  # return input_paths, reference_paths, descriptions


###-------------------------------------------------------------
### \brief   Parses a mapping csv file and generates a list of
###          test names out of test_id, test_description and an index.
###          (i.e. test_UT_FW2_RB_001_EmptyAfterInit_0)
###
### \param[in]   test_id   The ID of the test (i.e. test_UT_FW2_RB_001)
### \param[in]   test_description   The short description of the test (i.e. EmptyAfterInit)
### \param[in]   path_to_mapping_files   The path containing the mapping csv files
###
### \return   A list of test names that could be used as function names for example.
###
def csvGenerateTestNames(test_id, test_description, path_to_mapping_files):
  mappings = csvParseFile(test_id)

  test_names = [test_id + '_' + test_description + '_' + str(cnt) for cnt,mapping in enumerate(mappings)]
  return test_names


###-------------------------------------------------------------
### \brief   Uses csvGenerateTestNames and csvFetchTestDataPaths and
###          to one function and returns the combined output of these
###          funcitons.
###
### \param[in]   test_id   The ID of the test (i.e. test_UT_FW2_RB_001)
### \param[in]   test_description   The short description of the test (i.e. EmptyAfterInit)
### \param[in]   path_to_mapping_files   The path containing the mapping csv files
###
### \return   test_list   A list of test names that could be used as function names for example.
### \return   input_paths   A list of OrderedDict objects containing all
###                         input_paths of the mapping csv file (one entry
###                         per line in csv file).
### \return   reference_paths  A list of OrderedDict objects containing all
###                            reference_paths of the mapping csv file (one
###                            entry per line in csv file).
### \return   descriptions      A list of OrderedDict objects containing all
###                            descriptions of the mapping csv file (one
###                            entry per line in csv file).
###
def csvGetTestDataPathsAndNames(test_id, test_description, path_to_mapping_files):
  test_list = csvGenerateTestNames(test_id, test_description, path_to_mapping_files)
  input_paths = csvFetchTestDataPaths(test_id, test_description, path_to_mapping_files)
  return test_list, input_paths


# ###-------------------------------------------------------------
# ### \brief   Parses data with the given dataset index form a given
# ###          csv file and returns a OrderedDict object containing the
# ###          read data.
# ###
# ### \param[in]   data_path   The data path of the file to be parsed.
# ### \param[in]   dataset_idx The index of the dataset to be parsed within the csv file.
# ###                          i.e. dataset_idx = 1 all data with the string '_1'
# ###                          in their column name are returned.
# ###                          If dataset_idx is None then all data without an index
# ###                          are returned.
# ### \param[in]   *args       An optional argument where the index of the line within the
# ###                          csv file can be specified.
# ###
# def csvFetchCsvData_old(data_path, dataset_idx):
#   if (dataset_idx is None):
#     dataset_idx_str = ''
#   else:
#     dataset_idx_str = '_' + str(dataset_idx)
#
#   input_data = csvParseFile(os.path.abspath(data_path))[0]
#   input_data = collections.OrderedDict([(k,v) for k,v in input_data.items() if dataset_idx_str in k])
#
#   return input_data


###-------------------------------------------------------------
### \brief   Parses data with the given dataset index form a given
###          csv file and returns a list of OrderedDict objects containing
###          the read data.
###
### \param[in]   data_path   The data path of the file to be parsed.
### \param[in]   dataset_idx The index of the dataset to be parsed within the csv file.
###                          i.e. dataset_idx = 1 all data with the string '_1'
###                          in their column name are returned.
###                          If dataset_idx is None then all data without an index
###                          are returned.
###
def csvFetchCsvData(test_id, dataset_idx=None):
  if (dataset_idx is None):
    dataset_idx_str = ''
  else:
    dataset_idx_str = '_' + str(dataset_idx)

  input_data = csvParseFile(test_id)
  input_data = [collections.OrderedDict([(k,v) for k,v in d.items() if dataset_idx_str in k]) for d in input_data]

  return input_data


###-------------------------------------------------------------
### \brief   Try if given value could be converted into int.
###
def csvTryInt(s):
    try:
        return int(s)
    except:
        return s


### ------------------------------------------------------------
### \brief   Converts a given string into a list of string and
###          number chunks. i.e. "z23a" -> ["z", 23, "a"]
###
def csvAlphanumKey(s):
    return [ csvTryInt(c) for c in re.split('([0-9]+)', s) ]


### ------------------------------------------------------------
### \brief   Sort the given list in the way that humans expect.
def csvSortNicely(l):
    l.sort(key=csvAlphanumKey)
