#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <regex>
#include <stack>
#include <algorithm>
#include <fstream>
#include <limits>
#include <ctime>

#define RED     "\033[31m"  // overdue
#define GREEN   "\033[32m"  // completed
#define YELLOW  "\033[33m"  // overdue & completed
#define RESET   "\033[0m"

using namespace std;

class Basic_task {
protected:
    string* name;          // 任務名稱
    string* category;      // 任務類別
    bool* completed;       // 任務完成狀態
};

class Basic_task2 : public Basic_task {
protected:
    string* deadline;      // 截止日期 (yyyy-mm-dd)
    string* note;          // 備註
    int* importance;       // 重要度 (1-5)
public:
    // parameterized constructor
    Basic_task2(string* n, string* c, string* d, string* nt, int* imp) {
        name = new string(*n);
        category = new string(*c);
        deadline = new string(*d);
        note = new string(*nt);
        importance = new int(*imp);
        completed = new bool(false); // 預設未完成
    }

    // copy constructor
    Basic_task2(const Basic_task2* other) {
        name = new string(*(other->name));
        category = new string(*(other->category));
        deadline = new string(*(other->deadline));
        note = new string(*(other->note));
        importance = new int(*(other->importance));
        completed = new bool(*(other->completed));
    }

    // destructor
    virtual ~Basic_task2() {
        delete name;
        delete category;
        delete deadline;
        delete note;
        delete importance;
        delete completed;
    }

    // getter functions
    string* getName() { return name; }
    string* getCategory() { return category; }
    string* getDeadline() { return deadline; }
    string* getNote() { return note; }
    int* getImportance() { return importance; }
    bool* isCompleted() { return completed; }

    // setter functions
    void setName(string* n) { *name = *n; }
    void setCategory(string* c) { *category = *c; }
    void setDeadline(string* d) { *deadline = *d; }
    void setNote(string* nt) { *note = *nt; }
    void  setImportance(int* p) { *importance = *p; }
    void setCompleted(bool* c) { *completed = *c; }

    // 判斷任務是否逾期
    bool* isOverdue() const {
        time_t* raw = new time_t(time(nullptr)); // 取得目前timestamp
        tm* nowInfo = new tm;
        localtime_s(nowInfo, raw); // 轉成當地時間結構
        char* buf = new char[11];
        strftime(buf, 11, "%Y-%m-%d", nowInfo); // 轉成 yyyy-mm-dd 字串
        string* today = new string(buf);
        bool* flag = new bool(*deadline < *today);

        delete[] buf;
        delete today;
        delete nowInfo;
        delete raw;
        return flag;
    }

    void printTask(int* index) {
        bool* overdue = isOverdue();
        bool* done = completed;

        // 上色邏輯
        if (*overdue && *done) cout << YELLOW;    // 既逾期又已完成
        else if (*overdue) cout << RED;           // 逾期未完成
        else if (*done) cout << GREEN;            // 已完成且未逾期

        cout << *index << ". [" << (*completed ? "V" : "X") << "] "
            << *name << " (" << *category << ") "
            << *deadline << " "
            << string(*importance, '*'); // 用星號顯示重要度
        if (*overdue) cout << "  !overdue!";
        cout << "\n   note: " << *note << "\n";

        cout << RESET; // 顏色復原
        delete overdue;
    }


    // deep copy
    virtual Basic_task2* clone() const {
        return new Basic_task2(this);
    }
};

// 驗證日期格式 (yyyy-mm-dd)
bool* isValidDateFormat(string* dateStr) {
    // regex 檢查 yyyy-mm-dd 格式
    regex* pattern = new regex("^\\d{4}-\\d{2}-\\d{2}$");
    if (!regex_match(*dateStr, *pattern)) {
        delete pattern;
        return new bool(false);
    }
    delete pattern;

    int* year = new int;
    int* month = new int;
    int* day = new int;
    sscanf_s(dateStr->c_str(), "%d-%d-%d", year, month, day);

    // 月份檢查
    if (*month < 1 || *month > 12) {
        delete year; delete month; delete day;
        return new bool(false);
    }

    // 每月最大天數
    int* daysInMonth = new int[12] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // 閏年處理
    bool* isLeap = new bool(*year % 4 == 0 && (*year % 100 != 0 || *year % 400 == 0));
    if (*isLeap && *month == 2)
        daysInMonth[1] = 29;

    bool* result = new bool(*day >= 1 && *day <= daysInMonth[*month - 1]);

    delete[] daysInMonth;
    delete isLeap;
    delete year; delete month; delete day;
    return result;
}

class todo_list : public Basic_task2 {
private:
    vector<Basic_task2*>* li;                   // 任務列表
    stack<vector<Basic_task2*>*>* undoStack;    // undo 堆疊
    stack<vector<Basic_task2*>*>* redoStack;    // redo 堆疊

    // deep copy
    vector<Basic_task2*>* cloneState() {
        vector<Basic_task2*>* state = new vector<Basic_task2*>();
        int* i = new int(0);
        while (*i < li->size()) {
            state->push_back(((*li)[*i])->clone());
            ++(*i);
        }
        delete i;
        return state;
    }

    // 從 snapshot 還原
    void restoreState(vector<Basic_task2*>* snapshot) {
        int* i = new int(0);
        while (*i < li->size()) {
            delete (*li)[*i]; // 刪除舊物件
            ++(*i);
        }
        delete i;
        li->clear();
        li->swap(*snapshot); // 換成 snapshot
        delete snapshot;
    }

    // 清空整個堆疊
    void clearSnapshots(stack<vector<Basic_task2*>*>* snapshots) {
        while (!snapshots->empty()) {
            vector<Basic_task2*>* snap = snapshots->top();
            int* i = new int(0);
            while (*i < snap->size()) {
                delete (*snap)[*i];
                ++(*i);
            }
            delete i;
            snapshots->pop();
            delete snap;
        }
    }

    // 每次修改前，push 現狀態到 undo，並清空 redo
    void saveState() {
        undoStack->push(cloneState());
        while (!redoStack->empty()) {
            vector<Basic_task2*>* snap = redoStack->top();
            int* i = new int(0);
            while (*i < snap->size()) {
                delete (*snap)[*i];
                ++(*i);
            }
            delete i;
            redoStack->pop();
            delete snap;
        }
    }
public:
    // constructor
    todo_list() : Basic_task2(new string(""), new string(""), new string(""), new string(""), new int(1)) {
        li = new vector<Basic_task2*>();
        undoStack = new stack<vector<Basic_task2*>*>();
        redoStack = new stack<vector<Basic_task2*>*>();
    }

    // destructor
    ~todo_list() {
        int* i = new int(0);
        while (*i < li->size()) {
            delete (*li)[*i];
            ++(*i);
        }
        delete i;
        delete li;
        clearSnapshots(undoStack);
        clearSnapshots(redoStack);
        delete undoStack;
        delete redoStack;
    }

    void addTask() {
        saveState();
        string* name = new string();
        string* category = new string();
        string* deadline = new string();
        string* note = new string();

        // 1. 名稱
        cout << "Enter task name (I don't know): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, *name);
        if (name->empty()) {
            *name = "I don't know";
        }

        // 2. 類別：提供 1-4 選項，4 才允許自訂
        int* g = new int();
        while (true) {
            cout << "Enter task category:\n"
                << "1. School\n"
                << "2. Work\n"
                << "3. Personal\n"
                << "4. Other\n"
                << "Enter: ";
            if (!(cin >> *g)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Input must be a number, please try again.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (*g == 1) { *category = "School";  break; }
            else if (*g == 2) { *category = "Work";    break; }
            else if (*g == 3) { *category = "Personal"; break; }
            else if (*g == 4) {
                cout << "Enter category name: ";
                getline(cin, *category);
                if (!category->empty()) break;
            }
            cout << "Invalid input, please try again.\n";
        }
        delete g;

        // 3. Deadline：重複驗證格式
        while (true) {
            cout << "Enter task deadline (yyyy-mm-dd): ";
            getline(cin, *deadline);
            bool* valid = isValidDateFormat(deadline);
            if (*valid) {
                delete valid;
                break;
            }
            delete valid;
            cout << "Invalid date format. Please try again.\n";
        }

        // 4. Note（選填）
        cout << "Enter task note (optional): ";
        getline(cin, *note);
        if (note->empty()) {
            *note = "No notes";
        }

        // 5. Importance：1~5 之間
        int* imp = new int();
        while (true) {
            cout << "Enter importance (1-5, 5 = highest): ";
            if (!(cin >> *imp) || *imp < 1 || *imp > 5) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter 1-5.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        // push 新任務，並釋放暫存
        li->push_back(new Basic_task2(name, category, deadline, note, imp));
        delete name;
        delete category;
        delete deadline;
        delete note;
        delete imp;

        cout << "Task added successfully!\n";
    }

    void viewTasks() {
        if (li->empty()) {
            cout << "No tasks to show.\n";
            return;
        }

        // 選單
        cout << "\nFilter options:\n"
            << "1. View all tasks\n"
            << "2. Filter by category\n"
            << "3. Filter by completion status\n"
            << "4. Sort by deadline\n"
            << "5. Today list\n"
            << "6. Urgent list\n"
            << "Enter: ";
        string* opt = new string();
        cin >> *opt;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (opt->size() != 1 || (*opt)[0] < '1' || (*opt)[0] > '6') {
            cin.clear();

            cout << "Invalid input, try again.\n";
            delete opt;
            viewTasks();  // 重新呼叫
            return;
        }

        // 1. 全部
        char* iu = new char((*opt)[0]);
        if (*iu == '1') {
            cout << "\nAll Tasks:\n";
            int* i = new int(0);
            while (*i < li->size()) {
                int* index = new int(*i + 1);
                (*li)[*i]->printTask(index);
                delete index;
                ++(*i);
            }
            delete i;
        }

        // 2. 依分類
        else if (*iu == '2') {
            string* targetCat = new string();
            cout << "Enter category to filter (type the name): ";
            getline(cin, *targetCat);
            cout << "\nFiltered by category '" << *targetCat << "':\n";
            int* count = new int(0);
            int* i = new int(0);
            while (*i < li->size()) {
                if (*((*li)[*i]->getCategory()) == *targetCat) {
                    int* index = new int(*i + 1);
                    (*li)[*i]->printTask(index);
                    delete index;
                    ++(*count);
                }
                ++(*i);
            }
            if (*count == 0)
                cout << "No tasks.\n";
            delete targetCat;
            delete count;
            delete i;
        }

        // 3. 依完成狀態
        else if (*iu == '3') {
            cout << "Filter by:\n1. Completed\n2. Not completed\n";
            int* status = new int();
            if (!(cin >> *status)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter 1 or 2.\n";
                delete status;
                return;
            }
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            bool* comp = new bool(*status == 1);
            cout << "\nFiltered by status (" << (*comp ? "Completed" : "Not completed") << "):\n";
            int* count = new int(0);
            int* i = new int(0);
            while (*i < li->size()) {
                if (*((*li)[*i]->isCompleted()) == *comp) {
                    int* index = new int(*i + 1);
                    (*li)[*i]->printTask(index);
                    delete index;
                    ++(*count);
                }
                ++(*i);
            }
            if (*count == 0)
                cout << "No tasks match this status.\n";
            delete status;
            delete comp;
            delete count;
            delete i;
        }

        // 4. 依 deadline 排序
        else if (*iu == '4') {
            vector<Basic_task2*>* sortedTasks = new vector<Basic_task2*>();
            int* i = new int(0);
            while (*i < li->size()) {
                sortedTasks->push_back((*li)[*i]);
                ++(*i);
            }
            delete i;

            sort(sortedTasks->begin(), sortedTasks->end(), [](Basic_task2* a, Basic_task2* b) -> bool {
                return *(a->getDeadline()) < *(b->getDeadline());
                });

            cout << "\nTasks sorted by deadline (from soonest to latest):\n";
            int* j = new int(0);
            while (*j < sortedTasks->size()) {
                int* index = new int(*j + 1);
                (*sortedTasks)[*j]->printTask(index);
                delete index;
                ++(*j);
            }
            delete j;
            delete sortedTasks;
        }

        // 5. 今日清單
        else if (*iu == '5') {
            char* todayBuf = new char[11];
            time_t* raw = new time_t(time(nullptr));
            tm* now = new tm;
            localtime_s(now, raw);
            strftime(todayBuf, 11, "%Y-%m-%d", now);

            string* today = new string(todayBuf);
            delete[] todayBuf;

            cout << "\nToday's tasks (" << *today << "):\n";
            int* shown = new int(0);
            int* index = new int(0);
            while (*index < li->size()) {
                if (*((*li)[*index]->getDeadline()) == *today) {
                    int* idxPrint = new int(*index + 1);
                    (*li)[*index]->printTask(idxPrint);
                    delete idxPrint;
                    ++(*shown);
                }
                ++(*index);
            }
            if (*shown == 0)
                cout << "None for today.\n";

            delete today;
            delete shown;
            delete index;
            delete now;
            delete raw;
        }

        // 6. 急迫清單
        else if (*iu == '6') {
            vector<Basic_task2*>* urgent = new vector<Basic_task2*>();
            int* index = new int(0);
            while (*index < li->size()) {
                urgent->push_back((*li)[*index]);
                ++(*index);
            }
            delete index;

            sort(urgent->begin(), urgent->end(),
                [](Basic_task2* a, Basic_task2* b) -> bool {
                    auto daysLeft = [](string* date) -> int* {
                        tm* when = new tm{};
                        sscanf_s(date->c_str(), "%d-%d-%d",
                            &when->tm_year, &when->tm_mon, &when->tm_mday);
                        when->tm_year -= 1900;
                        when->tm_mon -= 1;

                        time_t* tWhen = new time_t(mktime(when));
                        time_t* tNow = new time_t(time(nullptr));

                        int* diff = new int(static_cast<int>((*tWhen - *tNow) / 86400));

                        delete tWhen;
                        delete tNow;
                        delete when;
                        return diff;
                        };

                    int* dA = daysLeft(a->getDeadline());
                    int* dB = daysLeft(b->getDeadline());

                    bool* result;
                    if (*dA != *dB) {
                        result = new bool(*dA < *dB);  // 天數少者優先
                    }
                    else if (*(a->getImportance()) != *(b->getImportance())) {
                        result = new bool(*(a->getImportance()) > *(b->getImportance())); // 重要度高者優先
                    }
                    else {
                        result = new bool(*(a->getDeadline()) < *(b->getDeadline()));     // 日期較早者優先
                    }

                    delete dA;
                    delete dB;
                    return *result;
                });


            cout << "\nUrgent list (overdue first, then soonest):\n";
            int* indexPrint = new int(1);
            for (auto* t : *urgent) {
                t->printTask(indexPrint);
                ++(*indexPrint);
            }

            delete indexPrint;
            delete urgent;
        }
        delete opt;
        delete iu;
    }

    void editTask() {
        if (li->empty()) {
            cout << "No tasks!\n";
            return;
        }
        // 列出所有任務
        cout << "\nAll Tasks:\n";
        int* i = new int(0);
        while (*i < li->size()) {
            int* index = new int(*i + 1);
            (*li)[*i]->printTask(index);
            delete index;
            ++(*i);
        }
        delete i;

        // 選取要編輯的編號
        int* index = new int();
        cout << "Enter the task number to edit: ";

        if (!(cin >> *index)
            || *index < 1
            || *index > li->size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Index must be a number between 1 and "
                << li->size() << ".\n";
            delete index;
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');


        saveState();  // 儲存現版本方便 Undo
        Basic_task2* task = (*li)[*index - 1];

        // 選單
        cout << "What do you want to edit?\n"
            << "1. Name\n"
            << "2. Category\n"
            << "3. Deadline\n"
            << "4. Mark as completed\n"
            << "5. Importance\n"
            << "6. Note\n"
            << "Enter: ";
        string* opt = new string();
        if (!(cin >> *opt) ||
            opt->size() != 1 ||
            (*opt)[0] < '1' || (*opt)[0] > '6')
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input, try again.\n";
            delete opt;
            delete index;
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // 各選項修改
        char* iu = new char((*opt)[0]);
        if (*iu == '1') {
            string* newName = new string();
            cout << "Enter new name: ";
            getline(cin, *newName);
            task->setName(newName);
            delete newName;
        }
        else if (*iu == '2') {
            string* newCat = new string();
            cout << "Enter new category: ";
            getline(cin, *newCat);
            task->setCategory(newCat);
            delete newCat;
        }
        else if (*iu == '3') {
            string* newDeadline = new string();
            while (true) {
                cout << "Enter new deadline (yyyy-mm-dd): ";
                getline(cin, *newDeadline);
                bool* valid = isValidDateFormat(newDeadline);
                if (*valid) {
                    delete valid;
                    break;
                }
                delete valid;
                cout << "Invalid date format. Please try again.\n";
            }
            task->setDeadline(newDeadline);
            delete newDeadline;
        }
        else if (*iu == '4') {
            bool* comp = new bool(true);
            task->setCompleted(comp);
            delete comp;
        }
        else if (*iu == '5') {
            int* newImp = new int();
            while (true) {
                cout << "Enter new importance (1~5): ";
                if (!(cin >> *newImp) || *newImp < 1 || *newImp > 5) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Please enter 1-5.\n";
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            task->setImportance(newImp);
            delete newImp;
        }
        else if (*iu == '6') {
            string* newNote = new string();
            cout << "Enter new note: ";
            getline(cin, *newNote);
            task->setNote(newNote);
            delete newNote;
        }
        cout << "Task updated!\n";
        delete opt;
        delete index;
        delete iu;
    }

    void deleteTask() {
        if (li->empty()) {
            cout << "No tasks!\n";
            return;
        }
        // 列出所有任務
        cout << "\nAll Tasks:\n";
        int* i = new int(0);
        while (*i < li->size()) {
            int* index = new int(*i + 1);
            (*li)[*i]->printTask(index);
            delete index;
            ++(*i);
        }
        delete i;
        // 選擇編號刪除
        int* index = new int();
        cout << "Enter the task number to delete: ";
        if (!(cin >> *index)
            || *index < 1
            || *index > li->size()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Index must be a number between 1 and "
                << li->size() << ".\n";
            delete index;
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        saveState();
        delete (*li)[*index - 1];
        li->erase(li->begin() + (*index - 1));
        delete index;
        cout << "Task deleted successfully.\n";
    }

    void undoAction() {
        if (undoStack->empty()) {
            cout << "No actions to undo.\n";
            return;
        }
        redoStack->push(cloneState());
        vector<Basic_task2*>* snapshot = undoStack->top();
        undoStack->pop();
        restoreState(snapshot);
        cout << "Undo performed.\n";
    }

    void redoAction() {
        if (redoStack->empty()) {
            cout << "No actions to redo.\n";
            return;
        }
        undoStack->push(cloneState());
        vector<Basic_task2*>* snapshot = redoStack->top();
        redoStack->pop();
        restoreState(snapshot);
        cout << "Redo performed.\n";
    }

    void run() {
        while (true) {
            // 顯示功能選單
            cout << "\nPlease enter which operation you want to do:\n"
                << "1. Add a task\n"
                << "2. View tasks\n"
                << "3. Edit a task\n"
                << "4. Delete a task\n"
                << "5. Undo\n"
                << "6. Redo\n"
                << "7. Exit\n"
                << "Enter: ";
            string* g = new string();
            cin >> *g;
            if (g->size() != 1 || (*g)[0] < '1' || (*g)[0] > '7') {
                cout << "Invalid input, try again.\n";
                delete g;
                continue;
            }
            int* t = new int((*g)[0] - '0');
            delete g;
            if (*t == 1)
                addTask();
            else if (*t == 2)
                viewTasks();
            else if (*t == 3)
                editTask();
            else if (*t == 4)
                deleteTask();
            else if (*t == 5)
                undoAction();
            else if (*t == 6)
                redoAction();
            else if (*t == 7) {
                cout << "Bye!\n";
                delete t;
                return;
            }
            delete t;
        }
    }
};

int main() {
    // 1. 打開或建立 password.txt
    fstream* file = new fstream("password.txt", ios::in | ios::out | ios::app);

    cout << "Welcome to the Todo List Application!\n";
    cout << "Is this your first time using it? (y/n): ";
    string* firstTime = new string();
    cin >> *firstTime;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (*firstTime == "y") {
        // 首次使用，設定密碼
        cout << "Please set up your password: ";
        string* password = new string();
        getline(cin, *password);
        file->close();
        file->open("password.txt", ios::out | ios::trunc);
        *file << *password;
        *file << "\n";
        cout << "Password set successfully!\n";

        delete password;

    }
    else if (*firstTime == "n") {
        // 不是首次，要求輸入密碼
        cout << "Welcome back!\n";

        file->clear();
        file->seekg(0, ios::beg);

        cout << "Please enter your password: ";
        string* password = new string();
        getline(cin, *password);

        string* storedPassword = new string();
        getline(*file, *storedPassword);

        if (*password != *storedPassword) {
            // 驗證失敗，結束程式
            cout << "Incorrect password, exiting.\n";
            delete password;
            delete storedPassword;
            delete firstTime;
            delete file;
            return 0;
        }
        else {
            cout << "Login successful!\n";
        }

        delete password;
        delete storedPassword;

    }
    else {
        // 非 y/n
        cout << "Invalid input, exiting.\n";
        delete firstTime;
        delete file;
        return 0;
    }

    // 建立並執行 todo_list
    todo_list* todoList = new todo_list();
    todoList->run();
    delete todoList;
    delete firstTime;
    delete file;
    return 0;
}