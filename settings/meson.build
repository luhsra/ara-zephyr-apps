
if get_option('arch') == 'arm'
  path_error = '@0@ "@1@" does not exist or is not absolute'
  # external gcc dir
  if get_option('arm_gcc_version') == 'default'
    system_gcc = find_program('arm-none-eabi-gcc')
    r = run_command(py3_inst, gcc_version, system_gcc.path(), check: true)
    if r.returncode() != 0
      error('Cannot extract default GCC option. Consider setting "arm_gcc_version".')
    endif
    gcc_arm_version = r.stdout().strip()
  else
    gcc_arm_version = get_option('arm_gcc_version')
  endif
  libgcc_dir = get_option('arm_gcc_dir').format(gcc_arm_version)
  # requires meson 0.54
  # assert(fs.exists(libgcc_dir) and fs.is_absolute(libgcc_dir), path_error.format('arm_gcc_dir', libgcc_dir))
  assert(fs.exists(libgcc_dir), path_error.format('arm_gcc_dir', libgcc_dir))
  assert(fs.exists(libgcc_dir), 'arm_gcc_dir is non-existent.')
  message('libgcc directory: @0@'.format(libgcc_dir))
endif