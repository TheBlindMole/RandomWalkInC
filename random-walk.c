#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define WIDTH 900
#define HEIGHT 600

typedef struct {
    float r;
    float g;
    float b;
} RGB;

typedef struct {
    int x;
    int y;
    RGB color;
} Agent;

typedef struct {
    int vx;
    int vy;
} Velocity;

int scale = 10;
int agent_size = 2;

float hue2rgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

RGB hsl2rgb(float h, float s, float l) {
    RGB rgb;
    if (s == 0.0f) {
        rgb.r = rgb.g = rgb.b = l * 255.0f;
    } else {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        rgb.r = hue2rgb(p, q, h + 1.0f / 3.0f) * 255.0f;
        rgb.g = hue2rgb(p, q, h) * 255.0f;
        rgb.b = hue2rgb(p, q, h - 1.0f / 3.0f) * 255.0f;
    }
    return rgb;
}

Velocity get_rand_v() {
    int choice = rand() % 4;
    Velocity v;
    switch (choice) {
        case 0:
            v.vx = 0; v.vy = -1;
            return v;
        case 1:
            v.vx = 0; v.vy = 1;
            return v;
        case 2:
            v.vx = -1; v.vy = 0;
            return v;
        case 3:
            v.vx = 1; v.vy = 0;
            return v;
        default:
            fprintf(stderr, "Valor aleatorio impossivel: %d\n", choice);
            exit(-1);
    }
}

void create_agents(Agent *pagents, int num_agents) {
    for (int i = 0; i < num_agents; i++) {
        pagents[i].x = WIDTH / 2;
        pagents[i].y = HEIGHT / 2;
        
        float h = (float)rand() / (float)RAND_MAX;
        pagents[i].color = hsl2rgb(h, 1.0f, 0.5f);
    }
}

void move_agent(SDL_Surface *psurface, Agent *pagent) {
    Velocity v = get_rand_v();
    Uint32 color = SDL_MapRGB(psurface->format, (Uint8)pagent->color.r, (Uint8)pagent->color.g, (Uint8)pagent->color.b);
    
    for (int i = 0; i < scale; i++) {
        pagent->x += v.vx;
        pagent->y += v.vy;
        
        SDL_Rect rect = {pagent->x, pagent->y, agent_size, agent_size};
        SDL_FillRect(psurface, &rect, color);
    }
}

int main(int argc, char *argv[]) {
    int num_agents = 5;
    if (argc == 2) {
        num_agents = atoi(argv[1]);
    } else if (argc > 2) {
        fprintf(stderr, "Uso: %s [num_agents]\n", argv[0]);
        return -1;
    }

    srand((unsigned int)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window *pwindow = SDL_CreateWindow("Random Walk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (pwindow == NULL) {
        SDL_Quit();
        return 1;
    }

    Agent *pagents = malloc(num_agents * sizeof(Agent));
    if (pagents == NULL) {
        SDL_DestroyWindow(pwindow);
        SDL_Quit();
        return 1;
    }

    create_agents(pagents, num_agents);

    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);
    SDL_FillRect(psurface, NULL, SDL_MapRGB(psurface->format, 0, 0, 0));

    int app_running = 1;
    while (app_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app_running = 0;
            }
        }

        for (int i = 0; i < num_agents; i++) {
            move_agent(psurface, &pagents[i]);
        }

        SDL_UpdateWindowSurface(pwindow);
        SDL_Delay(20);
    }

    free(pagents);
    SDL_DestroyWindow(pwindow);
    SDL_Quit();

    return 0;
}
