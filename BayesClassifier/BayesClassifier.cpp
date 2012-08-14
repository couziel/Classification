#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "ml.h"

/*typedef cv::Matx<double, 1, 3> Matx13d;
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
}*/

int main( int argc, char* argv[] )
{
    const int N = 4;
    const int N1 = (int)sqrt((double)N);
    const cv::Scalar colors[] =
    {
        cv::Scalar(0,0,255), cv::Scalar(0,255,0),
        cv::Scalar(0,255,255),cv::Scalar(255,255,0)
    };

    int i, j;
    /*int nsamples = 100;
    cv::Mat samples( nsamples, 2, CV_32FC1 );*/
    cv::Mat labels;

    cv::Mat img = cv::imread( argv[1] );
    if(!img.data)
      {
      return EXIT_FAILURE;
      }
    cv::Mat mask = cv::imread( argv[2],0 );
    if(!mask.data)
      {
      return EXIT_FAILURE;
      }
    //cv::Mat img = cv::Mat::zeros( cv::Size( 500, 500 ), CV_8UC3 );
    cv::Mat sample( 1, 2, CV_32FC1 );
    CvNormalBayesClassifier bc_model;
    //CvEMParams params;

  /*  samples = samples.reshape(2, 0);
    for( i = 0; i < N; i++ )
    {
        // form the training samples
        cv::Mat samples_part = samples.rowRange(i*nsamples/N, (i+1)*nsamples/N );

        cv::Scalar mean(((i%N1)+1)*img.rows/(N1+1),
                    ((i/N1)+1)*img.rows/(N1+1));
        cv::Scalar sigma(30,30);
        randn( samples_part, mean, sigma );
    }
    samples = samples.reshape(1, 0);*/
    int nsamples=0;
    int x=0;
    for (int i=0; i<img.rows;i++)
        for (int j=0; j<img.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                nsamples++;
            }
        }
    cv::Mat samples( nsamples, 2, CV_32FC1 );

    for (int i=0; i<img.rows;i++)
        for (int j=0; j<img.cols; j++)
        {
            if(mask.at<float>(i,j)!=0)
            {
                //samples.at<float>(x,0)=i;
               // samples.at<float>(x,1)=j;
                samples.at<float>(x,0)=img.at<cv::Vec3b>(i,j)[0];
                samples.at<float>(x,1)=img.at<cv::Vec3b>(i,j)[1];
                samples.at<float>(x,2)=img.at<cv::Vec3b>(i,j)[2];
            }
        } //samples.reshape(1,0);
    samples = samples.reshape(2, 0);
     /*   for( int i = 0; i < N; i++ )
        {
            // form the training samples
            cv::Mat samples_part = samples.rowRange(i*nsamples/N, (i+1)*nsamples/N );

           cv::Scalar mean(((i%N1)+1)*img.rows/(N1+1),
                        ((i/N1)+1)*img.rows/(N1+1));
            cv::Scalar sigma(30,30);
            randn( samples_part, mean, sigma );
        }*/
        samples = samples.reshape(1, 0);
    // initialize model parameters
  /*  params.covs      = NULL;
    params.means     = NULL;
    params.weights   = NULL;
    params.probs     = NULL;
    params.nclusters = N;
    params.cov_mat_type       = CvEM::COV_MAT_SPHERICAL;
    params.start_step         = CvEM::START_AUTO_STEP;
    params.term_crit.max_iter = 300;
    params.term_crit.epsilon  = 0.1;
    params.term_crit.type     = CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;*/

    // cluster the data
    bc_model.train( samples, cv::Mat(), params, &labels );

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
    // classify every image pixel
    for( i = 0; i < img.rows; i++ )
    {
        for( j = 0; j < img.cols; j++ )
        {
            sample.at<float>(0) = (float)j;
            sample.at<float>(1) = (float)i;
            int response = cvRound(bc_model.predict( sample ));
            cv::Scalar c = colors[response];

            circle( img, cv::Point(j, i), 1, c*0.75, CV_FILLED );
        }
    }

    //draw the clustered samples
    for( i = 0; i < nsamples; i++ )
    {
        cv::Point pt(cvRound(samples.at<float>(i, 0)), cvRound(samples.at<float>(i, 1)));
        circle( img, pt, 1, colors[labels.at<int>(i)], CV_FILLED );
    }

    imshow( "EM-clustering result", img );
    cv::waitKey(0);

    return 0;
}
