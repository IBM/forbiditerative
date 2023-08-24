import os
import shutil
import sys
from pathlib import Path
from setuptools import Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.build_py import build_py as build_py
import multiprocessing

try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel


    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            """we have executable binaries"""
            _bdist_wheel.finalize_options(self)
            self.root_is_pure = False

        def get_tag(self):
            """build for specific ABI and platform"""
            python, abi, plat = _bdist_wheel.get_tag(self)
            return python, abi, plat
except ImportError:
    bdist_wheel = None


class CMakeExtension(Extension):
    def __init__(self, name):
        # don't invoke the original build_ext for this special extension
        super().__init__(name, sources=[])


class BuildPy(build_py):
    def run(self):
        self.run_command("build_ext")
        return super().run()

    def initialize_options(self):
        super().initialize_options()
        if self.distribution.ext_modules is None:
            self.distribution.ext_modules = [CMakeExtension('src')]


class BuildCMakeExt(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, ext):
        cwd = Path().absolute()

        # these dirs will be created in build_py, so if you don't have
        # any python sources to bundle, the dirs will be missing
        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)

        config = 'Debug' if self.debug else 'Release'
        num_cpus = 4
        if os.name == "posix":
            try:
                num_cpus = multiprocessing.cpu_count()
            except NotImplementedError:
                pass
            CMAKE_GENERATOR = "Unix Makefiles"
        elif os.name == "nt":
            CMAKE_GENERATOR = "MinGW Makefiles"
        else:
            print("Unsupported OS: " + os.name)
            sys.exit(1)

        cmake_args = [
            '-G', CMAKE_GENERATOR,
            f'-DCMAKE_BUILD_TYPE={config}',
            Path(ext.name).absolute()  # src directory path
        ]

        build_args = [
            '--config', config,
            '--', f'-j{num_cpus}'
        ]

        os.chdir(str(build_temp))
        self.spawn(['cmake', str(cwd)] + cmake_args)
        if not self.dry_run:
            self.spawn(['cmake', '--build', '.'] + build_args)
        # Troubleshooting: if fail on line above then delete all possible
        # temporary CMake files including "CMakeCache.txt" in top level dir.
        os.chdir(str(cwd))
        shutil.copytree(build_temp / 'bin', Path(self.build_lib) / 'forbiditerative' / 'builds' / 'release' / 'bin', dirs_exist_ok=True)
