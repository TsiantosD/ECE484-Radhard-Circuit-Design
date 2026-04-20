import os
import sys
import re

# Maximum number of test vectors to run (simulations on s35932)
MAX_VECTORS = 1024

def extract_list(code, keyword):
    """Extracts comma-separated variable names following a specific keyword."""
    # Matches the keyword, followed by anything up to a semicolon
    pattern = rf'\b{keyword}\b\s+([^;]+);'
    items = []
    for match in re.finditer(pattern, code):
        raw_string = match.group(1)
        # Split by comma, strip whitespace, ignore empty strings
        items.extend([x.strip() for x in raw_string.split(',') if x.strip()])
    return items

def generate_testbench(circuit_name):
    v_file = f"tests/{circuit_name}/{circuit_name}.v"
    tb_file = f"tests/{circuit_name}/tb_{circuit_name}.v"

    if not os.path.exists(v_file):
        print(f"[!] Error: Could not find {v_file}")
        return

    with open(v_file, 'r') as f:
        code = f.read()

    # Clean the code: Remove block (/* */) and line (//) comments to prevent false parsing
    code = re.sub(r'//.*', '', code)
    code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)

    # 1. Extract definitions
    all_inputs = extract_list(code, 'input')
    outputs = extract_list(code, 'output')
    wires = extract_list(code, 'wire')

    # Ensure no overlaps (sometimes synthesis tools declare outputs as wires too)
    wires = [w for w in wires if w not in outputs and w not in all_inputs]

    # 2. Filter Primary Inputs (Ignore GND, VDD, CK)
    special_pins = ['GND', 'VDD', 'CK']
    primary_inputs = [i for i in all_inputs if i not in special_pins]

    # Calculate loop bounds
    total_possible_vectors = 1 << len(primary_inputs) # Equivalent to 2^N
    actual_vectors = min(total_possible_vectors, MAX_VECTORS)

    # 3. Build Verilog Strings
    vec_width = len(primary_inputs) - 1
    
    # Generate: wire G3 = vec[3]; wire G2 = vec[2]; ...
    input_assignments = "\n    ".join(
        [f"wire {name} = vec[{idx}];" for idx, name in enumerate(primary_inputs)]
    )

    # Generate: .GND(GND), .VDD(VDD), .G0(G0)...
    uut_ports = [f".{port}({port})" for port in all_inputs + outputs]
    uut_instantiation = ", ".join(uut_ports)
    
    # Format instantiation to wrap nicely (5 ports per line)
    uut_chunks = [", ".join(uut_ports[i:i+5]) for i in range(0, len(uut_ports), 5)]
    uut_formatted = ",\n        ".join(uut_chunks)

    # Generate CSV Header
    header_cols = ["VEC"] + primary_inputs + outputs + wires
    header_str = ",".join(header_cols)

    # Generate Format String (%0d,%b,%b,%b...)
    fmt_str = "%0d" + ",%b" * (len(primary_inputs) + len(outputs) + len(wires))

    # Generate Values List (i, G0, G1..., G17..., UUT.n1...)
    vars_list = ["i"] + primary_inputs + outputs + [f"UUT.{w}" for w in wires]
    
    # Chunk the variables so iverilog doesn't throw a wrapping/malformed syntax error
    vars_chunks = [", ".join(vars_list[i:i+15]) for i in range(0, len(vars_list), 15)]
    vars_formatted = ",\n                ".join(vars_chunks)

    # 4. Construct the Final Verilog File
    tb_content = f"""`timescale 1ns/1ps

module tb_{circuit_name};
    reg GND, VDD, CK;

    reg [{vec_width}:0] vec; 

    {input_assignments}

    // Primary Outputs
    wire {", ".join(outputs)};

    {circuit_name} UUT (
        {uut_formatted}
    );

    integer file_handle;
    integer i;

    initial begin
        file_handle = $fopen("{circuit_name}_golden.csv", "w");

        // Dynamically generated header
        $fdisplay(file_handle, "{header_str}");

        GND = 0;
        VDD = 1;
        CK  = 0;

        for (i = 0; i < {actual_vectors}; i = i + 1) begin
            vec = i;
            #10; 

            // Exactly 1 %0d and {len(vars_list)-1} %b format specifiers
            $fdisplay(file_handle, "{fmt_str}",
                {vars_formatted}
            );
        end

        $fclose(file_handle);
        $finish; 
    end
endmodule
"""

    with open(tb_file, "w") as f:
        f.write(tb_content)

    print(f"✅ Generated {tb_file}")
    if total_possible_vectors > MAX_VECTORS:
        print(f"   [!] Note: {circuit_name} has {len(primary_inputs)} inputs ({total_possible_vectors} combos). Capped loop at {MAX_VECTORS}.")


if __name__ == "__main__":
    if len(sys.argv) == 2:
        generate_testbench(sys.argv[1])
    else:
        # If no argument is provided, generate for ALL tests automatically!
        print("Generating testbenches for all circuits...\n")
        tests_dir = "tests"
        for folder in os.listdir(tests_dir):
            if os.path.isdir(os.path.join(tests_dir, folder)) and folder.startswith("s"):
                generate_testbench(folder)
