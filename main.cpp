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

class Customer {
private:
    int ID;

public:
    void setID(int IDNum) {
        this->ID = IDNum;
    }
    int getID() const {
        return ID;
    }

    Customer(int num) : ID(num) {}
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
    void setCustomerID(int ID) {
        this->customerID = ID;
    }
    int getCustomerID() const {
        return customerID;
    }

    Order(string recipeName, int ID) : recipeOrdered(recipeName), customerID(ID) {}
};

queue<Order> ordersQueue;

class IngredientInventory {
public:
    string name;
    int amount;
    int unitaryCost;

    bool checkAmount(int actualAmount, int neededAmount) {
        return actualAmount >= neededAmount;
    }
    void substractIngredient(int neededAmount) {
        amount -= neededAmount;
    }

    IngredientInventory(string n, int amt, int cost) : name(n), amount(amt), unitaryCost(cost) {}
};

vector<IngredientInventory> ingredientInventoryVector;

void readIngredients() {
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

        ingredientInventoryVector.emplace_back(name, stoi(amount), stoi(unitaryCost));
    }
    file.close();
}

class Recipes {
public:
    string recipeName;
    int price;
    int approxPrepTime;
    int approxEatingTime;
    vector<pair<string, int>> ingredients;

    Recipes(string name, int prc, int prepTime, int eatTime, vector<pair<string, int>> ing)
    : recipeName(name), price(prc), approxPrepTime(prepTime), approxEatingTime(eatTime), ingredients(ing) {}
};

vector<Recipes> recipesVector;

void readRecipes() {
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
        string recipeName, price, approxPrepTime, approxEatingTime, ingredient;
        int amount;
        vector<pair<string, int>> ingredients;

        getline(stream, recipeName, ',');
        getline(stream, price, ',');
        getline(stream, approxPrepTime, ',');
        getline(stream, approxEatingTime, ',');

        while (getline(stream, ingredient, ',') && getline(stream, line, ',')) {
            amount = stoi(line);
            ingredients.emplace_back(ingredient, amount);
        }

        recipesVector.emplace_back(recipeName, stoi(price), stoi(approxPrepTime), stoi(approxEatingTime), ingredients);
    }
    file.close();
}

void cooking_duration() {
    int prep_time;
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
        int ingredientVectorIndex = 0;
        int recipeIndex = clientOrders[i];
        while (ingredientVectorIndex < recipesVector[recipeIndex].ingredients.size()) {
            for (int j = 0; j < ingredientAmount; j++) {
                if (ingredientInventoryVector[i].name == recipesVector[recipeIndex].ingredients[searchIngredientInRecipe]) { // recipes[recipeIndex][searchIngredientInRecipe]
                    neededAmounts[j] = neededAmounts[j] + stoi(recipes[recipeIndex][searchIngredientInRecipe+1]);
                }         
            }
            ingredientVectorIndex++;
        }
    }
    for (int i = 0; i < ingredientAmount; i++) {
        if (neededAmounts[i] > ingredientInventoryVector[i].amount) {
            ingredientsAvailable = false;
        }
    }
    return ingredientsAvailable;
}

void substractIngredients() {
    for (int i = 0; i < ingredientAmount; i++) {
        int newValue = ingredientInventoryVector[i].amount - neededAmounts[i];
        ingredientInventoryVector[i].amount = newValue;
    }
    setArray0(neededAmounts, ingredientAmount);
}

void substractIngredientsToUse() {
    {
        unique_lock<mutex> lock(mtx); // Lock necessary for the ingredient substraction
        for (int i = 0; i < ingredientAmount; i++) {
            // Add class (with vector) substractIngredient(neededAmounts[i]);
        }
    } 
}

void sendOrderToPrepQueue(int OrdersArray[], int clientsAmount) {
    {
        unique_lock<mutex> lock(mtx); // Lock necessary for ordersQueue and contForIDs
        Order orderToQueue("", 0);
        for(int i = 0; i < clientsAmount; i++) {
            Order orderToQueue("insert", contForIDs); // Insertar valor utilizando el vector de recetas (utilizando OrdersArray)
            ordersQueue.push(orderToQueue);
            contForIDs++;
        }
    }
}

void tablesProcess(int clientsForTable) {
    int ordersArrayNum[clientsForTable];
    for (int i = 0; i < clientsForTable; i++) {
        int recipeForClient = rand() % recipeAmount;
        ordersArrayNum[i] = recipeForClient;
    }

    if (checkIngredientsAvailability(ordersArrayNum, clientsForTable)) {
        substractIngredientsToUse();
        sendOrderToPrepQueue(ordersArrayNum, clientsForTable);
    }
}

class Tables {
public:
    Tables(size_t threads);
    ~Tables();

    template<class F>
    void enqueue(F&& function);

private:
    struct table {
        thread tableThread;
        bool available;
        table(thread t) : tableThread(move(t)), available(true) {}
    };

    vector<table> tableVector;
    queue<function<void()>> tasks;

    mutex queue_mutex;
    condition_variable condition;
    bool stop;

    void tableFunction(int index);
};

Tables::Tables(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        tableVector.emplace_back(
            [this, i] {
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

                    {
                        unique_lock<mutex> lock(this->queue_mutex);
                        this->tableVector[i].available = false;
                    }
                    task();
                    {
                        unique_lock<mutex> lock(this->queue_mutex);
                        this->tableVector[i].available = true;
                    }
                }
            }
        );
    }
}

Tables::~Tables() {
    {
        unique_lock<mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (table &table : tableVector)
        table.tableThread.join();
}

template<class F>
void Tables::enqueue(F&& function) {
    {
        unique_lock<mutex> lock(queue_mutex);
        tasks.emplace(forward<F>(function));
    }
    condition.notify_one();
}

int main() {
    bool programLoop = true;
    readIngredients();
    readRecipes();
    setArray0(neededAmounts, ingredientAmount);
    
    thread threadAddCG(addClientGroupsQueue);

    Tables tables(6);

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
