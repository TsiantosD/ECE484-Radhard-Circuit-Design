#!/bin/bash

# 1. Build the C simulator
echo "========================================"
echo "Building C Simulator..."
echo "========================================"
make clean
make -C src || { echo "Make failed in src/"; exit 1; }

# 2. Select the test
TEST_NAME=$1

# If no argument was provided, prompt the user
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
C_OUT="outputs/${TEST_NAME}_c_sim.csv"

# Ensure outputs directory exists
mkdir -p outputs

if [ ! -f "$V_FILE" ]; then
    echo "Error: Verilog file $V_FILE not found!"
    exit 1
fi

# 3. Run the C Simulator
echo "-> Running C simulation..."
# Redirecting standard output to the CSV file
./src/main "$V_FILE" > "$C_OUT"
echo "-> C simulation output saved to $C_OUT"

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
    echo "Comparing C output ($C_OUT) against Verilog golden model ($V_OUT)..."
    
    # Run the diff. -q checks if they are identical silently.
    if diff -q "$C_OUT" "$V_OUT" > /dev/null; then
        echo -e "\n✅ SUCCESS: The C simulator output perfectly matches the Verilog output!"
    else
        echo -e "\n❌ FAILURE: Differences found! Here are the mismatched lines:"
        # Run diff again, showing the context, but limit to top 20 lines to avoid terminal spam
        diff -u "$C_OUT" "$V_OUT" | head -n 20 
    fi
else
    echo "   [!] Golden output file ($V_OUT) not found. Skipping comparison."
    echo "   (Make sure your Verilog Makefile generates a file named ${TEST_NAME}_golden.csv)"
fi
