//============================================================================
// Name        : HashTable.cpp
// Author      : Morgan Getkin
// Version     : 1.0
// Description : For ABCU: Hash table data structure
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi and stoi
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold course information
struct Course {
    string courseId; // unique identifier
    string courseTitle;
    vector<string> prerequisites;
    Course() { }
};

// For sorting
struct less_than_key {
    inline bool operator() (const Course& course1, const Course& course2)
    {
        return (course1.courseId < course2.courseId);
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(string courseId);

    double loadFactor = 0.0;

    int numEntries = 0;

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Sort(vector<Course>& sortCourses);
    Course Search(string courseId);
    void Resize();
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initializes the structures used to hold courses
    // Initalize node structure by resizing tableSize
    // Resize nodes list to tableSize
    nodes.resize(tableSize);
    // initialize vector nodes full of empty Nodes
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i] = Node();
    }
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;
    // resize nodes size
    nodes.resize(tableSize);
    // initialize vector nodes full of empty Nodes
    for (int i = 0; i < nodes.size(); i++) {
       nodes[i] = Node();
    }
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // Logic to free storage when class is destroyed
    Node* current;
    Node* temp;

    // For each item in vector nodes (buckets)
    for (int i = 0; i < nodes.size(); i++) {
        // current variable points to bucket's node
        current = &nodes[i];
        // While current node is not null (pointing to an entry)
        while (current != nullptr) {
            // temp points to bucket's node
            temp = current;
            // current becomes next node in bucket's linked list
            current = current->next;
            // delete the orphan node
            delete temp;
        }
    }
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(string courseId) {
    // Logic to calculate a hash value
    unsigned int hash;
    int key = 0;
    // Adds ASCII values of all characters in courseID
    // Then squares
    for (int i = 0; i < courseId.length(); i++) {
        key += int(courseId[i]);
        key *= key;
    }

    // hash variable stores value as it is generated
    hash = key % nodes.size();

    // return the finalized hash
    return hash;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // Logic to insert a course
    // 
    // create the key for the given course
    int key = hash(course.courseId);
    // retrieve node using key
    Node* current = &nodes[key];
    // if no entry found for the key
    if (current->key == UINT_MAX) {
        // assign this node to the key position
        nodes[key] = Node(course, key);
    }
    // else find the next open node
    else {
        // While the current node is pointing to something
        while (current->next != nullptr) {
            // Move to next node
            current = current->next;
        }
        // Once the current node is not pointing to anything,
        // Append newNode to bucket's linked list
        current->next = new Node(course, key);
    }
    numEntries += 1;

    // Check if hash table is sufficient size
    //Determine load factor
    loadFactor = double(numEntries) / tableSize;
    // If the load factor is too great
    if (loadFactor >= 1.0) {
        // Resize hash table
        Resize();
    }
}

/**
 * Print all courses
 */
void HashTable::PrintAll() {
    // Logic to print all courses
    // First call function to sort hash table
    vector<Course> sortedCourses;
    Sort(sortedCourses);

    // Iterate over entire nodes vector
    for (int i = 0; i < sortedCourses.size(); i++) {
        // Output course information
        cout << " " << sortedCourses[i].courseId << ", " 
            << sortedCourses[i].courseTitle << endl;
    }
}

void HashTable::Sort(vector<Course> &sortCourses)
{
    // Create new vector that isolates all courses
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].key != UINT_MAX) {
            sortCourses.push_back(nodes[i].course);
            Node* current = nodes[i].next;
            while (current != nullptr) {
                sortCourses.push_back(current->course);
                current = current->next;
            }
        }
    }

    // Sort vector of courses
    sort(sortCourses.begin(), sortCourses.end(), less_than_key());
}

/**
 * Search for the specified courseId
 *
 * @param courseId The course ID to search for
 */
Course HashTable::Search(string courseId) {
    // Create empty course
    Course course;

    // Logic to search for and return a bid

    // create the key for the given course
    int key = hash(courseId);

    // retrieve node using key
    Node* current = &nodes[key];

    // if no entry found for the key
    if (current->key == UINT_MAX) {
        // return course
        return course;
    }

    // if entry found for the key and courseId matches
    if (current->course.courseId == courseId) {
        //return node course
        return current->course;
    }
    // while node not equal to nullptr
    while (current->next != nullptr) {
        // if the current node matches, return it
        if (current->next->course.courseId == courseId) {
            return current->next->course;
        }
        //node is equal to next node
        current = current->next;
    }
    // Otherwise, no match found, return empty course
    return course;
}

/** 
* Checks load factor of hash table,
* resizes if necessary by doubling size
* then finding next prime
*/
void HashTable::Resize() {
    
    // Create temporary vector to copy existing hash table
    vector<Node> temp = nodes;
    // Initialize newSize with double current size
    unsigned int newSize = tableSize * 2;
    // Initialize isPrime boolean to false
    bool isPrime = false;
    // While newSize is not a prime number
    while (!isPrime) {
        // For loop that checks newSize for remainders
        for (int i = 2; i <= newSize / 2; ++i) {
            // If newSize is evenly divisible
            if (newSize % i == 0) {
                // Increment newSize
                newSize += 1;
                // Jump to new iteration of while loop
                continue;
            }
        }
        // newSize not divisible, so is a prime number
        isPrime = true;
    }
    // resize tableSize
    tableSize = newSize;
    // resize nodes size
    nodes.resize(tableSize);
    // initialize new vector nodes full of empty Nodes
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i] = Node();
    }
    // Reset numEntries so entries are not counted twice
    numEntries = 0;
    Node* tempNode;
    // Fill new vector with existing entries
    for (int i = 0; i < temp.size(); i++) {
        if (temp[i].key != UINT_MAX) {
            Insert(temp[i].course);
            tempNode = temp[i].next;
            while (tempNode != nullptr) {
                Insert(tempNode->course);
                tempNode = tempNode->next;
            }
        }
    }
    return;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    // output key, courseID, courseTitle, and any prerequisites
    cout << " " << course.courseId << ", " << course.courseTitle << endl;
    cout << " Prerequisites: ";
    for (int i = 0; i < course.prerequisites.size(); i++)
    {
        cout << course.prerequisites[i];
        // If not the last item in the list, print comma
        if ((i + 1) != course.prerequisites.size()) {
            cout << ", ";
        }
    }
    cout << endl;
    return;
}

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the courses read
 */
void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of courses
            Course course;
            course.courseId = file[i][0];
            course.courseTitle = file[i][1];

            // checks for prerequisistes and adds them
            for (int j = 2; j < file[i].size(); j++) {
                course.prerequisites.push_back (file[i][j]);
            }

            // push this course to the end
            hashTable->Insert(course);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, courseKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        courseKey = "";
        break;
    case 3:
        csvPath = argv[1];
        courseKey = argv[2];
        break;
    default:
        csvPath = "";
        courseKey = "";
    }

    // For upper-case conversion
    string tempStr;

    // Define a hash table to hold all the courses
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();
    
    cout << "Welcome to the course planner." << endl;
    int choice = 0;
    while (choice != 9) {
        cout << "\n  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course." << endl;
        cout << "  9. Exit\n" << endl;
        cout << "What would you like to do? ";
        cin >> choice;

        switch (choice) {

        case 1:
            // Get csv path from user
            cout << "Enter name of CSV file to load: ";
            cin.ignore();
            getline(cin, csvPath);

            // Complete the method call to load the courses
            loadCourses(csvPath, courseTable);
            break;

        case 2:
            // Complete the method call to print all courses
            cout << "Here is a sample schedule:\n" << endl;
            courseTable->PrintAll();
            break;

        case 3:
            // Prompts input for ID to search for
            cout << "What course do you want to know about? ";
            cin.ignore();
            getline(cin, courseKey);

            // Make sure course ID is in upper-case format
            tempStr = "";
            for (int c = 0; c < courseKey.size(); c++) {
                if (islower(courseKey[c])) {
                    tempStr.push_back(toupper(courseKey[c]));
                }
                else {
                    tempStr.push_back(courseKey[c]);
                }
            }
            courseKey = tempStr;

            // Complete the method call to search for course
            course = courseTable->Search(courseKey);

            // If course is found, call method to display course information
            if (!course.courseId.empty()) {
                displayCourse(course);
            } else {     // Else output error message
                cout << "Course ID " << courseKey << " not found." << endl;
            }
            break;

        case 9:
            // Exit
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            // Invalid entry error message
            cout << choice << " is not a valid option." << endl;
        }
    }
    return 0;
}