# 🍕 Online Food Delivery Route Updates — Linked List

**ITM Skills University | B.Tech CSE 2025-29 | DSA with CPP | Semester II**
**Case Study 170**

---

## 👤 Student Details

| Field | Details |
|-------|---------|
| **Name** | Anant Dhoundiyal |
| **Roll No.** | 150096725170 |
| **Batch** | Sam Altman |
| **Course** | Data Structure & Algorithms with CPP |
| **Semester** | II |

---

## 📌 Project Title

**Online Food Delivery Route Updates using Linked List**
A dynamic route management system simulating Swiggy / Zomato delivery operations.

---

## 📋 Problem Statement

Food delivery companies such as Swiggy and Zomato process thousands of orders every minute. Delivery agents continuously receive new orders while existing orders may be canceled or modified. Traffic congestion and road closures further require route adjustments.

The objective is to develop a **dynamic route management system** for food delivery platforms that can efficiently update delivery routes based on new orders, cancellations, and traffic conditions.

---

## 🎯 Objectives

- Build a dynamic delivery route using a **Singly Linked List**
- Support real-time **insertion** of new orders (end, middle, priority front)
- Support **cancellation** (deletion) of orders from any position
- Implement a **Traffic Monitoring Module** to assign and detect traffic levels
- Implement a **Route Optimization Engine** for ETA recalculation and rerouting
- Provide a **Delivery Agent Application** (interactive menu-driven CLI)
- Analyze **time and space complexity** of all operations

---

## 🏗️ System Overview / Architecture

The system is composed of **5 integrated modules**:

```
┌─────────────────────────────────────────────────────┐
│              FOOD DELIVERY ROUTE SYSTEM              │
├─────────────────┬──────────────┬────────────────────┤
│  Module 1       │  Module 2    │  Module 3          │
│  Customer Order │  Route       │  Traffic           │
│  Management     │  Linked List │  Monitoring        │
├─────────────────┴──────────────┴────────────────────┤
│  Module 4: Route Optimization Engine                 │
├──────────────────────────────────────────────────────┤
│  Module 5: Delivery Agent Application (CLI Menu)     │
└──────────────────────────────────────────────────────┘
```

### Module Descriptions

| Module | Description |
|--------|-------------|
| **Customer Order Management** | Stores order details: ID, customer name, address, phone, distance, ETA, status |
| **Route Linked List** | Core singly linked list of `RouteNode`; supports insert, delete, search, reverse, display |
| **Traffic Monitoring** | Assigns `LOW / MODERATE / HIGH / BLOCKED` traffic levels; auto-scan or manual set |
| **Route Optimization Engine** | Recalculates ETAs based on traffic; reroutes blocked stops; generates efficiency reports |
| **Delivery Agent Application** | Interactive CLI tying all modules together with sub-menus |

---

## 📊 Data Structures and Algorithms Used

### Core Data Structure: Singly Linked List

```
[Restaurant] → [Customer A] → [Customer C] → [Customer B] → NULL
     head          node           node            node
```

### Key Structs

```cpp
struct Order {
    int orderId;
    string customerName, address, phone;
    float distanceKm;
    int estimatedMins;      // 4 min/km base rate
    OrderStatus status;     // PENDING / IN_ROUTE / DELIVERED / CANCELLED
    bool isRestaurant;
};

struct RouteNode {
    Order data;
    TrafficLevel traffic;   // LOW / MODERATE / HIGH / BLOCKED
    RouteNode* next;
};
```

### Enums

```cpp
enum OrderStatus  { PENDING, IN_ROUTE, DELIVERED, CANCELLED };
enum TrafficLevel { LOW, MODERATE, HIGH, BLOCKED };
```

---

## ⚙️ Implementation Approach

### Route Operations

| Operation | Method | Logic |
|-----------|--------|-------|
| Insert at End | `insertAtEnd()` | Traverse to tail, append new node |
| Insert After ID | `insertAfter()` | Find target node, rewire pointers |
| Priority Insert (Front) | `priorityInsert()` | Insert immediately after restaurant node — O(1) |
| Cancel Order | `cancelOrder()` | Find & unlink node, free memory |
| Mark Delivered | `markDelivered()` | Linear scan, update status field |
| Search | `searchOrder()` | Linear scan, print details and position |
| Display Route | `displayRoute()` | Full traversal, formatted table output |
| Reverse Route | `reverseRoute()` | Three-pointer in-place reversal |

### Traffic Monitoring

- **Auto Scan**: Uses `rand()` seeded with `time(0)` to simulate real-time traffic changes
- **Manual Set**: Sets a specific stop's traffic level by order ID
- **Congestion Report**: Counts stops by traffic level

### Route Optimization

- **Recalculate ETAs**: Applies traffic multipliers (LOW=1×, MODERATE=1.5×, HIGH=2.5×, BLOCKED=5×)
- **Reroute Blocked**: Marks blocked stops as `CANCELLED`, adjusts distances
- **Efficiency Score**: `(non-blocked stops / total stops) × 100`

### Demo Scenario (auto-runs at startup)

```
Step 1: Load initial route — Restaurant → Customer A → Customer B
Step 2: New nearby order from Customer C inserted after Customer A
Step 3: Traffic scan (auto)
Step 4: Customer B cancels
Step 5: Recalculate ETAs
→ Interactive CLI session begins
```

---

## ⏱️ Time and Space Complexity Analysis

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|-----------------|-------|
| Insert Node (End) | O(n) | O(1) | Traverse to tail |
| Insert After (Mid) | O(n) | O(1) | Find target node |
| Priority Insert (Front) | **O(1)** | O(1) | Update head.next directly |
| Delete Node | O(n) | O(1) | Find & unlink node |
| Search Order | O(n) | O(1) | Linear scan |
| Mark Delivered | O(n) | O(1) | Linear scan |
| Reverse Route | O(n) | O(1) | Three-pointer swap |
| Recalculate ETAs | O(n) | O(1) | Single traversal |
| Reroute Blocked | O(n²) | O(1) | Rebuild on each block |
| Display Route | O(n) | O(1) | Single traversal |

**Overall Space Complexity**: O(n) for the linked list storing n delivery stops.

**Why Linked List over Array?**
- Dynamic insertion/deletion without shifting elements
- O(1) priority insert (insert after head)
- No pre-allocation needed — ideal for unpredictable order volumes

---

## 🚀 Execution Steps

### Prerequisites

- C++ compiler (g++ / clang++)
- Terminal / command prompt

### Compile

```bash
g++ food_delivery_route.cpp -o food_delivery_route
```

### Run

```bash
./food_delivery_route
```

### On macOS (from project directory)

```bash
cd "/path/to/DSA Project/" && g++ food_delivery_route.cpp -o food_delivery_route && ./food_delivery_route
```

---

## 📥 Sample Inputs and Outputs

### Adding an Order (Option 1 → 1)

```
Choice: 1
=== ORDER MANAGEMENT ===
Choice: 1
Order ID     : 34
Customer Name: anant
Address      : seawoods
Phone        : 77777
Distance(km) : 4
[+] Order #34 — anant added to route.
```

### Traffic Monitoring (Option 2 → 1)

```
Choice: 2
=== TRAFFIC MONITORING ===
Choice: 1
[TRAFFIC SCAN]
No traffic changes detected.
```

### Performance Summary Output

```
Total delivery stops : 3
Total route distance : 12.7 km
Low traffic stops    : 3
Moderate stops       : 0
High traffic stops   : 0
Blocked stops        : 0
Route efficiency     : 100.0 / 100
```

---

## 📸 Screenshots

### Screenshot 1 — Order Management & Traffic Monitoring

<img width="1372" height="892" alt="image" src="https://github.com/user-attachments/assets/e73d142e-1c5a-4d94-b689-4eb3f98f0937" />


*Shows Order Management menu, adding Order #34 for customer "anant", and Traffic Scan returning no changes.*

### Screenshot 2 — Main Menu & Performance Summary

<img width="1372" height="892" alt="image" src="https://github.com/user-attachments/assets/8883a287-2ed6-4279-8277-c008a1163f29" />


*Shows the main FOOD DELIVERY ROUTE SYSTEM menu, initial route loaded with 3 stops, 12.7 km total, 100% efficiency.*

---

## 📈 Results and Observations

1. **Linked List** proved efficient for dynamic order management — insertions and deletions are pointer operations with no data shifting.
2. **Priority Insert** achieves O(1) by always inserting right after the restaurant node (the fixed head), making urgent orders immediately next in queue.
3. **Traffic simulation** using randomized levels realistically models delivery disruptions.
4. **ETA recalculation** using multipliers (1×, 1.5×, 2.5×, 5×) provides meaningful delivery time estimates under varying traffic conditions.
5. The interactive CLI covers all required DSA operations: insert (3 variants), delete, search, traverse, and reverse.

---

## 🏁 Conclusion

This project successfully implements a **dynamic food delivery route management system** using a **Singly Linked List** in C++. The system demonstrates:

- Efficient O(1) priority insertion for urgent orders
- Flexible O(n) insertion, deletion, and search operations
- Real-world applicability to platforms like Swiggy and Zomato
- A complete 5-module architecture covering order management, traffic monitoring, route optimization, and an agent-facing CLI

The linked list data structure is well-suited for this use case due to its dynamic nature, enabling route updates in real time without costly memory reallocation.

---

## 🔗 GitHub Repository

https://github.com/anantd16/Food-delivery-route

*ITM Skills University | B.Tech CSE 2025-29 | Data Structure & Algorithms with CPP | Semester II*
