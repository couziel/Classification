#include <iostream>

#include "cv.h"
#include "highgui.h"

cv::Mat histogram(cv::Mat image,cv::Mat mask)
{
    int rangeMax=256;
    int histSize = rangeMax;
  float range[] = { 0, rangeMax } ;
  const float* histRange = { range };
  bool uniform = true;
  bool accumulate = false;
  cv::Mat hist;

  cv::calcHist( &image, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate );

  //Draw the histogram
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );
  cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

  normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

  for( int i = 1; i < histSize; i++ )
  {
      cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) , cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ), cv::Scalar( 255, 0, 255), 2, 8, 0  );
  }

    return histImage;
}

cv::Mat channelHistogram(cv::Mat image,cv::Mat mask,int minRange, int maxRange, int maxRange2)
{

    std::vector<cv::Mat> channels;
    cv::split( image, channels );

    int histSize = maxRange-minRange;
    float range[] = { minRange, maxRange } ;
    const float* histRange = { range };
    bool uniform = true;
    bool accumulate = false;
    cv::Mat b_hist, g_hist, r_hist;
    if (maxRange2!=0)
    {
        int histSize2 = maxRange2-minRange;
        float range2[] = { minRange, maxRange2 } ;
        const float* histRange2 = { range2 };
        cv::calcHist( &channels[0], 1, 0, mask, b_hist, 1, &histSize2, &histRange2, uniform, accumulate );
    }
    else
    {
        cv::calcHist( &channels[0], 1, 0, mask, b_hist, 1, &histSize, &histRange, uniform, accumulate );
    }
    cv::calcHist( &channels[1], 1, 0, mask, g_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist( &channels[2], 1, 0, mask, r_hist, 1, &histSize, &histRange, uniform, accumulate );

    //Draw the histograms
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );
    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

    normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

    for( int i = 1; i < histSize; i++ )
    {
        if(i<(maxRange2-minRange))
        {
            cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) , cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ), cv::Scalar( 255, 0, 0), 2, 8, 0  );
        }
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) , cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ), cv::Scalar( 0, 255, 0), 2, 8, 0  );
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) , cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ), cv::Scalar( 0, 0, 255), 2, 8, 0  );
    }

return histImage;

}
int main( int argc, char* argv[] )
{
  if( argc != 4)
    {
    std::cerr << "Usage: " << argv[0] << " <InputImage>" << std::endl;
    return EXIT_FAILURE;
    }

  cv::Mat image = cv::imread( argv[1] );
  if(!image.data)
    {
    return EXIT_FAILURE;
    }
  cv::Mat image_gray = cv::imread( argv[1],0 );
  if(!image_gray.data)
    {
    return EXIT_FAILURE;
    }
  cv::Mat mask = cv::imread( argv[2],0 );
  if(!mask.data)
    {
    return EXIT_FAILURE;
    }

    std::vector<cv::Mat> channels;
    cv::cvtColor( image, image, CV_BGR2YCrCb );
    cv::split( image, channels );
    //cv::Mat hist=histogram(channels[0],cv::Mat());
   // cv::Mat histmask=histogram(channels[0],mask);
    cv::Mat hist=channelHistogram(image,cv::Mat(),16,240,235);
    cv::Mat histmask=channelHistogram(image,mask,16,240,235);
    cv::namedWindow( "histogram", CV_WINDOW_AUTOSIZE );
    cv::imshow( "histogram", hist );
    cv::namedWindow( "mask histogram", CV_WINDOW_AUTOSIZE );
    cv::imshow( "mask histogram", histmask );
    cv::waitKey(0);

    cv::imwrite(argv[3],histmask);
    return 0;
}
