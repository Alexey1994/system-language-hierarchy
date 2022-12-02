@set CURRENT_PATH=%~dp0

@load %1 | "%CURRENT_PATH%compile" > "%~1.lv0" ^
	|| pause