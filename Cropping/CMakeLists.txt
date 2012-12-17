project( Cropping )

cmake_minimum_required( VERSION 2.8 )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( Cropping Main.cpp Landmark.h Crop.cpp Crop.h)
target_link_libraries( Cropping ${OpenCV_LIBS} )
