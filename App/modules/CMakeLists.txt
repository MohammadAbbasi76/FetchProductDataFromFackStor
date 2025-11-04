project(Modules LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

add_library(${PROJECT_NAME}
    ReceiveData.cpp
    DataProcessing.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)



# Include directories - include both current and project include dirs
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)



message(STATUS "Modules")