#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
using namespace std;

// ================================================================
//  CONSTANTS & ENUMS
// ================================================================
enum OrderStatus  { PENDING, IN_ROUTE, DELIVERED, CANCELLED };
enum TrafficLevel { LOW, MODERATE, HIGH, BLOCKED };

string statusStr(OrderStatus s) {
    switch(s) {
        case PENDING:   return "Pending";
        case IN_ROUTE:  return "In Route";
        case DELIVERED: return "Delivered";
        case CANCELLED: return "Cancelled";
    }
    return "Unknown";
}

string trafficStr(TrafficLevel t) {
    switch(t) {
        case LOW:      return "Low      ";
        case MODERATE: return "Moderate ";
        case HIGH:     return "High     ";
        case BLOCKED:  return "BLOCKED  ";
    }
    return "Unknown  ";
}

// ================================================================
//  MODULE 1 — CUSTOMER ORDER MANAGEMENT
//  Stores all order details per node
// ================================================================
struct Order {
    int         orderId;
    string      customerName;
    string      address;
    string      phone;
    float       distanceKm;      // distance from previous stop
    int         estimatedMins;   // ETA in minutes
    OrderStatus status;
    bool        isRestaurant;

    Order(int id, string name, string addr, string ph,
          float dist = 1.0f, bool restaurant = false)
        : orderId(id), customerName(name), address(addr),
          phone(ph), distanceKm(dist),
          estimatedMins((int)(dist * 4)),   // 4 min/km base rate
          status(restaurant ? DELIVERED : PENDING),
          isRestaurant(restaurant) {}
};

// ================================================================
//  MODULE 2 — ROUTE LINKED LIST
//  Core data structure: singly linked list of delivery stops
// ================================================================
struct RouteNode {
    Order      data;
    TrafficLevel traffic;   // traffic at THIS stop's road segment
    RouteNode* next;

    RouteNode(Order o, TrafficLevel t = LOW)
        : data(o), traffic(t), next(nullptr) {}
};

class DeliveryRoute {
private:
    RouteNode* head;
    int        totalStops;      // excludes restaurant
    int        deliveredCount;
    float      totalDistanceKm;

    void printLine(char c = '-', int w = 72) const {
        cout << string(w, c) << "\n";
    }

public:
    DeliveryRoute()
        : head(nullptr), totalStops(0),
          deliveredCount(0), totalDistanceKm(0.0f) {}

    ~DeliveryRoute() {
        RouteNode* curr = head;
        while (curr) {
            RouteNode* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
    }

    // ── Getters ────────────────────────────────────────────
    int   getStopCount()      const { return totalStops; }
    int   getDeliveredCount() const { return deliveredCount; }
    float getTotalDistance()  const { return totalDistanceKm; }
    RouteNode* getHead()      const { return head; }

    // ── A. Insert at END — O(n) ─────────────────────────────
    void insertAtEnd(Order o, TrafficLevel t = LOW) {
        RouteNode* node = new RouteNode(o, t);
        totalDistanceKm += o.distanceKm;
        if (!o.isRestaurant) totalStops++;

        if (!head) { head = node; return; }
        RouteNode* curr = head;
        while (curr->next) curr = curr->next;
        curr->next = node;

        if (!o.isRestaurant)
            cout << "  [+] Order #" << o.orderId << " — " << o.customerName
                 << " added to route.\n";
    }

    // ── B. Insert AFTER orderId — O(n) ─────────────────────
    bool insertAfter(int afterId, Order o, TrafficLevel t = LOW) {
        RouteNode* curr = head;
        while (curr && curr->data.orderId != afterId)
            curr = curr->next;

        if (!curr) {
            cout << "  [!] Order #" << afterId << " not found. Appending.\n";
            insertAtEnd(o, t);
            return false;
        }

        RouteNode* node = new RouteNode(o, t);
        node->next  = curr->next;
        curr->next  = node;
        totalStops++;
        totalDistanceKm += o.distanceKm;

        cout << "  [+] Order #" << o.orderId << " — " << o.customerName
             << " inserted after Order #" << afterId << ".\n";
        return true;
    }

    // ── C. Priority Insert (right after restaurant) — O(1) ─
    void priorityInsert(Order o, TrafficLevel t = LOW) {
        RouteNode* node = new RouteNode(o, t);
        totalStops++;
        totalDistanceKm += o.distanceKm;

        if (!head) { head = node; return; }
        if (head->data.isRestaurant) {
            node->next = head->next;
            head->next = node;
        } else {
            node->next = head;
            head       = node;
        }
        cout << "  [!] PRIORITY: Order #" << o.orderId << " — "
             << o.customerName << " inserted at front of queue.\n";
    }

    // ── D. Delete / Cancel — O(n) ──────────────────────────
    bool cancelOrder(int orderId) {
        if (!head) { cout << "  [!] Route is empty.\n"; return false; }

        if (head->data.orderId == orderId) {
            if (head->data.isRestaurant) {
                cout << "  [!] Cannot remove the restaurant node.\n";
                return false;
            }
            RouteNode* tmp = head;
            head = head->next;
            totalDistanceKm -= tmp->data.distanceKm;
            totalStops--;
            cout << "  [-] Cancelled: " << tmp->data.customerName
                 << " (Order #" << orderId << ").\n";
            delete tmp;
            return true;
        }

        RouteNode* prev = head;
        RouteNode* curr = head->next;
        while (curr && curr->data.orderId != orderId) {
            prev = curr; curr = curr->next;
        }
        if (!curr) {
            cout << "  [!] Order #" << orderId << " not found.\n";
            return false;
        }
        if (curr->data.isRestaurant) {
            cout << "  [!] Cannot remove the restaurant node.\n";
            return false;
        }
        prev->next = curr->next;
        totalDistanceKm -= curr->data.distanceKm;
        totalStops--;
        cout << "  [-] Cancelled: " << curr->data.customerName
             << " (Order #" << orderId << ").\n";
        delete curr;
        return true;
    }

    // ── E. Mark order as DELIVERED — O(n) ──────────────────
    bool markDelivered(int orderId) {
        RouteNode* curr = head;
        while (curr) {
            if (curr->data.orderId == orderId) {
                if (curr->data.status == DELIVERED) {
                    cout << "  [!] Already marked delivered.\n";
                    return false;
                }
                curr->data.status = DELIVERED;
                deliveredCount++;
                cout << "  [✓] Order #" << orderId << " — "
                     << curr->data.customerName << " DELIVERED!\n";
                return true;
            }
            curr = curr->next;
        }
        cout << "  [!] Order #" << orderId << " not found.\n";
        return false;
    }

    // ── F. Search — O(n) ───────────────────────────────────
    bool searchOrder(int orderId) const {
        RouteNode* curr = head;
        int pos = 0;
        while (curr) {
            if (curr->data.orderId == orderId) {
                printLine();
                cout << "  ORDER FOUND\n";
                printLine();
                cout << left
                     << "  Order ID   : " << curr->data.orderId       << "\n"
                     << "  Customer   : " << curr->data.customerName  << "\n"
                     << "  Address    : " << curr->data.address        << "\n"
                     << "  Phone      : " << curr->data.phone          << "\n"
                     << "  Distance   : " << curr->data.distanceKm << " km\n"
                     << "  ETA        : " << curr->data.estimatedMins << " min\n"
                     << "  Status     : " << statusStr(curr->data.status) << "\n"
                     << "  Traffic    : " << trafficStr(curr->traffic)    << "\n"
                     << "  Stop #     : " << pos                          << "\n";
                printLine();
                return true;
            }
            curr = curr->next;
            pos++;
        }
        cout << "  [!] Order #" << orderId << " not found.\n";
        return false;
    }

    // ── G. Display full route — O(n) ───────────────────────
    void displayRoute() const {
        if (!head) { cout << "  (Route is empty)\n"; return; }

        printLine('=');
        cout << "  CURRENT DELIVERY ROUTE\n";
        printLine('=');
        cout << left
             << setw(5)  << "Stop"
             << setw(8)  << "Order#"
             << setw(18) << "Customer"
             << setw(20) << "Address"
             << setw(8)  << "Dist"
             << setw(6)  << "ETA"
             << setw(10) << "Traffic"
             << setw(12) << "Status" << "\n";
        printLine();

        RouteNode* curr = head;
        int stop = 0;
        while (curr) {
            string label = curr->data.isRestaurant
                           ? "[R]" : to_string(stop);
            cout << setw(5)  << label
                 << setw(8)  << curr->data.orderId
                 << setw(18) << curr->data.customerName
                 << setw(20) << curr->data.address
                 << setw(8)  << to_string(curr->data.distanceKm) + "km"
                 << setw(6)  << to_string(curr->data.estimatedMins) + "m"
                 << setw(10) << trafficStr(curr->traffic)
                 << setw(12) << statusStr(curr->data.status) << "\n";

            if (curr->next) cout << "  ---->\n";
            curr = curr->next;
            if (!curr) break;
            if (!curr->data.isRestaurant) stop++;
        }
        printLine('=');
        cout << "  Stops: " << totalStops
             << "  |  Delivered: " << deliveredCount
             << "  |  Total distance: " << fixed << setprecision(1)
             << totalDistanceKm << " km\n";
        printLine('=');
    }

    // ── H. Reverse route — O(n) ────────────────────────────
    void reverseRoute() {
        RouteNode* prev = nullptr;
        RouteNode* curr = head;
        while (curr) {
            RouteNode* nxt = curr->next;
            curr->next = prev;
            prev = curr;
            curr = nxt;
        }
        head = prev;
        cout << "  [~] Route reversed.\n";
    }
};


// ================================================================
//  MODULE 3 — TRAFFIC MONITORING MODULE
//  Simulates real-time traffic updates on route segments
// ================================================================
class TrafficMonitor {
public:
    // Simulate traffic scan on the whole route (random for demo)
    void scanAndUpdate(DeliveryRoute& route) {
        srand((unsigned)time(nullptr));
        RouteNode* curr = route.getHead();
        int updated = 0;
        cout << "\n  [TRAFFIC SCAN]\n";
        while (curr) {
            if (!curr->data.isRestaurant) {
                TrafficLevel old = curr->traffic;
                // Simulate: 50% same, 25% better, 25% worse
                int r = rand() % 4;
                if      (r == 0 && curr->traffic != LOW)     curr->traffic = (TrafficLevel)(curr->traffic - 1);
                else if (r == 1 && curr->traffic != BLOCKED) curr->traffic = (TrafficLevel)(curr->traffic + 1);
                if (curr->traffic != old) {
                    cout << "  [~] Order #" << curr->data.orderId
                         << " (" << curr->data.address << "): "
                         << trafficStr(old) << " -> " << trafficStr(curr->traffic) << "\n";
                    updated++;
                }
            }
            curr = curr->next;
        }
        if (updated == 0) cout << "  No traffic changes detected.\n";
    }

    // Manually set traffic at a specific order node
    void setTraffic(DeliveryRoute& route, int orderId, TrafficLevel level) {
        RouteNode* curr = route.getHead();
        while (curr) {
            if (curr->data.orderId == orderId) {
                curr->traffic = level;
                cout << "  [T] Traffic for Order #" << orderId
                     << " set to: " << trafficStr(level) << "\n";
                return;
            }
            curr = curr->next;
        }
        cout << "  [!] Order #" << orderId << " not found.\n";
    }

    // Report: list all high/blocked segments
    void reportCongestion(const DeliveryRoute& route) const {
        cout << "\n  [CONGESTION REPORT]\n";
        cout << "  " << string(40, '-') << "\n";
        RouteNode* curr = route.getHead();
        bool found = false;
        while (curr) {
            if (curr->traffic >= HIGH) {
                cout << "  !! Order #" << curr->data.orderId
                     << " | " << curr->data.address
                     << " | " << trafficStr(curr->traffic) << "\n";
                found = true;
            }
            curr = curr->next;
        }
        if (!found) cout << "  All clear — no congestion detected.\n";
        cout << "  " << string(40, '-') << "\n";
    }
};


// ================================================================
//  MODULE 4 — ROUTE OPTIMIZATION ENGINE
//  Recalculates ETAs, skips BLOCKED roads, summarizes efficiency
// ================================================================
class OptimizationEngine {
private:
    // Extra minutes penalty per traffic level
    int trafficPenalty(TrafficLevel t) const {
        switch(t) {
            case LOW:      return 0;
            case MODERATE: return 3;
            case HIGH:     return 8;
            case BLOCKED:  return 999;  // marks as unserviceable
        }
        return 0;
    }

public:
    // Recalculate all ETAs based on current traffic — O(n)
    void recalculateETAs(DeliveryRoute& route) const {
        RouteNode* curr = route.getHead();
        int cumulative = 0;
        cout << "\n  [ETA RECALCULATION]\n";
        cout << "  " << string(50, '-') << "\n";
        while (curr) {
            if (!curr->data.isRestaurant) {
                int base    = (int)(curr->data.distanceKm * 4);
                int penalty = trafficPenalty(curr->traffic);
                int eta     = base + penalty;
                cumulative += eta;
                curr->data.estimatedMins = cumulative;

                cout << "  Order #" << setw(4) << curr->data.orderId
                     << " | " << setw(16) << curr->data.customerName
                     << " | Base: " << setw(3) << base
                     << "m  Penalty: +" << setw(3) << penalty
                     << "m  ETA: " << setw(4) << cumulative << "m";
                if (penalty == 999)
                    cout << "  *** ROAD BLOCKED ***";
                cout << "\n";
            }
            curr = curr->next;
        }
        cout << "  " << string(50, '-') << "\n";
        cout << "  Total estimated time: " << cumulative << " minutes\n\n";
    }

    // Move BLOCKED stops to end of route — O(n^2 worst case)
    void rerouteBlocked(DeliveryRoute& route) {
        RouteNode* curr = route.getHead();
        bool any = false;
        while (curr) {
            if (!curr->data.isRestaurant && curr->traffic == BLOCKED) {
                cout << "  [R] Rerouting blocked stop: Order #"
                     << curr->data.orderId << " — "
                     << curr->data.customerName
                     << " moved to end of route.\n";
                Order saved = curr->data;
                route.cancelOrder(curr->data.orderId);
                saved.status = PENDING;
                route.insertAtEnd(saved, MODERATE);
                any = true;
                // Restart from head after structural change
                curr = route.getHead();
                continue;
            }
            curr = curr->next;
        }
        if (!any) cout << "  No blocked stops to reroute.\n";
    }

    // Summary report — O(n)
    void optimizationReport(const DeliveryRoute& route) const {
        RouteNode* curr = route.getHead();
        int low=0, mod=0, high=0, blk=0, total=0;
        float totalDist = 0;
        while (curr) {
            if (!curr->data.isRestaurant) {
                total++;
                totalDist += curr->data.distanceKm;
                switch(curr->traffic) {
                    case LOW:      low++;  break;
                    case MODERATE: mod++;  break;
                    case HIGH:     high++; break;
                    case BLOCKED:  blk++;  break;
                }
            }
            curr = curr->next;
        }
        cout << "\n  [OPTIMIZATION REPORT]\n";
        cout << "  " << string(40, '-') << "\n";
        cout << "  Total delivery stops : " << total      << "\n";
        cout << "  Total route distance : " << fixed << setprecision(1)
             << totalDist << " km\n";
        cout << "  Low traffic stops    : " << low        << "\n";
        cout << "  Moderate stops       : " << mod        << "\n";
        cout << "  High traffic stops   : " << high       << "\n";
        cout << "  Blocked stops        : " << blk        << "\n";

        float score = total > 0
            ? 100.0f - ((float)(high*10 + blk*25) / total)
            : 100.0f;
        score = max(0.0f, min(100.0f, score));
        cout << "  Route efficiency     : " << fixed << setprecision(1)
             << score << " / 100\n";
        cout << "  " << string(40, '-') << "\n";
    }
};


// ================================================================
//  MODULE 5 — DELIVERY AGENT APPLICATION
//  Full interactive menu tying all modules together
// ================================================================

// ── Helper: get valid integer input ──────────────────────────
int getInt(const string& prompt) {
    int v; cout << prompt; cin >> v; cin.ignore(); return v;
}
string getStr(const string& prompt) {
    string s; cout << prompt; getline(cin, s); return s;
}
float getFloat(const string& prompt) {
    float v; cout << prompt; cin >> v; cin.ignore(); return v;
}

// ── Sub-menus ─────────────────────────────────────────────────
void menuOrderMgmt(DeliveryRoute& route) {
    cout << "\n  === ORDER MANAGEMENT ===\n"
         << "  1. Add Order (End)\n"
         << "  2. Insert After a Stop\n"
         << "  3. Priority Insert (Front)\n"
         << "  4. Cancel Order\n"
         << "  5. Mark Delivered\n"
         << "  6. Search Order\n"
         << "  7. Display Route\n"
         << "  8. Reverse Route\n"
         << "  0. Back\n"
         << "  Choice: ";
    int c; cin >> c; cin.ignore();

    if (c == 1) {
        int    id   = getInt  ("  Order ID     : ");
        string name = getStr  ("  Customer Name: ");
        string addr = getStr  ("  Address      : ");
        string ph   = getStr  ("  Phone        : ");
        float  dist = getFloat("  Distance(km) : ");
        route.insertAtEnd(Order(id, name, addr, ph, dist));

    } else if (c == 2) {
        int    aft  = getInt  ("  After Order# : ");
        int    id   = getInt  ("  New Order ID : ");
        string name = getStr  ("  Customer Name: ");
        string addr = getStr  ("  Address      : ");
        string ph   = getStr  ("  Phone        : ");
        float  dist = getFloat("  Distance(km) : ");
        route.insertAfter(aft, Order(id, name, addr, ph, dist));

    } else if (c == 3) {
        int    id   = getInt  ("  Order ID     : ");
        string name = getStr  ("  Customer Name: ");
        string addr = getStr  ("  Address      : ");
        string ph   = getStr  ("  Phone        : ");
        float  dist = getFloat("  Distance(km) : ");
        route.priorityInsert(Order(id, name, addr, ph, dist));

    } else if (c == 4) {
        route.cancelOrder(getInt("  Order ID to cancel: "));

    } else if (c == 5) {
        route.markDelivered(getInt("  Order ID delivered: "));

    } else if (c == 6) {
        route.searchOrder(getInt("  Order ID to search: "));

    } else if (c == 7) {
        route.displayRoute();

    } else if (c == 8) {
        route.reverseRoute();
        route.displayRoute();
    }
}

void menuTraffic(DeliveryRoute& route, TrafficMonitor& tm) {
    cout << "\n  === TRAFFIC MONITORING ===\n"
         << "  1. Run Traffic Scan (auto)\n"
         << "  2. Set Traffic Manually\n"
         << "  3. Congestion Report\n"
         << "  0. Back\n"
         << "  Choice: ";
    int c; cin >> c; cin.ignore();

    if (c == 1) {
        tm.scanAndUpdate(route);
    } else if (c == 2) {
        int id = getInt("  Order ID: ");
        cout << "  Traffic level (0=Low 1=Moderate 2=High 3=Blocked): ";
        int lv; cin >> lv; cin.ignore();
        lv = max(0, min(3, lv));
        tm.setTraffic(route, id, (TrafficLevel)lv);
    } else if (c == 3) {
        tm.reportCongestion(route);
    }
}

void menuOptimize(DeliveryRoute& route, OptimizationEngine& oe) {
    cout << "\n  === ROUTE OPTIMIZATION ===\n"
         << "  1. Recalculate ETAs\n"
         << "  2. Reroute Blocked Stops\n"
         << "  3. Optimization Report\n"
         << "  0. Back\n"
         << "  Choice: ";
    int c; cin >> c; cin.ignore();

    if (c == 1)      oe.recalculateETAs(route);
    else if (c == 2) oe.rerouteBlocked(route);
    else if (c == 3) oe.optimizationReport(route);
}

void showMainMenu() {
    cout << "\n" << string(40,'=') << "\n";
    cout << "   FOOD DELIVERY ROUTE SYSTEM\n";
    cout << "   Swiggy / Zomato Simulator\n";
    cout << string(40,'=') << "\n";
    cout << "  1. Order Management\n";
    cout << "  2. Traffic Monitoring\n";
    cout << "  3. Route Optimization\n";
    cout << "  4. View Full Route\n";
    cout << "  5. Performance Summary\n";
    cout << "  0. Exit\n";
    cout << string(40,'-') << "\n";
    cout << "  Choice: ";
}

// ── Performance Summary ───────────────────────────────────────
void performanceSummary(const DeliveryRoute& route,
                        const OptimizationEngine& oe) {
    cout << "\n" << string(50,'=') << "\n";
    cout << "  PERFORMANCE ANALYSIS\n";
    cout << string(50,'=') << "\n";
    cout << left
         << setw(30) << "  Operation"
         << setw(12) << "Complexity"
         << "Description\n";
    cout << string(50,'-') << "\n";
    cout << setw(30) << "  Insert Node (end)"
         << setw(12) << "O(n)"        << "Traverse to tail\n";
    cout << setw(30) << "  Insert After (mid)"
         << setw(12) << "O(n)"        << "Find target node\n";
    cout << setw(30) << "  Priority Insert (front)"
         << setw(12) << "O(1)"        << "Update head.next\n";
    cout << setw(30) << "  Delete Node"
         << setw(12) << "O(n)"        << "Find & unlink node\n";
    cout << setw(30) << "  Search Order"
         << setw(12) << "O(n)"        << "Linear scan\n";
    cout << setw(30) << "  Mark Delivered"
         << setw(12) << "O(n)"        << "Linear scan\n";
    cout << setw(30) << "  Reverse Route"
         << setw(12) << "O(n)"        << "Three-pointer swap\n";
    cout << setw(30) << "  Recalculate ETAs"
         << setw(12) << "O(n)"        << "Single traversal\n";
    cout << setw(30) << "  Reroute Blocked"
         << setw(12) << "O(n²)"       << "Rebuild on block\n";
    cout << string(50,'-') << "\n";
    cout << "  Stops in route     : " << route.getStopCount()      << "\n";
    cout << "  Orders delivered   : " << route.getDeliveredCount() << "\n";
    cout << "  Total distance     : " << fixed << setprecision(1)
         << route.getTotalDistance() << " km\n";
    oe.optimizationReport(route);
}

// ================================================================
//  MAIN — Demo Scenario + Interactive Session
// ================================================================
int main() {
    cout << "\n" << string(60,'=') << "\n";
    cout << "  ITM Skills University — Case Study 170\n";
    cout << "  Online Food Delivery Route Updates (Linked List)\n";
    cout << "  B.Tech CSE 2025-29 | DSA with CPP | Semester II\n";
    cout << string(60,'=') << "\n\n";

    // ── Instantiate all modules ──────────────────────────────
    DeliveryRoute    route;
    TrafficMonitor   traffic;
    OptimizationEngine optimizer;

    // ── MODULE 1+2: Load initial scenario from problem statement ─
    cout << ">>> STEP 1: Loading initial route...\n";
    cout << "    Restaurant → Customer A → Customer B\n\n";

    route.insertAtEnd(Order(0,   "Zomato Kitchen", "MG Road, Pune",      "N/A",  0.0f, true));
    route.insertAtEnd(Order(101, "Customer A",      "FC Road, Stop 1",    "9800001001", 2.5f), LOW);
    route.insertAtEnd(Order(102, "Customer B",      "JM Road, Stop 2",    "9800001002", 3.0f), MODERATE);

    route.displayRoute();

    // ── Scenario: Customer C inserts nearby ─────────────────
    cout << "\n>>> STEP 2: New nearby order from Customer C.\n";
    route.insertAfter(101,
        Order(103, "Customer C", "Deccan Gymkhana", "9800001003", 1.2f), LOW);

    // ── MODULE 3: Traffic scan ───────────────────────────────
    cout << "\n>>> STEP 3: Traffic scan.\n";
    traffic.scanAndUpdate(route);

    // ── Scenario: Customer B cancels ────────────────────────
    cout << "\n>>> STEP 4: Customer B cancels.\n";
    route.cancelOrder(102);

    // ── MODULE 4: Recalculate ETAs ───────────────────────────
    cout << "\n>>> STEP 5: Recalculate ETAs after changes.\n";
    optimizer.recalculateETAs(route);

    route.displayRoute();

    // ── MODULE 5: Interactive Agent Application ──────────────
    int choice;
    do {
        showMainMenu();
        cin >> choice; cin.ignore();

        switch(choice) {
            case 1: menuOrderMgmt(route);              break;
            case 2: menuTraffic(route, traffic);       break;
            case 3: menuOptimize(route, optimizer);    break;
            case 4: route.displayRoute();              break;
            case 5: performanceSummary(route, optimizer); break;
            case 0: break;
            default: cout << "  [!] Invalid choice.\n";
        }
    } while (choice != 0);

    cout << "\n[*] Session ended. Deliveries logged. Goodbye!\n\n";
    return 0;
}
