project('zephyr-apps', 'cpp',
    default_options : ['warning_level=3',
                       'cpp_std=c++17',
                       'buildtype=debugoptimized'])

llvm_version = ['>=14', '<16']
llvm_dep = dependency('llvm', method: 'config-tool', version: llvm_version, include_type: 'system')

py3_mod = import('python')
py3_inst = py3_mod.find_installation('python3')

subdir('appl')  # Zephyr samples
