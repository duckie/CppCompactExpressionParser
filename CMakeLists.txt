# Developed with CMake 2.8, but may work with older versions
cmake_minimum_required (VERSION 2.8)

project(CppCompactExpressionParser)
set(CMAKE_VERBOSE_MAKEFILE OFF)

### Commons includes ###
include_directories(
	${PROJECT_SOURCE_DIR}/ 
	${PROJECT_BINARY_DIR}/
	)

set(LIBRARIES)
### Boost support ###
FIND_PACKAGE(Boost)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
    # GCC does not support the boost auto link feature
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(LIBRARIES "${LIBRARIES}" "${Boost_LIBRARIES}")
    endif(CMAKE_COMPILER_IS_GNUCXX)
endif()

# C++11
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

### Common source files ###
set(SOURCES_FILES CompactExpressionParser/Expression.cpp)

### Compiling ###
add_executable(run_samples Main.cpp ${SOURCES_FILES})
### Linking ###
target_link_libraries(run_samples ${LIBRARIES})
