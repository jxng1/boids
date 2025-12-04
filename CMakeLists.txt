cmake_minimum_required(VERSION 3.28)
project(boids LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Only include cpp files
file(GLOB SOURCES "src/*.cpp")
add_executable(boids ${SOURCES})

# Link all required SFML components
target_link_libraries(boids PRIVATE SFML::Graphics SFML::Window SFML::System)
