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
    std::cerr << "Usage: " << argv[0] << " <ImageTest> <Model> <OutputImage> <colorspace> " << std::endl;
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

   CvEM em_model;
   em_model.load(argv[2]);
   int N=em_model.getNClusters();

   cv::Mat out=cv::Mat::zeros(target.rows,target.cols,CV_8UC1);

    Mat target_32f=target;
    Mat pr;
    Mat samp(1,3,CV_32FC1);
    Mat weight(1,3,CV_32FC1);

    Vec2d res;
    weight=em_model.getWeights();
    for(int y=0;y<target.rows;y++)
    {
        Vec3f* row = target_32f.ptr<Vec3f>(y);
        for(int x=0;x<target.cols;x++)
        {
                //memcpy(samp.data,&(row[x][0]),3*sizeof(float));
            samp.at<Vec3f>(0,0) = row[x];
//            samp.at<uchar>(0)=target_32f.at<Vec3b>(y,x)[0];
//            samp.at<uchar>(1)=target_32f.at<Vec3b>(y,x)[1];
//            samp.at<uchar>(2)=target_32f.at<Vec3b>(y,x)[2];
                res=em_model.predict(samp,&pr);
                //cout<<res(0)<<" "<<res(1)<<endl;
              // cout<< em_model.getProbs()<<endl;
                Mat samp_64f;
                samp.convertTo(samp_64f,CV_64F);
                float prob=0;
                Mat Xnew(1,3,CV_64FC1,Scalar(0));
                for(int i=0;i<N;i++)
                {
//                    if(((float*)pr.data)[i] <= 0.25)
//                    {
//                        continue;
//                    }
//                    else
//                    {
//                    Xnew+=(double)(((float*)pr.data)[i]);
//                    }
                    prob+=static_cast<float>(pr.at<uchar>(i))*static_cast<float>(weight.at<uchar>(i));
                }
                //cout<<prob<<endl;
                //cout<<out.at<float>(y,x)<<endl;
                if (prob< 20000)
                {out.at<uchar>(y,x)=255;}


                //Mat _tmp; Xnew.convertTo(_tmp,CV_32F);
               // memcpy(&(row[x][0]),_tmp.data,sizeof(float)*3);

               /* if(target_32f.at<Vec3f>(y,x)[0]<0.95 && target_32f.at<Vec3f>(y,x)[1]<0.95 && target_32f.at<Vec3f>(y,x)[2]<0.95 )
                {
                    target_32f.at<Vec3f>(y,x)[0]=0;
                    target_32f.at<Vec3f>(y,x)[1]=0;
                    target_32f.at<Vec3f>(y,x)[2]=0;

                }*/

        }
    }
    cvtColor(target_32f,target_32f,CV_RGB2GRAY);
    target_32f.convertTo(target_32f,CV_8UC1,255.0/1.0);
//    for(int y=0;y<target.rows;y++)
//    {
//        Vec3f* row = target_32f.ptr<Vec3f>(y);
//        for(int x=0;x<target.cols;x++)
//        {
////            if (static_cast<float>(row[x][0])>200)
////            {
////                row[x][0]=static_cast<uchar>(0);
////            }
//            cout<<static_cast<float>(target_32f.at<uchar>(y,x))<<endl;
////            if (static_cast<float>(target_32f.at<uchar>(y,x))>250)
////            {
////                target_32f.at<uchar>(y,x)=0;
////            }
//        }
//    }
//    for (int i=0;i<out.rows;i++)
//        for (int j=0;j<out.cols;j++)
//            cout<<static_cast<float>(out.at<uchar>(i,j))<<endl;

    namedWindow("EM-clustering result",CV_WINDOW_NORMAL);
    imshow( "EM-clustering result", out );
    waitKey(0);

   // imwrite(argv[3],target_32f);
    return 0;
}
