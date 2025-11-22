#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct SimpleNode {
    int key;
    SimpleNode* left;
    SimpleNode* right;
};

struct ColoredNode {
    int key;
    ColoredNode* left;
    ColoredNode* right;
    ColoredNode* parent;
    bool color_flag;
};

struct UniversalContainer {
    void* element;
    UniversalContainer* next;
};

struct ColorQueue {
    ColoredNode** elements;
    int start;
    int end;
    int max_size;
};

void addToContainer(UniversalContainer*& head, void* element) {
    UniversalContainer* new_item = new UniversalContainer;
    new_item->element = element;
    new_item->next = head;
    head = new_item;
}

void* removeFromContainer(UniversalContainer*& head) {
    if (!head) return nullptr;
    void* result = head->element;
    UniversalContainer* temp = head;
    head = head->next;
    delete temp;
    return result;
}

ColorQueue* createColorQueue(int size) {
    ColorQueue* queue = new ColorQueue;
    queue->elements = new ColoredNode * [size];
    queue->start = 0;
    queue->end = -1;
    queue->max_size = size;
    return queue;
}

void addToQueue(ColorQueue* queue, ColoredNode* node) {
    if (queue->end < queue->max_size - 1) {
        queue->elements[++queue->end] = node;
    }
}

ColoredNode* removeFromQueue(ColorQueue* queue) {
    if (queue->start <= queue->end) {
        return queue->elements[queue->start++];
    }
    return nullptr;
}

bool isQueueEmpty(ColorQueue* queue) {
    return queue->start > queue->end;
}

bool verifyBrackets(const string& input) {
    int counter = 0;
    for (char c : input) {
        if (c == '(') counter++;
        if (c == ')') counter--;
        if (counter < 0) return false;
    }
    return counter == 0;
}

bool verifyCharacters(const string& input) {
    for (char c : input) {
        if (!(c == ' ' || c == '(' || c == ')' || (c >= '0' && c <= '9') || c == '-')) {
            return false;
        }
    }
    return true;
}

SimpleNode* constructFromString(const string& input, int& position, bool& isValid) {
    while (position < input.length() && input[position] == ' ') position++;

    if (position >= input.length() || input[position] != '(') {
        isValid = false;
        return nullptr;
    }
    position++;

    while (position < input.length() && input[position] == ' ') position++;

    if (position >= input.length() || !(isdigit(input[position]) || input[position] == '-')) {
        isValid = false;
        return nullptr;
    }

    int number = 0;
    bool is_negative = false;

    if (input[position] == '-') {
        is_negative = true;
        position++;

        if (position >= input.length() || !isdigit(input[position])) {
            isValid = false;
            return nullptr;
        }
    }

    while (position < input.length() && isdigit(input[position])) {
        number = number * 10 + (input[position] - '0');
        position++;
    }

    if (is_negative) number = -number;

    SimpleNode* node = new SimpleNode{ number, nullptr, nullptr };

    int childCount = 0;

    while (position < input.length() && input[position] == ' ') position++;

    while (position < input.length() && input[position] == '(') {
        if (childCount >= 2) {
            isValid = false;
            delete node;
            return nullptr;
        }

        if (childCount == 0) {
            node->left = constructFromString(input, position, isValid);
            if (!isValid) {
                delete node;
                return nullptr;
            }
            childCount++;
        }
        else {
            node->right = constructFromString(input, position, isValid);
            if (!isValid) {
                delete node->left;
                delete node;
                return nullptr;
            }
            childCount++;
        }

        while (position < input.length() && input[position] == ' ') position++;
    }

    while (position < input.length() && input[position] == ' ') position++;

    if (position >= input.length() || input[position] != ')') {
        isValid = false;
        if (node->left) delete node->left;
        if (node->right) delete node->right;
        delete node;
        return nullptr;
    }
    position++;

    return node;
}

void recursiveTraversal(SimpleNode* root) {
    if (!root) return;
    cout << root->key << " ";
    recursiveTraversal(root->left);
    recursiveTraversal(root->right);
}

void iterativeTraversal(SimpleNode* root) {
    if (!root) return;

    UniversalContainer* container = nullptr;
    addToContainer(container, root);

    while (container) {
        SimpleNode* current = (SimpleNode*)removeFromContainer(container);
        cout << current->key << " ";

        if (current->right) addToContainer(container, current->right);
        if (current->left) addToContainer(container, current->left);
    }
}

void rotateLeftSide(ColoredNode*& root, ColoredNode* node) {
    ColoredNode* right_child = node->right;
    node->right = right_child->left;

    if (right_child->left) right_child->left->parent = node;
    right_child->parent = node->parent;

    if (!node->parent) root = right_child;
    else if (node == node->parent->left) node->parent->left = right_child;
    else node->parent->right = right_child;

    right_child->left = node;
    node->parent = right_child;
}

void rotateRightSide(ColoredNode*& root, ColoredNode* node) {
    ColoredNode* left_child = node->left;
    node->left = left_child->right;

    if (left_child->right) left_child->right->parent = node;
    left_child->parent = node->parent;

    if (!node->parent) root = left_child;
    else if (node == node->parent->right) node->parent->right = left_child;
    else node->parent->left = left_child;

    left_child->right = node;
    node->parent = left_child;
}

void balanceAfterInsert(ColoredNode*& root, ColoredNode* node) {
    while (node != root && node->parent->color_flag) {
        if (node->parent == node->parent->parent->left) {
            ColoredNode* uncle = node->parent->parent->right;

            if (uncle && uncle->color_flag) {
                node->parent->color_flag = false;
                uncle->color_flag = false;
                node->parent->parent->color_flag = true;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotateLeftSide(root, node);
                }
                node->parent->color_flag = false;
                node->parent->parent->color_flag = true;
                rotateRightSide(root, node->parent->parent);
            }
        }
        else {
            ColoredNode* uncle = node->parent->parent->left;

            if (uncle && uncle->color_flag) {
                node->parent->color_flag = false;
                uncle->color_flag = false;
                node->parent->parent->color_flag = true;
                node = node->parent->parent;
            }
            else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotateRightSide(root, node);
                }
                node->parent->color_flag = false;
                node->parent->parent->color_flag = true;
                rotateLeftSide(root, node->parent->parent);
            }
        }
    }
    root->color_flag = false;
}

ColoredNode* insertColored(ColoredNode* root, int value) {
    ColoredNode* new_node = new ColoredNode{ value, nullptr, nullptr, nullptr, true };

    if (!root) {
        new_node->color_flag = false;
        return new_node;
    }

    ColoredNode* parent = nullptr;
    ColoredNode* current = root;

    while (current) {
        parent = current;
        if (value < current->key) current = current->left;
        else if (value > current->key) current = current->right;
        else {
            delete new_node;
            return root;
        }
    }

    new_node->parent = parent;
    if (value < parent->key) parent->left = new_node;
    else parent->right = new_node;

    balanceAfterInsert(root, new_node);
    return root;
}

bool findInColored(ColoredNode* root, int value) {
    ColoredNode* current = root;
    while (current) {
        if (value == current->key) return true;
        if (value < current->key) current = current->left;
        else current = current->right;
    }
    return false;
}

ColoredNode* findMinColored(ColoredNode* node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

ColoredNode* transplantColored(ColoredNode* root, ColoredNode* u, ColoredNode* v) {
    if (!u->parent) {
        root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v) v->parent = u->parent;
    return root;
}

void fixDeleteColored(ColoredNode*& root, ColoredNode* x) {
    while (x && x != root && !x->color_flag) {
        if (x == x->parent->left) {
            ColoredNode* w = x->parent->right;
            if (w && w->color_flag) {
                w->color_flag = false;
                x->parent->color_flag = true;
                rotateLeftSide(root, x->parent);
                w = x->parent->right;
            }
            if (w && (!w->left || !w->left->color_flag) && (!w->right || !w->right->color_flag)) {
                w->color_flag = true;
                x = x->parent;
            }
            else {
                if (w && (!w->right || !w->right->color_flag)) {
                    if (w->left) w->left->color_flag = false;
                    w->color_flag = true;
                    rotateRightSide(root, w);
                    w = x->parent->right;
                }
                if (w) {
                    w->color_flag = x->parent->color_flag;
                    x->parent->color_flag = false;
                    if (w->right) w->right->color_flag = false;
                    rotateLeftSide(root, x->parent);
                }
                x = root;
            }
        }
        else {
            ColoredNode* w = x->parent->left;
            if (w && w->color_flag) {
                w->color_flag = false;
                x->parent->color_flag = true;
                rotateRightSide(root, x->parent);
                w = x->parent->left;
            }
            if (w && (!w->right || !w->right->color_flag) && (!w->left || !w->left->color_flag)) {
                w->color_flag = true;
                x = x->parent;
            }
            else {
                if (w && (!w->left || !w->left->color_flag)) {
                    if (w->right) w->right->color_flag = false;
                    w->color_flag = true;
                    rotateLeftSide(root, w);
                    w = x->parent->left;
                }
                if (w) {
                    w->color_flag = x->parent->color_flag;
                    x->parent->color_flag = false;
                    if (w->left) w->left->color_flag = false;
                    rotateRightSide(root, x->parent);
                }
                x = root;
            }
        }
    }
    if (x) x->color_flag = false;
}

ColoredNode* deleteColored(ColoredNode* root, int key) {
    if (!root) return nullptr;

    ColoredNode* z = root;
    ColoredNode* y = nullptr;
    ColoredNode* x = nullptr;

    while (z) {
        if (key == z->key) break;
        else if (key < z->key) z = z->left;
        else z = z->right;
    }

    if (!z) return root;

    bool original_color = z->color_flag;

    if (!z->left) {
        x = z->right;
        root = transplantColored(root, z, z->right);
    }
    else if (!z->right) {
        x = z->left;
        root = transplantColored(root, z, z->left);
    }
    else {
        y = findMinColored(z->right);
        original_color = y->color_flag;
        x = y->right;

        if (y->parent == z) {
            if (x) x->parent = y;
        }
        else {
            root = transplantColored(root, y, y->right);
            y->right = z->right;
            if (y->right) y->right->parent = y;
        }

        root = transplantColored(root, z, y);
        y->left = z->left;
        if (y->left) y->left->parent = y;
        y->color_flag = z->color_flag;
    }

    delete z;

    if (!original_color) {
        fixDeleteColored(root, x);
    }

    return root;
}

void levelOrderTraversal(ColoredNode* root) {
    if (!root) return;

    ColorQueue* queue = createColorQueue(100);
    addToQueue(queue, root);

    while (!isQueueEmpty(queue)) {
        ColoredNode* current = removeFromQueue(queue);

        if (current->color_flag)
            cout << "\033[1;31m" << current->key << "\033[0m ";
        else
            cout << "\033[1;30m" << current->key << "\033[0m ";

        if (current->left) addToQueue(queue, current->left);
        if (current->right) addToQueue(queue, current->right);
    }

    delete[] queue->elements;
    delete queue;
}

void preorderColored(ColoredNode* root) {
    if (!root) return;

    UniversalContainer* container = nullptr;
    addToContainer(container, root);

    while (container) {
        ColoredNode* current = (ColoredNode*)removeFromContainer(container);

        if (current->color_flag)
            cout << "\033[1;31m" << current->key << "\033[0m ";
        else
            cout << "\033[1;30m" << current->key << "\033[0m ";

        if (current->right) addToContainer(container, current->right);
        if (current->left) addToContainer(container, current->left);
    }
}

void inorderColored(ColoredNode* root) {
    UniversalContainer* container = nullptr;
    ColoredNode* current = root;

    while (current || container) {
        while (current) {
            addToContainer(container, current);
            current = current->left;
        }

        current = (ColoredNode*)removeFromContainer(container);

        if (current->color_flag)
            cout << "\033[1;31m" << current->key << "\033[0m ";
        else
            cout << "\033[1;30m" << current->key << "\033[0m ";

        current = current->right;
    }
}

void postorderColored(ColoredNode* root) {
    if (!root) return;

    UniversalContainer* container1 = nullptr;
    UniversalContainer* container2 = nullptr;
    addToContainer(container1, root);

    while (container1) {
        ColoredNode* current = (ColoredNode*)removeFromContainer(container1);
        addToContainer(container2, current);

        if (current->left) addToContainer(container1, current->left);
        if (current->right) addToContainer(container1, current->right);
    }

    while (container2) {
        ColoredNode* current = (ColoredNode*)removeFromContainer(container2);

        if (current->color_flag)
            cout << "\033[1;31m" << current->key << "\033[0m ";
        else
            cout << "\033[1;30m" << current->key << "\033[0m ";
    }
}

ColoredNode* createColoredTree(SimpleNode* root) {
    if (!root) return nullptr;

    ColoredNode* colored_root = nullptr;
    UniversalContainer* container = nullptr;
    addToContainer(container, root);

    while (container) {
        SimpleNode* current = (SimpleNode*)removeFromContainer(container);
        colored_root = insertColored(colored_root, current->key);

        if (current->right) addToContainer(container, current->right);
        if (current->left) addToContainer(container, current->left);
    }

    return colored_root;
}

void freeSimpleTree(SimpleNode* root) {
    if (!root) return;
    freeSimpleTree(root->left);
    freeSimpleTree(root->right);
    delete root;
}

void freeColoredTree(ColoredNode* root) {
    if (!root) return;
    freeColoredTree(root->left);
    freeColoredTree(root->right);
    delete root;
}

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");

    string input_line;
    ifstream input_file("tree.txt");
    if (input_file) {
        getline(input_file, input_line);
        input_file.close();
        cout << "Data successfully loaded from file" << endl;
    }
    else {
        input_line = "(8 (9 (5)) (1))";
        cout << "File not found, using default example" << endl;
    }

    cout << "Input data: " << input_line << endl;

    if (!verifyBrackets(input_line)) {
        cout << "Error: incorrect brackets" << endl;
        return 1;
    }

    if (!verifyCharacters(input_line)) {
        cout << "Error: invalid characters detected" << endl;
        return 1;
    }

    int pos = 0;
    bool isValid = true;
    SimpleNode* simple_tree = constructFromString(input_line, pos, isValid);

    if (!simple_tree || !isValid) {
        cout << "Error: invalid tree structure" << endl;
        return 1;
    }

    while (pos < input_line.length() && input_line[pos] == ' ') pos++;
    if (pos < input_line.length()) {
        cout << "Error: not all string was processed" << endl;
        freeSimpleTree(simple_tree);
        return 1;
    }

    cout << "Recursive traversal: ";
    recursiveTraversal(simple_tree);
    cout << endl;

    cout << "Iterative traversal: ";
    iterativeTraversal(simple_tree);
    cout << endl;

    ColoredNode* colored_tree = createColoredTree(simple_tree);

    cout << "\nRED-BLACK TREE TRAVERSALS" << endl;

    cout << "1. Level order traversal: ";
    levelOrderTraversal(colored_tree);
    cout << endl;

    cout << "2. Preorder traversal: ";
    preorderColored(colored_tree);
    cout << endl;

    cout << "3. Inorder traversal: ";
    inorderColored(colored_tree);
    cout << endl;

    cout << "4. Postorder traversal: ";
    postorderColored(colored_tree);
    cout << endl;
    cout << endl;

    cout << "SEARCH IN RED-BLACK TREE:" << endl;
    if (simple_tree) {
        int test_value = simple_tree->key;
        cout << "Search " << test_value << ": "
            << (findInColored(colored_tree, test_value) ? "found" : "not found") << endl;

        int missing_value = 100;
        cout << "Search " << missing_value << ": "
            << (findInColored(colored_tree, missing_value) ? "found" : "not found") << endl;
    }

    cout << "\nDELETION FROM RED-BLACK TREE:" << endl;
    int delete_key = 8;
    cout << "Deleting node with value " << delete_key << endl;
    colored_tree = deleteColored(colored_tree, delete_key);

    cout << "Tree after deletion:" << endl;
    cout << "Level order traversal: ";
    levelOrderTraversal(colored_tree);
    cout << endl;

    freeSimpleTree(simple_tree);
    freeColoredTree(colored_tree);

    return 0;
}