#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>

using namespace std;

struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void countFrequencies(const string& input, unordered_map<char, int>& frequencies) {
    for (char c : input)
        frequencies[c]++;
}

Node* buildHuffmanTree(unordered_map<char, int>& frequencies) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& pair : frequencies) {
        Node* node = new Node(pair.first, pair.second);
        pq.push(node);
    }

    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top();
}

void buildHuffmanCodes(Node* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr)
        return;

    if (root->data != '\0')
        huffmanCodes[root->data] = code;

    buildHuffmanCodes(root->left, code + '0', huffmanCodes);
    buildHuffmanCodes(root->right, code + '1', huffmanCodes);
}

string compress(const string& input, unordered_map<char, string>& huffmanCodes) {
    string compressed;
    for (char c : input)
        compressed += huffmanCodes[c];

    return compressed;
}

string decompress(const string& compressed, Node* root) {
    string decompressed;
    Node* current = root;

    for (char c : compressed) {
        if (c == '0')
            current = current->left;
        else if (c == '1')
            current = current->right;

        if (current->left == nullptr && current->right == nullptr) {
            decompressed += current->data;
            current = root;
        }
    }

    return decompressed;
}

void writeCompressedFile(const string& compressed, const string& outputFilename) {
    ofstream outfile(outputFilename, ios::binary);
    if (outfile) {
        int compressedSize = compressed.size();
        outfile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        outfile.write(compressed.c_str(), compressed.size());

        cout << "Compressed file written successfully!" << endl;
    } else {
        cerr << "Error writing compressed file: " << outputFilename << endl;
    }
}

string readCompressedFile(const string& inputFilename) {
    ifstream infile(inputFilename, ios::binary);
    if (infile) {
        int compressedSize;
        infile.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));

        string compressed;
        compressed.resize(compressedSize);
        infile.read(&compressed[0], compressedSize);

        cout << "Compressed file read successfully!" << endl;
        return compressed;
    } else {
        cerr << "Error reading compressed file: " << inputFilename << endl;
        return "";
    }
}

int main() {
    string inputFilename = "input.txt";
    string outputFilename = "compressed.bin";

    // Read input file
    ifstream infile(inputFilename);
    if (!infile) {
        cerr << "Error reading input file: " << inputFilename << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(infile)), (istreambuf_iterator<char>()));
    infile.close();

    // Count character frequencies
    unordered_map<char, int> frequencies;
    countFrequencies(input, frequencies);

    // Build Huffman tree
    Node* root = buildHuffmanTree(frequencies);

    // Build Huffman codes
    unordered_map<char, string> huffmanCodes;
    buildHuffmanCodes(root, "", huffmanCodes);

    // Compress input
    string compressed = compress(input, huffmanCodes);

    // Write compressed file
    writeCompressedFile(compressed, outputFilename);

    // Read compressed file
    string compressedRead = readCompressedFile(outputFilename);

    // Decompress
    string decompressed = decompress(compressedRead, root);

    // Output decompressed data
    cout << "Decompressed data:" << endl;
    cout << decompressed << endl;

    // Clean up memory
    delete root;

    return 0;
}
