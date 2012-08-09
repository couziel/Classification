#include <iostream>

#include "cv.h"
#include "highgui.h"

typedef cv::Matx<double, 1, 3> Matx13d;
typedef cv::Matx<double, 3, 1> Matx31d;
typedef cv::Matx<double, 3, 3> Matx33d;

int main( int argc, char* argv[] )
{
    if( argc != 4)
      {
      std::cerr << "Usage: " << argv[0] << " <InputImage> <Mask> <colorspace> " << std::endl;
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

    //Mu
    Matx31d mu;
    mu.zeros();
    int den=0;

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                den++;
                mu(0)+=image.at<cv::Vec3b>(i,j)[0];
                mu(1)+=image.at<cv::Vec3b>(i,j)[1];
                mu(2)+=image.at<cv::Vec3b>(i,j)[2];
            }
        }
    mu(0)/=den;
    mu(1)/=den;
    mu(2)/=den;

    //Sigma
    Matx31d pixel;
    Matx13d transPixel;
    Matx33d sigma;
    sigma.zeros();
    Matx33d den1;
    den1.all(1/(den-1));

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                pixel(0)=image.at<cv::Vec3b>(i,j)[0]-mu(0);
                pixel(1)=image.at<cv::Vec3b>(i,j)[1]-mu(1);
                pixel(2)=image.at<cv::Vec3b>(i,j)[2]-mu(2);
                transPixel=pixel.t();
                sigma+=pixel*transPixel;
            }
        }

    for(int i=0;i<3;i++)
        for (int j=0;j<3;j++)
        {
            sigma (i,j)/=den-1;
        }

    //prob
    std::vector<double> prob;
    Matx33d invSigma;
    double proba=0;

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            pixel(0)=image.at<cv::Vec3b>(i,j)[0]-mu(0);
            pixel(1)=image.at<cv::Vec3b>(i,j)[1]-mu(1);
            pixel(2)=image.at<cv::Vec3b>(i,j)[2]-mu(2);
            transPixel=pixel.t();
            invSigma=sigma.inv();
            proba=(transPixel*invSigma*pixel)(0);
            prob.push_back(exp(-proba/2)/(2*CV_PI*pow(cv::determinant(sigma),1/2)));
        }


    return 0;
}
