# Custom Unix Shell

A lightweight, Unix-like command-line shell built in C as part of the [CodeCrafters](https://www.codecrafters.io/) Unix Shell challenge. This shell supports a range of built-in commands, robust input parsing, file redirection, history management, and autocompletion for a user-friendly command-line experience.

## Features

- **Built-in Commands**  
  Supports common shell commands such as:
  - `cd`
  - `echo`
  - `exit`
  - and more

- **External Command Execution**  
  Executes standard Unix commands by forking and managing child processes.

- **Input Parsing**  
  Handles:
  - Quoted strings
  - Escape sequences
  - Arbitrary whitespace
  - Edge cases in user input

- **File Redirection**  
  Supports input (`<`), output (`>`), and append (`>>`) redirection.

- **Command History**  
  Keeps a history of executed commands with support for recall and reuse.

- **Autocompletion**  
  Tab-based autocompletion for improved usability.

## About

This project was built as part of the [CodeCrafters](https://www.codecrafters.io/) Unix Shell challenge. The goal was to implement a shell from scratch in C, gaining a deeper understanding of system-level programming and how Unix tools work under the hood. While certain parts could be optimized using existing libraries, I chose to implement them myself to maximize my learning experience.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Kaston-C/shell.git
   cd shell
   ```

2. Install dependencies (make sure you have `gcc` or another C compiler installed):
   - **On Debian-based systems (Ubuntu, etc.):**
     ```bash
     sudo apt-get install libreadline-dev
     ```
   - **On macOS (using Homebrew):**
     ```bash
     brew install readline
     ```

3. Run the shell (this will compile the code and start the shell):
   ```bash
   ./run.sh
   ```

## Usage

Once the shell is running, you can:

- Execute built-in or external commands
- Use redirection like `ls > files.txt`
- Recall previous commands with up/down arrows
- Use tab for autocompletion of file/command names

### Example

```bash
$ echo "Hello, Shell!"
Hello, Shell!

$ cd /home/user

$ ls -la > out.txt

$ cat out.txt
```

## Dependencies

- C Standard Library (stdlib.h, stdio.h, string.h, unistd.h, fcntl.h, sys/wait.h, etc.)
- Readline Library (readline/readline.h and readline/history.h)
   - Installation steps above
- POSIX-compliant system (Linux, macOS)

## License

[MIT License](LICENSE)