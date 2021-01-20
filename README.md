# OS-related

In this repository are some fundamentals related to operating systems that i have implemented.

## 1. CPU scheduler

- This section includes basic scheduling algorithms that can be used to schedules tasks that need to be done by the CPU. 
- Scheduling algorithms such as round-robin, priority-round-robin and shortest time remaining. 
- These algorithms were implemented by having a scheduler process that can prempt a resource( i.e dummy CPU) using a semaphore to block other processes from interrupting the CPU.
- Each task was given a timeslice and swapped out from the CPU if the task runs its entire allocated timeslice.

## 2. Calculating the digits of PI using threads and processes

- This program purpose was to use multithreading to calculate the decimal digits of PI and trying different configurations of threads and processes to understand the performance differences between using several process VS several threads to calculate the digits of PI.
- COnducting experiments and analysing results was the main goal of this program to better understand the trade-offs related to choosing threads over processes to complete the same task.

## 3. FAT32 file reader

- The main goal of this program was to learn the file system layout of windows FAT32. How the bytes of data are stored and how they can be accessed or modified by an operating system.
- The program can be tested with any image file, further enhansments need to be made to make it compatible with other types of FAT32 storage media e.g USB drives. 

## 4. Man in the middle attack

- This program was a test to understand how process and threads communicate with each other using signals and how the signals can be manipulated to create a "Man in the middle attack".

- More than one version of the program needs to be run on the same PC for this to work.
