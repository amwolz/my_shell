# My Shell

A miniature Linux shell that implements job control with builtin commands and signal handling.

## What This Does

**my_shell** is a simple user interface to the Linux OS. Much of the behavior is designed to emulate the style and syntax of the Ubuntu terminal. **my_shell** provides similar capabilities to Ubuntu with a simplified interface.

The skill implements a job control system with similar syntax. It checks whether the command is builtin and forks it to a child process if not. Builtin commands are run at the same depth as the shell process while the forked non-builtins are managed via the parent shell process. A child process may have its own child processes, which are added to the child-parent process group. This program can also manage zombie and orphan processes. A zombie process is an exited child proccess with an active parent, and an orphan process is an active child process with an exited parent. The two processes are handled differently. The zombie process is reaped upon completion, whereas the orphan process is reparented to **my_shell** to maintain control of it. Only the CTRL-D EOF condition can terminate an orphan process. The handler walks the process tree to terminate every child of the parent shell. The orphan process otherwise runs until exit because its parent has exited.

In addition to job control, **my_shell** is set up to handle signals. It handles foreground interuption signals and program exit signals through the ISO C99 standard signal handling library. These signals are handled by associated handler functions. The SIGHCHLD handler implements the job control and can also be called through the kill and stop builtin commands. Signal blocking is used when a child process is forked to prevent a race condition.

## Key Features

- **Job Control** — Maintains an array of active jobs that are accessible with the builtin command `my_shell> listjobs`. An active job can have multiple states - Running, Stopped, Terminated, Done.
- **Builtin Commands** — Acccepts several builtin commands. A list of these commands can be found in the help menu `my_shell> help`.
- **Signal Handling** — Listens for and handles program quit, child exit, terminate and stop signals with set up listeners.

## Requirements

### Supported Platforms

- Linux

### Dependencies

- GCC
- GNU Make
- GNU readline

## Installation

```bash
sudo apt update
sudo apt install build-essential libreadline-dev
git clone https://github.com/amwol/Maze-Solver.git
cd Maze-Solver
make
```

## Usage

### Interactive

From the root of this repo, run `./bin/main/main` to initialize **my_shell**. Run files and enter commands to learn the shell.

The test files **/bin/test/wait**, **/bin/test/wait_print**, **/bin/test/wait_fork_zombies**, and **/bin/test/wait_fork/orphans** are provided and demonstrate the behavior of the shell.

### In test

From the root of this repo, run `./driver.sh <int testfile_number>` to run a testfile. The user can create new .txt testfiles in /src/test.

## Output Example

![alt text](images/image1.png)

### Case Description

3 jobs are run in the background, followed by subsequent `kill` job 1 and `stop` job 2 commands. The `kill` command logging is delayed by one command, and the `stop` command works as intended. Another background job is run and is displayed in the next command through `listjobs`. The stopped command is then continued in the background and is also shown with a subsequent `listjobs` command.

## Files

| File | Purpose |
|------|---------|
| `/src/main/builtins.c` | Builtin commands for my_shell |
| `/src/main/executor.c` | Command execution |
| `/src/main/jobs.c` | Job control helpers |
| `/src/main/main.c` | Program entry and readline |
| `/src/main/parser.c` | Command parsing |
| `/src/main/signals.c` | Signal setup and handling |

## License

MIT — Use it, modify it, share it.
