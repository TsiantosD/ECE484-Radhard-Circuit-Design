import csv
import json
import sys
import subprocess
import os # NEW: Import the OS module to handle directories

# Define explicit top-level outputs (fixes G17 missing port)
KNOWN_OUTPUTS = ['G17']

def get_yosys_type(gate_type, num_inputs):
    """Safely map to standard shapes only if the pin count matches the skin."""
    if gate_type == 0: return '$_DFF_P_'
    if gate_type == 1: return '$_NOT_'
    
    if num_inputs <= 2:
        mapping = {2: '$_NAND_', 3: '$_NOR_', 4: '$_OR_', 5: '$_AND_'}
        return mapping.get(gate_type, 'GENERIC')
    else:
        mapping = {2: 'NAND', 3: 'NOR', 4: 'OR', 5: 'AND'}
        return mapping.get(gate_type, 'GENERIC') + f"_{num_inputs}"

def convert_to_yosys_json_and_svg(csv_filename, output_folder):
    
    # NEW: Create the target directory if it doesn't already exist
    os.makedirs(output_folder, exist_ok=True)
    print(f"📁 Saving all files to the '{output_folder}' directory...\n")

    # 1. First, group all rows by their Vector number
    vectors = {}
    with open(csv_filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            vec = int(row['VECTOR'])
            if vec not in vectors:
                vectors[vec] = []
            vectors[vec].append(row)
            
    # 2. Process each Vector independently
    for vec, rows in vectors.items():
        wire_ids = {}
        netnames = {}
        wire_values = {} 
        current_wire_id = 2 
        
        def get_wire_id(name):
            nonlocal current_wire_id
            if name not in wire_ids:
                wire_ids[name] = current_wire_id
                netnames[name] = {
                    "hide_name": 0,
                    "bits": [current_wire_id],
                    "attributes": {}
                }
                current_wire_id += 1
            return [wire_ids[name]]

        cells = {}
        all_inputs = set()
        all_outputs = set()
        
        for row in rows:
            gate_name = row['GATE_NAME']
            gate_type = int(row['GATE_TYPE'])
            
            inputs_raw = row['INPUTS(name=val)']
            outputs_raw = row['OUTPUTS(name=val)']
            
            in_pins = inputs_raw.split() if inputs_raw != "NONE" else []
            num_inputs = len(in_pins)
            
            yosys_type = get_yosys_type(gate_type, num_inputs)
            is_standard = yosys_type.startswith('$_')
            
            connections = {}
            port_directions = {}
            
            # --- Handle Inputs ---
            if num_inputs > 0:
                if gate_type == 0:
                    pin_names = ['D'] 
                elif is_standard:
                    pin_names = ['A', 'B'] 
                else:
                    pin_names = [f'IN{i}' for i in range(num_inputs)] 
                
                for idx, in_str in enumerate(in_pins):
                    wire_name, wire_val = in_str.split('=')
                    wire_values[wire_name] = wire_val 
                    
                    pin = pin_names[idx] if idx < len(pin_names) else f'IN{idx}'
                    connections[pin] = get_wire_id(wire_name)
                    port_directions[pin] = "input"
                    all_inputs.add(wire_name)
            
            # --- Handle Outputs ---
            if outputs_raw != "NONE":
                out_pins = outputs_raw.split()
                
                if gate_type == 0:
                    pin_names = ['Q', 'QN']
                elif is_standard:
                    pin_names = ['Y']
                else:
                    pin_names = ['OUT']
                
                for idx, out_str in enumerate(out_pins):
                    wire_name, wire_val = out_str.split('=')
                    wire_values[wire_name] = wire_val 
                    
                    pin = pin_names[idx] if idx < len(pin_names) else f'OUT{idx}'
                    connections[pin] = get_wire_id(wire_name)
                    port_directions[pin] = "output"
                    all_outputs.add(wire_name)
            
            cells[gate_name] = {
                "hide_name": 0,
                "type": yosys_type,
                "port_directions": port_directions,
                "connections": connections
            }

        primary_inputs = all_inputs - all_outputs
        guessed_outputs = all_outputs - all_inputs

        primary_outputs = set(KNOWN_OUTPUTS).union(guessed_outputs)

        ports = {}
        for pi in primary_inputs:
            val = wire_values.get(pi, "?")
            port_label = f"{pi} = {val}"
            ports[port_label] = {"direction": "input", "bits": get_wire_id(pi)}
            
        for po in primary_outputs:
            val = wire_values.get(po, "?")
            port_label = f"{po} = {val}"
            ports[port_label] = {"direction": "output", "bits": get_wire_id(po)}

        yosys_json = {
            "modules": {
                f"s27_vector_{vec}": {
                    "ports": ports,
                    "cells": cells,
                    "netnames": netnames
                }
            }
        }

        # NEW: Construct the file paths inside the new folder
        json_filename = os.path.join(output_folder, f"vec{vec}.json")
        svg_filename = os.path.join(output_folder, f"vec{vec}.svg")
        
        with open(json_filename, 'w') as f:
            json.dump(yosys_json, f, indent=2)
            
        # 3. Automatically run Netlistsvg to generate the image
        print(f"Generating SVG for Vector {vec}...")
        try:
            subprocess.run(['npx', 'netlistsvg', json_filename, '-o', svg_filename], check=True, stdout=subprocess.DEVNULL)
            print(f"  -> Saved {svg_filename}")
        except FileNotFoundError:
            print("  [!] Error: 'npx' or 'netlistsvg' not found on your system. JSON saved, but SVG skipped.")
        except subprocess.CalledProcessError:
            print("  [!] Error: netlistsvg failed to process the JSON.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python visualizer.py <input_data.csv> <output_folder_name>")
        print("Example: python visualizer.py visualization_data.csv s27_frames")
    else:
        convert_to_yosys_json_and_svg(sys.argv[1], sys.argv[2])