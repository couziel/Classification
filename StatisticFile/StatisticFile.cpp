#include <iostream>
#include <fstream>
#include "cv.h"
#include "highgui.h"
#include "stdlib.h"
#include "stdio.h"

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

 int median (cv::Mat image, cv::Mat mask, int minRange, int maxRange, int flag)
 {
     double m=0;
     if (flag==0)
        m=(image.rows*image.cols)/2;
     else
     {
         for (int i=0;i<mask.rows;i++)
             for (int j=0;j<mask.cols;j++)
             {
                 if(mask.at<float>(i,j)!=0)
                     m++;
             }
         m/=2;
     }
     int bin=0;
     int med=-1;
     int histSize = maxRange-minRange;
     float range[] = { minRange, maxRange } ;
     const float* histRange = { range };
     bool uniform = true;
     bool accumulate = false;
     cv::Mat hist;
     cv::calcHist( &image, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate );

     for (int i=minRange; i<maxRange && med<0; i++)
     {
         bin=bin+cvRound(hist.at<float>(i));
         if (bin>m && med<0)
             med=i;
     }

     return med;
 }

 int mode (cv::Mat image, cv::Mat mask, int minRange, int maxRange)
 {
     int bin=0;
     int mode=-1;
     int histSize = maxRange-minRange;
     float range[] = { minRange, maxRange } ;
     const float* histRange = { range };
     bool uniform = true;
     bool accumulate = false;
     cv::Mat hist;
     cv::calcHist( &image, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate );

     for (int i=minRange; i<maxRange ;i++)
     {
         if (bin<cvRound(hist.at<float>(i)))
         {
             bin=cvRound(hist.at<float>(i));
             mode=i;
         }
     }

     return mode;
 }

 int Min (cv::Mat image,cv::Mat mask, int flag)
 {
     int min,init;
     if (flag==0)
        min=image.at<uchar>(0,0);
     else
         init=0;

     for (int i=0;i<image.rows;i++)
         for (int j=0;j<image.cols;j++)
         {
             if(flag==0)
             {
                 if(min>image.at<uchar>(i,j))
                     min=image.at<uchar>(i,j);
             }
             else
             {
                 if(init==0 && mask.at<uchar>(i,j)!=0)
                 {
                    min=image.at<uchar>(i,j);
                    init=1;
                 }
                 if(min>image.at<uchar>(i,j) && mask.at<uchar>(i,j)!=0)
                     min=image.at<uchar>(i,j);
             }
         }
     return min;
 }

 int Max (cv::Mat image,cv::Mat mask, int flag)
 {
     int max, init;
     if (flag==0)
         max=image.at<uchar>(0,0);
     else
         init=0;

     for (int i=0;i<image.rows;i++)
         for (int j=0;j<image.cols;j++)
         {
             if (flag==0)
             {
                 if(max<image.at<uchar>(i,j))
                     max=image.at<uchar>(i,j);
             }
             else
             {
                 if(init==0 && mask.at<uchar>(i,j)!=0)
                 {
                    max=image.at<uchar>(i,j);
                    init=1;
                 }
                 if(max<image.at<uchar>(i,j) && mask.at<uchar>(i,j)!=0)
                     max=image.at<uchar>(i,j);
             }
         }
     return max;
 }

int main( int argc, char* argv[] )
{
    if( argc != 5)
      {
      std::cerr << "Usage: " << argv[0] << " <InputImage> <Mask> <channel> <OutputFile>" << std::endl;
      return EXIT_FAILURE;
      }
    cv::Mat image = cv::imread( argv[1] );
    if(!image.data)
      {
      return EXIT_FAILURE;
      }

    int flag;
    cv::Mat mask;
    if(strcmp(argv[2],"null")==0)
    {
        flag=0;
        mask=cv::Mat();
    }
    else
    {
        flag=1;
        mask = cv::imread( argv[2],0 );
        if(!mask.data)
          {
          return EXIT_FAILURE;
          }
        //if .mat file
       /* import scipy.io;
        mask = scipy.io.loadmat(argv[2]);*/
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
    int med=median(im,mask,minRange,maxRange,flag);
    int mod=mode(im,mask,minRange,maxRange);
    int min=Min(im,mask,flag);
    int max=Max(im,mask,flag);
    cv::Scalar mean,stddev;
    cv::meanStdDev(im,mean,stddev,mask);
    std::ofstream file(argv[4], std::ios::out | std::ios::app);
    if(file)
    {
        file<<argv[1]<<" Mean"<<" Variance"<<" Median"<<" Mode"<<" Min"<<" Max"<<"  Histogram";
        for (int i=minRange;i<maxRange;i++)
        {
            file<<" "<<i;
        }
        file<<std::endl;
        file<<" "<<mean.val[0]<<" "<<stddev.val[0]*stddev.val[0]<<" "<<med<<" "<<mod<<" "<<min<<" "<<max<<"  ";
        for (int i=minRange;i<maxRange;i++)
        {
            file<<" "<<hist.at<float>(i);
        }
        file<<std::endl;
        file.close();
    }
    else
        std::cerr << "Can't open file "<<argv[4] << std::endl;

    return 0;
}
