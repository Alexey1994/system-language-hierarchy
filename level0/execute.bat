@set CURRENT_PATH=%~dp0
@cd "%CURRENT_PATH%"

@load %1 | execute ^
	|| pause