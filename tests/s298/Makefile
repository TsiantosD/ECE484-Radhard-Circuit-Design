all: compile run

compile:
	iverilog -o s298.sim s298.v ../cells.v tb_s298.v

run: compile
	vvp s298.sim

clean:
	rm -f s298.sim s298_golden.csv