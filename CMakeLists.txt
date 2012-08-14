project( FaceContour )

cmake_minimum_required( VERSION 2.8 )

find_package( OpenCV REQUIRED )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${FaceContour_BINARY_DIR}/bin
)

add_subdirectory( Gaussian )
add_subdirectory( StatisticFile )
add_subdirectory( Histograms )
add_subdirectory( EllipticBoundary )
add_subdirectory( MixtureGaussian )
add_subdirectory( BayesClassifier )
