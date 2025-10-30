#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

def format_shaders():
    # Find all .gdshader and .glsl files
    shader_files = list(Path('.').rglob('*.gdshader')) + list(Path('.').rglob('*.glsl'))
    
    if not shader_files:
        print("No shader files found.")
        return
    
    print(f"Found {len(shader_files)} shader file(s) to format...")
    
    for file in shader_files:
        print(f"Formatting: {file}")
        try:
            subprocess.run(['clang-format', '-i', str(file)], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error formatting {file}: {e}", file=sys.stderr)
        except FileNotFoundError:
            print("Error: clang-format not found. Please install it first.", file=sys.stderr)
            sys.exit(1)
    
    print("Formatting complete!")

if __name__ == "__main__":
    format_shaders()