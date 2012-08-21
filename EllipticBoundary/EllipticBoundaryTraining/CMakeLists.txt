project( EllipticBoundaryTraining )

cmake_minimum_required( VERSION 2.8 )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( EllipticBoundaryTraining EllipticBoundaryTraining.cpp )
target_link_libraries( EllipticBoundaryTraining ${OpenCV_LIBS} )
