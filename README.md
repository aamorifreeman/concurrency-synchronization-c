# Project 2 – Synchronization

This project contains two parts focused on inter-process and thread-level synchronization using shared memory, semaphores, mutexes, and condition variables.

---

## 🔹 Part 1 — IPC with Shared Memory & Semaphores

Simulates the classic **Dear Old Dad & Poor Student** problem:

- A shared integer represents a bank account.
- Parent and child processes deposit/withdraw money.
- Access to shared memory is controlled using **POSIX semaphores**.
- Random timing ensures realistic concurrency.
- Optional extension adds **Lovable Mom** and multiple student processes.

### How to Run
```bash
cd PART1
make
./psdd 1 1      # 1 parent, 1 child
./psdd 2 10     # Mom + Dad + 10 students
```
# Part 2 — Thread Synchronization (BENSCHILLIBOWL)

This part of the project simulates customers placing orders at Ben’s Chili Bowl and cooks fulfilling them.  
The system uses **POSIX threads**, a **mutex**, and **two condition variables** to safely manage a bounded queue of orders.

---

## 🧵 Overview

- **Customers (threads)** create and submit orders.
- **Cooks (threads)** retrieve and fulfill orders.
- The restaurant is represented as a **monitor**:
  - `pthread_mutex_t` — protects all shared state
  - `pthread_cond_t can_add_orders` — wait when the queue is full
  - `pthread_cond_t can_get_orders` — wait when the queue is empty
- Orders are stored in a **linked-list queue** inside `BENSCHILLIBOWL`.

The program ends once all expected orders have been fulfilled.

---

## How to Run Part 2 (BENSCHILLIBOWL)

1. Navigate into the project folder:
```bash
cd BENSCHILLIBOWL
make
./main
```


