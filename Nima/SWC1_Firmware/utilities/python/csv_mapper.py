### ------------------------------------------------------------
### \file csv_mapper.py
### \brief This file contains functions for mapping csv data to C-structures
###        which are common for uC1 and uC2 unit tests.
###
### \author Lindner  lindner@gtec.at
### \author Kerbl kerbl@gtec.at
### \date 28.09.2017
### \copyright Guger Technologies OG
###
### Project: gEstim unit testing
### ------------------------------------------------------------


###-------------------------------------------------------------
### \brief   Generates an initialization string for initialization
###          of a single value with data from a csv file.
###
### \param[in]   data   An orderedDict object containing the data from
###                     the csv file.
###
### \param[in]  var_name  The name of the variable (name in csv file)
###
### \return   init_string   The initialization string which is generated
###                         out of the csv data, without the "description" column.
###
def getSingleValueInitialization(data, var_name):
  init_string = data[var_name]
  return init_string


###-------------------------------------------------------------
### \brief   Generates an initialization string for initialization
###          of C-structure with data from a csv file.
###
### \param[in]   data   An orderedDict object containing the data from
###                     the csv file.
###
### \return   init_string   The initialization string which is generated
###                         out of the csv data, without the "description" column.
###
def getStdStructInitialization(data):
  init_string = '{' + ','.join([str(v) for k,v in data.items() if 'description' not in k]) + '}'
  return init_string
