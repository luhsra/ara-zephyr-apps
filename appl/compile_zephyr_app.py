#!/usr/bin/env python
"""Build a Zephyr app."""
import argparse
import sys
import pathlib
import os
import shutil
import subprocess


def eprint(*args):
    """Error print"""
    print(*args, file=sys.stderr)


def run(message, cmd, **kwargs):
    """Print and execute a command."""
    env_fmt = ''
    if 'env' in kwargs:
        env_diff = set(kwargs['env'].items()) - set(os.environ.items())
        env_fmt = ' '.join([f"{key}='{val}'" for key, val in env_diff])
    eprint(message + ':', env_fmt, ' '.join([f"'{x}'" for x in cmd]))
    subprocess.run(cmd, check=True, **kwargs)


def main():
    parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
    parser.add_argument('--cmake-builddir',
                        help='Directory for the CMake build.',
                        required=True,
                        type=pathlib.Path)
    parser.add_argument('--app-dir',
                        help='Directory where the App source files are.',
                        required=True,
                        type=pathlib.Path)
    parser.add_argument('--cmake-program',
                        help='CMake executable.',
                        required=True,
                        type=pathlib.Path)
    parser.add_argument('--ninja-program',
                        help='Ninja executable.',
                        required=True,
                        type=pathlib.Path)
    # parser.add_argument('--get-bc-program',
    #                     help='get-bc executable.',
    #                     required=True,
    #                     type=pathlib.Path)
    # parser.add_argument('--llvm-objcopy-program',
    #                     help='llvm-objcopy executable.',
    #                     required=True,
    #                     type=pathlib.Path)
    # parser.add_argument('--llvm-ld-program',
    #                     help='lld executable.',
    #                     required=True,
    #                     type=pathlib.Path)
    parser.add_argument('--zephyr-dir',
                        help='Path to the Zephyr RTOS.',
                        required=True,
                        type=pathlib.Path)
    parser.add_argument('--target',
                        help='Ninja target.',
                        required=True,
                        type=pathlib.Path)
    parser.add_argument('--output',
                        help='Bitcode output file.',
                        required=True,
                        type=pathlib.Path)
    # parser.add_argument('--llvm-bindir',
    #                     help='Directory that contains the LLVM tools.',
    #                     required=True,
    #                     type=pathlib.Path)
    parser.add_argument('--cmake-args',
                        help='CMake arguments (given as "foo=bar")',
                        required=True,
                        nargs='*')
    args = parser.parse_args()
    if args.cmake_builddir.is_dir():
        shutil.rmtree(args.cmake_builddir)

    args.cmake_builddir.mkdir()

    assert args.cmake_program.is_file()
    assert args.ninja_program.is_file()
    assert args.app_dir.is_dir()

    eprint("BLALBAL", args.zephyr_dir, args.zephyr_dir.absolute())
    cmake_env = {**os.environ}
    cmake_env['ZEPHYR_BASE'] = str(args.zephyr_dir.absolute())
    # cmake_env['GLLVM_OBJCOPY'] = str(args.llvm_objcopy_program.absolute())
    # cmake_env['GLLVM_LD'] = str(args.llvm_ld_program.absolute())
    # cmake_env['WLLVM_OUTPUT_LEVEL'] = 'DEBUG'
    cmake_cmd = [
        args.cmake_program,
        '-S', str(args.app_dir.absolute()), '-GNinja'
    ] + ['-D' + x for x in args.cmake_args]
    run('Executing CMake', cmake_cmd, cwd=args.cmake_builddir, env=cmake_env)

    ninja_cmd = [args.ninja_program, args.target, '--verbose']
    run('Executing Ninja', ninja_cmd, cwd=args.cmake_builddir, env=cmake_env)

    image = args.cmake_builddir / args.target
    assert image.is_file()

    shutil.copyfile(image, args.output.absolute())
    # get_bc_cmd = [args.get_bc_program, '-o', args.output.absolute(), image]
    # run('Executing get-bc', get_bc_cmd, cwd=args.cmake_builddir, env=cmake_env)


if __name__ == '__main__':
    main()


# from argparse import ArgumentParser
# import os
# import shutil
# import importlib
# import sys
#
# parser = ArgumentParser()
# parser.add_argument('--objcopy', type=str)
# parser.add_argument('--objdump', type=str)
# parser.add_argument('--ranlib', type=str)
# parser.add_argument('--ld', type=str)
# parser.add_argument('--ar', type=str)
# parser.add_argument('--size', type=str)
# parser.add_argument('--nm', type=str)
# parser.add_argument('--source_dir', type=str)
# parser.add_argument('--build_dir', type=str)
# parser.add_argument('--zephyr_root', type=str)
# parser.add_argument('--board', type=str)
# parser.add_argument('--libgcc', type=str)
# parser.add_argument('--cc', type=str)
# parser.add_argument('--llc', type=str)
# parser.add_argument('--llvm_link', type=str)
# parser.add_argument('name', metavar='N', type=str)
#
# args = parser.parse_args()
#
# from util import KConfigFile
#
# # Clear the build dir if it exits and we can't prove that the existing build is for the same board.
# # This is required by the zephyr build system. Otherwise cmake runs into caching issues.
# # TODO: Investigate how ninja clean handles zephyr
# same_board = False
# try:
#     if os.path.exists(args.build_dir):
#         config = KConfigFile(os.path.join(args.build_dir, 'zephyr/.config'))
#         same_board = config['CONFIG_BOARD'] == args.board
#         if not same_board:
#             print('Board might have changed, regenerating cmake...')
#             shutil.rmtree(args.build_dir)
#         else:
#             print('Board has not changed, skipping cmake...')
#
# except FileNotFoundError:
#     print('Could not detect board, regenerating...')
#     shutil.rmtree(args.build_dir)
# try:
#     os.mkdir(args.build_dir)
# except FileExistsError:
#     pass
#
# # NOTE: Whenever any of these flags change, regenerate manually
# if not same_board:
#     cmake_call = 'cmake -G Ninja'
#     cmake_call += ' -D BOARD=' + args.board
#     cmake_call += ' -D ZEPHYR_TOOLCHAIN_VARIANT=llvm'
#     cmake_call += ' -D ZEPHYR_BASE=' + args.zephyr_root
#     cmake_call += ' -D TOOLCHAIN_ROOT=' + args.zephyr_root
#     cmake_call += ' -D CMAKE_C_COMPILER=' + args.cc
#     cmake_call += ' -D CMAKE_CXX_COMPILER=' + args.cc
#     cmake_call += ' -D CMAKE_LLC=' + args.llc
#     cmake_call += ' -D CMAKE_LLVM_LINK=' + args.llvm_link
#     # Normally cmake *should* find them by itself, but this seems to be the only way to make it work
#     # reliably
#     cmake_call += ' -D CMAKE_OBJCOPY=' + args.objcopy
#     cmake_call += ' -D CMAKE_OBJDUMP=' + args.objdump
#     cmake_call += ' -D CMAKE_NM=' + args.nm
#     cmake_call += ' -D CMAKE_AR=' + args.ar
#     # Override the linker that clang uses for the exe/elf. Clang defaults to lld which is incompatible
#     # with the generated linker scripts. Note that setting CMAKE_LINKER will NOT work since clang
#     # ignores that.
#     cmake_call += ' -D CMAKE_EXE_LINKER_FLAGS="-fuse-ld=' + args.ld + ' -L ' + args.libgcc + '"' #+ ' -v"'
#     # Disable common warnings that gcc does not emit but clang does. Typedef redefinition will be fixed
#     # in https://github.com/zephyrproject-rtos/zephyr/pull/29359
#     cmake_call += ' -D EXTRA_CFLAGS="-Wno-typedef-redefinition -Wno-unused-command-line-argument"'
#     cmake_call += ' -S ' + args.source_dir
#     cmake_call += ' -B ' + args.build_dir
#
#     # Generate cmake files
#     # Fail with an exeception so that meson/ninja knows something went wrong.
#     ret = os.system(cmake_call)
#     if ret != 0:
#         raise RuntimeError("Cmake failed!")
#
# # Invoke ninja to build
# ret = os.system('ninja -C ' + args.build_dir)
# if ret != 0:
#     raise RuntimeError("Ninja failed!")
# # Copy the lib.ll and .config to the top level for ease of use
# try:
#     shutil.copyfile(os.path.join(args.build_dir, 'app/libapp.ll'), os.path.join(args.build_dir,
#         '..', args.name + '.ll'))
#     shutil.copyfile(os.path.join(args.build_dir, 'zephyr/.config'), os.path.join(args.build_dir,
#         '..', args.name + '.config'))
# except FileNotFoundError:
#     pass
