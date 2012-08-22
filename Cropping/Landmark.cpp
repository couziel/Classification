#include <Landmark.h>

Landmark::Landmark():m_Name("") {}
Landmark::Landmark(const std::string & iName, const PointType & iP):m_Name(iName), m_Location(iP) {}
Landmark::Landmark(const Self & iOther):m_Name(iOther.m_Name), m_Location(iOther.m_Location) {}

Landmark::~Landmark() {}

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
