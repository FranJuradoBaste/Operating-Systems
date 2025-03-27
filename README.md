🐚 Scripter – Custom Shell Script Interpreter in C

Developed by: [Your Name]
Course: Operating Systems – Lab 2
University: Universidad Carlos III de Madrid
Academic Year: 2024/2025

---

📌 Project Overview

Scripter is a personal project I developed as part of the Operating Systems course. It is a lightweight shell script interpreter written in C for Linux systems, capable of executing scripts that contain UNIX-like commands, including support for redirection, piping, and background execution.

This project helped me deeply understand process management in POSIX systems and how real-world shells interact with the operating system using low-level system calls such as fork, execvp, pipe, and dup2.

---

🛠️ Key Features
	•	✅ Command execution from a script file
	•	⛓️ Piped command sequences (e.g., ls -l | wc)
	•	📁 Input (<), output (>), and error (!>) redirection
	•	🧵 Background process support (&)
	•	🔍 Custom mygrep command (grep-like functionality)
	•	🔒 Proper error handling and system call validation
	•	📜 Designed to follow strict Linux shell conventions

---

📁 Repository Structure

```bash
.
├── ES_SSOO_p2_scripter_diapositivas_24-25.pdf      # Slides (Spanish)
├── p2_scripter                                     # Main source code
│   ├── EN_OS_p2_scripter_statement_24-25.pdf       # Statement (English)
│   ├── ES_SSOO_p2_scripter_enunciado_24-25.pdf     # Statement (Spanish)
│   ├── Makefile                                     # Build system
│   ├── mygrep.c                                     # Custom grep implementation
│   └── scripter.c                                   # Script interpreter
└── ssoo_p2_tester                                  # Test suite
    ├── InputScripts/
    ├── README-ENG.md
    ├── README-ESP.md
    └── tester.sh
```
---
🚀 Compilation

In the p2_scripter directory, use:
```bash
make            # Compiles everything
make scripter   # Builds the main interpreter
make mygrep     # Builds the external command
make clean      # Cleans all binaries
```
---

▶️ Usage

To run the interpreter:
```bash
./scripter <script_file>
```
⚠️ The script file must start with the line: ## Script de SSOO
If this header is missing or an empty line is found, the program will terminate with an error.

---
🔍 About mygrep

This is a custom-made grep-like command I implemented. It searches for a string inside a text file and prints the matching lines.

Usage:
```bash
./mygrep <file_path> <string_to_search>
```
If the string is not found, the output will be:
```bash
"<string>" not found.
```
If there’s an error, it is printed using perror().

---
🧪 Testing

I included a test suite (ssoo_p2_tester/) with different Bash and Scripter input scripts. You can run them using:
```bash
cd ssoo_p2_tester
./tester.sh
```
More details are provided in the README-ENG.md and README-ESP.md inside the tester folder.

---
💬 About Me

I created this project to explore low-level systems programming and understand how real shell environments work under the hood. It was a challenging but highly rewarding experience.

If you have questions or want to collaborate, feel free to reach out!

---
📄 License

This project was created for educational purposes and is submitted as part of my coursework at UC3M. Please do not reuse without proper attribution.







