#include "netlist.h"
#include "parser.h"
#include "simulation.h"
#include <stdio.h>

/**
 * For each level, get each gate, run the simulation of that gate and
 * store the new output to the gate's output node.
 */
void simulateCircuit(LevelsArray *levels_array) {
    for (int i = 0; i < levels_array->size; i++) {
        GatesArray *curr_level = levels_array->data[i];

        for (int j = 0; j < curr_level->size; j++) {
            Gate *curr_gate = curr_level->data[j];

            if (curr_gate->type == TYPE_DFF) {
                simulateAndStoreFF(curr_gate);
            } else {
                // Simulate the gate and store the simulated value to the output node
                curr_gate->outputs[0]->value = simulateGate(curr_gate);
            }

            // Ignore FFs and gates connected to FFs
            if (curr_gate->type == TYPE_DFF || curr_gate->outputs[0]->is_ff_input == 1) {
                continue;
            }

            // When current gate is marked as hit, flip the value
            if (curr_gate->outputs[0]->SET_should_hit == 1) {
                curr_gate->outputs[0]->value = !curr_gate->outputs[0]->value;
            }
        }
    }
}

/**
 * Get a gate, check its type and call the appropriate simulation function.
 */
int simulateGate(Gate *gate) {
    int output;

    switch (gate->type) {
        case TYPE_AND:
            output = simulateAND(gate);
            break;
        case TYPE_OR:
            output = simulateOR(gate);
            break;
        case TYPE_INV:
            output = simulateINV(gate);
            break;
        case TYPE_NAND:
            output = simulateNAND(gate);
            break;
        case TYPE_NOR:
            output = simulateNOR(gate);
            break;
        default:
            output = 0;
            break;
    }

    return output;
}

int simulateAND(Gate *gate) {
    for (int i = 0; i < gate->no_inputs; i++) {
        if (gate->inputs[i]->value == 0) {
            return 0;
        }
    }

    return 1;
}

int simulateOR(Gate *gate) {
    for (int i = 0; i < gate->no_inputs; i++) {
        if (gate->inputs[i]->value == 1) {
            return 1;
        }
    }

    return 0;
}

int simulateINV(Gate *gate) {
    return !(gate->inputs[0]->value);
}

int simulateNAND(Gate *gate) {
     for (int i = 0; i < gate->no_inputs; i++) {
        if (gate->inputs[i]->value == 0) {
            return 1;
        }
    }

    return 0;
}

int simulateNOR(Gate *gate) {
    for (int i = 0; i < gate->no_inputs; i++) {
        if (gate->inputs[i]->value == 1) {
            return 0;
        }
    }

    return 1;
}

/**
 * In case of FFs, we forward 0 to QN and 1 to Q outputs.
 * The flip flop can have either one or two outputs.
 */
void simulateAndStoreFF(Gate *gate) {
    if (gate->outputs[0] != NULL) {
        if (gate->outputs[0]->name[0] == 'Q') {
            gate->outputs[0]->value = 1;
        } else { // if not Q, must be QN
            gate->outputs[0]->value = 0;
        }
    }

    if (gate->outputs[1] != NULL) {
        if (gate->outputs[1]->name[0] == 'Q') {
            gate->outputs[1]->value = 1;
        } else {
            gate->outputs[1]->value = 0;
        }
    }
}
