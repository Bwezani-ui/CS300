// ProjectTwo.cpp
// CS 300 - ABCU Advising Program using BST method
// Bwezani Mukuka 
// 10/19/2025


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

// Will store course ID, title, and list of prerequisite course IDs.
struct Course {
    string number;              
    string title;               
    vector<string> prereqs;     
};

// To remove leading/trailing spaces from a string
static string trim(const string& s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace((unsigned char)s[a])) a++;
    while (b > a && isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}

static string up(string s) { // will uppercase a string so course IDs compare consistently
    for (char& c : s) c = (char)toupper((unsigned char)c);
    return s;
}

static vector<string> splitCSV(const string& line) {
    vector<string> parts;
    string tok;
    stringstream ss(line);
    while (getline(ss, tok, ',')) parts.push_back(trim(tok));
    return parts;
}

// Node holds one Course and links to left/right in the BST. 
struct Node {
    Course data;
    Node* left;
    Node* right;
    Node(const Course& c) : data(c), left(nullptr), right(nullptr) {}
};

// stores courses sorted by course number.
class BST {
public:
    BST() : root(nullptr) {}
    ~BST() { destroy(root); }

    void insert(const Course& c) { root = insertRec(root, c); }
    const Course* find(const string& num) const {
        string key = up(trim(num));
        Node* cur = root;
        while (cur) {
            if (key == cur->data.number) return &cur->data;
            cur = (key < cur->data.number) ? cur->left : cur->right;
        }
        return nullptr;
    }
    void printInOrder() const { printRec(root); } // Print all courses in ascending order.
    bool empty() const { return root == nullptr; }
    void clear() { destroy(root); root = nullptr; }

private:
    Node* root;

    static Node* insertRec(Node* n, const Course& c) {
        if (!n) return new Node(c);
        if (c.number < n->data.number) n->left = insertRec(n->left, c);
        else if (c.number > n->data.number) n->right = insertRec(n->right, c);
        else n->data = c; // will overwrite duplicate
        return n;
    }
    static void printRec(Node* n) {// in-order traversal (Left → Node → Right)
        if (!n) return;
        printRec(n->left);
        cout << n->data.number << ", " << n->data.title << "\n";
        printRec(n->right);
    }
    static void destroy(Node* n) { // Helper to delete all nodes.
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
};


class ABCUAdvising {
public:
    bool load(const string& file) {
        ifstream in(file);
        if (!in.is_open()) {
            cerr << "Error: could not open file '" << file << "'.\n\n";
            return false;
        }
        tree.clear();

        string line;
        size_t ok = 0, bad = 0;
        while (getline(in, line)) {
            string t = trim(line);
            if (t.empty()) continue;
            if (!isCSVLike(t)) { bad++; continue; } 

            vector<string> f = splitCSV(t);
            if (f.size() < 2) { bad++; continue; }

            Course c;
            c.number = up(f[0]);
            c.title = f[1];
            for (size_t i = 2; i < f.size(); ++i)
                if (!f[i].empty()) c.prereqs.push_back(up(f[i]));

            tree.insert(c);
            ok++;
        }

        if (ok == 0) {
            cerr << "Error: no valid course records found in '" << file << "'.\n\n";
            return false;
        }
        cout << "Loaded " << ok << " course(s)";
        if (bad > 0) cout << " (" << bad << " malformed line(s) skipped)";
        cout << " from '" << file << "'.\n\n";
        return true;
    }

    bool hasData() const { return !tree.empty(); }

    void printAll() const {
        cout << "Here are your courses:\n\n";
        tree.printInOrder();
        cout << "\n";
    }

    void printOne(const string& num) const {
        const Course* c = tree.find(num);
        if (!c) { cout << "Course not found.\n\n"; return; }

        cout << c->number << ", " << c->title << "\n\n";
        if (c->prereqs.empty()) { cout << "Prerequisites: None\n\n"; return; }

        cout << "Prerequisites: ";
        for (size_t i = 0; i < c->prereqs.size(); ++i) {
            const Course* pc = tree.find(c->prereqs[i]);
            if (pc) cout << pc->number << " - " << pc->title;
            else    cout << c->prereqs[i];
            if (i + 1 < c->prereqs.size()) cout << ", ";
        }
        cout << "\n\n";
    }

private:
    BST tree;

    static bool isCSVLike(const string& s) {
        if (s.find('\t') != string::npos) return false;
        return s.find(',') != string::npos;
    }
};


static void menu() {
    cout << "1. Load Data.\n";
    cout << "2. Print Course List.\n";
    cout << "3. Print Course.\n";
    cout << "9. Exit\n\n";
    cout << "Welcome to ABCU Advising! How may i help you? ";
}

int main() {
    ABCUAdvising app;

    while (true) {
        menu();
        string choice;
        if (!getline(cin, choice)) { cout << "\nThank you for using ABCU Advisor!\n"; break; }
        choice = trim(choice);

        if (choice == "1") {
            cout << "\nEnter the file name: ";
            string name; if (!getline(cin, name)) { cout << "\n"; continue; }
            cout << "\n";
            app.load(trim(name));

        }
        else if (choice == "2") {
            cout << "\n";
            if (!app.hasData()) cout << "Please load the data structure first (option 1).\n\n";
            else app.printAll();

        }
        else if (choice == "3") {    // Print one course
            cout << "\n";
            if (!app.hasData()) cout << "Please load the data structure first (option 1).\n\n";
            else {
                cout << "Which course are you inquiring? ";
                string q; if (!getline(cin, q)) { cout << "\n"; continue; }
                cout << "\n";
                app.printOne(q);
            }

        }
        else if (choice == "9") {    // Exit
            cout << "\nThank you for using ABCU Advisor!\n";
            break;

        }
        else {   // Invalid menu option
            cout << "\n" << choice << " is not a valid option.\n\n";
        }
    }
    return 0;
}
