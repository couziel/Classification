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

    //n and fi
    int n=0;
    int flag=0;
    std::vector<double> temp(3);
    std::map<std::vector<double>, int> pixel;
    std::map<std::vector<double>, int>::iterator it;

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                n++;
                if (flag==0)
                {   //init map
                    temp.at(0)=image.at<cv::Vec3b>(i,j)[0];
                    temp.at(1)=image.at<cv::Vec3b>(i,j)[1];
                    temp.at(2)=image.at<cv::Vec3b>(i,j)[2];
                    pixel.insert(std::pair<std::vector<double>,int>(temp,0));
                    flag=1;
                }
            }
        }

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                temp.at(0)=image.at<cv::Vec3b>(i,j)[0];
                temp.at(1)=image.at<cv::Vec3b>(i,j)[1];
                temp.at(2)=image.at<cv::Vec3b>(i,j)[2];
                it=pixel.find(temp);
                if(it==pixel.end())
                {
                    pixel.insert(std::pair<std::vector<double>,int>(temp,1));
                }
                else
                {
                    it->second=it->second+1;
                }
            }
        }

    //N
    int N=0;

    for( it=pixel.begin(); it!=pixel.end(); ++it)
        N+=it->second;

  //Mu and phi
    Matx31d mu, phi;
    mu.zeros();
    phi.zeros();

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                temp.at(0)=image.at<cv::Vec3b>(i,j)[0];
                temp.at(1)=image.at<cv::Vec3b>(i,j)[1];
                temp.at(2)=image.at<cv::Vec3b>(i,j)[2];
                mu(0)+=image.at<cv::Vec3b>(i,j)[0]*pixel.find(temp)->second;
                mu(1)+=image.at<cv::Vec3b>(i,j)[1]*pixel.find(temp)->second;
                mu(2)+=image.at<cv::Vec3b>(i,j)[2]*pixel.find(temp)->second;
                phi(0)+=image.at<cv::Vec3b>(i,j)[0];
                phi(1)+=image.at<cv::Vec3b>(i,j)[1];
                phi(2)+=image.at<cv::Vec3b>(i,j)[2];
            }
        }
    mu(0)/=N;
    mu(1)/=N;
    mu(2)/=N;
    phi(0)/=n;
    phi(1)/=n;
    phi(2)/=n;

    //lambda
    Matx33d lambda;
    lambda.zeros();
    Matx31d vectColor;
    Matx13d tranVectColor;
    Matx33d fi;

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                temp.at(0)=image.at<cv::Vec3b>(i,j)[0];
                temp.at(1)=image.at<cv::Vec3b>(i,j)[1];
                temp.at(2)=image.at<cv::Vec3b>(i,j)[2];
                vectColor(0)=image.at<cv::Vec3b>(i,j)[0]-mu(0);
                vectColor(1)=image.at<cv::Vec3b>(i,j)[1]-mu(1);
                vectColor(2)=image.at<cv::Vec3b>(i,j)[2]-mu(2);
                tranVectColor=vectColor.t();
                fi=vectColor*tranVectColor;
                for(int x=0;x<3;x++)
                    for(int y=0;y<3;y++)
                    {
                        fi(x,y)*=pixel.find(temp)->second;
                    }
                lambda+=fi;
            }
        }
    for(int x=0;x<3;x++)
        for(int y=0;y<3;y++)
        {
            lambda(x,y)/=N;
        }


    //Phi
    Matx33d invLambda;
    double proba=0;
    cv::Mat skin=cv::Mat::zeros(image.rows,image.cols,CV_8UC1);

    for (int i=0; i<image.rows;i++)
        for (int j=0; j<image.cols; j++)
        {
            vectColor(0)=image.at<cv::Vec3b>(i,j)[0]-phi(0);
            vectColor(1)=image.at<cv::Vec3b>(i,j)[1]-phi(1);
            vectColor(2)=image.at<cv::Vec3b>(i,j)[2]-phi(2);
            tranVectColor=vectColor.t();
            invLambda=lambda.inv();
            proba=(tranVectColor*invLambda*vectColor)(0);
            if(proba>0.0025 &&proba<0.0075)
            {
                skin.at<float>(i,j)=255;
            }
            //std::cout<<proba<<std::endl;
        }
    cv::namedWindow( argv[1], CV_WINDOW_AUTOSIZE );
    cv::imshow( argv[1], skin );;
    cv::waitKey(0);

    return 0;
}
