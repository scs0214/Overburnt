#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
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

#define ingredients_csv "ingredients.csv"
#define recipes_csv "recipes.csv"
#define ingredientAmount 12
#define ingredientCategories 3
#define recipeAmount 5
#define recipeCategories 12
#define tableAmount 6
#define PREP_DELAY 10
#define EATING_DELAY 15
#define MIN_PREP_TIME 5
#define MIN_EATING_TIME 5

using namespace std;

queue<int> clientGroupsQueue;
mutex mtx;
condition_variable cv;
int contForIDs = 1;
int neededAmounts[ingredientAmount];

bool init(SDL_Window** window, SDL_Renderer** renderer, const int width, const int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not be initialized. " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Overburnt Restaurant (Memory Leaks)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!*window) {
        std::cerr << "Window could not be created. " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        std::cerr << "Renderer could not be created. " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not be initialized. " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cerr << "Image could not be loaded. " << IMG_GetError() << std::endl;
        return nullptr;
    }

    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!newTexture) {
        std::cerr << "Texture could not be created. " << SDL_GetError() << std::endl;
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

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

void addClientGroupsQueue() {
    int spawn_rate_time = 20000;
    while (true) {
        int clientsForTable = 1 + rand() % 4;
        {
            lock_guard<mutex> guard(mtx);
            clientGroupsQueue.push(clientsForTable);
        }
        cv.notify_all();
        this_thread::sleep_for(chrono::milliseconds(spawn_rate_time));
    }
}

void setArray0(int array[], int length) {
    for (int i = 0; i < length; i++) {
        array[i] = 0;
    }
}

bool checkIngredientsAvailability(int clientOrders[], int clientsForTable) {
    lock_guard<mutex> lock(mtx);
    bool ingredientsAvailable = true;
    for (int i = 0; i < clientsForTable; i++) {
        int ingredientVectorIndex = 0;
        int recipeIndex = clientOrders[i];
        while (ingredientVectorIndex < recipesVector[recipeIndex].ingredients.size()) {
            for (int j = 0; j < ingredientAmount; j++) {
                if (ingredientInventoryVector[j].name == recipesVector[recipeIndex].ingredients[ingredientVectorIndex].first) {
                    neededAmounts[j] += recipesVector[recipeIndex].ingredients[ingredientVectorIndex].second;
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

void substractIngredientsToUse() {
    {
        unique_lock<mutex> lock(mtx); // Lock necessary for the ingredient substraction
        for (int i = 0; i < ingredientAmount; i++) {
            ingredientInventoryVector[i].substractIngredient(neededAmounts[i]);
        }
        setArray0(neededAmounts, ingredientAmount);
    }
}

int cookingDuration(const string& recipeName) {
    string tempName;
    int finalPrep_time;

    srand(static_cast<unsigned int>(time(nullptr)));
    int randomBinary = (rand() % 2 == 0) ? -1 : 1;
    int randomRange = rand() % (PREP_DELAY + 1);

    for (size_t i = 0; i < recipesVector.size(); i++) {
        tempName = recipesVector[i].recipeName;
        if (tempName == recipeName) {
            int approxPrep_time = recipesVector[i].approxPrepTime;
            finalPrep_time = approxPrep_time + (approxPrep_time * randomBinary * randomRange);

            if (finalPrep_time < 0) {
                finalPrep_time = MIN_PREP_TIME;
            }
        }
    }
    return finalPrep_time;
}

int eatingDuration(const string& recipeForClient) {
    string tempName;
    int finalEating_time;

    srand(static_cast<unsigned int>(time(nullptr)));
    int randomBinary = (rand() % 2 == 0) ? -1 : 1;
    int randomRange = rand() % (EATING_DELAY + 1);

    for (size_t i = 0; i < recipesVector.size(); i++) {
        tempName = recipesVector[i].recipeName;
        if (tempName == recipeForClient) {
            int approxEating = recipesVector[i].approxEatingTime;
            finalEating_time = approxEating + (approxEating * randomBinary * randomRange);

            if (finalEating_time < 0) {
                finalEating_time = MIN_EATING_TIME;
            }
        }
    }
    return finalEating_time;
}

void sendOrderToPrepQueue(int ordersArray[], int clientsAmount) {
    {
        unique_lock<mutex> lock(mtx); // Lock necessary for ordersQueue and contForIDs
        for(int i = 0; i < clientsAmount; i++) {
            int recipeNum = ordersArray[i];
            Order orderToQueue(recipesVector[recipeNum].recipeName, contForIDs);
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
            table(thread([this, i] {
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
            }))
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

int main(int argc, char* args[]) {
    bool programLoop = true;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    const int screenWidth = 800;
    const int screenHeight = 600;
    if (!init(&window, &renderer, screenWidth, screenHeight)) {
        std::cerr << "Initialization failed." << std::endl;
        return -1;
    }
    SDL_Texture* backgroundTexture = loadTexture("images/restaurantFloor.png", renderer);
    if (!backgroundTexture) {
        std::cerr << "Texture load failed." << std::endl;
        return -1;
    }
    SDL_Event e;

    readIngredients();
    readRecipes();
    setArray0(neededAmounts, ingredientAmount);
    thread threadAddCG(addClientGroupsQueue);
    Tables tables(6);

    while(programLoop) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                programLoop = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

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
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
