project(untitled)

find_package(Arrow REQUIRED)
find_package(ArrowAcero REQUIRED)
find_package(Parquet REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(example ${PROJECT_SOURCE_DIR}/

        src_share_point/example.cpp
        src_share_point/main.c
)

target_link_libraries(example PRIVATE arrow_acero parquet arrow)



