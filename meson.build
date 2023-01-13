project('zephyr', 'cpp',
    default_options : ['warning_level=3',
                       'cpp_std=c++17',
                       'buildtype=debugoptimized'])

fs = import('fs')

# check zephyrproject path
if fs.is_absolute(get_option('zephyr_toolchain_dir'))
    zephyrproject = get_option('zephyr_toolchain_dir')
else
    zephyrproject = fs.expanduser('~/' + get_option('zephyr_toolchain_dir'))
endif

if not fs.is_dir(zephyrproject)
    error('''zephyr_toolchain_dir is not a directory.
Run init_zephyr_toolchain target in parrot to generate the toolchain.
''')
endif
toolchain_ready_signalizer = join_paths(zephyrproject, 'ara_toolchain_ready')
if not fs.is_file(toolchain_ready_signalizer)
    error('''zephyr_toolchain_dir is not ready.
Run init_zephyr_toolchain target in parrot to generate the toolchain.
''')
endif
files(toolchain_ready_signalizer)

## llvm dependencies
llvm_version = ['>=14', '<16']
llvm_dep = dependency('llvm', method: 'config-tool', version: llvm_version, include_type: 'system')

llvm_bindir = llvm_dep.get_variable(configtool: 'bindir')
clang = find_program(llvm_bindir / 'clang', version: llvm_version)
llc = find_program(llvm_bindir / 'llc', version: llvm_version)
llvm_link = find_program(llvm_bindir / 'llvm-link', version: llvm_version)

## tools
nop = find_program('true')

# Python
py3_mod = import('python')
py3_inst = py3_mod.find_installation('python3', modules: [])
if py3_inst.language_version().version_compare('<3.9')
    error('At least Python 3.9 is required.')
endif

## arch specific build tools
if get_option('arch') == 'i386'
    target_triple='x86_64-linux-gnu'
elif get_option('arch') == 'arm'
    target_triple = 'arm-none-eabi'
else
    error('unknown arch ' + get_option('arch'))
endif

foreach prog_name : ['ar', 'ld', 'objcopy', 'objdump', 'nm']
    full_prog_name = target_triple + '-' + prog_name
    prog = find_program(full_prog_name, required: false)
    if not prog.found()
        error(arch_error.format(get_option('arch').to_upper()))
    endif
    set_variable(prog_name, prog)
endforeach

subdir('tools')
subdir('settings')
subdir('appl')  # Zephyr samples

zephyr_examples = alias_target('zephyr_examples', zephyr_targets)

run_target('z_clean',
    command: ['rm', '-rf', join_paths(meson.current_build_dir(), 'appl')])

examples = run_target('zephyr_examples_rt',
                      command: nop,
                      depends: [zephyr_targets])
