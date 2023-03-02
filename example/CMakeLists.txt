find_package(unifex CONFIG REQUIRED)

add_executable(example example.cpp)

target_link_libraries(example PRIVATE unifex::unifex variate::variate)
