@echo off
for /f "tokens=* delims= " %%a in ('wd -n %1') do set DIR=%%a
cd %DIR%
