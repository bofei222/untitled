project(untitled)

find_package(Arrow REQUIRED)
find_package(ArrowAcero REQUIRED)
find_package(Parquet REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(untitled2 ${PROJECT_SOURCE_DIR}/
        src/sequence_plan2.cpp)

target_link_libraries(untitled2 PRIVATE   arrow_acero parquet arrow)



