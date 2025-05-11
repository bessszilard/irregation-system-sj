# Find GoogleTest package
find_package(GTest REQUIRED)

set (SRC
    SolenoidCtrlCmdTest.cpp
    SolenoidManagerTest.cpp
    ../embedded/main/SolenoidManager.cpp
    ../embedded/main/SolenoidCtrlCmd.cpp
    ../embedded/main/Enums.cpp
    ../embedded/main/Utils.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Add test_main.cpp file
add_executable(UnitTests ${SRC})

# Link test executable against GoogleTest libraries
target_link_libraries(UnitTests GTest::GTest GTest::Main)

# Register the test
add_test(NAME UnitTests COMMAND UnitTests)
