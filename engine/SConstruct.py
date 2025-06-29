APPLICATION_NAME = "van_engine"

import os
import sys
import re
import json
from datetime import datetime
from termcolor import colored
os.system("clear")

TIME = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")

build_print   = lambda msg: print(f"[{colored(APPLICATION_NAME, "magenta")}]", msg)
build_warning = lambda msg: build_print(f"[{colored("WARNING", "yellow")}] {msg}")
gray          = lambda text: colored(text, "grey")
barrier       = lambda : print(f"{colored("--------------------", "red")}")

build_print(f"Build started at {TIME}")

def getTargetType():
    if ("-rel" in COMMAND_LINE_TARGETS or "--release" in COMMAND_LINE_TARGETS):
        return "release"
    elif ("-dbg" in COMMAND_LINE_TARGETS or "--debug" in COMMAND_LINE_TARGETS):
        return "debug"
    else:
        build_warning(f"No build target type specified.")
        return "debug"

# TARGET = getTargetType()
TARGET = "debug"
build_print(f"Building for '{gray(TARGET)}'")

with open("../package.json") as package_json:
    package_json_data = json.load(package_json)
if ("name" not in package_json_data):
    package_json_data["name"] = "default_project_name"
print(package_json_data["name"])
BUILD_PATH = f"../build/{re.sub(r"\s", "_", package_json_data["name"].strip().lower())}"
SOURCE = [
    'src/app.cpp',
    'src/base_rw.cpp',
    'src/cache.cpp',
    'src/config.cpp',
    'src/custom.cpp',
    'src/html.cpp',
    'src/main.cpp',
    'src/webview_binds.cpp',
    'src/webview_info.cpp',
    'src/webview.cpp'
]


compile_time_flags = [#https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
    # '--verbose', 
    '-Wextra', 
    '-Werror',
    '-Wno-missing-braces',
    '-Wcast-qual',
    '-Wpointer-arith',
    '-Wunused', 
    '-Wshadow', 
    '-Wall', 
    '-Werror',
    # '-DWEBVIEW_GTK=1',
    '-O0' if TARGET == "debug" else '-Ofast'
]

header_search_path = 'include/'

libs = [
    'dl',
    'spdlog'
]
#g++ main.cc -O2 --std=c++11 -Ilibs $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1) -ldl -o example

# if os.path.exists(BUILD_PATH):
#     os.rename(BUILD_PATH, f"build/{TARGET}/backups/{TIME}")
#     build_print(f"Backed up previous build at timestamp\n\t'{gray(TIME)}'")
barrier()
env = Environment(CCFLAGS=compile_time_flags, CPPPATH=header_search_path, LIBS=libs)
env['ENV']['TERM'] = os.environ['TERM']
env.Append(COMPILATIONDB_USE_ABSPATH=True)
env.Tool('compilation_db')
cdb = env.CompilationDatabase('compile_database.json')
Alias('cdb', cdb)
env.CompilationDatabase()
env.ParseConfig("pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1")
Default(env.Program(target=BUILD_PATH, source=SOURCE))
env.Program(target=BUILD_PATH, source=SOURCE)

if "run" in COMMAND_LINE_TARGETS:
    test = env.Command(target="testoutput",
                    source=f"./{BUILD_PATH}",
                    action="./{BUILD_PATH} > $TARGET")
    env.Depends(test, main)
