"""
obfuscator
Description: A basic python script to aid in the obfuscation of c and c++ source code files
Authors: Sam "Alice" Blair, Winston Howard, Chance Sweetser
Created Date: 05/04/20
"""

import os
import re
import random
import string
import argparse
import shutil


DEFAULT_INPUT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "configloader"))
DEFAULT_OUTPUT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "out"))


def variable_renamer(given_string):
    """
    Function to rename all variables and fuctions. 
    given_string is a string of C/C++ code
    """
    return variable_renamer_with_dictionary(given_string, None)


def build_variable_dictionary(given_string, variable_dictionary):
    """
    Collect rename candidates from a string and extend the supplied dictionary.
    """
    special_cases = {"typedef", "unsigned", "const", "struct", "bool", "void", "char", "int", "size_t", "FILE"}

    renamable_lines = []
    in_preprocessor_block = False
    for line in given_string.splitlines(keepends=True):
        if in_preprocessor_block or line.lstrip().startswith("#"):
            in_preprocessor_block = line.rstrip().endswith("\\")
            continue

        renamable_lines.append(line)

    filtered_code = []
    declaration_pattern = re.compile(
        r"^\s*(?:[a-zA-Z_][a-zA-Z0-9_]*\s+|[a-zA-Z_][a-zA-Z0-9_]*\s*\*+\s*)+(?!main\b)([a-zA-Z_][a-zA-Z0-9_]*)\s*(?=\(|=|;|\[|\{)"
    )

    skip_prefixes = (
        "return ",
        "if ",
        "if(",
        "for ",
        "for(",
        "while ",
        "while(",
        "switch ",
        "switch(",
        "case ",
        "else",
        "do",
    )

    for line in renamable_lines:
        stripped = line.lstrip()
        if stripped.startswith(skip_prefixes):
            continue

        matches = declaration_pattern.findall(line)
        filtered_code.extend(matches)

    for found_example in filtered_code:

        if(found_example.isupper()):
            continue

        if(found_example not in special_cases):

            if(found_example not in variable_dictionary):

                variable_dictionary[found_example] = random_string(12)

    return variable_dictionary


def variable_renamer_with_dictionary(given_string, variable_dictionary):
    """
    Rename variables and functions using a supplied dictionary or build a new one.
    given_string is a string of C/C++ code
    """

    # Variable declarations:
    if variable_dictionary is None:
        variable_dictionary = {}

        build_variable_dictionary(given_string, variable_dictionary)
    index = 0
    new_string = ""

    # For each line, keep preprocessor directives intact and only rename identifiers in code.
    in_preprocessor_block = False
    for line in given_string.splitlines(keepends=True):
        if in_preprocessor_block or line.lstrip().startswith("#"):
            in_preprocessor_block = line.rstrip().endswith("\\")
            new_string += line
            continue

        split_code = re.split('"', line)
        index = 0

        # For each even section in split code (odd indicates that it is in a string)
        # replace all of the variable and function names with what is defined in the dictionary
        for section in split_code:

                if(index%2==0):
                    for entry in variable_dictionary:

                        # Replace identifiers only when they are not part of a larger token.
                        re_string = r"(?<![a-zA-Z0-9_]){}(?![a-zA-Z0-9_])".format(re.escape(entry))

                        # While loop to go through every entry and replace it
                        # Breaks when it cannot find another instance
                        def replace_identifier(match):
                            prefix = section[:match.start()]
                            if prefix.endswith("struct ") or prefix.endswith("enum ") or prefix.endswith("union "):
                                return match.group(0)
                            return variable_dictionary[entry]

                        section = re.sub(re_string, replace_identifier, section)

                # Add the current section back to make the original string but with obfuscated names
                # Accounts for adding a quote everytime except for the first scenario
                if(index >= 1):
                    new_string = new_string + "\"" + section
                else:
                    new_string = new_string + section

                index+=1
    
    # Return the obfuscated code
    return new_string


def random_string(stringLength=8):
    """
    Function to generate a random string.
    Can pass it an integer string length to make it that size else it will be 8
    """
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for i in range(stringLength))



def whitespace_remover(a):
    """
    Function to remove all whitespace, except for after functions, variables, and imports
    """
    splits = re.split('\"',a)
    code_string = r"((\w+\s+)[a-zA-Z_*][|a-zA-Z0-9_]*|#.*|return [a-zA-Z0-9_]*|\[\.\]|else)"
    index = 0
    a = ""
    for s in splits:
            # If its not the contents of a string, remove spaces of everything but code
            if(index%2==0):                
              s_spaceless = re.sub(r"[\s]", "", s)          # Create a spaceless version of s
              s_code = re.findall(code_string,s)           # find all spaced code blocks in s

              for code in s_code:
               old = re.sub(r"[\s]", "", code[0])
               new = code[0]

               if(code[0][0] == '#'):
                 new = code[0] + "\n"                      # Adding a newline for preprocesser commands
               elif("unsigned" in code[0] or "else" in code[0]):
                 new = code[0] + " "
               s_spaceless = s_spaceless.replace(old,new) # Replace the spaceless code blocks in s with their spaced equivilents                
            else:
              s_spaceless = s

            if(index >= 1):
             a = a + "\"" + s_spaceless
            else:
             a = a + s_spaceless
            index+=1
    return a

def comment_remover(given_string):
    """
    Function to (currently) remove C++ style comments 
    given_string is a string of C/C++ code
    """

    #This does not take into account if a C++ style comment happens within a string
    # i.e. "Normal String // With a C++ comment embedded inside"
    # This is a barebones start for C style block comments
    # Current issue is it is only single line C style comments
    # It also finds C style comments in strings
    given_string = re.sub(r"/\*.*?\*/", "", given_string, flags=re.S)

    cpp_filtered_code = re.findall(
        r"\/\/.*", given_string)
    for entry in cpp_filtered_code:
        given_string = given_string.replace(entry, "")
    
    return given_string


def rewrite_local_includes(given_string, filename_map):
    """
    Rewrite quoted local includes to the obfuscated output names.
    """
    def replace_include(match):
        include_name = match.group(2)
        return match.group(1) + filename_map.get(include_name, include_name) + match.group(3)

    return re.sub(r'(?m)^([ \t]*#include\s+")([^"]+)(")', replace_include, given_string)


def build_output_name_map(source_files):
    """
    Build randomized output names while keeping .c/.h pairs on the same base name.
    """
    stem_map = {}
    output_name_map = {}

    for filename in source_files:
        stem, extension = os.path.splitext(filename)
        if stem not in stem_map:
            stem_map[stem] = random_string(12)
        output_name_map[filename] = stem_map[stem] + extension

    return output_name_map


def clear_output_directory(output_dir):
    """
    Remove all files and folders from the output directory.
    """
    if not os.path.exists(output_dir):
        return

    if not os.path.isdir(output_dir):
        raise NotADirectoryError("Output path exists but is not a directory: {}".format(output_dir))

    for entry in os.listdir(output_dir):
        entry_path = os.path.join(output_dir, entry)
        if os.path.isdir(entry_path):
            shutil.rmtree(entry_path)
        else:
            os.remove(entry_path)


def copy_makefile_if_present(input_dir, output_dir):
    """
    Copy Makefile from input directory into output directory when available.
    """
    source_makefile = os.path.join(input_dir, "Makefile")
    if os.path.isfile(source_makefile):
        shutil.copy2(source_makefile, os.path.join(output_dir, "Makefile"))


def parse_args():
    """
    Parse command line arguments for source and output directories.
    """
    parser = argparse.ArgumentParser(description="Obfuscate C/C++ source and header files.")
    parser.add_argument(
        "-i",
        "--input-dir",
        default=DEFAULT_INPUT_DIR,
        help="Directory containing .c/.h files (default: obfuscator directory)",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        default=DEFAULT_OUTPUT_DIR,
        help="Directory to write obfuscated files (default: ./out)",
    )
    return parser.parse_args()

def main():
    """
    The main function to begin the obfuscation of c code files
    """
    args = parse_args()
    input_dir = os.path.abspath(args.input_dir)
    output_dir = os.path.abspath(args.output_dir)

    if not os.path.isdir(input_dir):
        raise FileNotFoundError("Input directory does not exist: {}".format(input_dir))

    print("Looking for C Source Files in {}...".format(input_dir))

    clear_output_directory(output_dir)
    os.makedirs(output_dir, exist_ok=True)
    copy_makefile_if_present(input_dir, output_dir)

    print("Log: ")
    source_files = sorted([filename for filename in os.listdir(input_dir) if filename.endswith((".c", ".h"))])
    obfuscated_names = build_output_name_map(source_files)

    file_contents = {}
    for filename in source_files:
        with open(os.path.join(input_dir, filename)) as file_data:
            file_string = file_data.read()
            file_contents[filename] = comment_remover(file_string)

    shared_dictionary = {}
    for file_string in file_contents.values():
        build_variable_dictionary(file_string, shared_dictionary)

    for filename in os.listdir(input_dir):
        print("\n {} : \r".format(filename))
        if(filename in file_contents):
            print("PASS\n")
            file_string = variable_renamer_with_dictionary(file_contents[filename], shared_dictionary)
            file_string = rewrite_local_includes(file_string, obfuscated_names)
            output_path = os.path.join(output_dir, obfuscated_names[filename])
            with open(output_path, "w+") as f:
                f.write(file_string)
            print(file_string)

        else:
            print("FAIL")


if __name__ == "__main__":
    main()