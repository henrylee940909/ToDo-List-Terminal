# ToDo-List-Terminal

A terminal-based to-do list application implemented in **C++17** for NYCU’s Object-Oriented Programming and Data Structures course (HW1). It provides complete task management features, including sorting, filtering, deadline alerts, undo/redo, and password protection.

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
└── todo_list.cpp     # Main application source file
```

---

## Author

| Name            | Affiliation / ID   | Course              |
| --------------- | ------------------ | ------------------- |
| 李品翰 (Henry Lee) | NYCU CS, 113550193 | OOPDS (Spring 2025) |

---

*This repository contains academic homework code for learning purposes.*
