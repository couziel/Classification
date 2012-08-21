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
      std::cerr << "Usage: " << argv[0] << " <InputImage> <Mask> <colorspace> <OutputPhi> <OutputLambda>" << std::endl;
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

    //n and fi
    int n=0;
    int flag=0;
    std::vector<float> temp(3);
    std::map<std::vector<float>, int> pixel;
    std::map<std::vector<float>, int>::iterator it;

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                n++;
                if (flag==0)
                {   //init map
                    temp.at(0)=static_cast<float>(image.at<cv::Vec3b>(i,j)[0]);
                    temp.at(1)=static_cast<float>(image.at<cv::Vec3b>(i,j)[1]);
                    temp.at(2)=static_cast<float>(image.at<cv::Vec3b>(i,j)[2]);
                    pixel.insert(std::pair<std::vector<float>,int>(temp,0));
                    flag=1;
                }
            }
        }
    }

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                temp.at(0)=static_cast<float>(image.at<cv::Vec3b>(i,j)[0]);
                temp.at(1)=static_cast<float>(image.at<cv::Vec3b>(i,j)[1]);
                temp.at(2)=static_cast<float>(image.at<cv::Vec3b>(i,j)[2]);
                it=pixel.find(temp);
                if(it==pixel.end())
                {
                    pixel.insert(std::pair<std::vector<float>,int>(temp,1));
                }
                else
                {
                    it->second=it->second+1;
                }
            }
        }
    }

    //N
    int N=0;

    for( it=pixel.begin(); it!=pixel.end(); ++it)
    {
        N+=it->second;
    }

  //Mu and phi
    Matx31f mu, phi;
    mu.zeros();
    phi.zeros();

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                temp.at(0)=static_cast<float>(image.at<cv::Vec3b>(i,j)[0]);
                temp.at(1)=static_cast<float>(image.at<cv::Vec3b>(i,j)[1]);
                temp.at(2)=static_cast<float>(image.at<cv::Vec3b>(i,j)[2]);
                mu(0)+=static_cast<float>(image.at<cv::Vec3b>(i,j)[0])*pixel.find(temp)->second;
                mu(1)+=static_cast<float>(image.at<cv::Vec3b>(i,j)[1])*pixel.find(temp)->second;
                mu(2)+=static_cast<float>(image.at<cv::Vec3b>(i,j)[2])*pixel.find(temp)->second;
                phi+=image.at<cv::Vec3b>(i,j);
            }
        }
    }
    mu(0)/=static_cast<float>(N);
    mu(1)/=static_cast<float>(N);
    mu(2)/=static_cast<float>(N);
    phi(0)/=static_cast<float>(n);
    phi(1)/=static_cast<float>(n);
    phi(2)/=static_cast<float>(n);

    //lambda
    Matx33f lambda;
    lambda.zeros();
    Matx31f vectColor;
    Matx13f tranVectColor;
    Matx33f fi;

    for (int i=0; i<image.rows;i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                temp.at(0)=static_cast<float>(image.at<cv::Vec3b>(i,j)[0]);
                temp.at(1)=static_cast<float>(image.at<cv::Vec3b>(i,j)[1]);
                temp.at(2)=static_cast<float>(image.at<cv::Vec3b>(i,j)[2]);
                vectColor=image.at<cv::Vec3b>(i,j);
                vectColor-=mu;
                tranVectColor=vectColor.t();
                fi=vectColor*tranVectColor;
                for(int x=0;x<3;x++)
                {
                    for(int y=0;y<3;y++)
                    {
                        fi(x,y)*=pixel.find(temp)->second;
                    }
                }
                lambda+=fi;
            }
        }
    }
    for(int x=0;x<3;x++)
    {
        for(int y=0;y<3;y++)
        {
            lambda(x,y)/=static_cast<float>(N);
        }
    }

    std::ofstream filePhi(argv[4], std::ios::out | std::ios::trunc);
    if(filePhi)
    {
        for (int i=0; i<3; i++)
        {
            filePhi<<phi(i)<<endl;
        }
        filePhi.close();
    }
    else
    {
        std::cerr << "Can't open file "<<argv[4] << std::endl;
    }

    std::ofstream fileLambda(argv[5], std::ios::out | std::ios::trunc);
    if(fileLambda)
    {
        for (int i=0; i<3; i++)
        {
            for (int j=0; j<3; j++)
            {
                fileLambda<<lambda(i,j)<<endl;
            }
        }
        fileLambda.close();
    }
    else
    {
        std::cerr << "Can't open file "<<argv[5] << std::endl;
    }


    return 0;
}
