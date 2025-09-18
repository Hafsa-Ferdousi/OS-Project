
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

### Output

<img width="312" height="232" alt="Screenshot 2025-09-01 231200" src="https://github.com/user-attachments/assets/19d4c3c8-a7c9-4c75-84ae-8a7ff914223e" />

<img width="312" height="232" alt="Screenshot 2025-09-01 231316" src="https://github.com/user-attachments/assets/b835e47b-0d2d-4bd5-90bd-cf2029657c0a" />

<img width="312" height="232" alt="Screenshot 2025-09-01 231404" src="https://github.com/user-attachments/assets/ceb6518a-a2dd-41a4-817c-c3dc41222107" />







