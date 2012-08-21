#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "ml.h"

using namespace cv;
using namespace std;

int main( int argc, char* argv[] )
{

    if( argc != 8)
      {
      std::cerr << "Usage: " << argv[0] << "  <ImageTraining> <Mask> <OutputModel> <colorspace> <NumberOfMixture> <COV_MAT> <Max iter.>" << std::endl;
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

    const int N = atoi(argv[5]);

    CvEMParams params;
    // initialize model parameters
    params.covs      = NULL;
    params.means     = NULL;
    params.weights   = NULL;
    params.probs     = NULL;
    params.nclusters = N;
    if (strcmp(argv[6],"spherical")==0)
    {
        params.cov_mat_type       = CvEM::COV_MAT_SPHERICAL;
    }
    else if(strcmp(argv[6],"diagonal")==0)
    {
        params.cov_mat_type       = CvEM::COV_MAT_DIAGONAL;
    }
    else if(strcmp(argv[6],"generic")==0)
    {
        params.cov_mat_type       = CvEM::COV_MAT_GENERIC;
    }
    else
    {
        std::cerr<<"COV_MAT: spherical or diagonal or generic"<<std::endl;
        return EXIT_FAILURE;
    }
    params.start_step         = CvEM::START_AUTO_STEP;
    params.term_crit.max_iter = atoi(argv[7]);
    params.term_crit.epsilon  = 0.1;
    params.term_crit.type     = CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;


    //samples
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

    //training
    CvEM em_model;
    em_model.clear();
    em_model.train(samples,Mat(),params,NULL);

#if 0
    // the piece of code shows how to repeatedly optimize the model
    // with less-constrained parameters
    //(COV_MAT_DIAGONAL instead of COV_MAT_SPHERICAL)
    // when the output of the first stage is used as input for the second one.
    CvEM em_model2;
    params.cov_mat_type = CvEM::COV_MAT_DIAGONAL;
    params.start_step = CvEM::START_E_STEP;
    params.means = em_model.get_means();
    params.covs = (const CvMat**)em_model.get_covs();
    params.weights = em_model.get_weights();

    em_model2.train( samples, Mat(), params, &labels );
    // to use em_model2, replace em_model.predict()
    // with em_model2.predict() below
#endif

    em_model.save(argv[3]);

    return 0;
}
