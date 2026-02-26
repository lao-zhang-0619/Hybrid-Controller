@echo off
chcp 65001 >nul 2>&1  :: Set encoding to UTF-8 to avoid garbled characters
cls

echo ============================================
echo          Python Project Dependency Installer (Windows)
echo ============================================
echo.

:: Step 1: Check if requirements.txt exists
if not exist "requirements.txt" (
    echo "[ERROR] requirements.txt file not found!"
    echo "Please ensure the script and requirements.txt are in the same directory"
    pause
    exit /b 1
)
echo "[SUCCESS] Found requirements.txt dependency list"
echo.

:: Step 2: Detect Python/Pip environment (Python 3 first)
set "PYTHON_CMD="
set "PIP_CMD="

:: Detect python3/pip3 first
where python3 >nul 2>&1
if %errorlevel% equ 0 (
    set "PYTHON_CMD=python3"
    set "PIP_CMD=pip3"
    echo "[INFO] Detected Python 3, using: %PYTHON_CMD%"
) else (
    :: Detect default python (check if it's 3.x version)
    where python >nul 2>&1
    if %errorlevel% equ 0 (
        for /f "tokens=2 delims= " %%i in ('python --version 2^>^&1') do set "PY_VER=%%i"
        :: Check if version starts with 3
        if "%PY_VER:~0,1%" equ "3" (
            set "PYTHON_CMD=python"
            set "PIP_CMD=pip"
            echo "[INFO] Detected Python 3 (default), using: %PYTHON_CMD%"
        ) else (
            set "PYTHON_CMD=python"
            set "PIP_CMD=pip"
            echo "[WARNING] Detected Python 2.x (%PY_VER%), may install to Python 2 environment!"
            echo "Recommend specifying Python 3 path manually or adding Python 3 to system PATH"
        )
    ) else (
        echo "[ERROR] No Python environment found! Please install Python and add to system PATH first"
        pause
        exit /b 1
    )
)

:: Verify pip availability
%PIP_CMD% --version >nul 2>&1
if %errorlevel% neq 0 (
    echo "[INFO] %PIP_CMD% not found, trying %PYTHON_CMD% -m pip"
    set "PIP_CMD=%PYTHON_CMD% -m pip"
    :: Verify again
    %PIP_CMD% --version >nul 2>&1
    if %errorlevel% neq 0 (
        echo "[ERROR] pip is not installed! Please run: %PYTHON_CMD% -m ensurepip first"
        pause
        exit /b 1
    )
)
echo.

:: Step 3: Upgrade pip (optional but recommended)
echo "[INFO] Upgrading pip to the latest version..."
%PIP_CMD% install --upgrade pip --user >nul 2>&1
if %errorlevel% equ 0 (
    echo "[SUCCESS] pip upgraded successfully"
) else (
    echo "[WARNING] pip upgrade failed, continuing to install dependencies (core functions unaffected)"
)
echo.

:: Step 4: Install dependencies from requirements.txt
echo "[INFO] Starting to install dependency packages..."
%PIP_CMD% install --user -r requirements.txt
if %errorlevel% equ 0 (
    echo "[SUCCESS] All dependencies installed successfully!"
) else (
    echo "[ERROR] Dependency installation failed!"
    echo "[SOLUTIONS]:"
    echo "1. Check network connection"
    echo "2. Try using domestic PyPI mirror: %PIP_CMD% install -r requirements.txt --user -i https://pypi.tuna.tsinghua.edu.cn/simple"
    echo "3. Ensure Visual C++ Redistributable is installed (required by matplotlib)"
    pause
    exit /b 1
)
echo.

:: Step 5: Verify installation results of key dependencies
echo "[INFO] Verifying dependency installation results..."
%PYTHON_CMD% -c "import matplotlib; print('matplotlib: ' + matplotlib.__version__)" >nul 2>&1
if %errorlevel% equ 0 (
    echo "[SUCCESS] matplotlib verification passed"
) else (
    echo "[WARNING] matplotlib verification failed, may lack system dependencies"
)

%PYTHON_CMD% -c "import serial; print('pyserial: ' + serial.VERSION)" >nul 2>&1
if %errorlevel% equ 0 (
    echo "[SUCCESS] pyserial verification passed"
) else (
    echo "[ERROR] pyserial verification failed, please reinstall"
    pause
    exit /b 1
)
echo.

echo ============================================
echo "[SUCCESS] Dependency installation script completed!"
echo "Tip: Run your project with command: %PYTHON_CMD% your_code.py"
echo ============================================
pause