#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_SIMULATION_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_SIMULATION_H

#include "netlist.h"

void simulateCircuit(LevelsArray *levels_array);
int simulateGate(Gate *gate);
int simulateAND(Gate *gate);
int simulateOR(Gate *gate);
int simulateINV(Gate *gate);
int simulateNAND(Gate *gate);
int simulateNOR(Gate *gate);
void simulateAndStoreFF(Gate *gate);

#endif // ECE484_RADHARD_CIRCUIT_DESIGN_SIMULATION_H
