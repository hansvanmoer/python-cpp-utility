#
# Configuration file for the test executable
#

include_directories(..)

add_executable(python-script-util-tests Test.cpp main.cpp)
target_link_libraries(python-script-util-tests python-cpp-util python boost-python)
