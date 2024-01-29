#include <iostream>
#include <ctime> 
#include <fstream> 
#include <vector> 
#include <chrono> 

 
using namespace std; 

string inputFile = "data.txt"; 
string outputFile = "output.txt"; 
string timeStampsFile = "timestamps.txt"; 
 
int batchNum = 7; 
 
int dims[7] = { 100, 500, 1000, 5000, 10000, 100000, 1000000 }; 
 
//  match   ->   name   ->  bals 
//  д)  Пирамидальная сортировка 
//  е)  Быстрая сортировка 
//  ж)  Сортировка слиянием

 /*
 \brief структура для работы с данными о футболистах
 */
struct Futbolers{
    string country; ///< страна 
    string name;    ///< ФИО футболиста
    string club;    ///< Название клуба
    string amplua;  ///< Амплуа, принимающее следующие значения: вратарь, защитник, полузащитник, нападающий
    int match;      ///< Количество матчей 
    int bals;       ///< Количество забитых мячей
 
    Futbolers(string country, string name, string club, string amplua, int match,int bals ) 
    { 
        this-> country = country;
        this-> name = name; 
        this-> club = club; 
        this-> amplua = amplua;
        this-> match = match;
        this-> bals = bals;
    } 
 /*
 \brief перегрузка оператора == в соответствии с заданным условием
 */
    friend bool operator== (const Futbolers& a, const Futbolers& b) { 
        if (a.match == b.match && a.name == b.name && a.bals == b.bals) 
            return true; 
        return false; 
    } 
  /*
 \brief перегрузка оператора < в соответствии с заданным условием
 */
    friend bool operator< (const Futbolers& a, const Futbolers& b) { 
        if (a.match < b.match  ||((a.match == b.match) && (a.name < b.name))|| ((a.match == b.match) && (a.name == b.name) && (a.bals < b.bals))) 
            return true; 
        return false; 
    } 
   /*
 \brief перегрузка оператора <= в соответствии с заданным условием
 */
    friend bool operator<= (const Futbolers& a, const Futbolers& b) { 
        if (a < b || a == b) 
            return true; 
        return false; 
    } 
   /*
 \brief перегрузка оператора > в соответствии с заданным условием
 */
    friend bool operator> (const Futbolers& a, const Futbolers& b) { 
        if (!(a < b) && !(a == b)) 
            return true; 
        return false; 
    } 
   /*
 \brief перегрузка оператора >= в соответствии с заданным условием
 */
    friend bool operator>= (const Futbolers& a, const Futbolers& b) { 
        if (!(a < b)) 
            return true; 
        return false; 
    } 
 
   /*
 \brief перегрузка оператора << для вывода данных в файл
 */
    friend ostream& operator<<(ostream& os, const Futbolers& a) { 
        os << a.match << ' ' << a.name << ' ' << a.bals << '\n'; 
        return os; 
    } 
}; 
   /*
 \brief функция создания произвольной строки для задания страны и ФИО
 */
string randomString(const int len) { 
    static const char alphanum[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
        "abcdefghijklmnopqrstuvwxyz"; 
    string tmp_s; 
    tmp_s.reserve(len); 
    for (int i = 0; i < len; ++i) { 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)]; 
    } 
    return tmp_s; 
} 
  /*
 \brief функция заполнения Амплуа 
 */
string randomAmplua(){
    static const string varAmplua[] = {"вратарь","защитник", "полузащитник", "нападающий"};
    return varAmplua[rand()%3];
}

   /*
 \brief функция создания текстового файла для проверки работоспособности функций сортировок

 в файл записываются данные о стране, ФИО, названии клуба, амплуа, количество матчей и количестве мячей
 */
void makeTxtFile() { 
    srand((unsigned)time(NULL)); 
    ofstream fout(inputFile); 
 
    for (int i = 0; i < batchNum; ++i) { 
        //Вывод числа записей 
        fout << dims[i] << "\n"; 
        for (int j = 0; j < dims[i]; ++j) { 
            //Вывод полей по порядку 
            fout << randomString(10)   << "\n";   // Страна 
            fout << randomString(10)   << "_";   // Фамилия 
            fout << randomString(10)   << "_";   // Имя 
            fout << randomString(10)   << "\n";   // Отчество
            fout << randomString(10)   << "\n";   // Название клуба
            fout << randomAmplua()     << "\n";   // Амплуа
            fout << rand() % 10 + 1    << "\n";   // Кол-во матчей
            fout << rand() % 10 + 1    << "\n";   // Кол-во мячей забитых/пропущенных
        } 
    } 
} 
  /*
 \brief функция чтения данных из файла
 После чтения данных создается вектор, состоящий из векторов структуры с данными о футболистах
 */
vector<vector<Futbolers>> readTxtFile() { 
    ifstream fin(inputFile); 
 
    vector<vector<Futbolers>> result; 
 
    int dim; 
    string country, name , club, amplua; 
    int bals, match; 
 
    for (int i = 0; i < batchNum; ++i) { 
        //Ввод числа записей 
        fin >> dim; 
        vector<Futbolers> v; 
        for (int j = 0; j < dim; ++j) { 
            //Ввод полей по порядку 
            fin >>country >> name >> club >> amplua >>match>> bals; 
            Futbolers temp(country, name, club, amplua, match, bals); 
            v.push_back(temp); 
        } 
        result.push_back(v); 
    } 
 
    return result; 
}

  /*
 \brief Вспомогательная функция для пирамидальной сортировки

 для создания пирамиды из поддерева с корнем i
 */
void heapify(vector<Futbolers>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

  /*
 \brief Основная функция для сортировки массива с использованием пирамиды
 */
vector<Futbolers> heapSort(vector<Futbolers> vec) { 
    vector<Futbolers> arr = vec; 
    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
 
    return arr; 
} 
 
  /*
 \brief Основная функция для сортировки массива с помощью быстрой сортировки
 */
vector<Futbolers> quikSort(vector<Futbolers> vec) {
    if (vec.size() <= 1) {
        return vec;
    }

    int pivotIndex = rand() % vec.size();
    Futbolers pivot = vec[pivotIndex];

    vector<Futbolers> less, equal, greater;

    for (const Futbolers& f : vec) {
        if (f < pivot) {
            less.push_back(f);
        } else if (f == pivot) {
            equal.push_back(f);
        } else {
            greater.push_back(f);
        }
    }

    vector<Futbolers> sortedLess = quikSort(less);
    vector<Futbolers> sortedGreater = quikSort(greater);

    sortedLess.insert(sortedLess.end(), equal.begin(), equal.end());
    sortedLess.insert(sortedLess.end(), sortedGreater.begin(), sortedGreater.end());

    return sortedLess;
}
  /*
 \brief Вспомогательная функция для сортировки слиянием
 */
vector<Futbolers> merge(vector<Futbolers>& left, vector<Futbolers>& right) {
    vector<Futbolers> result;
    int i = 0, j = 0;

    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            result.push_back(left[i]);
            i++;
        } else {
            result.push_back(right[j]);
            j++;
        }
    }

    while (i < left.size()) {
        result.push_back(left[i]);
        i++;
    }

    while (j < right.size()) {
        result.push_back(right[j]);
        j++;
    }

    return result;
}
  /*
 \brief Основная функция для сортировки массива слиянием
 */
vector<Futbolers> mergeSort(vector<Futbolers>& vec) {
    if (vec.size() <= 1) {
        return vec;
    }

    int mid = vec.size() / 2;
    vector<Futbolers> left(vec.begin(), vec.begin() + mid);
    vector<Futbolers> right(vec.begin() + mid, vec.end());

    left = mergeSort(left);
    right = mergeSort(right);

    return merge(left, right);
}

   /*
 \brief Функция вывода отсортированных массивов
 */
void writeOutput(string title, vector<vector<Futbolers>>& res) { 
    ofstream fout(outputFile, ios::app); 
 
    fout << title << "\n"; 
 
    for (int i = 0; i < batchNum; ++i) { 
        //Вывод числа записей 
        fout << "Размер: " << res[i].size() << "\n"; 
        for (int j = 0; j < res[i].size(); ++j) { 
            //Вывод объекта 
            fout << res[i][j] << "\n"; 
        } 
    } 
} 

  /*
 \brief Функция вывода времени работы
 */
void writeTime(string title, vector<std::chrono::steady_clock::time_point> time) { 
    ofstream fout(timeStampsFile, ios::app); 
 
    fout << title << "\n"; 
 
    for (int i = 0; i < time.size() - 1; ++i) 
        fout << "Время для сортировки " << dims[i] << " значений: " << chrono::duration_cast<chrono::milliseconds>(time[i+1] - time[i]).count() << " [ms]\n"; 
 
    fout << "\n"; 
} 
   /*
 \brief Основная функция для сортировки массива с использованием пирамиды
 */
int main() 
{ 
    setlocale(LC_ALL, "Russian"); 
    makeTxtFile(); 
 
    vector<vector<Futbolers>> initialArray = readTxtFile(); 
 
    vector<vector<Futbolers>> heapSortRes, quikSortRes, mergeSortRes; 
 
    vector<std::chrono::steady_clock::time_point> time; 
 
    time.push_back(std::chrono::steady_clock::now()); 
 
    for (int i = 0; i < batchNum; ++i) { 
        heapSortRes.push_back(heapSort(initialArray[i])); 
        time.push_back(std::chrono::steady_clock::now()); 
    } 
 
    writeOutput("Пирамидальная сортировка:\n", heapSortRes); 
    writeTime("Пирамидальная сортировка:\n", time); 
    time.clear(); 
 
    time.push_back(std::chrono::steady_clock::now()); 
 
    for (int i = 0; i < batchNum; ++i) { 
        
        quikSortRes.push_back(quikSort(initialArray[i])); 
        time.push_back(std::chrono::steady_clock::now()); 
        cout << i;
    } 
 
    writeOutput("Быстрая сортировка:\n", quikSortRes); 
    writeTime("Быстрая сортировка:\n", time); 
    time.clear(); 
 
    time.push_back(std::chrono::steady_clock::now()); 
 
    for (int i = 0; i < batchNum; ++i) { 
        mergeSortRes.push_back(mergeSort(initialArray[i])); 
        time.push_back(std::chrono::steady_clock::now()); 
    } 
 
    writeOutput("Сортировка слиянием:\n", mergeSortRes); 
    writeTime("Сортировка слиянием:\n", time); 
    time.clear(); 
 
 
    return 0; 
}
