#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// This struct holds all the sweet details for a single course
struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

// Node structure for our Binary Search Tree setup
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Quick constructor to make node creation easy
    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Class handling the tree logic
class CourseBST {
private:
    Node* root;

    // Helper to clear out memory when we're done
    void destroyTree(Node* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    // Recursive helper to slide a new course into the right spot
    Node* insertHelper(Node* node, Course course) {
        if (node == nullptr) {
            return new Node(course);
        }

        // Standard string comparison works perfectly for alphanumeric sorting
        if (course.courseNumber < node->course.courseNumber) {
            node->left = insertHelper(node->left, course);
        }
        else if (course.courseNumber > node->course.courseNumber) {
            node->right = insertHelper(node->right, course);
        }
        // If it's equal, we just ignore it to avoid messy duplicates

        return node;
    }

    // In-order traversal helper (Left, Root, Right) to get things beautifully sorted
    void inOrderHelper(Node* node) {
        if (node != nullptr) {
            inOrderHelper(node->left);
            cout << node->course.courseNumber << ", " << node->course.title << endl;
            inOrderHelper(node->right);
        }
    }

    // Recursive search helper to track down a specific course ID
    Node* searchHelper(Node* node, string courseNumber) {
        if (node == nullptr || node->course.courseNumber == courseNumber) {
            return node;
        }

        if (courseNumber < node->course.courseNumber) {
            return searchHelper(node->left, courseNumber);
        }

        return searchHelper(node->right, courseNumber);
    }

public:
    CourseBST() {
        root = nullptr;
    }

    ~CourseBST() {
        destroyTree(root);
    }

    void insert(Course course) {
        root = insertHelper(root, course);
    }

    void printAllCourses() {
        if (root == nullptr) {
            cout << "The data structure is empty! Load a file first." << endl;
            return;
        }
        inOrderHelper(root);
    }

    Course search(string courseNumber) {
        Node* foundNode = searchHelper(root, courseNumber);
        if (foundNode != nullptr) {
            return foundNode->course;
        }
        // Return an empty course if we strike out
        Course emptyCourse;
        emptyCourse.courseNumber = "";
        return emptyCourse;
    }

    bool isEmpty() {
        return root == nullptr;
    }
};

// Helper function to strip out carriage returns or weird whitespace if the file came from Windows
string cleanString(string str) {
    if (!str.empty() && str.back() == '\r') {
        str.pop_back();
    }
    return str;
}

// Function to load up the file and parse it into our BST
void loadCourseData(string fileName, CourseBST& bst) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Bummer, couldn't open the file '" << fileName << "'. Double check the name!" << endl;
        return;
    }

    string line;
    int count = 0;

    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines so they don't break things

        stringstream ss(line);
        string token;
        vector<string> rowTokens;

        // Break the line apart by commas
        while (getline(ss, token, ',')) {
            rowTokens.push_back(cleanString(token));
        }

        // Data validation check: every course needs at least a number and a title
        if (rowTokens.size() < 2) {
            cout << "Skipping malformed line: " << line << endl;
            continue;
        }

        Course course;
        course.courseNumber = rowTokens[0];
        course.title = rowTokens[1];

        // Any leftover tokens on this line are prerequisites
        for (size_t i = 2; i < rowTokens.size(); ++i) {
            if (!rowTokens[i].empty()) {
                course.prerequisites.push_back(rowTokens[i]);
            }
        }

        // Shove the course right into the tree
        bst.insert(course);
        count++;
    }

    file.close();
    cout << "Success! Successfully parsed and loaded " << count << " courses into the system." << endl;
}

// Main execution loop
int main() {
    CourseBST bst;
    string fileName = "";
    int choice = 0;

    cout << "Welcome to the ABCU Computer Science Advising Assistant!" << endl;

    // Loop until user explicitly decides to quit with option 9
    while (choice != 9) {
        cout << "\n==== Main Menu ====" << endl;
        cout << "1. Load File Data" << endl;
        cout << "2. Print Alphanumeric Course List" << endl;
        cout << "3. Print Individual Course Info" << endl;
        cout << "9. Exit Program" << endl;
        cout << "What would you like to do? ";

        // Handling user input and checking for bad formatting/types
        if (!(cin >> choice)) {
            cout << "Error: That's not a valid number. Try again!" << endl;
            cin.clear(); // Clear the error state flags
            cin.ignore(10000, '\n'); // Purge the input buffer completely
            continue;
        }

        // Handle the chosen menu option
        switch (choice) {
        case 1:
            cout << "Enter the name of the data file containing course info: ";
            cin >> fileName;
            loadCourseData(fileName, bst);
            break;

        case 2:
            cout << "\nHere is the sorted department schedule:" << endl;
            bst.printAllCourses();
            break;

        case 3: {
            if (bst.isEmpty()) {
                cout << "Hold on, you need to load the file data (Option 1) before searching!" << endl;
                break;
            }

            string targetCourse;
            cout << "Which course number are you looking for? (e.g., CSCI300): ";
            cin >> targetCourse;

            // Make the search case-insensitive just to be nice to the user
            transform(targetCourse.begin(), targetCourse.end(), targetCourse.begin(), ::toupper);

            Course found = bst.search(targetCourse);

            if (!found.courseNumber.empty()) {
                cout << "\nFound it!" << endl;
                cout << found.courseNumber << ": " << found.title << endl;
                cout << "Prerequisites: ";

                if (found.prerequisites.empty()) {
                    cout << "None" << endl;
                }
                else {
                    for (size_t i = 0; i < found.prerequisites.size(); ++i) {
                        cout << found.prerequisites[i];
                        if (i < found.prerequisites.size() - 1) {
                            cout << ", ";
                        }
                    }
                    cout << endl;
                }
            }
            else {
                cout << "Course '" << targetCourse << "' wasn't found in our records." << endl;
            }
            break;
        }

        case 9:
            cout << "Thank you for using the ABCU Advising Assistant. Goodbye!" << endl;
            break;

        default:
            cout << "Error: " << choice << " isn't a valid option. Stick to 1, 2, 3, or 9!" << endl;
            break;
        }
    }

    return 0;
}
