import csv
import networkx as nx
import matplotlib.pyplot as plt

# Map your C definitions to human-readable names and Matplotlib geometric shapes
# Matplotlib markers: 's'=square, '>'='right triangle', 'p'=pentagon, 'D'=diamond, 'd'=thin diamond, '8'=octagon
GATE_MAPPING = {
    0: ('DFF', 's'),   
    1: ('INV', '>'),   
    2: ('NAND', 'p'),  
    3: ('NOR', 'D'),   
    4: ('OR', 'd'),    
    5: ('AND', '8')    
}

def visualize_vector(csv_filename, target_vector):
    G = nx.DiGraph()
    
    wire_sources = {}
    wire_values = {}
    
    # --- STEP 1: Parse the CSV and build the Gates ---
    with open(csv_filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            if int(row['VECTOR']) != target_vector:
                continue
                
            gate_name = row['GATE_NAME']
            level = int(row['LEVEL'])
            gate_type_int = int(row['GATE_TYPE'])
            
            # Fetch the name and shape from our dictionary (default to UNKNOWN/Circle if missing)
            gate_str, gate_shape = GATE_MAPPING.get(gate_type_int, ('UNKNOWN', 'o'))
            
            # Add the gate as a node in the graph, storing its shape
            G.add_node(gate_name, 
                       level=level, 
                       label=f"{gate_name}\n({gate_str})", 
                       node_color="lightblue",
                       shape=gate_shape)
            
            # Record the outputs this gate drives
            outputs_raw = row['OUTPUTS(name=val)']
            if outputs_raw != "NONE":
                for out_str in outputs_raw.split():
                    wire_name, wire_val = out_str.split('=')
                    wire_sources[wire_name] = gate_name
                    wire_values[wire_name] = int(wire_val)

    # --- STEP 2: Connect the wires (Edges) and Primary Inputs ---
    with open(csv_filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            if int(row['VECTOR']) != target_vector:
                continue
                
            dest_gate = row['GATE_NAME']
            inputs_raw = row['INPUTS(name=val)']
            
            if inputs_raw != "NONE":
                for in_str in inputs_raw.split():
                    wire_name, wire_val = in_str.split('=')
                    
                    # If the wire comes from a known gate, connect them
                    if wire_name in wire_sources:
                        source_node = wire_sources[wire_name]
                        G.add_edge(source_node, dest_gate, 
                                   label=wire_name, 
                                   val=int(wire_val))
                    else:
                        # If the wire has no source, it's a Primary Input!
                        # Create a pseudo-node for it at level -1 with a circle shape ('o')
                        if not G.has_node(wire_name):
                            G.add_node(wire_name, 
                                       level=-1, 
                                       label=f"INPUT\n{wire_name}", 
                                       node_color="lightgreen",
                                       shape='o')
                        
                        G.add_edge(wire_name, dest_gate, 
                                   label=wire_name, 
                                   val=int(wire_val))

    # --- STEP 3: Draw the Graph ---
    plt.figure(figsize=(14, 8))
    plt.title(f"Circuit State for Vector {target_vector}", fontsize=16, fontweight='bold')

    # Calculate layout using the 'level' attribute we assigned
    pos = nx.multipartite_layout(G, subset_key="level", align="horizontal")
    
    # Extract node and edge attributes for drawing
    node_labels = nx.get_node_attributes(G, 'label')
    edge_labels = nx.get_edge_attributes(G, 'label')
    edge_vals = nx.get_edge_attributes(G, 'val')
    
    # Color edges based on logic value: 1 = Red, 0 = Black
    edge_colors = ['red' if val == 1 else 'black' for val in edge_vals.values()]
    edge_widths = [2.0 if val == 1 else 1.0 for val in edge_vals.values()]

    # DRAW NODES: Group by shape to bypass networkx limitation
    shapes = set(nx.get_node_attributes(G, 'shape').values())
    for shape in shapes:
        # Filter nodes that match the current shape in the loop
        node_list = [n for n in G.nodes() if G.nodes[n]['shape'] == shape]
        node_colors = [G.nodes[n]['node_color'] for n in node_list]
        
        nx.draw_networkx_nodes(G, pos, 
                               nodelist=node_list, 
                               node_size=2000, 
                               node_color=node_colors, 
                               edgecolors="black", 
                               node_shape=shape)

    # Draw Labels and Edges
    nx.draw_networkx_labels(G, pos, labels=node_labels, font_size=9, font_weight="bold")
    nx.draw_networkx_edges(G, pos, arrowstyle="->", arrowsize=20, edge_color=edge_colors, width=edge_widths)
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=9, font_color="blue")

    # Final adjustments
    plt.axis("off")
    plt.tight_layout()
    plt.show()

# Run the visualization for Vector 0
if __name__ == "__main__":
    visualize_vector('./visualization_data.csv', target_vector=0)