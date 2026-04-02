## Circuit Simulator

### Authors

- Tsiantos Dimitrios (3796)
- Kalousis Anastasios (3792)
- Tsimponidis Alexandros (3739)

### Execution
To run the simulation, use the `run.sh` script. To see all the available option and arguments, use the `-h` or `--help` option.

### The simulation

The `src` directory contains the source code of the simulator written in C. The logic can be broken into three main parts:

1. **The parser**: it is responsible for reading a verilog file containing post synthesis verilog. It initializes the structs and populates the related data structures, preparing them for step 2.
2. **Levelization**: gets an array of gates and levelizes them using a levelization algorithm. Marks each gate with a level and then stores that gate to the related array in the levels array data structure.
> **_Note_**: the flip flops are marked with a level for visualization purposes but are not saved in the levels array data structure.
3. **Simulation**: once the gates are levelized, the simulation can begin. The simulation runs for all input vectors, one level at a time, calculating the output of each gante.

### Data structures
![](/docs/data_structures.png)
![](/docs/data_types.png)