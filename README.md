# Custom Unix Shell 🐚

A lightweight, Unix-like command-line shell built in C as part of the [CodeCrafters](https://www.codecrafters.io/) Unix Shell challenge. This shell provides a seamless and user-friendly command-line experience, featuring built-in commands, robust input parsing, file redirection, history management, and autocompletion. 🚀

## Features 🌟

* **Built-in Commands** 💻

  Supports common shell commands, including:

   * `cd` (Change directories)
   * `echo` (Display messages)
   * `exit` (Quit the shell)
   * and more!

* **External Command Execution** 🏃‍♂️

  Executes standard Unix commands by forking and managing child processes.

* **Input Parsing** 🔍

  Smart handling of:

   * Quoted strings ("text")
   * Escape sequences (e.g., `\n`, `\t`)
   * Arbitrary whitespace
   * Edge cases in user input (no command left behind!)

* **File Redirection** 📂

  Supports output (`>`), and append (`>>`) redirection.

* **Piping Support** 🔗

  Enables command chaining using the pipe operator (|). Output from one command can be seamlessly passed as input to the next.

* **Command History** ⏳

  Tracks your executed commands and allows you to easily recall and reuse them. No more typing the same thing twice!

* **Autocompletion** ⌨️

  Tab-based autocompletion for commands, file paths, and more for an enhanced user experience.

## About 👨‍💻

This project was built as part of the [CodeCrafters](https://www.codecrafters.io/) Unix Shell challenge. The goal? To implement a shell from scratch in C and dive deep into system-level programming. While certain parts could be optimized using existing libraries, I decided to implement them myself for the maximum learning experience. 💡

## Installation ⚙️

1. Clone the repository:

   ```bash
   git clone https://github.com/Kaston-C/shell.git
   cd shell
   ```

2. Install dependencies (make sure you have `gcc` or another C compiler installed):

   * **On Debian-based systems (Ubuntu, etc.):**

     ```bash
     sudo apt-get install libreadline-dev
     ```
   * **On macOS (using Homebrew):**

     ```bash
     brew install readline
     ```

3. Run the shell (compiles and runs the shell):

   ```bash
   ./start.sh
   ```

## Usage 🎮

Once the shell is running, you can:

* Execute built-in or external commands
* Use redirection like `ls > files.txt` to capture output
* Recall previous commands with up/down arrows
* Use Tab for autocompletion of file/command names (no more typos!)

### Example 📝

```bash
$ echo "Hello, Shell!"
Hello, Shell!

$ ls -la > out.txt

$ cat out.txt | grep ".c" | wc
```

## Dependencies 📦

* C Standard Library (stdlib.h, stdio.h, string.h, unistd.h, fcntl.h, sys/wait.h, etc.)
* Readline Library (readline/readline.h and readline/history.h)

   * Installation steps above
* POSIX-compliant system (Linux, macOS)

## License 📄

[MIT License](LICENSE)