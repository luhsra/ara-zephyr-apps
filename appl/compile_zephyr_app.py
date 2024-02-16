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


if __name__ == '__main__':
    main()
