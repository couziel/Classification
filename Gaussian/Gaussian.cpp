#include <iostream>
#include "math.h"

#include "cv.h"
#include "highgui.h"

cv::Mat Histogram(cv::Mat image,cv::Mat mask, int minRange, int maxRange)
{

   int histSize = maxRange-minRange;
   float range[] = { minRange, maxRange } ;
   const float* histRange = { range };
   bool uniform = true;
   bool accumulate = false;
   cv::Mat hist;

   cv::calcHist( &image, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate );

   return hist;

}

int main( int argc, char* argv[] )
{
  if( argc != 4 )
    {
    std::cerr << "Usage: " << argv[0] << " <InputImage> <mask> <channel>" << std::endl;
    return EXIT_FAILURE;
    }
  cv::Mat image = cv::imread( argv[1],1);
  if(!image.data)
    {
    return EXIT_FAILURE;
    }
  cv::Mat mask = cv::imread( argv[2],0 );
  if(!mask.data)
    {
    return EXIT_FAILURE;
    }

  int minRange, maxRange;
  cv::Mat im;

  if (strcmp(argv[3],"B")==0 || strcmp(argv[3],"G")==0 || strcmp(argv[3],"R")==0)
  {
     maxRange=256;
     minRange=0;
     std::vector<cv::Mat> channels;
     cv::split( image, channels );
     if (strcmp(argv[3],"B")==0)
         im=channels[0];
     else if(strcmp(argv[3],"G")==0)
         im=channels[1];
     else
         im=channels[2];
  }
  else if (strcmp(argv[3],"Y")==0 || strcmp(argv[3],"Cr")==0 || strcmp(argv[3],"Cb")==0)
  {
     minRange=16;
     cv::Mat YCrCb;
     cv::cvtColor( image, YCrCb, CV_BGR2YCrCb );
     std::vector<cv::Mat> channels;
     cv::split( YCrCb, channels );
     if (strcmp(argv[3],"Y")==0)
     {
         im=channels[0];
         maxRange=235;
     }
     else
     {
         maxRange=240;
         if (strcmp(argv[3],"Cr")==0)
             im=channels[1];
         else
             im=channels[2];
     }
  }
  else if (strcmp(argv[3],"H")==0 || strcmp(argv[3],"S")==0 || strcmp(argv[3],"V")==0)
  {
     minRange=0;
     cv::Mat HSV;
     cv::cvtColor( image, HSV, CV_BGR2HSV );
     std::vector<cv::Mat> channels;
     cv::split( HSV, channels );
     if (strcmp(argv[3],"H")==0)
     {
         im=channels[0];
         maxRange=180;
     }
     else
     {
         maxRange=256;
         if (strcmp(argv[3],"S")==0)
             im=channels[1];
         else
             im=channels[2];
     }
  }

  cv::Mat hist=Histogram(im,mask,minRange,maxRange);

  float mu=0, sigma=0;
  int den=0;

  //mu
  for (int i=0;i<hist.rows;i++)
  {
      den+=hist.at<float>(i);
      mu+=i*hist.at<float>(i);
  }
  mu/=den;

//sigma
  for (int i=0;i<hist.rows;i++)
  {
      sigma+=(i-mu)*(i-mu)*hist.at<float>(i);
  }
  sigma/=(den-1);

  cv::Mat prob;
  for (int i=0;i<hist.rows;i++)
  {
      prob.at<float>(i)=exp(-pow((i-mu),2)/(2*sigma))/(2*CV_PI*pow(fabs(sigma),1/2));
  }
  return 0;
}
