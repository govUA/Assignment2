#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

const int INITIAL_CAPACITY = 10;
const int COMMAND_LENGTH = 3;
const int LINE_LENGTH = 64;

struct Line {
    char *text;
    int length;
    int capacity;
    Line *next;

    Line() {
        capacity = INITIAL_CAPACITY;
        text = new char[capacity];
        length = 0;
        next = nullptr;
    }

    ~Line() {
        delete[] text;
    }

    void append(const char *str) {
        int str_len = strlen(str);
        ensureCapacity(length + str_len + 1);
        strcpy(text + length, str);
        length += str_len;
    }

    void ensureCapacity(int minCapacity) {
        if (minCapacity > capacity) {
            capacity = minCapacity * 2;
            char *newText = new char[capacity];
            strcpy(newText, text);
            delete[] text;
            text = newText;
        }
    }

    void insertSubstring(int pos, const char *substring) {
        int subLen = strlen(substring);
        ensureCapacity(length + subLen + 1);
        memmove(text + pos + subLen, text + pos, length - pos + 1);
        memcpy(text + pos, substring, subLen);
        length += subLen;
    }

    void replaceSubstring(int pos, const char *replacement) {
        int repLen = strlen(replacement);
        int endPos = pos + repLen;
        if (endPos > length)
            endPos = length;
        memcpy(text + pos, replacement, repLen);
        text[endPos] = '\0';
    }
};

struct TextDocument {
    Line *head;
    Line *tail;
    int lineCount;

    TextDocument() {
        head = nullptr;
        tail = nullptr;
        lineCount = 0;
    }

    ~TextDocument() {
        Line *current = head;
        while (current != nullptr) {
            Line *toDelete = current;
            current = current->next;
            delete toDelete;
        }
    }

    void appendText(const char *text) {
        if (tail == nullptr) {
            head = new Line();
            tail = head;
        }
        tail->append(text);
    }

    void startNewLine() {
        if (tail == nullptr) {
            head = new Line();
            tail = head;
        } else {
            tail->next = new Line();
            tail = tail->next;
        }
        lineCount++;
    }

    void print() {
        Line *current = head;
        while (current != nullptr) {
            std::cout << current->text << std::endl;
            current = current->next;
        }
    }

    void saveToFile(const char *filename) {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }
        Line *current = head;
        while (current != nullptr) {
            outFile << current->text << std::endl;
            current = current->next;
        }
        outFile.close();
        std::cout << "Document saved to " << filename << std::endl;
    }

    void loadFromFile(const char *filename) {
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cerr << "Error opening file for reading: " << filename << std::endl;
            return;
        }
        clear();
        char buffer[LINE_LENGTH];
        while (inFile.getline(buffer, LINE_LENGTH) && !inFile.eof()) {
            appendText(buffer);
            startNewLine();
        }
        inFile.close();
        std::cout << "Document loaded from " << filename << std::endl;
    }

    void clear() {
        Line *current = head;
        while (current != nullptr) {
            Line *toDelete = current;
            current = current->next;
            delete toDelete;
        }
        head = nullptr;
        tail = nullptr;
        lineCount = 0;
    }

    void searchSubstring(const char *substring) {
        Line *current = head;
        int lineNum = 0;
        while (current != nullptr) {
            int pos = 0;
            int textLength = strlen(current->text);
            while (pos < textLength) {
                pos = strstr(current->text + pos, substring) - current->text;
                if (pos >= 0 && pos < textLength) {
                    std::cout << "Found at line " << lineNum << ", position " << pos << std::endl;
                    pos += strlen(substring);
                } else {
                    break;
                }
            }
            current = current->next;
            lineNum++;
        }
    }

    void insertSubstring(int lineNum, int pos, const char *substring) {
        Line *current = head;
        int currentLine = 0;
        while (current != nullptr && currentLine < lineNum) {
            current = current->next;
            currentLine++;
        }
        if (current == nullptr) {
            std::cerr << "Invalid line number" << std::endl;
            return;
        }
        current->insertSubstring(pos, substring);
    }

    void replaceSubstring(int lineNum, int pos, const char *replacement) {
        Line *current = head;
        int currentLine = 0;
        while (current != nullptr && currentLine < lineNum) {
            current = current->next;
            currentLine++;
        }
        if (current == nullptr) {
            std::cerr << "Invalid line number" << std::endl;
            return;
        }

        int repLen = strlen(replacement);
        int newLength = pos + repLen;

        current->ensureCapacity(newLength + 1);

        int remainingLength = current->length - (pos + repLen);

        memmove(current->text + pos + repLen, current->text + pos + strlen(current->text + pos), remainingLength + 1);

        memcpy(current->text + pos, replacement, repLen);

        current->length = newLength;
    }


};

void clearConsole() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void printHelp() {
    std::cout << "Available commands:\n";
    std::cout << "1. append <text> - Append text symbols to the end\n";
    std::cout << "2. newline - Start a new line\n";
    std::cout << "3. load <filename> - Load text from a file\n";
    std::cout << "4. save <filename> - Save text to a file\n";
    std::cout << "5. print - Print the current text to console\n";
    std::cout << "6. insert <line> <index> <text> - Insert text by line and symbol index\n";
    std::cout << "7. search <text> - Search for text in the document\n";
    std::cout << "8. clear - Clear the console\n";
    std::cout << "9. help - Print this help information\n";
    std::cout << "10. exit - Exit the editor\n";
}

void handleCommand(char *command, TextDocument &doc) {
    if (strncmp(command, "1", 2) == 0) {
        char text[LINE_LENGTH];
        std::cin.getline(text, LINE_LENGTH);
        doc.appendText(text);
    } else if (strncmp(command, "2", 2) == 0) {
        doc.startNewLine();
    } else if (strncmp(command, "3", 2) == 0) {
        char filename[LINE_LENGTH];
        std::cin.getline(filename, LINE_LENGTH);
        doc.loadFromFile(filename);
    } else if (strncmp(command, "4", 2) == 0) {
        char filename[LINE_LENGTH];
        std::cin.getline(filename, LINE_LENGTH);
        doc.saveToFile(filename);
    } else if (strncmp(command, "5", 2) == 0) {
        doc.print();
    } else if (strncmp(command, "6", 2) == 0) {
        int lineNum, pos;
        char substring[LINE_LENGTH];
        std::cin >> lineNum;
        std::cin >> pos;
        std::cin >> substring;
        doc.insertSubstring(lineNum, pos, substring);
    } else if (strncmp(command, "7", 2) == 0) {
        char substring[LINE_LENGTH];
        std::cin.getline(substring, LINE_LENGTH);
        doc.searchSubstring(substring);
    } else if (strncmp(command, "8", 2) == 0) {
        clearConsole();
    } else if (strncmp(command, "9", 2) == 0) {
        printHelp();
    } else if (strncmp(command, "10", 2) == 0) {
        std::cout << "Exiting the editor.\n";
        exit(0);
    } else if (strncmp(command, "11", 2) == 0) {
        int lineNum, pos;
        char substring[LINE_LENGTH];
        std::cin >> lineNum;
        std::cin >> pos;
        std::cin >> substring;
        doc.replaceSubstring(lineNum, pos, substring);
    } else {
        std::cout << "The command is not implemented.\n";
    }
}

int main() {
    char command[COMMAND_LENGTH];
    TextDocument doc;

    printHelp();

    while (true) {
        std::cout << "\nEnter command: ";
        std::cin.getline(command, COMMAND_LENGTH);
        handleCommand(command, doc);
    }

    return 0;
}