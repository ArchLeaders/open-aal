add_executable(open_aal_test
    ./src/include/utils/file_util.h
    ./src/main.cpp
)

target_include_directories(open_aal_test PRIVATE ./src/include)

target_link_libraries(open_aal_test PUBLIC open_aal)

add_test(
    NAME open_aal_test
    COMMAND open_aal_test ./test_file.wav
)