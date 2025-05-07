from subprocess import check_output, TimeoutExpired, CalledProcessError

##-------------------------------------------------------------
### \brief Checks the svn state for a directory and returns either
###        '####' if modifications are done or the revision
###        number if no modifiations were done.
###
### \param[in]  dir_to_check   The directory to check.
###
### \return  '####'  if modifications were done
### \return  revision number  if no modifications were done
###
def svnCheckDirectory(dir_to_check):
  try:
    svn_status = check_output(["svn", "stat", "-u", dir_to_check], universal_newlines=True, timeout = 10).split('\n')
  except CalledProcessError:
    ret_str = '####'
  except TimeoutExpired:
    ret_str = '####'
  else:
    if ('Status against revision:' in svn_status[0]):
      ret_str = svn_status[0].rsplit(':',1)[1].strip()
    else:
      ret_str = '####'
  return ret_str
