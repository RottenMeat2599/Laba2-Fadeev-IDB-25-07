#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;


struct Node {
    string url;
    string time;
    bool isBookmark;
    Node* prev;
    Node* next;

    Node(string u, string t) : url(u), time(t), isBookmark(false), prev(nullptr), next(nullptr) {}
};


class BrowserHistory {
private:
    Node* head;       
    Node* current;    

    string getCurrentTime() {
        time_t now = time(0);
        string dt = ctime(&now);
        if (!dt.empty()) dt.pop_back();
        return dt;
    }

   
    string base64_encode(const string& in) {
        const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        string ret;
        int i = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        int in_len = in.size();
        const char* bytes = in.c_str();
        while (in_len--) {
            char_array_3[i++] = *(bytes++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                for (i = 0; i < 4; i++) ret += chars[char_array_4[i]];
                i = 0;
            }
        }
        if (i) {
            for (int j = i; j < 3; j++) char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            for (int j = 0; j < i + 1; j++) ret += chars[char_array_4[j]];
            while (i++ < 3) ret += '=';
        }
        return ret;
    }

    
    void clearMemory() {
        Node* temp = head;
        while (temp != nullptr) {
            Node* toDelete = temp;
            temp = temp->next;
            delete toDelete;
        }
        head = nullptr;
        current = nullptr;
    }

public:
    BrowserHistory() : head(nullptr), current(nullptr) {}

    ~BrowserHistory() {
        clearMemory();
    }

   
    void visit(string url) {
        if (current != nullptr) {
            Node* temp = current->next;
            while (temp != nullptr) {
                Node* toDelete = temp;
                temp = temp->next;
                delete toDelete;
            }
            current->next = nullptr;
        }

        
        Node* newNode = new Node(url, getCurrentTime());

       
        if (head == nullptr) {
            head = newNode;
        }
        else {
            newNode->prev = current;
            current->next = newNode;
        }

       
        current = newNode;
        cout << "Посещено: " << url << endl;
    }

    
    void back() {
        if (current != nullptr && current->prev != nullptr) {
            current = current->prev;
            cout << "Назад: " << current->url << endl;
        }
        else {
            cout << "Нельзя пойти назад" << endl;
        }
    }

   
    void forward() {
        if (current != nullptr && current->next != nullptr) {
            current = current->next;
            cout << "Вперед: " << current->url << endl;
        }
        else {
            cout << "Нельзя пойти вперед" << endl;
        }
    }

   
    void clearHistory() {
        clearMemory();
        cout << "История очищена" << endl;
    }

    
    void searchByDomain(string domain) {
        cout << "\n--- Поиск: " << domain << " ---" << endl;
        Node* temp = head;
        int index = 0;
        bool found = false;
        while (temp != nullptr) {
            if (temp->url.find(domain) != string::npos) {
                cout << "  [" << index << "] " << temp->url << endl;
                found = true;
            }
            temp = temp->next;
            index++;
        }
        if (!found) cout << " Ничего не найдено" << endl;
    }

    void saveToFile(string filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка файла" << endl;
            return;
        }
        Node* temp = head;
        while (temp != nullptr) {
            string rawLine = temp->url + "|" + temp->time + "|" + (temp->isBookmark ? "1" : "0");
            file << base64_encode(rawLine) << endl;
            temp = temp->next;
        }
        file.close();
        cout << "Сохранено в " << filename << endl;
    }

    void printTable() {
        if (head == nullptr) {
            cout << "История пуста." << endl;
            return;
        }

        cout << left << setw(5) << "ID"
            << setw(25) << "URL"
            << setw(20) << "Время"
            << setw(10) << "Статус" << endl;
        cout << string(60, '-') << endl;

        Node* temp = head;
        int index = 0;
        while (temp != nullptr) {
            string status = "";
            if (temp == current) status += "* "; 
            if (temp->isBookmark) status += "[B]";

            string shortUrl = temp->url.length() > 22 ? temp->url.substr(0, 22) + "..." : temp->url;

            cout << left << setw(5) << index
                << setw(25) << shortUrl
                << setw(20) << temp->time
                << setw(10) << status << endl;

            temp = temp->next;
            index++;
        }
        cout << "* - текущая страница, [B] - закладка" << endl;
    }

    void toggleBookmark() {
        if (current != nullptr) {
            current->isBookmark = !current->isBookmark;
            cout << "Закладка изменена: " << current->url << endl;
        }
    }
};


int main() {
    setlocale(LC_ALL, "Russian");
    BrowserHistory browser;
    int choice;
    string input;

    cout << "=== История браузера ===" << endl;

    do {
        cout << "\n1. Посетить сайт" << endl;
        cout << "2. Назад" << endl;
        cout << "3. Вперед" << endl;
        cout << "4. Показать историю" << endl;
        cout << "5. Поиск" << endl;
        cout << "6. Закладка" << endl;
        cout << "7. Сохранить в файл" << endl;
        cout << "8. Очистить" << endl;
        cout << "0. Выход" << endl;
        cout << "Выбор: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            cout << "URL: ";
            getline(cin, input);
            if (!input.empty()) browser.visit(input);
            break;
        case 2: browser.back(); break;
        case 3: browser.forward(); break;
        case 4: browser.printTable(); break;
        case 5:
            cout << "Домен: ";
            getline(cin, input);
            browser.searchByDomain(input);
            break;
        case 6: browser.toggleBookmark(); break;
        case 7: browser.saveToFile("history.txt"); break;
        case 8: browser.clearHistory(); break;
        case 0: cout << "Exit..." << endl; break;
        default: cout << "Error" << endl;
        }
    } while (choice != 0);

    return 0;
}
