#ifndef ECE484_RADHARD_CIRCUIT_DESIGN_PARSER_H
#define ECE484_RADHARD_CIRCUIT_DESIGN_PARSER_H

int parseVerilogFile(char* pathname);
void parseNode(char *buffer, int type);
void parseGate(char *buffer, int type);

#endif //ECE484_RADHARD_CIRCUIT_DESIGN_PARSER_H
