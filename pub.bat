@echo off

clear

SET script=%1
SET local=%2

if "%script%" == "" (
  echo Example use
  echo ./pub.bat scriptFile.c
  exit
)
if "%script%" == "space-game.c" (
  if "%local%" == "local" (
    SET apiKey=39s0s9x601h61gp454hb9ebhtv1jamtmg1gup8bcl9jqp83art
  ) else (
    SET apiKey=0
  )
)

if "%apiKey%" == "" (
  echo apiKey not assigned for %script%
  exit
)

if "%local%" == "local" (
  SET server=https://localhost:7086/api/Wasm/Upload/
  ) else (
  SET server=https://www.d1ag0n.com/api/Wasm/Upload/
)

REM echo Building...
REM -fno-inline -nostdlib -nostdlibinc -nostdinc -nostdinc++  -fno-builtin
clang %script% -fno-inline -I header --target=wasm32-unknown-unknown -Wl,-z,stack-size=65536 ^
--optimize=3 -nostdlib -nostdlibinc -nostdinc -nostdinc++  -fno-builtin ^
-Wl,--no-entry -Wl,--export-all -Wl,--error-limit=0 ^
-Wl,--allow-undefined --wasm-opt --output %script%.wasm

if %ERRORLEVEL% == 0 (
  REM echo Uploading...
  if "%local%" == "local" (
    curl -k --fail -F "data=@./%script%.wasm" "%server%%apiKey%%"
    ) else (
    curl --fail -F "data=@./%script%.wasm" "%server%%apiKey%%"
  )
  ) else (
  echo Build failed
  exit
)

if %ERRORLEVEL% == 0 (
  echo.
  echo %script%.wasm uploaded to %server%
  rm %script%.wasm
) else (
  echo %script%.wasm upload failed
)
