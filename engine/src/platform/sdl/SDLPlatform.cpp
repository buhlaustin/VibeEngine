#include "vibe/platform/Platform.hpp"

#include "vibe/core/Log.hpp"

#include <SDL.h>

namespace vibe {

namespace {

class SDLPlatform final : public Platform {
 public:
  bool initialize(const PlatformInit& init) override {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
      log(LogLevel::Error, std::string("SDL_Init failed: ") + SDL_GetError());
      return false;
    }

    window_ = SDL_CreateWindow(init.windowTitle.data(), SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, init.width, init.height,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window_) {
      log(LogLevel::Error, std::string("SDL_CreateWindow failed: ") + SDL_GetError());
      SDL_Quit();
      return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
      log(LogLevel::Error, std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
      SDL_DestroyWindow(window_);
      window_ = nullptr;
      SDL_Quit();
      return false;
    }

    return true;
  }

  void shutdown() override {
    if (renderer_) {
      SDL_DestroyRenderer(renderer_);
      renderer_ = nullptr;
    }
    if (window_) {
      SDL_DestroyWindow(window_);
      window_ = nullptr;
    }
    SDL_Quit();
  }

  void pollEvents() override {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        shouldClose_ = true;
      }
    }
  }

  bool shouldClose() const override { return shouldClose_; }

  void beginFrame() override {}

  void endFrame() override {
    if (renderer_) {
      SDL_RenderPresent(renderer_);
    }
  }

  void clear(float r, float g, float b) override {
    if (renderer_) {
      SDL_SetRenderDrawColor(renderer_, static_cast<Uint8>(r * 255.f),
                             static_cast<Uint8>(g * 255.f), static_cast<Uint8>(b * 255.f), 255);
      SDL_RenderClear(renderer_);
    }
  }

 private:
  SDL_Window* window_{nullptr};
  SDL_Renderer* renderer_{nullptr};
  bool shouldClose_{false};
};

}  // namespace

std::unique_ptr<Platform> createPlatform() {
  return std::make_unique<SDLPlatform>();
}

}  // namespace vibe
