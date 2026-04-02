all: compile run

compile:
	iverilog -o s27.sim s27.v ../cells.v tb_s27.v

run: compile
	vvp s27.sim

clean:
	rm -f s27.sim s27_golden.csv
