#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "ml.h"

using namespace cv;
using namespace std;


int main( int argc, char* argv[] )
{

    if( argc != 5)
      {
      std::cerr << "Usage: " << argv[0] << " <ImageTest> <BayesModel> <OutputImage> <colorspace> " << std::endl;
      return EXIT_FAILURE;
      }

    cv::Mat target = cv::imread( argv[1] );
    if(!target.data)
      {
      return EXIT_FAILURE;
      }
    target.convertTo(target,CV_32F,1.0/255.0);

    if (strcmp(argv[4],"YCrCb")==0)
    {
       cv::cvtColor( target, target, CV_BGR2YCrCb );
    }
    else if (strcmp(argv[4],"HSV")==0 )
    {
       cv::cvtColor( target, target, CV_BGR2HSV );
    }
    else if (strcmp(argv[4],"RGB")!=0)
    {
        std::cerr<<"Colorspaces: RGB or YCrCb or HSV"<<std::endl;
        return EXIT_FAILURE;
    }

    CvNormalBayesClassifier bayes;
    bayes.load(argv[2]);

    Mat target_32f=target;
    Mat pr;
    Mat samp(1,3,CV_32FC1);

    for(int y=0;y<target.rows;y++)
    {
        Vec3f* row = target_32f.ptr<Vec3f>(y);
        for(int x=0;x<target.cols;x++)
        {
                memcpy(samp.data,&(row[x][0]),3*sizeof(float));
                bayes.predict(samp,&pr);

                //Mat samp_64f;
                //samp.convertTo(samp_64f,CV_64F);
                Mat Xnew(1,3,CV_64FC1,Scalar(0));
                if(((float*)pr.data)[0] >0)
                {
                    Xnew+=(double)(((float*)pr.data)[0]);
                }
                Mat _tmp; Xnew.convertTo(_tmp,CV_32F);
                memcpy(&(row[x][0]),_tmp.data,sizeof(float)*3);

                if(target_32f.at<Vec3f>(y,x)[0]==0 && target_32f.at<Vec3f>(y,x)[1]==0 && target_32f.at<Vec3f>(y,x)[2]==0 )
                {
                    target_32f.at<Vec3f>(y,x)[0]=1;
                    target_32f.at<Vec3f>(y,x)[1]=1;
                    target_32f.at<Vec3f>(y,x)[2]=1;

                }
                else
                {
                    target_32f.at<Vec3f>(y,x)[0]=0;
                    target_32f.at<Vec3f>(y,x)[1]=0;
                    target_32f.at<Vec3f>(y,x)[2]=0;
                }

            }
        }
    cvtColor(target_32f,target_32f,CV_RGB2GRAY);
    target_32f.convertTo(target_32f,CV_8UC1,255.0/1.0);
    namedWindow("Bayes Classification result",CV_WINDOW_NORMAL);
    imshow( "Bayes Classification result", target_32f );
    waitKey(0);

    imwrite(argv[3],target_32f);

    return 0;
}
