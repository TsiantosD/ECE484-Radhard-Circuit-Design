#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

rm -f src/main

gcc -fsanitize=address -Wall -fopenmp -g \
    src/main.c src/parser.c src/levelization.c src/netlist.c src/simulation.c \
    src/flat_netlist.c src/backend_legacy.c src/backend_flat_cpu.c src/backend_cuda_stub.c \
    -o src/main -lm

TMP_DIR="/tmp/ece484_s27_backend_test"
rm -rf "$TMP_DIR" timing_logs
mkdir -p "$TMP_DIR"

./src/main tests/s27/s27.v "$TMP_DIR/legacy_nodes.csv" "$TMP_DIR/legacy_levels.csv" --backend legacy > "$TMP_DIR/legacy.log"
./src/main tests/s27/s27.v "$TMP_DIR/flat_nodes.csv" "$TMP_DIR/flat_levels.csv" --backend flat > "$TMP_DIR/flat.log"

for log in legacy flat; do
    grep -q "Total simulations: 96" "$TMP_DIR/$log.log"
    grep -q "Number of hit gates: 6" "$TMP_DIR/$log.log"
    grep -q "Number of simulations with Soft Error(s): 8" "$TMP_DIR/$log.log"
    grep -q "SER: 8.33%" "$TMP_DIR/$log.log"
done

diff -u "$TMP_DIR/legacy_nodes.csv" "$TMP_DIR/flat_nodes.csv"

[[ -d timing_logs ]] || { echo "timing_logs directory was not created" >&2; exit 1; }
[[ -f timing_logs/timings.csv ]] || { echo "timing_logs/timings.csv was not created" >&2; exit 1; }
grep -q "backend,test,backend_elapsed_seconds,total_elapsed_seconds,total_simulations,hit_gates,soft_errors,ser_percent" timing_logs/timings.csv
grep -q "legacy,s27," timing_logs/timings.csv
grep -q "flat,s27," timing_logs/timings.csv
[[ $(find timing_logs -maxdepth 1 -name 's27_legacy_*.log' | wc -l) -ge 1 ]] || { echo "legacy timing log was not created" >&2; exit 1; }
[[ $(find timing_logs -maxdepth 1 -name 's27_flat_*.log' | wc -l) -ge 1 ]] || { echo "flat timing log was not created" >&2; exit 1; }

grep -q "CUDA backend was selected" <(./src/main tests/s27/s27.v "$TMP_DIR/cuda_nodes.csv" "$TMP_DIR/cuda_levels.csv" --backend cuda 2>&1) || {
    echo "Expected CUDA stub to explain that CUDA support is not built in this binary" >&2
    exit 1
}

echo "s27 backend regression passed"
