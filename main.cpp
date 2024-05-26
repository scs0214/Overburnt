#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <condition_variable>
#include <functional>
#include <windows.h>
#define ingredients_csv "ingredients.csv"
#define recipes_csv "recipes.csv"
#define ingredientAmount 12
#define ingredientCategories 3
#define recipeAmount 5
#define recipeCategories 12
#define tableAmount 6

using namespace std;

queue<int> clientGroupsQueue;
mutex mtx;
condition_variable cv;

int contForIDs = 1;
int neededAmounts[ingredientAmount];
string** ingredients = new string*[ingredientAmount];
string** recipes = new string*[recipeAmount];

class Customer {
private:
    int ID;
public:
};

class Order {
private:
    string recipeOrdered;
    int customerID;
public:
    void setRecipeOrdered(string orderedDish) {
        this->recipeOrdered = orderedDish;
    }

    string getRecipeOrdered() const {
        return recipeOrdered;
    }

    void setCustomerID(string ID) {
        this->recipeOrdered = ID;
    }

    int getCustomerID() const {
        return customerID;
    }
};

queue<Order> ordersQueue;

class IngredientInventory {
public:
    string name;
    int amount;
    int unitaryCost;

    IngredientInventory(string n, int amt, int cost) : name(n), amount(amt), unitaryCost(cost) {}

    bool checkAmount(int actualAmount, int neededAmount) {
        return actualAmount >= neededAmount;
    }

    void substractIngredient(int neededAmount) {
        amount -= neededAmount;
    }
};

vector<IngredientInventory> ingredientInventoryVector;

void readIngredients(string** inventory) {
    ifstream file(ingredients_csv);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file 'ingredients.csv'" << endl;
        return;
    }
    string line;
    getline(file, line);
    int position = 0;

    while (getline(file, line)) {
        stringstream stream(line);
        string name, amount, unitaryCost;
        getline(stream, name, ',');
        getline(stream, amount, ',');
        getline(stream, unitaryCost, ',');

        inventory[position][0] = name;
        inventory[position][1] = amount;
        inventory[position][2] = unitaryCost;
        position++;
    }
    file.close();
}

class Recipes {
public:
    string recipeName;
    int price;
    int approxPrepTime;
    int approxEatingTime;
    string ingredient1;
    int amount1;
    string ingredient2;
    int amount2;
    string ingredient3;
    int amount3;
    string ingredient4;
    int amount4;

    Recipes(string name, int prc, int prepTime, int eatTime, string ing1, int amt1, string ing2, int amt2, string ing3, int amt3, const string& ing4, int amt4)
        : recipeName(name), price(prc), approxPrepTime(prepTime), approxEatingTime(eatTime),
        ingredient1(ing1), amount1(amt1), ingredient2(ing2), amount2(amt2), ingredient3(ing3), amount3(amt3),
        ingredient4(ing4), amount4(amt4) {}
};

vector<IngredientInventory> recipesVector;

void readRecipes(string** recipes) {
    ifstream file(recipes_csv);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file 'recipes.csv'" << endl;
        return;
    }
    string line;
    getline(file, line);
    int position = 0;

    while (getline(file, line)) {
        stringstream stream(line);
        string recipeName, price, approxPrepTime, approxEatingTime, ingredient1, amount1, ingredient2, amount2, ingredient3, amount3, ingredient4, amount4, ingredient5, amount5, ingredient6, amount6;
        getline(stream, recipeName, ',');
        getline(stream, price, ',');
        getline(stream, approxPrepTime, ',');
        getline(stream, approxEatingTime, ',');
        getline(stream, ingredient1, ',');
        getline(stream, amount1, ',');
        getline(stream, ingredient2, ',');
        getline(stream, amount2, ',');
        getline(stream, ingredient3, ',');
        getline(stream, amount3, ',');
        getline(stream, ingredient4, ',');
        getline(stream, amount4, ',');
        getline(stream, ingredient5, ',');
        getline(stream, amount5, ',');
        getline(stream, ingredient6, ',');
        getline(stream, amount6, ',');

        recipes[position][0] = recipeName;
        recipes[position][1] = price;
        recipes[position][2] = approxPrepTime;
        recipes[position][3] = approxEatingTime;
        recipes[position][4] = ingredient1;
        recipes[position][5] = amount1;
        recipes[position][6] = ingredient2;
        recipes[position][7] = amount2;
        recipes[position][8] = ingredient3;
        recipes[position][9] = amount3;
        recipes[position][10] = ingredient4;
        recipes[position][11] = amount4;

        position++;
    }
    file.close();
}


class Table {
private:
    bool available;
public:
    Table(int cap) : available(true) {}
    bool isAvailable() const { return available; }
    void setAvailability(bool avail) { available = avail; }
};

void cooking_duration() {
    int prep_time;
}

void initializeGlobalMatrix(string** mainMatrix, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        mainMatrix[i] = new string[columns];
    }
}

void addClientGroupsQueue() {
    int spawn_rate_time = 20000;
    while (true) {
        int clientsForTable = 1 + rand() % 4;
        {
            lock_guard<mutex> guard(mtx);
            clientGroupsQueue.push(clientsForTable);
        }
        this_thread::sleep_for(chrono::milliseconds(spawn_rate_time));
    }
}

void setArray0(int array[], int length) {
    for (int i = 0; i < length; i++) {
        array[i] = 0;
    }
}

bool checkIngredientsAvailability(int clientOrders[], int clientsForTable) {
    bool ingredientsAvailable = true;
    for (int i = 0; i < clientsForTable; i++) {
        int searchIngredientInRecipe = 4;
        int recipeIndex = clientOrders[i];
        while (searchIngredientInRecipe < recipeCategories) {
            for (int j = 0; j < ingredientAmount; j++) {
                if (ingredients[j][0] == recipes[recipeIndex][searchIngredientInRecipe]) {
                    neededAmounts[j] = neededAmounts[j] + stoi(recipes[recipeIndex][searchIngredientInRecipe+1]);
                }         
            }
            searchIngredientInRecipe += 2;
        }
    }
    for (int i = 0; i < ingredientAmount; i++) {
        if (neededAmounts[i] > stoi(ingredients[i][1])) {
            ingredientsAvailable = false;
        }
    }
    return ingredientsAvailable;
}

void substractIngredients() {
    for (int i = 0; i < ingredientAmount; i++) {
        int newValue = stoi(ingredients[i][1]) - neededAmounts[i];
        ingredients[i][1] = to_string(newValue);
    }
    setArray0(neededAmounts, ingredientAmount);
}

void sendOrderToPrepQueue(int OrdersArray[]) {

}

void tablesProcess(int clientsForTable) {
    int clientOrdersArrayNum[clientsForTable];
    for (int i = 0; i < clientsForTable; i++) {
        int recipeForClient = rand() % recipeAmount;
        clientOrdersArrayNum[i] = recipeForClient;
    }
    if (checkIngredientsAvailability(clientOrdersArrayNum, clientsForTable)) {
        {
            unique_lock<mutex> lock(mtx);
            Order orderToQueue;
            for (int i = 0; i < ingredientAmount; i++) {
                // Add class (with vector) substractIngredient(neededAmounts[i]);
            }
            orderToQueue.setRecipeOrdered("insertar"); // Insertar valor utilizando el vector de recetas

        }   
    }
}

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F>
    void enqueue(F&& function);

private:
    vector<thread> workers;
    queue<function<void()>> tasks;

    mutex queue_mutex;
    condition_variable condition;
    bool stop;
};

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
            [this] {
                for (;;) {
                    function<void()> task;

                    {
                        unique_lock<mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (thread &worker : workers)
        worker.join();
}

template<class F>
void ThreadPool::enqueue(F&& function) {
    {
        unique_lock<mutex> lock(queue_mutex);
        tasks.emplace(forward<F>(f));
    }
    condition.notify_one();
}

int main() {
    bool programLoop = true;
    initializeGlobalMatrix(ingredients, ingredientAmount, ingredientCategories);
    initializeGlobalMatrix(recipes, recipeAmount, recipeCategories);
    readIngredients(ingredients);
    readRecipes(recipes);
    setArray0(neededAmounts, ingredientAmount);
    
    thread threadAddCG(addClientGroupsQueue);

    ThreadPool tables(6);

    while(programLoop) {
        int clientsForTable = 0;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return !clientGroupsQueue.empty(); });
            if (!clientGroupsQueue.empty()) {
                clientsForTable = clientGroupsQueue.front();
                clientGroupsQueue.pop();
            }
        }
        if (clientsForTable > 0) {
            tables.enqueue([clientsForTable] { tablesProcess(clientsForTable); });
        }
    }

    threadAddCG.join();
    return 0;

    return 0;
}
