project( EllipticBoundaryPrediction )

cmake_minimum_required( VERSION 2.8 )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( EllipticBoundaryPrediction EllipticBoundaryPrediction.cpp )
target_link_libraries( EllipticBoundaryPrediction ${OpenCV_LIBS} )
