# OS-RiscV-kernel

This project implements a small, functional operating system kernel that supports multithreading and time-sharing. The kernel includes memory allocation, thread management, semaphores, and preemption via timer and keyboard interrupts. It is developed for the RISC-V architecture and runs in a virtual environment.

Features
Multithreading: Supports creation and management of lightweight processes (threads).
Time-sharing: Implements time-sharing mechanisms for process scheduling.
Memory Allocation: Custom memory allocator for dynamic memory management.
Preemption: Context switching and preemption triggered by timer and keyboard interrupts.
Semaphores: Provides synchronization for concurrent threads.
Requirements
RISC-V Emulator: The kernel is designed to run on a RISC-V architecture.
C/C++ Compiler: Code is written in C/C++ for ease of integration with the kernel.
Assembler: RISC-V assembly for low-level hardware interaction.
