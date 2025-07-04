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

file(GLOB test_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/files/*.py")
foreach(file IN LISTS test_files)
    get_filename_component(file_name ${file} NAME)
    configure_file(${file} "${CMAKE_CURRENT_BINARY_DIR}/${file_name}" COPYONLY)
endforeach()

add_test(NAME ${target} COMMAND ${target})