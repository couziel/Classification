project( StatisticFile )

cmake_minimum_required( VERSION 2.8 )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} ${NIFTILIB_SOURCE_DIR} ${ZNZLIB_SOURCE_DIR} ${FSLIOLIB_SOURCE_DIR})

add_executable( StatisticFile StatisticFile.cpp )
target_link_libraries( StatisticFile ${OpenCV_LIBS} )
