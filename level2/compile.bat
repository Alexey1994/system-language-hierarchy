@set CURRENT_PATH=%~dp0

@"%CURRENT_PATH%compile" %1 > "%~1.lv1" ^
	&& "%~1.lv1" ^
	|| pause