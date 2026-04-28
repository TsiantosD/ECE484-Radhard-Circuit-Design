## Circuit Simulator

### Authors

- Tsiantos Dimitrios (3796)
- Kalousis Anastasios (3792)
- Tsimponidis Alexandros (3739)

### Execution
To run the simulation, use the `run.sh` script. To see all the available option and arguments, use the `-h` or `--help` option.

For example, you can run the following:
```bash
./run.sh s27 -v
```
The `-v` is for the visualization of the circuit which can be opened in the browser by copy-pasting the HTML file.

> **_Note_**: Don't forget to give execution permissions to the `run.sh` script with `sudo chmod +x run.sh`.

### Docker container
To ensure portability, a `Dockerfile` was created. Please run the following command to build and then run the container (it might take some time to build):

```bash
# Build the container - only the first time
docker build -t circuit-sim-test .

# Open the container's terminal
docker run -it --rm circuit-sim-test

# Run the simulation
./run.sh
```

### The simulation

The `src` directory contains the source code of the simulator written in C. The logic can be broken into three main parts:

1. **The parser**: it is responsible for reading a verilog file containing post synthesis verilog. It initializes the structs and populates the related data structures, preparing them for step 2.
2. **Levelization**: gets an array of gates and levelizes them using a levelization algorithm. Marks each gate with a level and then stores that gate to the related array in the levels array data structure.
> **_Note_**: the flip flops are marked with a level for visualization purposes but are not saved in the levels array data structure.
3. **Simulation**: once the gates are levelized, the simulation can begin. The simulation runs for all input vectors, one level at a time, calculating the output of each gate.
> **_Note_**: Flip flops have a constant output of `0` for `ZN` and `1` for `Z` outputs.
4. **Soft Error Rate (SER)**: for each simulation, some selected gates will be "hit". Those gates should i) not be DFFs ii) not be connected directly to DFFs and iii) not be connected directly to a primary output. The "hit" is a bit-flip of the output. For each gate hit, the simulation re-runs. If after a simulation an error has been propagated to a DFF, this specific steady state is skipped, the soft errors counter is incremented and the next steady state is simulated. The Soft Error Rate is calculated as the number of steady states with at least one soft error, over the number of steady states multiplied by the number of possible gates that can be hit, i.e. the gates that satisfy the three criteria - i, ii, iii.

### Validation
**To validate the correctness of our simulator**, the `run.sh` script compares the C simulation results with the verilog simulation results.

For the C simulation two files are stored in the `outputs/<simulation_name>/` directory:
- `nodes.csv`: Used for validation. For each input vector, contains the value of every node.
- `levels.csv`: Used for visualization. For each input vector, contains each gate's inputs, outputs and values.

For the Verilog simulation, there are the following files in the `tests/<simulation_name>/` directory:
- `<simulation_name>.v`: The original circuit to be simulated.
- `tb_<simulation_name>.v`: The testbench of that circuit (only for `s27` and and `s298` for the moment).
- `Makefile`: To compile and run the simulation using `iverilog` and `vvp`.

### Data structures
The data structures can be found in the `/src/netlist.h` header file.

![](/docs/data_structures.png)
![](/docs/data_types.png)
