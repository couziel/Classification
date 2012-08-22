#ifndef LANDMARK
#define LANDMARK

#include <string>

#include "itkBasicTypeSerialization.h"

class Landmark
{
public:
   typedef Landmark Self;
   typedef TPoint   PointType;

   Landmark();
   Landmark(const std::string & iName, const PointType & iP);
   Landmark(const Self & iOther);

   ~Landmark() {}

   void operator=(const Self & iOther);

   bool operator==(const Self & iOther) const;

   bool operator!=(const Self & iOther) const;

   void SetName(const std::string & iName);

   std::string GetName() const;

   void SetLocation(const PointType & iP);

   PointType GetLocation() const;

private:
   std::string m_Name;
   PointType   m_Location;
};
