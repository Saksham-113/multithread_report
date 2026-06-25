Question-2

A. Process states
-The possible states of an xv6 process are UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, and ZOMBIE.
-Right after fork() returns in the child, the process is in the RUNNABLE state.
-Between exit() and wait(), the process is in the ZOMBIE state.

B. The boot process
-The initialization steps executed in main(), in order, are: console, printf, memory allocation, kernel page table, hart page table, process table, traps, hart traps, PLIC, hart PLIC, disk buffers, inodes, files, virtio disk, and the first user process.
-The console subsystem is initialized first. The first user process (userinit) is initialized last.
-The scheduler() function runs an infinite loop where it finds a RUNNABLE process, switches context to run it, and waits until the process yields the CPU.

C. The system call table
-xv6 currently supports 21 standard system calls.
-The indices of the syscalls array exactly match the system call numbers defined in syscall.h.
-If a user program uses an invalid system call number, the kernel prints an error message and returns -1.

D. The shell
-To run a command, the shell calls fork() to create a child process. The child process calls exec() to run the command, while the parent process calls wait() to wait for the child to finish.
-When you type ls | cat, the shell creates a pipe and forks two child processes. The left child redirects its standard output to the write end of the pipe and executes ls. The right child redirects its standard input to the read end of the pipe and executes cat.