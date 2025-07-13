set(target cmd_options_test)

add_executable(${target}
    tests/cmd_options.cpp
)

target_link_libraries(${target}
    PRIVATE
        GTest::GTest
        GTest::Main
        range-v3::range-v3
        cmd_options
        ${Boost_LIBRARIES}
)

add_test(NAME ${target} COMMAND ${target})