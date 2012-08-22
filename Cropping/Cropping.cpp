#ifndef __cxLandmark_h
#define __cxLandmark_h

#include <string>
#include <cv.h>
#include<highgui.h>
//#include "boost/serialization/access.hpp"
//#include "boost/serialization/nvp.hpp"
//#include "itkBasicTypeSerialization.h"

//namespace cx
//{
template< class TPoint > class Landmark
{
   //friend class boost::serialization::access;
public:
   typedef Landmark Self;
   typedef TPoint   PointType;

   Landmark():m_Name("") {}
   Landmark(const std::string & iName, const PointType & iP):m_Name(iName), m_Location(iP) {}
   Landmark(const Self & iOther):m_Name(iOther.m_Name), m_Location(iOther.m_Location) {}

   ~Landmark() {}

   void operator=(const Self & iOther)
   {
     this->m_Name = iOther.m_Name;
     this->m_Location = iOther.m_Location;
   }

   bool operator==(const Self & iOther) const
   {
     if ( this->m_Name == iOther.m_Name )
       {
       return this->m_Location == iOther.m_Location;
       }
     return false;
   }

   bool operator!=(const Self & iOther) const
   {
     return !( ( *this ) == iOther );
   }

   void SetName(const std::string & iName)
   {
     this->m_Name = iName;
   }

   std::string GetName() const
   {
     return this->m_Name;
   }

   void SetLocation(const PointType & iP)
   {
     this->m_Location = iP;
   }

   PointType GetLocation() const
   {
     return this->m_Location;
   }

private:
   std::string m_Name;
   PointType   m_Location;

//   template< class Archive >
//   void serialize(Archive & ar, const unsigned int /* version */)
//   {
//     ar & BOOST_SERIALIZATION_NVP(m_Name);
//     ar & BOOST_SERIALIZATION_NVP(m_Location);
//   }
};

//template< class TLandmark >
//struct ChangeName {
//   ChangeName(const std::string & iName):m_NewName(iName) {}

//   void operator()(const TLandmark & iL)
//   {
//     iL.SetName(m_NewName);
//   }

//private:
//   std::string m_NewName;
//};
//}
#endif

class Cropping
{
public:
    cv::Mat CroppedImage(cv::Mat img,Landmark<cv::Point> eye,Landmark<cv::Point> lips, Landmark<cv::Point>nose)
    {
        cv::Point Eye=eye.GetLocation();
        cv::Point Lips=lips.GetLocation();
        cv::Point Nose=nose.GetLocation();
        int minx, maxx;

        if(Eye.x<Nose.x) //right side
        {
            int minX=Eye.x;
            if (minX>Lips.x)
            {
                minX=Lips.x;
            }
            int maxX=Nose.x;
            minx=4*minX-3*maxX; //horizontal distance nose-eye = quarter of the face
            if(minx<0)
            {
                minx=0;
            }
            maxx=maxX+20;//nose supposed to be on the contour
            if(maxx>img.cols)
            {
                maxx=img.cols;
            }
        }
        else //left side
        {
            int maxX=Eye.x;
            if (maxX<Lips.x)
            {
                maxX=Lips.x;
            }
            int minX=Nose.x;

            minx=minX-20;
            if(minx<0)
            {
                minx=0;
            }
            maxx=4*maxX-3*-minX;//nose supposed to be on the contour
            if(maxx>img.cols)
            {
                maxx=img.cols;
            }
        }

        int minY=Eye.y;
        int maxY=Nose.y;
        int miny=4*minY-3*maxY; //vertical distance nose-eye = 1/7 of the face
        if(miny<0)
        {
            miny=0;
        }
        int maxy=4*maxY-3*minY;
        if(maxy>img.rows)
        {
            maxy=img.rows;
        }

        cv::Rect roi(minx,miny,maxx-minx,maxy-miny);
        cv::Mat crop=img(roi);

        return crop;

    }
    std::vector<cv::Mat> Backgroung(cv::Mat img,Landmark<cv::Point> eye,Landmark<cv::Point> lips, Landmark<cv::Point>nose)
    {
        cv::Point Eye=eye.GetLocation();
        cv::Point Lips=lips.GetLocation();
        cv::Point Nose=nose.GetLocation();
        int minx, maxx;

        if(Eye.x<Nose.x) //right side
        {
            int minX=Eye.x;
            if (minX>Lips.x)
            {
                minX=Lips.x;
            }
            int maxX=Nose.x;
            minx=4*minX-3*maxX; //horizontal distance nose-eye = quarter of the face
            if(minx<0)
            {
                minx=0;
            }
            maxx=maxX+20;//nose supposed to be on the contour
            if(maxx>img.cols)
            {
                maxx=img.cols;
            }std::cout<<"he"<<std::endl;
        }
        else //left side
        {
            int maxX=Eye.x;
            if (maxX<Lips.x)
            {
                maxX=Lips.x;
            }
            int minX=Nose.x;

            minx=minX-20;
            if(minx<0)
            {
                minx=0;
            }
            maxx=/*maxX+3*(maxX-minX)*/4*maxX-3*minX;//nose supposed to be on the contour
            if(maxx>img.cols)
            {
                maxx=img.cols;
            }
        }

        int minY=Eye.y;
        int maxY=Nose.y;
        int miny=4*minY-3*maxY; //vertical distance nose-eye = 1/7 of the face
        if(miny<0)
        {
            miny=0;
        }
        int maxy=4*maxY-3*minY;
        if(maxy>img.rows)
        {
            maxy=img.rows;
        }

        std::vector<cv::Mat> bg;
        if (minx!=0 && miny!=0)
        {
            bg.push_back(img(cv::Rect(0,0,minx,miny)));
        }
        if (minx!=0 )
        {
            bg.push_back(img(cv::Rect(0,miny,minx,maxy-miny)));
        }
        if (miny!=0 )
        {
            bg.push_back(img(cv::Rect(minx,0,maxx-minx,miny)));
        }
        if (minx!=0 && maxy!=img.rows)
        {
            bg.push_back(img(cv::Rect(0,maxy,minx,img.rows-maxy)));
        }
        if (miny!=0 && maxx!=img.cols)
        {
            bg.push_back(img(cv::Rect(maxx,0,img.cols-maxx,miny)));
        }
        if (maxy!=img.rows)
        {
            bg.push_back(img(cv::Rect(minx,maxy,maxx-minx,img.rows-maxy)));
        }
        if(maxx!=img.cols)
        {
            bg.push_back(img(cv::Rect(maxx,miny,img.cols-maxx,maxy-miny)));
        }
        if (maxx!=img.cols && maxy!=img.rows)
        {
            bg.push_back(img(cv::Rect(maxx,maxy,img.cols-maxx,img.rows-maxy)));
        }

        return bg;
    }
private:
    Landmark<cv::Point> eye, lips, nose;
};

int main( int argc, char* argv[] )
{

    cv::Mat img = cv::imread( argv[1] );
    if(!img.data)
    {
    return EXIT_FAILURE;
    }
    Landmark<cv::Point> eye, lips, nose;
    eye.SetLocation(cv::Point(575,800));
    lips.SetLocation(cv::Point(500,1200));
    nose.SetLocation(cv::Point(325,975));
    Cropping Crop;
    cv::Mat crop=Crop.CroppedImage(img,eye,lips,nose);
    cv::namedWindow( argv[1], CV_WINDOW_NORMAL );
    cv::imshow( argv[1], crop );
    cv::waitKey(0);
    std::vector<cv::Mat> bg=Crop.Backgroung(img,eye,lips,nose);
    for (int i=0;i<bg.size();i++)
    {
        cv::namedWindow( argv[1], CV_WINDOW_NORMAL );
        cv::imshow( argv[1], bg.at(i) );
        cv::waitKey(0);
    }
    return 0;

}
