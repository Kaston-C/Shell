#!/bin/sh
#
# This script is used to compile your program on CodeCrafters
# 
# This runs before .codecrafters/run.sh
#
# Learn more: https://codecrafters.io/program-interface

# Exit early if any commands fail
set -e

gcc -o /tmp/shell-target app/*.c \
      -I/opt/homebrew/Cellar/readline/8.2.13/include \
      -L/opt/homebrew/Cellar/readline/8.2.13/lib -lreadline