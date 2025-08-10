# Find GoogleTest package
find_package(GTest REQUIRED)

set (SRC
    main.cpp
    ../embedded/main/SolenoidManager.cpp
    ../embedded/main/SolenoidCtrlCmd.cpp
    ../embedded/main/Enums.cpp
    ../embedded/main/Utils.cpp
    ../embedded/main/RelayGroupManager.cpp
    ../embedded/main/Structures.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Add test_main.cpp file
add_executable(simulation ${SRC})

# Link test executable against GoogleTest libraries
# Register the test
# add_test(NAME UnitTests COMMAND UnitTests)
