# build and run verilog & GTKWave sims.

COMPILER = iverilog
SIMULATOR = vvp
DIAGRAM_TOOL = gtkwave

# sources for task
SOURCES = src/a5-cipher.sv src/rc-loss.sv
# file(s) with tests
TEST = testbench/testbench.sv
# generated scheme name
GENERATED = out
# name of testing module (entrypoint)
MAIN = testbench

build: $(SOURCES)
	$(COMPILER) -o $(GENERATED) -s $(MAIN) $(TEST) $(SOURCES)

sim:
	vvp -la.lst -n $(GENERATED) -vcd

plot:
	gtkwave dump.vcd

clean:
	rm dump.vcd $(GENERATED) a.lst