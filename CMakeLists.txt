cmake_minimum_required(VERSION 3.10.0)
project(OffroadRally VERSION 0.1.0)

include(CMakeDependentOption)

option(DEVELOPER "Use development build options.")
cmake_dependent_option(BUILD_TEST "Include tests in the build." ON
    "DEVELOPER" OFF)

if(DEVELOPER)
    if(MSVC)
        add_compile_options(/W3)
    else()
        add_compile_options(-Wall)
    endif()
endif()

if(BUILD_TEST)
    include(CTest)
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)

    add_executable(RallyTest 
        test/testMain.cpp 
        test/mapTests.cpp 
        src/map/rallyMap.cpp
    )
    target_include_directories(RallyTest PUBLIC includes)
    target_link_libraries(RallyTest GTest::GTest)
    gtest_discover_tests(RallyTest)
endif()

add_executable(OffroadRally 
    src/main.cpp 
    src/agentManager.cpp
    src/agentWrapper.cpp
    src/map/mapInterface.cpp
    src/map/rallyMap.cpp

    src/agents/agentAStar.cpp
    # src/agents/agentAStarD.cpp
    src/agents/agentAStarDM.cpp
    # src/agents/agentAStarM.cpp
    src/agents/agentCrow.cpp
    src/agents/agentDijkstra.cpp
    # src/agents/agentDijkstraD.cpp
    src/agents/agentDijkstraDM.cpp
    # src/agents/agentDijkstraM.cpp
    # src/agents/agentNop.cpp
    # src/agents/agentOneStep.cpp
    # src/agents/agentRandomWalk.cpp
)
target_include_directories(OffroadRally PUBLIC includes)
target_compile_features(OffroadRally PUBLIC cxx_std_11)
