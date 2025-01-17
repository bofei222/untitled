project(untitled)
# Adding unit tests part of the "arrow" portion of the test suite
# Adding unit tests part of the "arrow" portion of the test suite

find_package(Arrow REQUIRED)
find_package(ArrowAcero REQUIRED)
find_package(Parquet REQUIRED)
#find_package(ArrowTesting REQUIRED)
find_package(GTest REQUIRED)
# 查找 GTest 库
find_package(GTest REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(export32 ${PROJECT_SOURCE_DIR}/
        src-export32/example.cpp
        src-export32/main.c)
target_link_libraries(export32 PRIVATE arrow_acero parquet arrow)

#add_executable(arrow_example src-example/arrow_example.cc)
#target_link_libraries(arrow_example PRIVATE Arrow::arrow_shared)


#add_executable(arrow_utils
#        src-mytest/main.cpp
#        src-mytest/arrow_utils.cpp
#        src-mytest/arrow_utils.h)
#target_link_libraries(arrow_utils PRIVATE Arrow::arrow_shared GTest::gtest)


#add_executable(bridge_test
#        src-test/bridge_test.cc)
#target_link_libraries(bridge_test PRIVATE Arrow::arrow_shared GTest::gtest GTest::Main)



#add_executable(gtest_util_test
#        src-test/gtest_util_test.cc)
#target_link_libraries(gtest_util_test PRIVATE Arrow::arrow_shared  GTest::gtest GTest::Main )




#add_executable(main src-gtest/main.cpp
#        src-gtest/main.h)
#target_link_libraries(main PRIVATE Arrow::arrow_shared  GTest::gtest)