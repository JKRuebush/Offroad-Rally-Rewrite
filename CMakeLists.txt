cmake_minimum_required(VERSION 3.10.0)
project(OffroadRally VERSION 1.0.0)

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
        src/map/hex-direction.cpp
        src/map/rally-map.cpp

        test/main-test.cpp 

        test/map/rally-map-test.cpp
    )
    target_include_directories(RallyTest PUBLIC 
        includes
        includes/map
        includes/agent
    )
    target_link_libraries(RallyTest GTest::GTest)
    gtest_discover_tests(RallyTest)
endif()

add_executable(OffroadRally 
    src/main.cpp 

    src/agent/agent-manager.cpp
    src/agent/agent-wrapper.cpp

    src/map/hex-direction.cpp
    src/map/map-interface.cpp
    src/map/rally-map.cpp

    src/agent-impl/agentAStar.cpp
    src/agent-impl/agentAStarOpt.cpp
    
    src/agent-impl/agentDijkstra.cpp
    src/agent-impl/agentDijkstraOpt.cpp

    src/agent-impl/agentCrow.cpp
    # src/agent-impl/agentNop.cpp
    # src/agent-impl/agentOneStep.cpp
    # src/agent-impl/agentRandomWalk.cpp
)
target_include_directories(OffroadRally PUBLIC 
    includes
    includes/map
    includes/agent
)
target_compile_features(OffroadRally PUBLIC cxx_std_11)
