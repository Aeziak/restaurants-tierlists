cmake_minimum_required(VERSION 3.15)
project(restaurant_tier_list_backend CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/controllers/HealthController.cc
)

# Find Drogon
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

# Find jwt-cpp
find_package(jwt-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE jwt-cpp::jwt-cpp)

# Find redis-plus-plus
find_package(redis++ CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE redis++::redis++)

# Find PostgreSQL
find_package(PostgreSQL REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PostgreSQL_LIBRARIES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
