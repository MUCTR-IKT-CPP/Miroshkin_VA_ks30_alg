#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <set>
#include <climits>
using namespace std;

struct TreeNode {
    int key;
    TreeNode* left;
    TreeNode* right;
    int height;

    TreeNode(int k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

class BinarySearchTree {
private:
    TreeNode* root;

    TreeNode* insert(TreeNode* node, int key) {
        if (node == nullptr) return new TreeNode(key);
        if (key < node->key) node->left = insert(node->left, key);
        else if (key > node->key) node->right = insert(node->right, key);
        return node;
    }

    TreeNode* search(TreeNode* node, int key) {
        if (node == nullptr || node->key == key) return node;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }

    TreeNode* minValueNode(TreeNode* node) {
        TreeNode* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    TreeNode* remove(TreeNode* root, int key) {
        if (root == nullptr) return root;
        if (key < root->key) root->left = remove(root->left, key);
        else if (key > root->key) root->right = remove(root->right, key);
        else {
            if (root->left == nullptr) {
                TreeNode* temp = root->right;
                delete root;
                return temp;
            } else if (root->right == nullptr) {
                TreeNode* temp = root->left;
                delete root;
                return temp;
            }
            TreeNode* temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = remove(root->right, temp->key);
        }
        return root;
    }

    void clear(TreeNode* node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    BinarySearchTree() : root(nullptr) {}

    ~BinarySearchTree() { clear(root); }

    void insert(int key) { root = insert(root, key); }
    TreeNode* search(int key) { return search(root, key); }
    void remove(int key) { root = remove(root, key); }
    void clear() { clear(root); root = nullptr; }
};

class AVLTree {
private:
    int height(TreeNode* node) {
        return node ? node->height : 0;
    }

    int balanceFactor(TreeNode* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(TreeNode* node) {
        if (node) node->height = 1 + max(height(node->left), height(node->right));
    }

    TreeNode* rotateRight(TreeNode* y) {
        TreeNode* x = y->left;
        TreeNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    TreeNode* rotateLeft(TreeNode* x) {
        TreeNode* y = x->right;
        TreeNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    TreeNode* balance(TreeNode* node) {
        updateHeight(node);
        int bf = balanceFactor(node);

        if (bf > 1) {
            if (balanceFactor(node->left) < 0)
                node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (bf < -1) {
            if (balanceFactor(node->right) > 0)
                node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    TreeNode* insert(TreeNode* node, int key) {
        if (node == nullptr) return new TreeNode(key);
        if (key < node->key) node->left = insert(node->left, key);
        else if (key > node->key) node->right = insert(node->right, key);
        else return node;
        return balance(node);
    }

    TreeNode* minValueNode(TreeNode* node) {
        TreeNode* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    TreeNode* remove(TreeNode* root, int key) {
        if (root == nullptr) return root;
        if (key < root->key) root->left = remove(root->left, key);
        else if (key > root->key) root->right = remove(root->right, key);
        else {
            if (root->left == nullptr || root->right == nullptr) {
                TreeNode* temp = root->left ? root->left : root->right;
                if (temp == nullptr) {
                    temp = root;
                    root = nullptr;
                } else *root = *temp;
                delete temp;
            } else {
                TreeNode* temp = minValueNode(root->right);
                root->key = temp->key;
                root->right = remove(root->right, temp->key);
            }
        }
        if (root == nullptr) return root;
        return balance(root);
    }

    TreeNode* search(TreeNode* node, int key) {
        if (node == nullptr || node->key == key) return node;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }

    void clear(TreeNode* node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    TreeNode* root;

    AVLTree() : root(nullptr) {}

    ~AVLTree() { clear(root); }

    void insert(int key) { root = insert(root, key); }
    TreeNode* search(int key) { return search(root, key); }
    void remove(int key) { root = remove(root, key); }
    void clear() { clear(root); root = nullptr; }
};

int linearSearch(vector<int>& arr, int key) {
    for (int i = 0; i < arr.size(); i++)
        if (arr[i] == key) return i;
    return -1;
}

// Генерация уникальных случайных чисел
vector<int> generateUniqueRandom(int n, mt19937& gen) {
    vector<int> data(n);
    uniform_int_distribution<> dis(0, INT_MAX);
    set<int> used;
    for (int i = 0; i < n; i++) {
        int val;
        do {
            val = dis(gen);
        } while (used.count(val));
        used.insert(val);
        data[i] = val;
    }
    return data;
}

int main() {
    setlocale(LC_ALL, "Rus");

    random_device rd;
    mt19937 gen(rd());

    ofstream fout("Results.txt");

    const int NUM_SERIES = 10;
    const int NUM_CYCLES = 20;
    const int NUM_SEARCH_OPS = 1000;
    const int NUM_DELETE_OPS = 1000;

    vector<long long> bstInsertRand(NUM_SERIES), bstInsertSort(NUM_SERIES);
    vector<long long> avlInsertRand(NUM_SERIES), avlInsertSort(NUM_SERIES);
    vector<long long> bstSearchRand(NUM_SERIES), bstSearchSort(NUM_SERIES);
    vector<long long> avlSearchRand(NUM_SERIES), avlSearchSort(NUM_SERIES);
    vector<long long> arrSearchRand(NUM_SERIES), arrSearchSort(NUM_SERIES);
    vector<long long> bstDeleteRand(NUM_SERIES), bstDeleteSort(NUM_SERIES);
    vector<long long> avlDeleteRand(NUM_SERIES), avlDeleteSort(NUM_SERIES);
    vector<int> sizes(NUM_SERIES);

    for (int series = 0; series < NUM_SERIES; series++) {
        int n = pow(2, 10 + series);
        sizes[series] = n;

        cout << "Серия " << series + 1 << ", размер массива: " << n << endl;

        long long bstInsR = 0, bstInsS = 0, avlInsR = 0, avlInsS = 0;
        long long bstSeaR = 0, bstSeaS = 0, avlSeaR = 0, avlSeaS = 0;
        long long arrSeaR = 0, arrSeaS = 0;
        long long bstDelR = 0, bstDelS = 0, avlDelR = 0, avlDelS = 0;

        for (int cycle = 0; cycle < NUM_CYCLES; cycle++) {
            bool sorted = (cycle >= NUM_CYCLES / 2);

            vector<int> data;
            if (sorted) {
                data.resize(n);
                for (int i = 0; i < n; i++) data[i] = i;
            } else {
                data = generateUniqueRandom(n, gen);
            }

            BinarySearchTree bst;
            AVLTree avl;

            // Замер вставки BST (микросекунды на всю операцию)
            auto start = chrono::high_resolution_clock::now();
            for (int x : data) bst.insert(x);
            auto end = chrono::high_resolution_clock::now();
            long long time = chrono::duration_cast<chrono::microseconds>(end - start).count();
            if (sorted) bstInsS += time; else bstInsR += time;

            // Замер вставки AVL (микросекунды на всю операцию)
            start = chrono::high_resolution_clock::now();
            for (int x : data) avl.insert(x);
            end = chrono::high_resolution_clock::now();
            time = chrono::duration_cast<chrono::microseconds>(end - start).count();
            if (sorted) avlInsS += time; else avlInsR += time;

            // Генерация ключей для поиска (в диапазоне данных)
            vector<int> searchKeys;
            uniform_int_distribution<> searchDis(0, sorted ? 2*n : INT_MAX);
            for (int i = 0; i < NUM_SEARCH_OPS; i++)
                searchKeys.push_back(searchDis(gen));

            // Замер поиска BST (наносекунды на 1000 операций)
            start = chrono::high_resolution_clock::now();
            for (int key : searchKeys) bst.search(key);
            end = chrono::high_resolution_clock::now();
            time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            if (sorted) bstSeaS += time; else bstSeaR += time;

            // Замер поиска AVL (наносекунды на 1000 операций)
            start = chrono::high_resolution_clock::now();
            for (int key : searchKeys) avl.search(key);
            end = chrono::high_resolution_clock::now();
            time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            if (sorted) avlSeaS += time; else avlSeaR += time;

            // Замер поиска в массиве (наносекунды на 1000 операций)
            start = chrono::high_resolution_clock::now();
            for (int key : searchKeys) linearSearch(data, key);
            end = chrono::high_resolution_clock::now();
            time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            if (sorted) arrSeaS += time; else arrSeaR += time;

            // Генерация ключей для удаления (существующие в дереве ключи)
            vector<int> deleteKeys;
            uniform_int_distribution<> deleteDis(0, n-1);
            for (int i = 0; i < NUM_DELETE_OPS; i++)
                deleteKeys.push_back(data[deleteDis(gen)]);

            // Замер удаления BST (наносекунды на 1000 операций)
            start = chrono::high_resolution_clock::now();
            for (int key : deleteKeys) bst.remove(key);
            end = chrono::high_resolution_clock::now();
            time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            if (sorted) bstDelS += time; else bstDelR += time;

            // Замер удаления AVL (наносекунды на 1000 операций)
            start = chrono::high_resolution_clock::now();
            for (int key : deleteKeys) avl.remove(key);
            end = chrono::high_resolution_clock::now();
            time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            if (sorted) avlDelS += time; else avlDelR += time;

            bst.clear();
            avl.clear();
        }

        // Усреднение: делим на количество циклов (10 для каждого типа)
        bstInsertRand[series] = bstInsR / (NUM_CYCLES/2);
        bstInsertSort[series] = bstInsS / (NUM_CYCLES/2);
        avlInsertRand[series] = avlInsR / (NUM_CYCLES/2);
        avlInsertSort[series] = avlInsS / (NUM_CYCLES/2);

        // Для поиска: делим на количество циклов и на количество операций
        bstSearchRand[series] = bstSeaR / (NUM_CYCLES/2) / NUM_SEARCH_OPS;
        bstSearchSort[series] = bstSeaS / (NUM_CYCLES/2) / NUM_SEARCH_OPS;
        avlSearchRand[series] = avlSeaR / (NUM_CYCLES/2) / NUM_SEARCH_OPS;
        avlSearchSort[series] = avlSeaS / (NUM_CYCLES/2) / NUM_SEARCH_OPS;
        arrSearchRand[series] = arrSeaR / (NUM_CYCLES/2) / NUM_SEARCH_OPS;
        arrSearchSort[series] = arrSeaS / (NUM_CYCLES/2) / NUM_SEARCH_OPS;

        // Для удаления: делим на количество циклов и на количество операций (ТЕПЕРЬ В НАНОСЕКУНДАХ!)
        bstDeleteRand[series] = bstDelR / (NUM_CYCLES/2) / NUM_DELETE_OPS;
        bstDeleteSort[series] = bstDelS / (NUM_CYCLES/2) / NUM_DELETE_OPS;
        avlDeleteRand[series] = avlDelR / (NUM_CYCLES/2) / NUM_DELETE_OPS;
        avlDeleteSort[series] = avlDelS / (NUM_CYCLES/2) / NUM_DELETE_OPS;
    }

    fout << "Результаты измерений времени операций" << endl << endl;
    fout << "Время вставки всех элементов (микросекунды):" << endl;
    fout << "N\tBST_rand\tBST_sort\tAVL_rand\tAVL_sort" << endl;
    for (int i = 0; i < NUM_SERIES; i++)
        fout << sizes[i] << "\t" << bstInsertRand[i] << "\t" << bstInsertSort[i] 
             << "\t" << avlInsertRand[i] << "\t" << avlInsertSort[i] << endl;

    fout << endl << "Время одной операции поиска (наносекунды):" << endl;
    fout << "N\tBST_rand\tBST_sort\tAVL_rand\tAVL_sort\tArray_rand\tArray_sort" << endl;
    for (int i = 0; i < NUM_SERIES; i++)
        fout << sizes[i] << "\t" << bstSearchRand[i] << "\t" << bstSearchSort[i] 
             << "\t" << avlSearchRand[i] << "\t" << avlSearchSort[i]
             << "\t" << arrSearchRand[i] << "\t" << arrSearchSort[i] << endl;

    fout << endl << "Время одной операции удаления (наносекунды):" << endl;
    fout << "N\tBST_rand\tBST_sort\tAVL_rand\tAVL_sort" << endl;
    for (int i = 0; i < NUM_SERIES; i++)
        fout << sizes[i] << "\t" << bstDeleteRand[i] << "\t" << bstDeleteSort[i] 
             << "\t" << avlDeleteRand[i] << "\t" << avlDeleteSort[i] << endl;

    fout.close();

    cout << "Результаты сохранены в файл Results.txt" << endl;

    return 0;
}
