# Persistent Key-Value Store (WAL + Crash Recovery)

A **single-process persistent key-value store** implemented from scratch with **Write-Ahead Logging (WAL)** and **crash-safe recovery**.

This project focuses on **storage fundamentals**: durability, atomicity, crash consistency, and recovery semantics—without relying on external databases or frameworks.

---

## 🚀 Goals

- Implement a minimal yet **correct persistent storage engine**
- Understand and demonstrate **WAL invariants**
- Explore **crash recovery** and data corruption handling
- Build systems intuition relevant to **infra / distributed systems roles**

---

## 🧠 Design Overview

### Architecture
- **Single process**
- **Append-only WAL**
- **In-memory index**
- **Local filesystem persistence**

Client API  
   │  
   ▼  
In-Memory KV Map  
   │  
   ▼  
Write-Ahead Log (append-only)  
   │  
   ▼  
Disk (fsync for durability)  


---

## 🔐 Durability Model

This store follows a **redo-only WAL protocol**:

1. **Append mutation to WAL**
2. **fsync WAL**
3. **Apply mutation to in-memory state**

> If a key-value update is visible in memory, it is guaranteed to exist in the WAL on disk.

---

## 💥 Crash Recovery

On startup:
1. Open the WAL
2. Sequentially scan records
3. Stop at the first invalid or corrupted entry
4. Replay valid entries into memory

Partial writes, torn records, and crashes during writes are safely handled.

---

## 📁 On-Disk Format

Each WAL record is length-prefixed and self-contained:  
[ total_length ]  
[ key_length   ]  
[ key_bytes    ]  
[ value_length ]  
[ value_bytes  ]  
[ checksum     ]  


- Append-only writes
- Checksum detects corruption
- Invalid records terminate recovery replay

---

## 🧩 Features

- `put(key, value)` — durable write
- `get(key)` — in-memory read
- Crash-safe persistence
- Corruption detection via checksums
- Deterministic recovery

---

## 🛠️ Implementation Notes

- Language: **C++**
- File I/O via POSIX syscalls
- Explicit `fsync()` for durability
- In-memory index backed by `unordered_map`

---

## 🧪 Testing Strategy

- Manual crash testing (SIGKILL mid-write)
- Restart and verify data correctness
- WAL corruption simulation
- Recovery idempotence verification

---

## 📈 Future Extensions

- Snapshotting + WAL truncation
- Group commit / batching
- Background fsync thread
- mmap-based reads
- Benchmarks (latency vs throughput)
- Multi-process safety

---

## 📚 Motivation

This project exists to deeply understand:
- Why databases use WAL
- What durability *actually* means
- How crashes break naïve storage systems
- The tradeoffs behind real systems like SQLite, RocksDB, and Postgres
