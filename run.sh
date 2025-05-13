#!/bin/sh
#
# Use this script to run your program LOCALLY.
#
# Note: Changing this script WILL NOT affect how CodeCrafters runs your program.
#
# Learn more: https://codecrafters.io/program-interface

set -e # Exit early if any commands fail

# Ensure compile steps are run within the repository directory
cd "$(dirname "$0")"

# Check for macOS architecture (Silicon or Intel)
if [ "$(uname -m)" = "arm64" ]; then
  # For Apple Silicon Macs
  READLINE_INCLUDE_PATH="/opt/homebrew/Cellar/readline/8.2.13/include"
  READLINE_LIB_PATH="/opt/homebrew/Cellar/readline/8.2.13/lib"
else
  # For Intel Macs or other systems (e.g., Linux)
  READLINE_INCLUDE_PATH="/usr/local/include"
  READLINE_LIB_PATH="/usr/local/lib"
fi

# Compile the program with appropriate paths for readline
gcc -o /tmp/shell-target app/*.c \
    -I$READLINE_INCLUDE_PATH \
    -L$READLINE_LIB_PATH -lreadline

# Run the compiled program
exec /tmp/shell-target "$@"