@assoc .lv2=
@ftype executable_process2=

@set CURRENT_PATH=%~dp0

@ftype executable_process2="%CURRENT_PATH%compile.bat" "%%1"
@assoc .lv2=executable_process2