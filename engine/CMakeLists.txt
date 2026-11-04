add_library(vibe_engine STATIC
  src/core/Application.cpp
  src/core/Log.cpp
  src/core/Types.cpp
  src/project/Project.cpp
  src/scene/Scene.cpp
  src/platform/sdl/SDLPlatform.cpp
)

target_include_directories(vibe_engine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vibe_engine
  PUBLIC
    nlohmann_json::nlohmann_json
    SDL2::SDL2
)

target_compile_definitions(vibe_engine
  PRIVATE
    VIBE_ENGINE_VERSION="${PROJECT_VERSION}"
)
