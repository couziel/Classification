project( Histograms )

cmake_minimum_required( VERSION 2.8 )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( Histograms Histograms.cpp )
target_link_libraries( Histograms ${OpenCV_LIBS} )
