# 🏫 Campus Scheduling System

A C++-based Campus Scheduling System developed as part of the **CS200 – Introduction to Programming** course. This project simulates the scheduling of university courses, professors, classrooms, and time slots while demonstrating object-oriented programming, modular software design, and algorithmic problem solving.

---

## 📖 Overview

The Campus Scheduling System automates the process of generating and managing class schedules while respecting scheduling constraints such as classroom availability, professor assignments, and time conflicts.

The project focuses on building a structured and maintainable scheduling engine using object-oriented design principles.

---

## ✨ Features

- 📅 Automatic course scheduling
- 👨‍🏫 Professor assignment
- 🏛️ Classroom allocation
- ⏰ Time slot management
- 🚫 Conflict detection and prevention
- 📊 Schedule generation
- 🔄 Schedule validation
- 🧩 Modular architecture

---

## 🏗️ System Architecture

```
                 Input Data
                      │
                      ▼
            Schedule Loader Module
                      │
                      ▼
             Schedule State Manager
                      │
                      ▼
              Scheduling Engine
                      │
                      ▼
             Conflict Resolution
                      │
                      ▼
             Final Schedule Output
```

---

## 📂 Project Structure

```
Campus-Scheduling-System/
│
├── data/
|
├── include/
│
├── src/
│
├── tests/
│
├── Makefile
└── README.md
```

---

## ⚙️ Building the Project

This project was developed and tested inside a standardized Linux development environment using **Docker**.

From PowerShell:

```powershell
# Navigate to the project directory
cd "path\to\Campus-Scheduling-System"

# Launch the university-provided Docker environment
docker run -it --rm -v "${PWD}:/home/..." <university-provided-container>
```

Inside the container:

```bash
cd /home/...
```

Common Makefile commands:

```bash
make            # Build the project
make test       # Run the complete test suite
make clean      # Remove generated files
```

---

## 🧪 Testing

The project includes automated tests covering:

- Schedule loading
- Schedule state management
- Scheduling engine
- Score evaluation
- End-to-end integration testing

The implementation was verified using the university-provided grading framework to ensure correctness across all required modules.

<p align="center">
  <img src="assets/grader result.png" width="750">
</p>

---

## 🧠 Concepts Demonstrated

- Object-Oriented Programming
- Classes and Objects
- Inheritance
- Encapsulation
- File Handling
- STL Containers
- Algorithm Design
- Constraint Validation
- Modular Programming

---

## 📚 Academic Note

This project was completed as a **semester programming assignment** for **CS200 – Introduction to Programming**.

To ensure a consistent development and grading environment across all students, the project was developed and tested inside a **Docker container provided by the university**. The container standardized the compiler, libraries, build tools, and testing environment used throughout the assignment.

The source code in this repository represents my implementation of the project requirements.

---

## 🔒 Privacy Notice

To protect the privacy of faculty members, the **names of professors included in the original assignment dataset have been modified** in this repository. These changes do not affect the functionality or behavior of the scheduling system and were made solely for privacy purposes.

---

## 👩‍💻 Author

**Hamnah Sultan**

---

## ⭐ Support the Project

If you found this project interesting or helpful, consider giving the repository a ⭐.

---

## 📄 License

This repository is shared for educational and portfolio purposes.

Please do not copy or submit this work as your own for academic credit.
