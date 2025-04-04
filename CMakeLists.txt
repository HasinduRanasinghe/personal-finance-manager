cmake_minimum_required(VERSION 3.14)
project(PersonalFinanceManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt5 COMPONENTS Core Widgets Charts REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/ExpenseManager.cpp
    src/ui/MainWindow.cpp
)

# Header files
set(HEADERS
    include/core/ExpenseManager.h
    include/core/Expense.h
    include/core/Category.h
    include/ui/MainWindow.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set include directories for target
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Charts
)

# Copy nlohmann/json
file(COPY ${CMAKE_SOURCE_DIR}/external/nlohmann/json.hpp DESTINATION ${CMAKE_BINARY_DIR}/include/nlohmann)

# Install targets
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy resources
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/resources
)