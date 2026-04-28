#!/bin/bash

# ========================================
# Helper Functions & Argument Parsing
# ========================================

show_help() {
    echo "Usage: ./run.sh [OPTIONS] [TEST_NAME]"
    echo ""
    echo "This script builds and runs the C simulator, compares its output"
    echo "against the Verilog golden model, and optionally generates circuit SVGs."
    echo ""
    echo "Options:"
    echo "  -h, --help       Show this help message and exit."
    echo "  -v, --visualize  Run the Python visualizer on the generated levels CSV."
    echo ""
    echo "Example:"
    echo "  ./run.sh -v s27  # Runs test 's27' and generates SVGs."
    echo "  ./run.sh         # Interactive mode (prompts for test selection)."
}

VISUALIZE=false
TEST_NAME=""

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -h|--help) show_help; exit 0 ;;
        -v|--visualize) VISUALIZE=true; shift ;;
        -*) echo "Error: Unknown parameter passed: $1"; echo "Use --help for usage."; exit 1 ;;
        *) TEST_NAME="$1"; shift ;; # Positional argument (Test Name)
    esac
done

# 1. Build the C simulator
echo "========================================"
echo "Building C Simulator..."
echo "========================================"
make clean -C src
make sanitize -C src || { echo "Make failed in src/"; exit 1; }

# 2. Select the test
# If no test name was provided via positional arguments, prompt the user
if [ -z "$TEST_NAME" ]; then
    echo ""
    echo "========================================"
    echo "Available Tests:"
    echo "========================================"
    
    # Store all directories inside 'tests/' into an array
    TEST_DIRS=(tests/*/)
    
    # Enumerate and print them
    for i in "${!TEST_DIRS[@]}"; do
        DIR_NAME=$(basename "${TEST_DIRS[$i]}")
        echo "$((i+1))) $DIR_NAME"
        TEST_NAMES[$((i+1))]=$DIR_NAME
    done
    
    echo ""
    read -p "Enter the number of the test you want to run: " SELECTION
    
    # Map the number back to the folder name
    TEST_NAME=${TEST_NAMES[$SELECTION]}
    
    if [ -z "$TEST_NAME" ]; then
        echo "Error: Invalid selection."
        exit 1
    fi
fi

echo ""
echo "========================================"
echo "Running Test: $TEST_NAME"
echo "========================================"

# Define paths
TEST_DIR="tests/$TEST_NAME"
V_FILE="$TEST_DIR/$TEST_NAME.v"
OUT_DIR="outputs/$TEST_NAME"
C_NODES_OUT="$OUT_DIR/nodes.csv"
C_LEVELS_OUT="$OUT_DIR/levels.csv"

# Ensure circuit specific outputs directory exists
mkdir -p "$OUT_DIR"

if [ ! -f "$V_FILE" ]; then
    echo "Error: Verilog file $V_FILE not found!"
    exit 1
fi

# 3. Run the C Simulator
echo "-> Running C simulation..."
# Redirecting standard output to nodes.csv and standard error to levels.csv
./src/main "$V_FILE" "$C_NODES_OUT" "$C_LEVELS_OUT"
echo "-> C simulation nodes saved to $C_NODES_OUT"
echo "-> C simulation levels saved to $C_LEVELS_OUT"

# 4. Build and run the Verilog Golden Model
echo ""
echo "-> Building and running Verilog simulation..."
if [ -f "$TEST_DIR/Makefile" ]; then
    # Run make inside the specific test directory
    make -C "$TEST_DIR"
else
    echo "   [!] No Makefile found in $TEST_DIR. Skipping Verilog build."
fi

# 5. Compare the results
echo ""
echo "========================================"
echo "Comparing Results"
echo "========================================"

# Define expected Verilog output name
V_OUT="$TEST_DIR/${TEST_NAME}_golden.csv"

# Fallback specifically for your s27 directory structure (27_golden.csv)
if [ ! -f "$V_OUT" ]; then
    STRIPPED_NAME=$(echo "$TEST_NAME" | sed 's/^s//')
    if [ -f "$TEST_DIR/${STRIPPED_NAME}_golden.csv" ]; then
        V_OUT="$TEST_DIR/${STRIPPED_NAME}_golden.csv"
    fi
fi

if [ -f "$V_OUT" ]; then
    echo "Comparing C output ($C_NODES_OUT) against Verilog golden model ($V_OUT)..."
    
    # Run the diff. -q checks if they are identical silently.
    if diff -q "$C_NODES_OUT" "$V_OUT" > /dev/null; then
        echo -e "\n✅ SUCCESS: The C simulator output perfectly matches the Verilog output!"
    else
        echo -e "\n❌ FAILURE: Differences found! Here are the mismatched lines:"
        # Run diff again, showing the context, but limit to top 20 lines to avoid terminal spam
        diff -u "$C_NODES_OUT" "$V_OUT" | head -n 20 
    fi
else
    echo "   [!] Golden output file ($V_OUT) not found. Skipping comparison."
    echo "   (Make sure your Verilog Makefile generates a file named ${TEST_NAME}_golden.csv)"
fi

# 6. Optional: Generate Visualizations
if [ "$VISUALIZE" = true ]; then
    echo ""
    echo "========================================"
    echo "Generating Visualizations"
    echo "========================================"
    VISUALIZER_DIR="./visualizer"
    VISUALIZER_OUT_DIR="$VISUALIZER_DIR/$TEST_NAME"
    VENV_DIR="$VISUALIZER_DIR/venv"

    # Ensure Node.js and npx are installed on the system before proceeding
    if ! command -v npx &> /dev/null || ! command -v npm &> /dev/null; then
        echo "   [!] Error: 'npx' or 'npm' command not found."
        echo "       Node.js is required to run netlistsvg."
        echo "       Please install it (e.g., 'sudo apt install nodejs npm' on Ubuntu) and try again."
    elif [ -f "$VISUALIZER_DIR/visualizer.py" ]; then
        echo "-> Checking environments and dependencies..."

        # Check if the venv directory exists
        if [ ! -d "$VENV_DIR" ]; then
            echo "   [*] Virtual environment not found. Creating one in $VENV_DIR..."
            python3 -m venv "$VENV_DIR"

            echo "   [*] Activating venv and installing Python dependencies..."
            source "$VENV_DIR/bin/activate"

            # Upgrade pip and install the required packages quietly
            pip install --upgrade pip -q
            pip install networkx matplotlib scipy -q

            echo "   [*] Installing netlistsvg locally via npm..."
            # Change directory temporarily to install netlistsvg inside the visualizer folder
            (cd "$VISUALIZER_DIR" && npm install netlistsvg --silent)

            echo "   [*] All dependencies installed successfully."
        else
            echo "   [*] Existing environments found. Activating..."
            source "$VENV_DIR/bin/activate"
        fi

        echo "-> Running Python visualizer..."
        # Run the script using the python executable from the activated venv
        python3 "$VISUALIZER_DIR/visualizer.py" "$C_LEVELS_OUT" "$VISUALIZER_OUT_DIR"

        # Deactivate the virtual environment when done
        deactivate
        echo "-> Visualization finished."
    else
        echo "   [!] Error: $VISUALIZER_DIR/visualizer.py not found!"
    fi
fi