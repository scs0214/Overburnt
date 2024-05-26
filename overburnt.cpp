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
#define recipeCategories 16
#define tableAmount 6

using namespace std;

queue<int> clientGroupsQueue;
mutex mtx;

string** ingredients = new string*[ingredientAmount];
string** recipes = new string*[recipeAmount];

class Customer{
    private:
    int groupSize;
    public:
    Customer(int size) : groupSize(size) {}
    int getGroupSize() const { return groupSize; }
};

class IngredientInventory {
    private:

    public:
        string name;
        int amount;
        int unitaryCost;

        IngredientInventory(string n, int amt, int cost) : name(n), amount(amt), unitaryCost(cost) {}

        bool checkAmount (int actualAmount, int neededAmount) { // Habra una variable fuera q ira contando la cantidad de ingredientes necesarios y sumandolos; esto con el fin de sumar la cantidad de ingredientes necesarios en una sola mesa
            if (actualAmount >= neededAmount) {
                return true;
            }
            else {
                return false;
            }
        }

        void substractIngredient(int neededAmount) {
            amount = amount - neededAmount;
        }
};

void initializeGlobalMatrix (string** mainMatrix, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        mainMatrix[i] = new string[columns];
    }
}

void readIngredients (string** inventory){
    ifstream file("ingredients.csv");
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo 'ingredients.csv'" << endl;
        return;
    }
    string line;
    getline(file, line);
    int position = 0;

    while(getline(file, line)){
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

class Table {
private:
    bool available;
    int capacity;
    int customerId; 
public:
    Table(int cap) : available(true), capacity(cap), customerId(-1) {}
    bool isAvailable() const { return available; }
    int getCapacity() const { return capacity; }
    int getCustomerId() const { return customerId; }
    void setAvailability(bool avail) { available = avail; }
    void assignCustomer(int id) { customerId = id; }
    void clear() { available = true; customerId = -1; }
};


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
        string ingredient5;
        int amount5;
        string ingredient6;
        int amount6;
Recipes (const string& name, int prc, int prepTime, int eatTime, const string& ing1, int amt1, const string& ing2, int amt2, const string& ing3, int amt3, const string& ing4, int amt4, const string& ing5, int amt5, const string& ing6, int amt6)
        : recipeName(name), price(prc), approxPrepTime(prepTime), approxEatingTime(eatTime),
          ingredient1(ing1), amount1(amt1), ingredient2(ing2), amount2(amt2), ingredient3(ing3), amount3(amt3),
          ingredient4(ing4), amount4(amt4), ingredient5(ing5), amount5(amt5), ingredient6(ing6), amount6(amt6) {}
};

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
        recipes[position][12] = ingredient5;
        recipes[position][13] = amount5;
        recipes[position][14] = ingredient6;
        recipes[position][15] = amount6;

        position++;
    }
    file.close();
}

void cooking_duration(){
    int prep_time;
}

void addClientGroupsQueue() {
    int spawn_rate_time = 20000;
    while(true){
        lock_guard<mutex> guard(mtx);
        int clientsForTable = 1 + rand() % 6;
        clientGroupsQueue.push(clientsForTable);

        this_thread::sleep_for(chrono::milliseconds(spawn_rate_time));
    }
}

void runFunctionsForTable (int clientsForTable) {
    int* clientRecipes = new int[clientsForTable];
    for (int i = 0; i < clientsForTable; i++) {
        int recipeForClient = rand() % recipeAmount;
        clientRecipes[i] = recipeForClient;
    } 
    // Se recorre cada pedido para buscar sus ingredientes y se van acumulando para ver si es posible preparar todos los platillos de la mesa (funcion aparte)
    // Si es posible, se restan los ingredientes y se inicia su preparacion; si no, el grupo de clientes se va (funcion aparte)
    // En caso se preparen los pedidos, los clientes los consumen y una vez consumidos, pagan y se retiran del restaurante (puede estar vinculada a la funcion de arriba)
}

void printM(string** matrix, int R, int C) {
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            printf("| %s ", matrix[i][j].c_str());
        }
        printf("| \n");
    }
}

int main() { // Boceto del proceso
    bool programLoop = true;
    initializeGlobalMatrix(ingredients, ingredientAmount, ingredientCategories);
    initializeGlobalMatrix(recipes, recipeAmount, recipeCategories);
    readIngredients(ingredients);
    readRecipes(recipes);

    thread threadAddCG(addClientGroupsQueue);
    
    while (programLoop) {
        // Implementar SDL
        // Se busca una mesa vacía
        if (!clientGroupsQueue.empty()) {
            int clientsForTable = clientGroupsQueue.front();
            clientGroupsQueue.pop();
        }
        // Se envia el grupo de clientes a una mesa vacia
        
    }
    return 0;
}