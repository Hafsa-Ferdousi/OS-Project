
# Exam Room Simulation

This project simulates an exam management system using processes, threads, semaphores, mutexes, and condition variables in C.
It assigns students to exam rooms, controls exam timing, tracks attendance, and enforces room capacity limits.

## Features
**Student Allocation:** Students are evenly assigned to rooms based on capacity.

**Room Capacity Control:** Each room has a maximum capacity enforced by semaphores.

**Exam Start & End Synchronization:** All students start together after the exam controller signals.

**Attendance Tracking:** Thread-safe attendance counters track how many students entered each room.

### Concurrency Handling

**Semaphores** → Control student entry and synchronize exam start.

**Mutex Locks** → Protect shared attendance data.

**Condition Variables** → Signal when the exam is over.



