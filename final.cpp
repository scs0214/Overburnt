#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

// Constantes para el tamaño de la ventana
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Función para inicializar SDL y crear la ventana
bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "No se pudo inicializar SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Overburnt Memory Leaks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        std::cerr << "No se pudo crear la ventana: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        std::cerr << "No se pudo crear el renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "No se pudo inicializar SDL_image: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

// Función para cargar una textura desde un archivo de imagen
SDL_Texture* loadTexture(const std::string &path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
    if (newTexture == nullptr) {
        std::cerr << "No se pudo cargar la textura: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}

// Función para cerrar SDL y liberar recursos
void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    if (!init(&window, &renderer)) {
        std::cerr << "Fallo en la inicialización." << std::endl;
        return -1;
    }

    texture = loadTexture("images/floor.jpg", renderer);
    if (texture == nullptr) {
        std::cerr << "Fallo en la carga de la imagen." << std::endl;
        close(window, renderer, texture);
        return -1;
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    close(window, renderer, texture);
    return 0;
}