#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL.h>


#define pi 3.14159265358979
#define rad 0.0174532925
#define CHECK_ERROR(test, message) \
    do { \
        if((test)) { \
            fprintf(stderr, "%s\n", (message)); \
            exit(1); \
        } \
    } while(0)

int map_x = 12;
int map_y = 12;
int map_s = 12;
int row;
int col;

int map[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,1,0,0,0,0,0,0,0,0,1,
    1,0,1,0,0,0,0,0,0,0,0,1,
    1,0,1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,0,0,0,1,
    1,0,0,0,0,1,1,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,
};

void draw_map(SDL_Renderer * renderer) {
    for (int row = 0; row < map_x; row++) {
        for (int col = 0; col < map_y; col++) {
            int tile = row * map_s + col;
            SDL_Rect map_rect;
            map_rect.x = 100 * col;
            map_rect.y = 100 * row;
            map_rect.w = 100;
            map_rect.h = 100;
            if (map[tile] == 1) {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            }
            SDL_RenderFillRect(renderer, &map_rect);
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        }
    }
}

void collision() {
    for (int row = 0; row < map_x; row++) {
        for (int col = 0; col < map_y; col++) {
            int tile = row * map_s + col;
            SDL_Rect map_rect;
            map_rect.x = 100 * col;
            map_rect.y = 100 * row;
            map_rect.w = 100;
            map_rect.h = 100;
        }
    }
}

int ray_x = 600; // Start position X
int ray_y = 450; // Start position Y
int theta = 0;
int mouse = 0;
int vel_r = 0;
int mag_x = 200;
int mag_y = 200;
int vel_x = 0;
int vel_y = 0;

int theta_vel = 0;

static const int width = 800;
static const int height = 600;

void* ray(SDL_Renderer* renderer, int start_x, int start_y, int angle) {
    int scan_x = 0;
    for (int i = angle; i - angle < 120; i++) {
        for (int depth = 0; depth < 1000; depth++) {
            int x_prime = start_x - sin((i - 120) * rad) * depth;
            int y_prime = start_y + cos((i - 120) * rad) * depth;
 
            int col = (x_prime / 100);
            int row = (y_prime / 100);
            int tile = row * map_s + col;
            int dist = sqrt(((x_prime - start_x) * (x_prime - start_x) + ((y_prime - start_y) * (y_prime - start_y))));
            float ca = 60 - i;
            if (ca < 0){
              ca += 2*pi;
            }
            if (ca > 2*pi){
              ca -= 2*pi;
            }
            dist = dist*cos((ca) * rad);
            int height = (100 * 600/ (dist + 1));

            SDL_Rect voxel;
            voxel.x = scan_x;
            voxel.y = (600 - height) / 2;
            voxel.w = 10;
            voxel.h = height;

            int red_val = height / 5;
            int green_val = height / 5;
            int blue_val = height / 5;

            if (red_val > 254){
              red_val = 255;
            }
            if (green_val > 254){
              green_val = 255;
            }
            if (blue_val > 254){
              blue_val = 255;
            }

            
            if (map[tile] == 1) {
                SDL_SetRenderDrawColor(renderer, red_val, green_val, blue_val, 0xFF);
                SDL_RenderFillRect(renderer, &voxel);
                //SDL_RenderDrawLine(renderer, start_x, start_y, x_prime, y_prime);
                break;
            }
        }
        scan_x += 10;
    }
    return NULL;
}

void crosshair(SDL_Renderer* renderer, int screen_width, int screen_height) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawLine(renderer, (screen_width / 2), (screen_height / 2) - 10, (screen_width / 2), (screen_height / 2) + 10);
    SDL_RenderDrawLine(renderer, (screen_width / 2) - 10, (screen_height / 2), (screen_width / 2) + 10, (screen_height / 2));
}

int main(int argc, char** argv) {
    srand(time(NULL));

    CHECK_ERROR(SDL_Init(SDL_INIT_VIDEO) != 0, SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Raycasting", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
    CHECK_ERROR(window == NULL, SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    CHECK_ERROR(renderer == NULL, SDL_GetError());

    bool running = true;
    SDL_Event event;
    while (running) {
        ray_x += vel_x;
        ray_y += vel_y;
        theta += theta_vel;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                const char* key = SDL_GetKeyName(event.key.keysym.sym);
                if (strcmp(key, "Q") == 0) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    vel_x = ((mag_x * cos((theta) * rad)) / 30);
                    vel_y = ((mag_y * sin((theta) * rad)) / 30);
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    vel_x = -1 * ((mag_x * cos((theta) * rad)) / 30);
                    vel_y = -1 * ((mag_y * sin((theta) * rad)) / 30);
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    vel_x = ((mag_x * cos((theta - 90) * rad)) / 30);
                    vel_y = ((mag_y * sin((theta - 90) * rad)) / 30);
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    vel_x = ((mag_x * cos((theta + 90) * rad)) / 30);
                    vel_y = ((mag_y * sin((theta + 90) * rad)) / 30);
                }
            }
            else if (event.type == SDL_KEYUP) {
                vel_x = 0;
                vel_y = 0;
                theta_vel = 0;
            }
            else if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&theta, NULL);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        //draw_map(renderer);
        ray(renderer, ray_x, ray_y, theta);
        crosshair(renderer, width, height);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

        SDL_RenderPresent(renderer);

        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
