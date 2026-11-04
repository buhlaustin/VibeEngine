cmake_minimum_required(VERSION 3.20)
project(VibeEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VIBE_BUILD_RUNTIME "Build the Vibe runtime executable" ON)
option(VIBE_BUILD_TESTS "Build engine tests" OFF)

include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(SDL2 REQUIRED)

add_subdirectory(engine)

if(VIBE_BUILD_RUNTIME)
  add_executable(vibe-runtime engine/src/main/RuntimeMain.cpp)
  target_link_libraries(vibe-runtime PRIVATE vibe_engine SDL2::SDL2)
  target_include_directories(vibe-runtime PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)
endif()

install(TARGETS vibe-runtime RUNTIME DESTINATION bin)
install(DIRECTORY templates/ DESTINATION share/vibe/templates)
