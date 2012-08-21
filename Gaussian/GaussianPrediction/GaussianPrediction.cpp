#include <iostream>
#include <fstream>
#include <sstream>
#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

int main( int argc, char* argv[] )
{
    if( argc != 7)
    {
    std::cerr << "Usage: " << argv[0] << " <InputImage> <OutputImage> <Mu> <Sigma> <colorspace> <Threshold>" << std::endl;
    return EXIT_FAILURE;
    }

    cv::Mat image = cv::imread( argv[1] );
    if(!image.data)
    {
    return EXIT_FAILURE;
    }

    typedef cv::Matx<float, 1, 3> Matx13f;
    typedef cv::Matx<float, 3, 1> Matx31f;
    typedef cv::Matx<float, 3, 3> Matx33f;

    Matx31f mu;
    Matx33f sigma;
    string line;
    ifstream fileMu (argv[3]);
    if (fileMu.is_open())
    {
        int i=0;
        while ( fileMu.good() )
        {
          getline (fileMu,line);
          istringstream iss( line );
          iss>>mu(i);
          i++;
        }
        fileMu.close();
    }

    else
    {
        cout << "Unable to open file "<<fileMu<<endl;
    }

    ifstream fileSigma (argv[4]);
    if (fileSigma.is_open())
    {
        int i=0,j=0;
        while ( fileSigma.good() )
        {
          getline (fileSigma,line);
          istringstream iss( line );
          iss>>sigma(i,j);
          j++;
          if(j==3)
          {
            j=0;
            i++;
          }
        }
        fileSigma.close();
    }

    else
    {
        cout << "Unable to open file "<<fileSigma<<endl;
    }

    if (strcmp(argv[5],"YCrCb")==0)
    {
       cv::cvtColor( image, image, CV_BGR2YCrCb );
    }
    else if (strcmp(argv[5],"HSV")==0 )
    {
       cv::cvtColor( image, image, CV_BGR2HSV );
    }
    else if (strcmp(argv[5],"RGB")!=0)
    {
        std::cerr<<"Colorspaces: RGB or YCrCb or HSV"<<std::endl;
        return EXIT_FAILURE;
    }


    //prob
    Matx31f pixel;
    Matx13f transPixel;
    Matx33f invSigma=sigma.inv();
    float detSigma=cv::determinant(sigma);
    double proba=0;
    cv::Mat skin=cv::Mat::zeros(image.rows,image.cols,CV_8UC1);

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {

            pixel=image.at<cv::Vec3b>(i,j);
            pixel-=mu;
            transPixel=pixel.t();
            proba=(transPixel*invSigma*pixel)(0);
            proba=exp(-proba/2)/(2*CV_PI*sqrt(detSigma));//std::cout<<proba<<std::endl;
            if(proba<atof(argv[6]))
            {
                skin.at<uchar>(i,j)=255;
            }

        }
    }
    cv::namedWindow( argv[1], CV_WINDOW_NORMAL );
    cv::imshow( argv[1], skin );
    cv::waitKey(0);

    imwrite(argv[2],skin);

    return 0;
}
