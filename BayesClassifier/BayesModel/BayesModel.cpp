#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "ml.h"

using namespace cv;
using namespace std;

// function to learn
int f(float x, float y, int equation)
{
    switch(equation) {
    case 0:
        return y > sin(x*10) ? -1 : 1;
        break;
    case 1:
        return y > cos(x * 10) ? -1 : 1;
        break;
    case 2:
        return y > 2*x ? -1 : 1;
        break;
    case 3:
        return y > tan(x*10) ? -1 : 1;
        break;
    default:
        return y > cos(x*10) ? -1 : 1;
    }
}

// label data with equation
cv::Mat labelData(cv::Mat points, int equation)
{
    cv::Mat labels(points.rows, 1, CV_32FC1);
    for(int i = 0; i < points.rows; i++) {
             float x = points.at<float>(i,0);
             float y = points.at<float>(i,1);
             labels.at<float>(i, 0) = f(x, y, equation);
        }
    return labels;
}

int main( int argc, char* argv[] )
{

    if( argc != 6)
      {
      std::cerr << "Usage: " << argv[0] << "  <ImageTraining> <Mask> <OutputModel> <colorspace> <equation>" << std::endl;
      return EXIT_FAILURE;
      }

    cv::Mat img = cv::imread( argv[1] );
    if(!img.data)
      {
      return EXIT_FAILURE;
      }
    img.convertTo(img,CV_32F,1.0/255.0);
    cv::Mat mask = cv::imread( argv[2],0 );
    if(!mask.data)
      {
      return EXIT_FAILURE;
      }

    if (strcmp(argv[4],"YCrCb")==0)
    {
       cv::cvtColor( img, img, CV_BGR2YCrCb );
    }
    else if (strcmp(argv[4],"HSV")==0 )
    {
       cv::cvtColor( img, img, CV_BGR2HSV );
    }
    else if (strcmp(argv[4],"RGB")!=0)
    {
        std::cerr<<"Colorspaces: RGB or YCrCb or HSV"<<std::endl;
        return EXIT_FAILURE;
    }

    int nsamples = countNonZero(mask);
    Mat samples( nsamples, 3, CV_32FC1 );
    Mat source_32f= img;
    int sample_count = 0;
    for(int y=0;y<img.rows;y++)
    {
        Vec3f* row = source_32f.ptr<Vec3f>(y);
        uchar* mask_row = mask.ptr<uchar>(y);
        for(int x=0;x<img.cols;x++)
        {
            if(mask_row[x] > 0)
            {
                samples.at<Vec3f>(sample_count++,0) = row[x];
            }
        }
    }

    cv::Mat trainingData(nsamples, 3, CV_32FC1);
    cv::randu(trainingData,0,1);
    cv::Mat trainingClasses = labelData(trainingData, atoi(argv[5]));
    //cv::Mat trainingClasses = labelData(samples, atoi(argv[5]));

    CvNormalBayesClassifier bayes(samples, trainingClasses);
    bayes.save(argv[3]);

    return 0;
}
