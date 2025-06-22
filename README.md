# ToDo-List-CLI

A CLI-based to-do list application implemented in **C++17** for NYCU’s Object-Oriented Programming and Data Structures course (HW1). It provides complete task management features, including sorting, filtering, deadline alerts, undo/redo, and password protection.

---

## Design Highlight: Pointer-Based Architecture

This project was intentionally written with **raw pointers (`string*`, `int*`, `bool*`)** to deepen understanding of C++ memory management and object-oriented design. All class attributes, constructors, destructors, getters/setters, and internal logic use explicit pointer allocation and deallocation (`new` / `delete`).

This design choice emphasizes:

* Manual memory allocation and deallocation
* Deep copy semantics (copy constructors and `clone()` methods)
* Destructor correctness and pointer safety
* Explicit ownership and lifetime management

> ⚠️ *This is for educational purposes. In real-world applications, smart pointers (e.g., `std::unique_ptr`) or STL containers would be preferred.*

---

## Features

| Feature             | Description                                                                         |
| ------------------- | ----------------------------------------------------------------------------------- |
| Add / Edit / Delete | Interactive task creation and modification with default values and input validation |
| Sorting / Filtering | Sort and filter tasks by deadline, importance, today’s tasks, and overdue status    |
| Deadline Alerts     | Highlight overdue tasks using ANSI colors                                           |
| Importance Display  | Represent task importance with 1–5 stars                                            |
| Undo / Redo         | Maintain two-layer snapshots for undo and redo operations                           |
| Password Protection | Require password setup on first run and verification on subsequent runs             |
| Pointer-Based OOP   | All class member variables and logic are implemented using raw pointers             |

---

## Build & Run

To compile and run the application:

```bash
# Requires g++ ≥ 7.0 (C++17 support)
g++ -std=c++17 -O2 -Wall -o todo todo_list.cpp
./todo
```

> On first execution, the program will prompt to set a password and generate a `password.txt` file.

---

## File Structure

```
.
├── todo_list.cpp     # Main application source file
├── README.md         # Project overview and instructions
└── report.pdf        # Full project report (PDF)
```

---

## Report

The complete HW1 project report is available here:

* [report.pdf](./report.pdf)

---

## Author

| Name            | Affiliation / ID   | Course              |
| --------------- | ------------------ | ------------------- |
| 李品翰 (Henry Lee) | NYCU CS, 113550193 | OOPDS (Spring 2025) |

---

*This repository contains academic homework code and documentation for learning purposes.*
