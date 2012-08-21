#include <iostream>
#include <fstream>

#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

int main( int argc, char* argv[] )
{
    if( argc != 6)
      {
      std::cerr << "Usage: " << argv[0] << " <InputImage> <Mask> <colorspace> <OutputMu> <OutputSigma>" << std::endl;
      return EXIT_FAILURE;
      }
    cv::Mat image = cv::imread( argv[1] );
    if(!image.data)
      {
      return EXIT_FAILURE;
      }

    cv::Mat mask = cv::imread( argv[2],0 );
    if(!mask.data)
      {
      return EXIT_FAILURE;
      }


    if (strcmp(argv[3],"YCrCb")==0)
    {
       cv::cvtColor( image, image, CV_BGR2YCrCb );
    }
    else if (strcmp(argv[3],"HSV")==0 )
    {
       cv::cvtColor( image, image, CV_BGR2HSV );
    }
    else if (strcmp(argv[3],"RGB")!=0)
    {
        std::cerr<<"Colorspaces: RGB or YCrCb or HSV"<<std::endl;
        return EXIT_FAILURE;
    }

    typedef cv::Matx<float, 1, 3> Matx13f;
    typedef cv::Matx<float, 3, 1> Matx31f;
    typedef cv::Matx<float, 3, 3> Matx33f;

    //Mu
    Matx31f mu;
    mu.zeros();
    int den=0;

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                den++;
                mu+=image.at<cv::Vec3b>(i,j);
            }
        }
    }
    mu(0)/=static_cast<float>(den);
    mu(1)/=static_cast<float>(den);
    mu(2)/=static_cast<float>(den);

    //Sigma
    Matx31f pixel;
    Matx13f transPixel;
    Matx33f sigma;
    sigma.zeros();

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                pixel=image.at<cv::Vec3b>(i,j);
                pixel-=mu;
                transPixel=pixel.t();
                sigma+=pixel*transPixel;
            }
        }
    }

    for(int i=0;i<3;i++)
        for (int j=0;j<3;j++)
        {
            sigma (i,j)/=den-1;
        }

    std::ofstream fileMu(argv[4], std::ios::out | std::ios::trunc);
    if(fileMu)
    {
        for (int i=0; i<3; i++)
        {
            fileMu<<mu(i)<<endl;
        }
        fileMu.close();
    }
    else
    {
        std::cerr << "Can't open file "<<argv[4] << std::endl;
    }

    std::ofstream fileSigma(argv[5], std::ios::out | std::ios::trunc);
    if(fileSigma)
    {
        for (int i=0; i<3; i++)
        {
            for (int j=0; j<3; j++)
            {
                fileSigma<<sigma(i,j)<<endl;
            }
        }
        fileSigma.close();
    }
    else
    {
        std::cerr << "Can't open file "<<argv[5] << std::endl;
    }

    return 0;
}
