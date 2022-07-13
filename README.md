# PA 2017

This is the code of the programming assignment for the course *Introduction to Computer Systems* in 2017 in NJU. This project implements a x86 emulator (NEMU).

## NEMU

NEMU (NJU Emulator) is a simple but complete full-system x86 emulator designed for teaching purpose. Small x86 programs can run under NEMU.

The main features of NEMU include

- a small monitor with a simple debugger
  - single step
  - register/memory examination
  - expression evaluation without the support of symbols
  - watch point
  - differential testing with QEMU
- CPU core with support of most common used x86 instructions in protected mode
  - real mode is not supported
  - x87 floating point instructions are not supported
- DRAM
- I386 paging with TLB
  - protection is not supported
- I386 interrupt and exception
  - protection is not supported
- 4 devices
  - serial, timer, keyboard, VGA
  - most of them are simplified and unprogrammable
- 2 types of I/O
  - port-mapped I/O and memory-mapped I/O

## OS setup

Debian 32-bit with desktop environment (tested with MATE)

## Guide book

[](https://nju-ics.gitbooks.io/ics2017-programming-assignment/content/)