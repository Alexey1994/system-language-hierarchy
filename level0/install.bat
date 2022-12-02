@assoc .proc=
@assoc .lv0=
@ftype executable_process=

@set CURRENT_PATH=%~dp0

@ftype executable_process="%CURRENT_PATH%execute.bat" "%%1"
@assoc .lv0=executable_process