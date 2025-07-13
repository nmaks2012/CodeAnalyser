
set(target analyse_test)

add_executable(${target}
    tests/analyse.cpp
)

target_link_libraries(${target}
    PRIVATE
        GTest::GTest
        GTest::Main
        range-v3::range-v3
        metric_accumulator
        metric
        ${Boost_LIBRARIES}
)

# Определяем путь к директории с тестовыми данными
set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/files")
# Передаем путь в C++ код через макрос препроцессора
target_compile_definitions(${target} PRIVATE TEST_DATA_DIR="${TEST_DATA_DIR}")

add_test(NAME ${target} COMMAND ${target})