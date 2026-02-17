@echo off
setlocal enabledelayedexpansion

echo ============================================================
echo GeoChecker - Упаковка в единый EXE (Enigma Virtual Box)
echo ============================================================
echo.

REM Пути
set PROJECT_DIR=%~dp0..
set DEPLOY_DIR=%PROJECT_DIR%\deploy
set ENIGMA=C:\EnigmaVB\enigmavbconsole.exe
set OUTPUT_EXE=%PROJECT_DIR%\GeoChecker_packed.exe

REM Проверка Enigma
if not exist "%ENIGMA%" (
    echo Ошибка: Enigma Virtual Box не найден в C:\EnigmaVB
    echo Скачайте с https://enigmaprotector.com/assets/files/enigmavb.exe
    exit /b 1
)

REM Проверка deploy
if not exist "%DEPLOY_DIR%\GeoChecker.exe" (
    echo Ошибка: GeoChecker.exe не найден в %DEPLOY_DIR%
    echo Сначала соберите проект и выполните windeployqt
    exit /b 1
)

REM Проверка node.js для generate-evb
where node >nul 2>nul
if errorlevel 1 (
    echo Ошибка: Node.js не найден. Нужен для генерации EVB проекта.
    exit /b 1
)

echo [1/2] Генерация EVB проекта...
cd /d "%PROJECT_DIR%"
node make_evb.js
if errorlevel 1 (
    echo Ошибка генерации EVB проекта!
    exit /b 1
)
echo OK: geochecker.evb создан
echo.

echo [2/2] Упаковка EXE...
if exist "%OUTPUT_EXE%" del "%OUTPUT_EXE%"
"%ENIGMA%" "%PROJECT_DIR%\geochecker.evb"

if exist "%OUTPUT_EXE%" (
    echo.
    echo ============================================================
    echo   Упаковка завершена!
    echo ============================================================
    echo.
    echo Результат: %OUTPUT_EXE%
    for %%F in ("%OUTPUT_EXE%") do echo Размер: %%~zF байт
    echo.
    echo Это единый EXE файл с иконкой, без DLL, без распаковки.
    echo Можно запускать на любом компьютере с Windows.
) else (
    echo Ошибка: GeoChecker_packed.exe не создан!
    exit /b 1
)
