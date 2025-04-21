# 🔍 GTK Port Scanner

A simple multithreaded TCP port scanner with a GUI built using **GTK** in C. This tool allows you to scan a range of ports on a target IP address and displays which ports are open, along with their service names (if available).

---

## 🖼️ Features

- GUI built using **GTK 3**
- Multithreaded scanning for faster results
- Displays **open ports** and their **service names**
- Custom IP and port range input
- Scrollable output window

---

## 🛠️ Requirements

Make sure you have the following dependencies installed on your Linux system:

```bash
sudo apt update
sudo apt install libgtk-3-dev libpthread-stubs0-dev build-essential

TO COMPILE
gcc port_scanner.c -o port_scanner `pkg-config --cflags --libs gtk+-3.0` -lpthread

TO RUN
./port_scanner


