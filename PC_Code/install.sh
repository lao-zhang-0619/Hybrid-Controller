#!/bin/bash
set -euo pipefail  # Strict mode: exit on error, unset var, or pipe failure
export LC_ALL=en_US.UTF-8  # Set UTF-8 encoding to avoid garbled text

# Color definitions for better readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No color

# Clear screen and print header
clear
echo -e "${BLUE}=============================================${NC}"
echo -e "${BLUE}          Python Project Dependency Installer${NC}"
echo -e "${BLUE}            (Linux/macOS - Bash/Zsh)         ${NC}"
echo -e "${BLUE}=============================================${NC}"
echo

# Step 1: Check if requirements.txt exists
check_requirements_file() {
    if [ ! -f "requirements.txt" ]; then
        echo -e "${RED}[ERROR] requirements.txt file not found!${NC}"
        echo -e "Please ensure the script and requirements.txt are in the same directory"
        exit 1
    fi
    echo -e "${GREEN}[SUCCESS] Found requirements.txt dependency list${NC}"
    echo
}

# Step 2: Detect Python/Pip environment (Python 3 priority)
detect_python_environment() {
    echo -e "${YELLOW}[INFO] Detecting Python environment...${NC}"
    local PYTHON_CMD=""
    local PIP_CMD=""

    # Prioritize python3/pip3
    if command -v python3 &>/dev/null; then
        PYTHON_CMD="python3"
        PIP_CMD="pip3"
        echo -e "${GREEN}[INFO] Detected Python 3, using: ${PYTHON_CMD}${NC}"
    elif command -v python &>/dev/null; then
        # Check if default python is 3.x
        local PY_VER
        PY_VER=$(python --version 2>&1 | awk '{print $2}' | cut -c1)
        if [ "$PY_VER" = "3" ]; then
            PYTHON_CMD="python"
            PIP_CMD="pip"
            echo -e "${GREEN}[INFO] Detected Python 3 (default), using: ${PYTHON_CMD}${NC}"
        else
            PYTHON_CMD="python"
            PIP_CMD="pip"
            echo -e "${YELLOW}[WARNING] Detected Python 2.x, may install to Python 2 environment!${NC}"
            echo "Recommend specifying Python 3 path or adding Python 3 to PATH"
        fi
    else
        echo -e "${RED}[ERROR] No Python environment found! Please install Python first${NC}"
        exit 1
    fi

    # Verify pip availability
    if ! command -v "${PIP_CMD}" &>/dev/null; then
        echo -e "${YELLOW}[INFO] ${PIP_CMD} not found, trying ${PYTHON_CMD} -m pip${NC}"
        PIP_CMD="${PYTHON_CMD} -m pip"
        # Re-verify pip
        if ! "${PYTHON_CMD}" -m pip --version &>/dev/null; then
            echo -e "${RED}[ERROR] pip is not installed! Please run: ${PYTHON_CMD} -m ensurepip${NC}"
            exit 1
        fi
    fi

    # Export variables for global use
    export PYTHON_CMD
    export PIP_CMD
    echo
}

# Step 3: Upgrade pip (optional but recommended)
upgrade_pip() {
    echo -e "${YELLOW}[INFO] Upgrading pip to the latest version...${NC}"
    if "${PIP_CMD}" install --upgrade pip --user; then
        echo -e "${GREEN}[SUCCESS] pip upgraded successfully${NC}"
    else
        echo -e "${YELLOW}[WARNING] pip upgrade failed, continuing with dependency installation${NC}"
    fi
    echo
}

# Step 4: Install dependencies from requirements.txt
install_dependencies() {
    echo -e "${YELLOW}[INFO] Starting dependency installation...${NC}"
    if "${PIP_CMD}" install --user -r requirements.txt; then
        echo -e "${GREEN}[SUCCESS] All dependencies installed successfully!${NC}"
    else
        echo -e "${RED}[ERROR] Dependency installation failed!${NC}"
        echo -e "${BLUE}[SOLUTIONS]:${NC}"
        echo "1. Check network connection"
        echo "2. Try domestic PyPI mirror: ${PIP_CMD} install -r requirements.txt --user -i https://pypi.tuna.tsinghua.edu.cn/simple"
        echo "3. Install system dependencies (Linux):"
        echo "   - Ubuntu/Debian: sudo apt install python3-dev libfreetype6-dev libpng-dev"
        echo "   - CentOS/RHEL: sudo yum install python3-devel freetype-devel libpng-devel"
        echo "4. Install Xcode Command Line Tools (macOS): xcode-select --install"
        exit 1
    fi
    echo
}

# Step 5: Verify key dependencies
verify_installation() {
    echo -e "${YELLOW}[INFO] Verifying dependency installation...${NC}"
    # Verify matplotlib
    if "${PYTHON_CMD}" -c "import matplotlib; print(f'matplotlib: {matplotlib.__version__}')" &>/dev/null; then
        echo -e "${GREEN}[SUCCESS] matplotlib verification passed${NC}"
    else
        echo -e "${YELLOW}[WARNING] matplotlib verification failed (may lack system dependencies)${NC}"
    fi

    # Verify pyserial
    if "${PYTHON_CMD}" -c "import serial; print(f'pyserial: {serial.VERSION}')" &>/dev/null; then
        echo -e "${GREEN}[SUCCESS] pyserial verification passed${NC}"
    else
        echo -e "${RED}[ERROR] pyserial verification failed, please reinstall${NC}"
        exit 1
    fi
    echo
}

# Main execution flow
main() {
    check_requirements_file
    detect_python_environment
    upgrade_pip
    install_dependencies
    verify_installation

    echo -e "${GREEN}=============================================${NC}"
    echo -e "${GREEN}[SUCCESS] Dependency installation completed!${NC}"
    echo -e "${BLUE}[TIP] Run your project with: ${PYTHON_CMD} your_code.py${NC}"
    echo -e "${GREEN}=============================================${NC}"
}

# Start main process
main