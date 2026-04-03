import csv
import json
import sys
import subprocess
import os

# Define explicit top-level outputs 
KNOWN_OUTPUTS = []

def get_gate_label(gate_type):
    """Maps the integer gate type to a readable string label."""
    mapping = {0: 'DFF', 1: 'NOT', 2: 'NAND', 3: 'NOR', 4: 'OR', 5: 'AND'}
    return mapping.get(gate_type, 'GATE')

def convert_to_yosys_json_and_svg(csv_filename, output_folder):
    
    # Create the target directory if it doesn't already exist
    os.makedirs(output_folder, exist_ok=True)
    print(f"📁 Saving all files to the '{output_folder}' directory...")

    # 1. Group all rows by their Vector number
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
                    "hide_name": 1, # Hide floating text, we will embed it in the pins!
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
            
            # Wraps the type in brackets to force a detailed Generic Box shape
            safe_type = f"[{get_gate_label(gate_type)}]"
            
            connections = {}
            port_directions = {}
            
            # --- Handle Inputs ---
            if num_inputs > 0:
                for idx, in_str in enumerate(in_pins):
                    wire_name, wire_val = in_str.split('=')
                    wire_values[wire_name] = wire_val 
                    
                    if gate_type == 0:
                        base_pin = "D" 
                    elif gate_type == 1:
                        base_pin = "A" 
                    else:
                        base_pin = ["A", "B", "C", "D", "E"][idx] if idx < 5 else f"in{idx}"
                    
                    # MAGIC TRICK: Embed the wire name and value directly into the port's label!
                    pin_label = f"{base_pin}={wire_val} ({wire_name})"
                    
                    connections[pin_label] = get_wire_id(wire_name)
                    port_directions[pin_label] = "input"
                    all_inputs.add(wire_name)
            
            # --- Handle Outputs ---
            if outputs_raw != "NONE":
                out_pins = outputs_raw.split()
                for idx, out_str in enumerate(out_pins):
                    wire_name, wire_val = out_str.split('=')
                    wire_values[wire_name] = wire_val 
                    
                    if gate_type == 0:
                        base_pin = "Q" if idx == 0 else "QN"
                    else:
                        base_pin = "Y"
                    
                    # MAGIC TRICK: Embed the output state directly into the port's label!
                    pin_label = f"({wire_name}) {base_pin}={wire_val}"
                    
                    connections[pin_label] = get_wire_id(wire_name)
                    port_directions[pin_label] = "output"
                    all_outputs.add(wire_name)
            
            cells[gate_name] = {
                "hide_name": 0,
                "type": safe_type,
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

        json_filename = os.path.join(output_folder, f"vec{vec}.json")
        svg_filename = os.path.join(output_folder, f"vec{vec}.svg")
        
        with open(json_filename, 'w') as f:
            json.dump(yosys_json, f, indent=2)
            
        print(f"Generating SVG for Vector {vec}...")
        try:
            subprocess.run(['npx', 'netlistsvg', json_filename, '-o', svg_filename], check=True, stdout=subprocess.DEVNULL)
            print(f"  -> Saved {svg_filename}")
        except FileNotFoundError:
            print("  [!] Error: 'npx' or 'netlistsvg' not found on your system. JSON saved, but SVG skipped.")
        except subprocess.CalledProcessError:
            print("  [!] Error: netlistsvg failed to process the JSON.")

    # 4. Generate interactive HTML viewer
    # 4. Generate interactive HTML viewer
    max_vec = len(vectors) - 1
    html_content = f"""<!DOCTYPE html>
<html>
<head>
    <title>Circuit Visualization Viewer</title>
    <style>
        body {{ text-align: center; font-family: sans-serif; background-color: #f4f4f9; margin: 0; padding: 20px; overflow: hidden; }}
        #viewer-container {{ background: white; padding: 20px; border-radius: 8px; display: inline-block; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }}
        #svg-container {{ width: 90vw; height: 75vh; overflow: hidden; border: 1px solid #ccc; cursor: grab; position: relative; }}
        #schematic {{ transform-origin: 0 0; transition: transform 0.05s linear; position: absolute; top: 0; left: 0; max-width: none; max-height: none; }}
        #controls {{ margin-bottom: 15px; font-size: 1.2em; }}
        .key {{ background: #eee; padding: 4px 8px; border-radius: 4px; border: 1px solid #ccc; font-family: monospace; font-size: 0.9em; }}
    </style>
</head>
<body>
    <div id="viewer-container">
        <div id="controls">
            <strong>Vector: <span id="vecNum" style="color: #0056b3; font-size: 1.4em;">0</span> / {max_vec}</strong><br>
            <span style="font-size: 0.85em; color: #555; margin-top: 5px; display: inline-block;">
                Use <span class="key">&larr;</span> / <span class="key">&rarr;</span> to navigate. Scroll to Zoom. Click & Drag to Pan.
            </span>
        </div>
        <div id="svg-container">
            <img id="schematic" src="vec0.svg" alt="Circuit Schematic">
        </div>
    </div>

    <script>
        let currentVec = 0;
        const maxVec = {max_vec};
        const imgElement = document.getElementById('schematic');
        const svgContainer = document.getElementById('svg-container');
        const vecText = document.getElementById('vecNum');

        // Zoom & Pan Variables
        let scale = 1;
        let panning = false;
        let pointX = 0;
        let pointY = 0;
        let startX = 0;
        let startY = 0;

        function setTransform() {{
            imgElement.style.transform = `translate(${{pointX}}px, ${{pointY}}px) scale(${{scale}})`;
        }}

        // Mouse Events for Panning
        svgContainer.onmousedown = function (e) {{
            e.preventDefault();
            startX = e.clientX - pointX;
            startY = e.clientY - pointY;
            panning = true;
            svgContainer.style.cursor = 'grabbing';
        }};

        svgContainer.onmouseup = function (e) {{
            panning = false;
            svgContainer.style.cursor = 'grab';
        }};

        svgContainer.onmouseleave = function (e) {{
            panning = false;
            svgContainer.style.cursor = 'grab';
        }};

        svgContainer.onmousemove = function (e) {{
            e.preventDefault();
            if (!panning) return;
            pointX = e.clientX - startX;
            pointY = e.clientY - startY;
            setTransform();
        }};

        // Wheel Event for Zooming
        svgContainer.onwheel = function (e) {{
            e.preventDefault();
            let xs = (e.clientX - svgContainer.getBoundingClientRect().left - pointX) / scale;
            let ys = (e.clientY - svgContainer.getBoundingClientRect().top - pointY) / scale;
            let delta = (e.wheelDelta ? e.wheelDelta : -e.deltaY);
            
            (delta > 0) ? (scale *= 1.2) : (scale /= 1.2);
            scale = Math.max(0.1, Math.min(scale, 10)); // Restrict scale limits

            pointX = e.clientX - svgContainer.getBoundingClientRect().left - xs * scale;
            pointY = e.clientY - svgContainer.getBoundingClientRect().top - ys * scale;
            setTransform();
        }};

        function preload(vec) {{
            if (vec >= 0 && vec <= maxVec) {{
                new Image().src = 'vec' + vec + '.svg';
            }}
        }}

        document.addEventListener('keydown', function(event) {{
            let changed = false;
            if (event.key === 'ArrowRight') {{
                if (currentVec < maxVec) {{ currentVec++; changed = true; }}
            }} else if (event.key === 'ArrowLeft') {{
                if (currentVec > 0) {{ currentVec--; changed = true; }}
            }}

            if (changed) {{
                imgElement.src = 'vec' + currentVec + '.svg';
                vecText.innerText = currentVec;
                preload(currentVec + 1);
                preload(currentVec - 1);
            }}
        }});
        
        preload(1);
    </script>
</body>
</html>
"""

    html_path = os.path.join(output_folder, "index.html")
    with open(html_path, "w") as f:
        f.write(html_content)
    
    print(f"\n🎉 Visualization Complete! Open this file in your browser to view:")
    print(f"   file://{os.path.abspath(html_path)}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python visualizer.py <input_data.csv> <output_folder_name>")
        print("Example: python visualizer.py visualization_data.csv s27_frames")
    else:
        convert_to_yosys_json_and_svg(sys.argv[1], sys.argv[2])