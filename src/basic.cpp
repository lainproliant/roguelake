/*
 * basic.cpp
 *
 * Author: Lain Musgrove (lain.proliant@gmail.com)
 * Date: Monday November 25, 2024
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <sstream>

// ------------------------------------------------------------------
const char* WINDOW_TITLE = "Basic SDL3 Example";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// ------------------------------------------------------------------
struct Context {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* lake_texture = nullptr;
    SDL_Texture* window_size_texture;
    SDL_Event event;

    SDL_FRect window_rect = {.x=0, .y=0, .w=0, .h=0};
    SDL_FRect lake_rect;
    SDL_FRect window_size_rect;

    TTF_Font* font = nullptr;

    bool running = true;
};

// ------------------------------------------------------------------
void die(const std::string& msg) {
    if (std::string(SDL_GetError()).length() != 0) {
        const std::string msg_fmt = msg + "\n    -> %s";
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg_fmt.c_str(), SDL_GetError());

    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str());
    }

    exit(1);
}

// ------------------------------------------------------------------
void update_window_size(Context& ctx) {
    int w, h;
    SDL_GetCurrentRenderOutputSize(ctx.renderer, &w, &h);
    ctx.window_rect.w = w;
    ctx.window_rect.h = h;

    ctx.lake_rect.x = (ctx.window_rect.w / 2.0) - (ctx.lake_texture->w / 2.0);
    ctx.lake_rect.y = (ctx.window_rect.h / 2.0) - (ctx.lake_texture->h / 2.0);
    ctx.lake_rect.w = ctx.lake_texture->w;
    ctx.lake_rect.h = ctx.lake_texture->h;

    std::ostringstream sb;
    sb << w << "x" << h;
    auto surface = TTF_RenderText_Solid(ctx.font, sb.str().c_str(), sb.str().length(), {0xff, 0xff, 0xff, 0});
    ctx.window_size_texture = SDL_CreateTextureFromSurface(ctx.renderer, surface);
    ctx.window_size_rect = {
        .x = ctx.window_rect.w - ctx.window_size_texture->w,
        .y = 0,
        .w = static_cast<float>(ctx.window_size_texture->w),
        .h = static_cast<float>(ctx.window_size_texture->h)
    };
}

// ------------------------------------------------------------------
void initialize(Context& ctx) {
    if (! SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        die("Failed to initialize SDL3 subsystems.");
    }

    if (! SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &ctx.window, &ctx.renderer)) {
        die("Failed to create SDL3 window.");
    }

    if (! TTF_Init()) {
        die("Failed to initialize SDL_ttf.");
    }

    ctx.font = TTF_OpenFont("assets/nes.ttf", 16);
    auto surface = IMG_Load("assets/basic.png");
    ctx.lake_texture = SDL_CreateTextureFromSurface(ctx.renderer, surface);

    update_window_size(ctx);
}

// ------------------------------------------------------------------
void cleanup(Context& ctx) {
    if (ctx.lake_texture != nullptr) {
        SDL_DestroyTexture(ctx.lake_texture);
        ctx.lake_texture = nullptr;
    }

    if (ctx.window_size_texture != nullptr) {
        SDL_DestroyTexture(ctx.window_size_texture);
        ctx.window_size_texture = nullptr;
    }

    if (ctx.renderer != nullptr) {
        SDL_DestroyRenderer(ctx.renderer);
        ctx.renderer = nullptr;
    }

    if (ctx.window != nullptr) {
        SDL_DestroyWindow(ctx.window);
        ctx.window = nullptr;
    }
}

// ------------------------------------------------------------------
int main(int argc, char** argv) {
    Context ctx;

    initialize(ctx);

    while (ctx.running) {
        while (SDL_PollEvent(&ctx.event)) {
            switch (ctx.event.type) {
            case SDL_EVENT_QUIT:
                ctx.running = false;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                update_window_size(ctx);
                break;
            default:
                break;
            }
        }

        SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 0);
        SDL_RenderClear(ctx.renderer);
        SDL_RenderTexture(ctx.renderer, ctx.lake_texture, nullptr, &ctx.lake_rect);
        SDL_RenderTexture(ctx.renderer, ctx.window_size_texture, nullptr, &ctx.window_size_rect);
        SDL_RenderPresent(ctx.renderer);
        SDL_Delay(100);
    }

    cleanup(ctx);

    return 0;
}
