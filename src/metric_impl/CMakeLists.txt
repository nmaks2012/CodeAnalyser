set(target metric_test)

add_executable(${target}
    tests/code_lines_count.cpp
    tests/cyclomatic_complexity.cpp
    tests/parameters_count.cpp
    tests/naming_style.cpp
)

target_link_libraries(${target}
    PRIVATE
        GTest::GTest
        GTest::Main
        metric
        function
        file
)

# Определяем путь к директории с тестовыми данными
set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/files")
# Передаем путь в C++ код через макрос препроцессора
target_compile_definitions(${target} PRIVATE TEST_DATA_DIR="${TEST_DATA_DIR}")

add_test(NAME ${target} COMMAND ${target})
