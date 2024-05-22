#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include<fstream>
#include<windows.h>
#define ingredientAmount 13
#define ingredientCategories 3
#define recipeAmount 5
#define recipeCategories 16
#define tableAmount 6


using namespace std;

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

        void substractIngredient(int neededAmount) {
            amount = amount - neededAmount;
        }
};

void openIngredients (IngredientInventory read) {
    ofstream inventory("ingredientes.csv");
    if (!inventory.is_open()) {
        cerr << "Error: No se pudo abrir el archivo 'ingredients.csv'" << endl;
        return;
    }
    inventory.open("ingredients.csv",ios::app);
    inventory<<read.name<<","<<read.amount<<","<<read.unitaryCost<<","<<endl;
    inventory.close();

}

void readIngredients (){
    ifstream file("ingredients.csv");
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo 'ingredients.csv'" << endl;
        return;
    }
    string line;
    getline(file, line);
    int position = 0;
    string inventory[ingredientAmount][ingredientCategories];

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

void openRecipes (Recipes read) {
    ofstream file("recipes.csv");
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo 'recipes.csv'" << endl;
        return;
    }
    file.open("recipes.csv",ios::app);
    file<<read.recipeName<<","<<read.price<<","<<read.approxPrepTime<<","<<read.ingredient1<<","<<read.amount1<<","<<read.ingredient2<<","<<read.amount2<<","<<read.ingredient3<<","<<read.amount3<<","<<read.ingredient4<<","<<read.amount4<<","<<read.ingredient5<<","<<read.amount5<<","<<read.ingredient6<<","<<read.amount6<<","<<endl;
    file.close();

}

void readRecipes (){
    ifstream file("recipes.csv");
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo 'recipes.csv'" << endl;
        return;
    }
    string line;
    getline(file, line);
    int position = 0;
    string recipes[recipeAmount][recipeCategories];

    while(getline(file, line)){
        stringstream stream(line); 
        string recipeName, price, approxPrepTime, approxEatingtime, ingredient1, amount1, ingredient2, amount2, ingredient3, amount3, ingredient4, amount4, ingredient5, amount5, ingredient6, amount6,
        getline(stream, recipeName, ',');
        getline(stream, price, ',');
        getline(stream, approxPrepTime, ',');
        getline(stream, approxEatingtime, ',');
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
        recipes[position][3] = approxEatingtime;
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