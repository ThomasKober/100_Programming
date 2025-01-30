@echo off
:: This ensures that Python and pip are in the system PATH.
:: If they are not, you'll need to provide the full path to python or pip.

echo Installing package...
pip install IntelHex
pip install shutil
pip install secrets
pip install struct

echo Package installed.
pause
