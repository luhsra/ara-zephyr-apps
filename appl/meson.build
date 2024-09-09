fs = import('fs')
zephyr = subproject('zephyr-toolchain')
zephyr_source_dir = zephyr.get_variable('zephyr_source_dir')
zephyr_work_dir = zephyr.get_variable('zephyr_work_dir')
zephyr_modules = ';'.join(zephyr.get_variable('modules'))

# Apps that run on virtually any board since there are no hw dependencies
zephyr_apps = [
    zephyr_source_dir / 'samples' / 'basic' / 'minimal',
    zephyr_source_dir / 'samples' / 'hello_world',
    zephyr_source_dir / 'tests' / 'benchmarks' / 'sys_kernel',
    # zephyr_source_dir / 'tests' / 'benchmarks' / 'app_kernel', # too big for nucleo
    # zephyr_source_dir / 'samples' / 'cpp' / 'cpp_synchronization' # c++ not supported
    # zephyr_source_dir / 'tests' / 'benchmarks' / 'latency_measure', # get_vfg_node pattern 3
    meson.current_source_dir() / 'app_kernel',
    meson.current_source_dir() / 'latency_measure',
    meson.current_source_dir() / 'duplicate_syscalls',
    meson.current_source_dir() / 'dyn_heap',
    meson.current_source_dir() / 'dyn_msgq',
    meson.current_source_dir() / 'dyn_pipe',
    meson.current_source_dir() / 'dyn_queue',
    meson.current_source_dir() / 'dyn_stack',
    meson.current_source_dir() / 'dyn_sys_sems',
    meson.current_source_dir() / 'dyn_sems',
    meson.current_source_dir() / 'multi_init',
    meson.current_source_dir() / 'static_heap',
    meson.current_source_dir() / 'static_msgq',
    meson.current_source_dir() / 'static_pipe',
    meson.current_source_dir() / 'static_queue',
    meson.current_source_dir() / 'static_sems',
    meson.current_source_dir() / 'static_stack',
    meson.current_source_dir() / 'static_sys_sems',
    meson.current_source_dir() / 'static_threads',
    meson.current_source_dir() / 'OSPERT_2022'
]

# Apps that require boards that support userspace applications
z_userspace_apps = [
    zephyr_source_dir / 'samples' / 'userspace' / 'prod_consumer',
    zephyr_source_dir / 'samples' / 'userspace' / 'shared_mem'
]

# Apps that require hardware (gpio, interrupt controller) and therefore run on the nucleo_f103rb
z_hw_apps = [
    zephyr_source_dir / 'samples' / 'basic' / 'blinky',
    zephyr_source_dir / 'samples' / 'basic' / 'button',
    meson.current_source_dir() / 'dyn_isr',
    meson.current_source_dir() / 'static_isr'
]

toolchains = subproject('toolchains')

llvm_ld = toolchains.get_variable('llvm_ld')
llvm_objcopy = toolchains.get_variable('llvm_objcopy')
llvm_objdump = toolchains.get_variable('llvm_objdump')
llvm_nm = toolchains.get_variable('llvm_nm')
llvm_ar = toolchains.get_variable('llvm_ar')
llc = toolchains.get_variable('llc')
clang = toolchains.get_variable('clang')
clangpp = toolchains.get_variable('clangpp')
ara_link = toolchains.get_variable('ara_link')

cmake_tool = find_program('cmake')
ninja = find_program('ninja')

# not directly used within Meson but needed by Zephyr
dtc = find_program('dtc')

compile_zephyr_app = files('compile_zephyr_app.py')[0]

boards = []
if toolchains.get_variable('posix_enabled')
  boards += [{
    'name': 'native_sim',
    'apps': zephyr_apps,
    'cflags': '',
    'ldflags': '',
  }]
endif
arm_boards = []
if toolchains.get_variable('arm_enabled')
  cflags = '-mfloat-abi=soft'
  ldflags = '-fuse-ld=@0@ -L @1@'.format(toolchains.get_variable('arm_ld').full_path(),toolchains.get_variable('arm_libgcc_dir'))

  if get_option('build_nucleo_f303re')
    boards += [{
      'name': 'nucleo_f303re',
      'apps': zephyr_apps + z_hw_apps,
      'cflags': cflags,
      'ldflags': ldflags,
    }]
    arm_boards += ['nucleo_f303re']
  endif

  if get_option('build_nucleo_f103rb')
    boards += [{
      'name': 'nucleo_f103rb',
      'apps': zephyr_apps + z_hw_apps,
      'cflags': cflags,
      'ldflags': ldflags,
    }]
    arm_boards += ['nucleo_f103rb']
  endif
endif

# Add all available zephyr apps to zephyr_apps.
# The build process looks like this:
# 1) All apps are build using a custom target which invokes the compile_zephyr.py script.
# 2) The build script checks if the board has changed since last build and might clear all artifacts
# 3) CMake is configured and the ninja backend is executed
# 4) The zephyr build process runs as it normally would, but the app is compiled to llvm ir and in a
#       second step compiled with llc. At this point linking finishes normally and a flashable
#       image as well as *app*.ll is produced.
# This process is not ideal, but the other options are even worse:
# CMake module: While meson explicitly states that it does not support mixing build systems, it
#   provides a cmake module. There are multiple reasons why this is not feasable:
#   * All cmake projects have to be located in /subprojects
#   * Most compiler and linker flags are inferred from the meson project (ARA) which is a pain when
#       crosscompiling/changing boards.
#   * When changing boards the entire cmake project has to be purged to avoid caching issues. This
#       can not be automated.
# run_command(): This does not create a top level target.
# run_target(): Meson expects this to have no output.

zephyr_targets = []
foreach board : boards
  board_name = board['name']
  apps = board['apps']
  foreach app : apps
    app_name = fs.name(app)
    tgt_name = f'@board_name@-@app_name@'
    target = custom_target(tgt_name,
      output: [tgt_name + '.ll', tgt_name + '.config'],
      depends: ara_link,
      command: [
        py3_inst, compile_zephyr_app,
        '--cmake-builddir', meson.current_build_dir() / tgt_name + '_build',
        '--app-dir', app,
        '--cmake-program', cmake_tool.full_path(),
        '--ninja-program', ninja.full_path(),
        '--zephyr-dir', zephyr_work_dir / 'zephyr',
        '--target', 'app/libapp.ll',
        '--output', '@OUTPUT0@',
        '--kconfig', '@OUTPUT1@',
        '--cmake-args', ['BOARD=' + board_name,
                         'ZEPHYR_TOOLCHAIN_VARIANT=llvm',
                         'TOOLCHAIN_ROOT=' + zephyr_work_dir / 'zephyr',
                         'ZEPHYR_BASE=' + zephyr_work_dir / 'zephyr',
                         'ZEPHYR_MODULES=' + zephyr_modules,
                         'CMAKE_C_COMPILER=' + clang.full_path(),
                         'CMAKE_CXX_COMPILER=' + clangpp.full_path(),
                         'CMAKE_LLC=' + llc.full_path(),
                         'CMAKE_LLVM_LINK=' + ara_link.full_path(),
                         'CMAKE_OBJCOPY=' + llvm_objcopy.full_path(),
                         'CMAKE_OBJDUMP=' + llvm_objdump.full_path(),
                         'CMAKE_NM=' + llvm_nm.full_path(),
                         'CMAKE_AR=' + llvm_ar.full_path(),
                         #'KCONFIG_ROOT=' TODO
                         # Disable common warnings that gcc does not emit but clang does. Typedef redefinition will be fixed
                         # in https://github.com/zephyrproject-rtos/zephyr/pull/29359
                         'EXTRA_CFLAGS=-Wno-typedef-redefinition -Wno-unused-command-line-argument @0@'.format(board['cflags']),
                         'CMAKE_EXE_LINKER_FLAGS=@0@'.format(board['ldflags']),
        ]]
    )
    #set_variable(tgt_name.underscorify(), {'ll': target[0], 'config': target[1]})
    zephyr_targets += target
  endforeach
endforeach

nop = find_program('true')
all_apps = 'build-zephyr-apps'
run_target(all_apps, command: nop, depends: zephyr_targets)
summary('Build all ARA Zephyr apps', f'meson compile @all_apps@')

