project(untitled)
set(CMAKE_CXX_STANDARD 20)
find_package(Arrow REQUIRED)
find_package(ArrowAcero REQUIRED)
find_package(Parquet REQUIRED)




include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(lib1 SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/sequence_plan.cpp)