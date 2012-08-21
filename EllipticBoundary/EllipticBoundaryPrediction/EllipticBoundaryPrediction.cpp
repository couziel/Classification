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
      std::cerr << "Usage: " << argv[0] << " <InputImage> <OutputImage> <Phi> <Lambda> <colorspace> <Threshold>" << std::endl;
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

    Matx31f phi;
    Matx33f lambda;
    string line;
    ifstream filePhi (argv[3]);
    if (filePhi.is_open())
    {
        int i=0;
        while ( filePhi.good() )
        {
          getline (filePhi,line);
          istringstream iss( line );
          iss>>phi(i);
          i++;
        }
        filePhi.close();
    }

    else
    {
        cout << "Unable to open file "<<filePhi<<endl;
    }

    ifstream fileLambda (argv[4]);
    if (fileLambda.is_open())
    {
        int i=0,j=0;
        while ( fileLambda.good() )
        {
          getline (fileLambda,line);
          istringstream iss( line );
          iss>>lambda(i,j);
          j++;
          if(j==3)
          {
            j=0;
            i++;
          }
        }
        fileLambda.close();
    }

    else
    {
        cout << "Unable to open file "<<fileLambda<<endl;
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


    //Phi
    Matx33f invLambda;
    invLambda=lambda.inv();
    Matx31f vectColor;
    Matx13f tranVectColor;
    float proba=0;
    cv::Mat skin=cv::Mat::zeros(image.rows,image.cols,CV_8UC1);

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            vectColor=image.at<cv::Vec3b>(i,j);
            vectColor-=phi;
            tranVectColor=vectColor.t();
            proba=(tranVectColor*invLambda*vectColor)(0);
            if(proba<atof(argv[6]))
            {
                skin.at<uchar>(i,j)=255;
            }
            //std::cout<<proba<<std::endl;
         }
    }
    cv::namedWindow( argv[1], CV_WINDOW_NORMAL );
    cv::imshow( argv[1], skin );;
    cv::waitKey(0);

    imwrite(argv[2],skin);
    return 0;
}
