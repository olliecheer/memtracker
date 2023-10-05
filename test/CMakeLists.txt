
enable_testing()
include(GoogleTest)

link_libraries(GTest::gtest_main)

add_executable(printer_test printer_test.cpp)
target_link_libraries(printer_test printer)


add_executable(shmbuf_test shmbuf_test.cpp)
target_link_libraries(shmbuf_test shmbuf)



gtest_discover_tests(
        printer_test
        shmbuf_test
)