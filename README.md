# Concurrency & Synchronization in C

Two classic synchronization problems implemented in C: **inter-process** synchronization with shared memory and POSIX semaphores, and **inter-thread** synchronization with pthreads, a mutex, and condition variables around a bounded queue.

| Part | Directory | Concurrency model | Primitives |
|------|-----------|-------------------|------------|
| 1 — Bank account simulation | `PART1/` | Multiple processes (`fork`) | System V shared memory, named POSIX semaphore |
| 2 — Restaurant simulation | `BENSCHILLIBOWL/` | Multiple threads (`pthread`) | Mutex + two condition variables (monitor pattern) |

---

## Part 1 — Shared Memory & Semaphores (`PART1/`)

A producer/consumer-style simulation of the **"Dear Old Dad & Poor Student"** problem:

- A single integer in **System V shared memory** (`shmget`/`shmat`) represents a shared bank account.
- The parent forks **Dear Old Dad**, who periodically checks the balance and deposits $0–100 when it drops below $100.
- One or more **Poor Student** processes randomly check the balance or withdraw $0–50 — but only if the funds are there.
- An optional **Lovable Mom** process deposits $0–125 whenever the balance is at or below $100.
- Every read-modify-write of the account is protected by a **named POSIX semaphore** (`sem_open`/`sem_wait`/`sem_post`), so concurrent processes never corrupt the balance.
- Randomized sleeps interleave the processes so you can watch the semaphore doing its job in the output.

Also included: `example.c`, a minimal warm-up demo of two processes sharing an `mmap`-backed counter guarded by a semaphore.

### Build & run

```bash
cd PART1
make                # builds shm_proc
./shm_proc 1 1      # Dad + 1 student
./shm_proc 2 10     # Dad + Mom + 10 students

make example        # optional: the minimal semaphore demo
./example
```

The simulation runs until you stop it with `Ctrl+C`.

Sample output:

```
Poor Student: Attempting to Check Balance
Poor Student needs $29
Poor Student: Not Enough Cash ($18)
Dear old Dad: Deposits $28 / Balance = $46
Poor Student: Withdraws $29 / Balance = $17
Lovable Mom: Deposits $124 / Balance = $141
```

---

## Part 2 — Threads, Mutex & Condition Variables (`BENSCHILLIBOWL/`)

A multithreaded simulation of **Ben's Chili Bowl**: customer threads place orders, cook threads fulfill them, and a bounded queue sits in between.

- **10 customer threads** each pick a random menu item and add an order to the queue.
- **3 cook threads** pull orders off the queue and fulfill them until every expected order is done.
- The restaurant is a **monitor**: one `pthread_mutex_t` protects all shared state, and two condition variables coordinate the threads:
  - `can_add_orders` — customers wait here while the queue is full (capacity 5)
  - `can_get_orders` — cooks wait here while the queue is empty
- Orders live in a **linked-list FIFO queue**. When the final order is taken, waiting cooks are woken with a broadcast so every thread exits cleanly and the program terminates on its own.

### Build & run

```bash
cd BENSCHILLIBOWL
make
./main
```

Sample output:

```
Restaurant is open!
Customer 1 is placing order
Cook 2 fulfilled order 1 from customer 1 (Half Smoke)
...
Cook 3 fulfilled order 10 from customer 9 (Ben's Chili Burger)
Restaurant is closed!
```

---

## Requirements

- `gcc` and `make`
- A POSIX system — developed for Linux; also builds and runs on macOS
