#!/usr/bin/env python3

import os
import sys
import shutil
import subprocess
import argparse

# Constants
BUILD_DIR = "build"
DEBUG_DIR = os.path.join(BUILD_DIR, "debug")
RELEASE_DIR = os.path.join(BUILD_DIR, "release")

def run_command(cmd, cwd=None):
  try:
    print(f"\n>>> Running: {' '.join(cmd)}")
    subprocess.check_call(cmd, cwd=cwd)
  except subprocess.CalledProcessError:
    print("Command failed.")
    exit(1)

def configure(preset):
  run_command(["cmake", "--preset", preset])

def build(preset):
  run_command(["cmake", "--build", "--preset", preset])

def clean():
  for path in [DEBUG_DIR, RELEASE_DIR]:
    if os.path.exists(path):
      print(f"Removing {path}...")
      shutil.rmtree(path)
  print("Cleaned all build directories.")

def run(preset):
  build_dir = DEBUG_DIR if preset == "debug" else RELEASE_DIR
  exe_path = os.path.join(build_dir, "bin", "FallingSandSim")

  if os.name == "nt":
    exe_path += ".exe"

  if not os.path.isfile(exe_path):
    print(f"Executable not found: {exe_path}")
    print("Try building first with 'python build.py debug' or 'python build.py release'")
    sys.exit(1)

  print(f"Running executable: {exe_path}\n")
  # Use shell=True on Windows for proper .exe execution
  subprocess.run(exe_path, shell=(os.name=="nt"))

def main():
  parser = argparse.ArgumentParser(description="Build and manage your CMake project.")
  parser.add_argument("action", choices=["build-debug", "build-release", "run-debug", "run-release", "clean"], help="Action to perform")
  args = parser.parse_args()

  if args.action == "clean":
    clean()
  elif args.action == "build-debug":
    configure("debug")
    build("debug")
  elif args.action == "build-release":
    configure("release")
    build("release")
  elif args.action == "run-debug":
    run("debug")
  elif args.action == "run-release":
    run("release")

if __name__ == "__main__":
  main()
