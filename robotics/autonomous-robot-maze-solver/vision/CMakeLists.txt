# cmake needs this line
cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME maze_vision)
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(OpenCV_DIR ${PROJECT_DIR}/opencv/build/installation/OpenCV-3.4.6/share/OpenCV)

set(Common_DIR ${PROJECT_DIR}/../common)

message(STATUS "OpenCv_DIR: ${OpenCV_DIR}")

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable C++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


# Define project name
project(${PROJECT_NAME})

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV 3.4.6 REQUIRED)
#find_package(libserial 0 REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

set(EXTERNAL_LIBS ${OpenCV_LIBS} serial pthread)

include_directories(
    include
    ${Common_DIR}
    )

add_library(utils
  src/math_utils.cpp
  )
target_link_libraries(utils
  )

# Declare the executable target built from your sources
add_executable(aruko_generator 
 src/aruko_generator_main.cpp
 )

# Link your application with OpenCV libraries
target_link_libraries(aruko_generator 
    aruko_localiser
    ${OpenCV_LIBS})

add_library(maze
    src/maze.cpp
    src/maze_template.cpp
    )
target_link_libraries(maze
    utils
    ${OpenCV_LIBS}
    )

add_library(aruko_localiser
  src/aruko_generator.cpp
  src/aruko_localiser.cpp
  )

target_link_libraries(aruko_localiser 
    maze
    ${OpenCV_LIBS})

add_executable(aruko_localiser_test
  src/aruko_localiser_test.cpp
  )
target_link_libraries(aruko_localiser_test
    aruko_localiser
    ${OpenCV_LIBS}
    )

add_executable(line_extractor_test
  src/line_extractor_test.cpp
  src/maze_template.cpp
  )

add_library(line_extractor
  src/line_extractor.cpp
)

target_link_libraries(line_extractor
    maze
    )

target_link_libraries(line_extractor_test
    line_extractor
    ${OpenCV_LIBS}
    )

add_library(robot_interface
    src/robot_interface.cpp
    )

target_link_libraries(robot_interface
    maze
    serial
    pthread
    )

add_executable(robot_interface_test
    src/robot_interface_test.cpp
    )

target_link_libraries(robot_interface_test
    robot_interface
    serial
    pthread
    )

add_executable(image_capture_test
    src/image_capture_test
    )
target_link_libraries(image_capture_test
    ${OpenCV_LIBS}
    )

add_executable(charuko_calibrate
    src/calibrate_camera_charuco.cpp
    )
target_link_libraries(charuko_calibrate
    ${OpenCV_LIBS}
    )

add_library(maze_view
    src/maze_view.cpp
    )
target_link_libraries(maze_view
    aruko_localiser
    line_extractor
    robot_interface
    maze
    ${OpenCV_LIBS}
    )

add_executable(phase_b_test
    src/phase_b_test.cpp
    )

target_link_libraries(phase_b_test
    maze_view
    ${EXTERNAL_LIBS}
    ${OpenCV_LIBS}
    )

