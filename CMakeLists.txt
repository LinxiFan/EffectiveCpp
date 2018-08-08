# Following Meyer's book "Effective Modern C++"
cmake_minimum_required(VERSION 3.00)
project(EffectiveCpp)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS
    program_options
    thread
    system
    chrono
    date_time
REQUIRED)

# SYSTEM arg tells compiler that the path contains system files. Compiler will thus ignore warning.
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

function(add_boost target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} ${Boost_LIBRARIES})
endfunction()

function(link_boost target)
    target_link_libraries(${target} ${Boost_LIBRARIES})
endfunction()


foreach(FILE_NAME
        cpp17_fold boost_hana
        ch1_deducing_types ch2_auto ch3_moving_to_modern
        ch3_typetraits ch3_enum ch3_class_qualifier ch3_constexpr
        ch4_smartpointers)
    add_boost(${FILE_NAME} ${FILE_NAME}.cpp)
endforeach()


#add_boost(ch1_deducing_types ch1_deducing_types.cpp)
#add_boost(ch2_auto ch2_auto.cpp)
#add_boost(ch3_moving_to_modern ch3_moving_to_modern.cpp)
#add_boost(ch3_typetraits ch3_typetraits.cpp)
#add_boost(ch3_enum ch3_enum.cpp)
#add_boost(ch3_class_qualifier ch3_class_qualifier.cpp)
