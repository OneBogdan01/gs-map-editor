# https://github.com/godotengine/godot-cpp-template/blob/main/SConstruct
#!/usr/bin/env python
import os
import sys

from methods import print_error

# From https://github.com/nikoladevelops/godot-blast-bullets-2d/blob/main/SConstruct
# Function to recursively find .cpp files in the given directories
def find_sources(dirs, exts):
    """
    Recursively searches the specified directories for .cpp files.
    
    Args:
        dirs (list): List of directory paths to search.
        exts (list): List of file extensions that are acceptable and should contain C++ code.
    Returns:
        list: List of full paths to .cpp files found.
    """
    sources = []
    for dir in dirs:
        for root, _, files in os.walk(dir):
            for file in files:
                if any(file.endswith(ext) for ext in exts):
                    sources.append(os.path.join(root, file))
    return sources
libname = "map_editor"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Add('source_dirs', 'List of source directories (comma-separated)', 'src/source') # Directory for source files
opts.Add('source_exts', 'List of source file extensions (comma-separated)', '.cpp,.c,.cc,.cxx') 
opts.Add('include_dirs', 'List of include directories (comma-separated)', 'src/include') # Directory for headers - some might want to create a separate include directory
opts.Add('doc_output_dir', 'Directory for documentation output', 'gen')
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

source_dirs = env['source_dirs'].split(',')   # Convert comma-separated string to list
source_exts = env['source_exts'].split(',')   # Convert comma-separated string to list
include_dirs = env['include_dirs'].split(',') # Convert comma-separated string to list
doc_output_dir = env['doc_output_dir']        # Directory for documentation output

env.Append(CPPPATH=include_dirs)

sources = find_sources(source_dirs, source_exts)
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)