import os
import argparse
import sys


def run(args):
    args.name = args.name.replace('.asm', '').replace('examples/direct/', '')
    assert os.system(
        f'{sys.executable} verigpu/assembler.py'
        f' --in-asm examples/direct/{args.name}.asm'
        ' --offset 128'
        ' --out-hex build/prog.hex') == 0
    with open('test/behav/core_and_mem_driver.sv') as f:
        comp_driver = f.read()
    comp_driver = comp_driver.replace('{PROG}', 'prog')
    with open('build/core_and_mem_driver.sv', 'w') as f:
        f.write(comp_driver)
    os.system(f'cat examples/direct/{args.name}.asm')
    if args.verilator:
        os.system('test/behav/verilator/run.sh | tee /tmp/out.txt')
    else:
        assert os.system(
            'iverilog -Wall -g2012 -pfileline=1 src/assert.sv src/op_const.sv'
            ' src/const.sv src/int/int_div_regfile.sv'
            ' src/float/float_params.sv src/float/float_add_pipeline.sv'
            ' src/int/chunked_add_task.sv src/int/chunked_sub_task.sv '
            ' src/generated/mul_pipeline_cycle_24bit_2bpc.sv src/float/float_mul_pipeline.sv'
            ' src/generated/mul_pipeline_cycle_32bit_2bpc.sv src/int/mul_pipeline_32bit.sv'
            ' src/core.sv test/behav/core_and_mem.sv'
            ' src/mem_large.sv src/global_mem_controller.sv build/core_and_mem_driver.sv') == 0
        os.system('vvp ./a.out -fst +TEST_NAME='+ args.name +' | tee /tmp/out.txt')
    with open('/tmp/out.txt') as f:
        output = f.read()
        if 'ERROR' in output:
            for line in output.split('\n'):
                if 'ERROR' in line:
                    print(line)
            print('')
            raise Exception('Error')
    if os.path.exists(f'examples/direct/expected/{args.name}_expected.txt'):
        with open('/tmp/out.txt') as f:
            output = f.read()
            output = '\n'.join([line for line in output.split('\n') if line.startswith('out')])
        with open(f'examples/direct/expected/{args.name}_expected.txt') as f:
            expected = f.read().strip()
        print('')
        print('Target prog: ' + args.name)
        print('')
        if output.startswith('out 0'):
            output_l = ['out 0 ' + out.strip() for out in output.split('out 0 ') if out.strip() != '']
        else:
            output_l = ['out.s 0 ' + out.strip() for out in output.split('out.s 0 ') if out.strip() != '']
        if len(expected) > 0:
            assert len(output_l) > 0
        for i, output in enumerate(output_l):
            print('run', i)
            print('output')
            print(output)
            if expected != output:
                print('TEST ERROR: output mismatch')
                print('')
                print('actual:')
                print(output)
                print('')
                print('expected:')
                print(expected)
                raise Exception('assert failed')
            print('output verified')


if __name__ == '__main__':
    os.system("mkdir -p sim.out")
    parser = argparse.ArgumentParser()
    parser.add_argument('--name', type=str, required=True, help='eg prog5')
    parser.add_argument('--verilator', action='store_true', help='use verilator')
    args = parser.parse_args()
    run(args)
