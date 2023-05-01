rm -rf $PROJ_ROOT/coverage.out
mkdir -p $PROJ_ROOT/coverage.out/annotated
coverage_out_dir=$PROJ_ROOT/coverage.out
coverage_source_dir=$PROJ_ROOT/test/behav/verilator/build/sim.out/cov_logs
echo $coverage_out_dir
echo $coverage_source_dir

verilator_coverage --rank --write $coverage_out_dir/merged.dat $coverage_source_dir/*.dat
verilator_coverage --annotate $coverage_out_dir/annotated --annotate-points --rank --write-info $coverage_out_dir/merged.info $coverage_out_dir/merged.dat
genhtml $coverage_out_dir/merged.info --output-directory $coverage_out_dir/html
