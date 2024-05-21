#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

class ingredientInventory {
    private:
        int amount;

    public:
        void substractIngredient(int neededAmount) {
            amount = amount - neededAmount;
        }
};

class person {
    private:
        string name;

    public:
        
};

class table {
    private:
        bool availability;
        int satisfaction;
        int clientAmount;

    public:
        
};