# Educational OS Kernel for the 8086

A small **preemptive multitasking kernel with time sharing for the Intel 8086**, written in **C++** with inline assembly. It was built as a university project for the *Operating Systems 1* course at the School of Electrical Engineering, University of Belgrade (ETF).

The kernel gives user programs a thread abstraction and the primitives to coordinate them (semaphores, events and signals), and it handles the interrupts that make multitasking work. The code uses Borland C++ extensions (`interrupt` functions, `<dos.h>`, inline `asm`).

## Features

- **Threads**: a `Thread` class with `start()`, `waitToComplete()`, thread ids and lookup by id. User threads subclass `Thread` and override `run()`.
- **Context switching and context preservation**: the kernel saves the stack pointer and stack segment of the running thread, picks the next ready thread, and restores its context. Threads can also give up the processor explicitly with `dispatch()`.
- **Preemption with time sharing**: every thread has a time slice, counted in timer ticks of about 55 ms (default: 2 ticks). A slice of 0 means the thread is never preempted.
- **Idle thread**: runs when no other thread is ready.
- **Interrupt handling**: the kernel installs its own timer interrupt routine and chains to the original one, so the system clock keeps working.
- **Semaphores**: `wait(maxTimeToWait)` with an optional timeout, `signal(n)` that wakes up to `n` threads and returns how many were woken, and `val()`.
- **Events**: an `Event` is bound to an interrupt vector entry. When the interrupt fires, the waiting thread is woken. The `PREPAREENTRY` macro installs the interrupt routine and can optionally call the previous one.
- **Thread signals**: 16 signals per thread, with several handlers per signal (register, unregister all, swap), blocking and unblocking per thread and globally. Signal 0 kills the thread by default. Signals 1 and 2 are raised automatically when a thread finishes: signal 1 for its parent thread and signal 2 for the thread itself.

## How it works

- **Timer takeover**: `set_new_timer()` replaces the timer interrupt (vector 08h) with the kernel's routine and keeps the original one at vector 60h. The kernel calls it on every tick, and `restore_previous_timer()` puts everything back when the program ends.
- **Thread creation**: each thread gets its own stack with a prepared initial frame. It starts in a wrapper that calls `run()`, then marks the thread finished, raises its signals, wakes the threads waiting for it, and dispatches.
- **The main thread** is wrapped in a thread object too, so it can use semaphores, events and signals like any other thread.
- **Scheduling**: on each tick the kernel decrements the running thread's remaining slice. When it reaches zero (or on an explicit `dispatch()`), the running thread is put back into the scheduler and the next one is taken from it. If nothing is ready, the idle thread runs.
- **Semaphore blocking**: each semaphore keeps two queues of blocked threads, one for threads that wait with a timeout and one for those that wait indefinitely. The timed waiters are counted down on every tick, and `wait` returns 0 on timeout and 1 when the thread was woken by a `signal`. The kernel's critical sections disable interrupts (`cli`/`sti`).
- **Waiting for a thread**: `waitToComplete()` blocks on a per-thread semaphore that is signaled when the thread finishes.
- **Signal delivery**: signals sent to a thread are queued in its control block. When the thread is picked to run again, the kernel runs the handlers of the queued signals, unless they are blocked.

## Project structure

```
.
├── h/     # thread.h, semaphor.h, event.h (user-facing API)
│          # pcb.h, KSem.h, KEvent.h, IVTEntry.h (kernel internals)
└── src/   # thread.cpp (threads, timer, dispatch, signals), pcb.cpp, KSem.cpp,
           # semaphor.cpp, KEvent.cpp, event.cpp, IVTEntry.cpp, hello.cpp (entry point)
```

## Build and run

> **Status:** the kernel was developed, tested and defended in a virtual machine during the course. That environment is no longer available, so the code has not been rebuilt since.

The kernel depends on two things that come with the course and are **not** included in this repository:
- the scheduler library (`SCHEDULE.h`), which provides `Scheduler::put` and `Scheduler::get`
- a user test program that defines `int userMain(int argc, char** argv)`, which `main` in `hello.cpp` calls after installing the kernel's timer routine

## Notes

This is a course project built for learning purposes, not a production kernel.

## Author

**Adriana Vidić**: software engineering student at ETF and programming teacher.
[GitHub](https://github.com/adriiance)
