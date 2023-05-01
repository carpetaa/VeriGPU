.PHONY: clean run_regression
clean:
	rm -rf build obj_dir\
		a.out \
		sim.out \
		test/behav/verilator/build \
		test/behav/verilator/sim.out \
		coverage.out

run_regression:
	bash test/behav/run_examples_verilator.sh

gen_coverage:
	bash test/behav/verilator/coverage_gen.sh
