// This file is part of liblmbcam.
//
// liblmbcam is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// liblmbcam is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with liblmbcam.  If not, see <http://www.gnu.org/licenses/>.

#include "LMBCam.hh"

#include "LMBCamParam.hh"

#include <sstream>
#include <cstdlib>

std::istream& eatWsAndComments( std::istream &s)
{
  s >> std::ws; // eat whitespaces
  
  while( s.peek() == '#')
  { 
    s.ignore( 10000, '\n');  // ignore characters until line end
    s >> std::ws;                 // eat whitespaces
  }
  return s;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBCam::read( std::istream& is)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::LMBCam::readState( std::istream& is)
{
  std::string buf;

  is >> eatWsAndComments >> buf;
  if( buf != "<LMBCam")
  {
    LMBCamError error( "File is not a LMBCam settings file");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  
  is >> eatWsAndComments >> buf;
  if( buf != "type=camsettings")
  {
    LMBCamError error( "File is not a LMBCam settings file");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  
  is >> eatWsAndComments >> buf;
  if( buf != "version=1.0>")
  {
    LMBCamError error( "File is in a wrong version");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }

  is >> eatWsAndComments >> buf;

  int bufInt;
  double bufDouble;
  
  while( buf != "</LMBCam>")
  {
    if( buf == "<CamInfo")
    {
      do
      {
        is >> eatWsAndComments >> buf;
      }
      while( buf != "</CamInfo>");
    }
    else if( buf == "<CamSetting")
    {
      is >> eatWsAndComments >> buf;
      if( buf == "name=mode>")
      {
        is >> eatWsAndComments;
        getline( is, buf);
        setMode( buf);
      }
      else if( buf == "name=framerate>")
      {
        is >> eatWsAndComments >> bufDouble;
        setFramerate( bufDouble);
      }
      else if( buf == "name=colorCoding>")
      {
        is >> eatWsAndComments >> buf;
        setColorCoding( buf.c_str());
      }
      else if( buf == "name=left>")
      {
        is >> eatWsAndComments >> bufInt;
        setLeft( bufInt);
      }
      else if( buf == "name=top>")
      {
        is >> eatWsAndComments >> bufInt;
        setTop( bufInt);
      }
      else if( buf == "name=width>")
      {
        is >> eatWsAndComments >> bufInt;
        setWidth( bufInt);
      }
      else if( buf == "name=height>")
      {
        is >> eatWsAndComments >> bufInt;
        setHeight( bufInt);
      }
      else if( buf == "name=maxWidth>" || 
               buf == "name=maxHeight>")
      {
        is >> eatWsAndComments >> buf;
      }
      else
      {
        LMBCamError error( "Error parsing CamSetting section");
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
      
      is >> eatWsAndComments >> buf;
      if( buf != "</CamSetting>")
      {
        LMBCamError error( "Error parsing CamSetting section");
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    else if( buf == "<CamParam")
    {
      is >> eatWsAndComments;
      std::getline(is,buf);
      buf = buf.substr( 5);
      
      LMBCamParam* parameter = param( buf);
      if( parameter == 0)
      {
        std::stringstream errString;
        errString << "Parameter " << buf << " unknown";
        LMBCamError error( errString.str());
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
      is >> eatWsAndComments >> buf; // eat min
      is >> eatWsAndComments >> buf; // eat max

      float val;
      is >> eatWsAndComments >> val; // read value
      parameter->setValue( val);
      
      is >> eatWsAndComments >> buf;
      if( buf != "</CamParam>")
      {
        LMBCamError error( "Error parsing CamParam section");
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    else
    {
      LMBCamError error( "Error parsing LMBCam settings file");
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }

    is >> eatWsAndComments >> buf;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBCam::write( std::ostream& os)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::LMBCam::writeState( std::ostream& os)
{
  os << "<LMBCam type=camsettings version=1.0>\n"
     << "  <CamInfo name=vendor>\n"
     << "    " << vendor() << "\n"
     << "  </CamInfo>\n"
     << "  <CamInfo name=model>\n"
     << "    " << model() << "\n"
     << "  </CamInfo>\n"
     << "  <CamInfo name=guid>\n"
     << "    " << guid() << "\n"
     << "  </CamInfo>\n";
  
  os << "  <CamSetting name=mode>\n"
     << "    " << mode() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=framerate>\n"
     << "    " << framerate() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=colorCoding>\n"
     << "    " << colorCoding() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=left>\n"
     << "    " << left() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=top>\n"
     << "    " << top() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=width>\n"
     << "    " << width() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=maxWidth>\n"
     << "    " << maxWidth() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=height>\n"
     << "    " << height() << "\n"
     << "  </CamSetting>\n"
     << "  <CamSetting name=maxHeight>\n"
     << "    " << maxHeight() << "\n"
     << "  </CamSetting>\n";
  
  std::map<std::string,LMBCamParam*> parameters = parameterMap();
  for( std::map<std::string,LMBCamParam*>::const_iterator it=parameters.begin();
       it != parameters.end(); ++it)
  {
    os << "  <CamParam name=" << it->first << "\n"
       << "            min=" << it->second->minValueAsFloat() << "\n"
       << "            max=" << it->second->maxValueAsFloat() << ">\n"
       << "    " << it->second->asFloat() << "\n"
       << "  </CamParam>\n";
  }

  os  << "</LMBCam>\n";
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBCam::loadParametersFromMap(
 *                       const std::map<std::string,std::string>& paramMap,
 *                       const std::string& prefix)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::LMBCam::loadParametersFromMap(
    const std::map<std::string,std::string>& paramMap,
    const std::string& prefix, bool clearExtended)
{
  if( clearExtended)
      p_extendedParameters.clear();
  
  std::map<std::string,std::string> mapCopy;
  std::map<std::string,std::string>::iterator it;

  for( std::map<std::string,std::string>::const_iterator cit=paramMap.begin();
       cit!=paramMap.end(); ++cit)
  {
    if( cit->first.substr( 0, prefix.size()) == prefix)
    {
      mapCopy[cit->first] = cit->second;
    }
  }
  
  it = mapCopy.find( prefix + "mode");
  if( it != mapCopy.end())
  {
    setMode( it->second);
    mapCopy.erase( it);
  }
    
  it = mapCopy.find( prefix + "colorcoding");
  if( it != mapCopy.end())
  {
    setColorCoding( it->second);
    mapCopy.erase( it);
  }
  
  it = mapCopy.find( prefix + "left");
  if( it != mapCopy.end())
  {
    setLeft( atoi( it->second.c_str()));
    mapCopy.erase( it);
  }
  
  it = mapCopy.find( prefix + "top");
  if( it != mapCopy.end())
  {
    setTop( atoi( it->second.c_str()));
    mapCopy.erase( it);
  }
  
  it = mapCopy.find( prefix + "width");
  if( it != mapCopy.end())
  {
    setWidth( atoi( it->second.c_str()));
    mapCopy.erase( it);
  }
  
  it = mapCopy.find( prefix + "height");
  if( it != mapCopy.end())
  {
    setHeight( atoi( it->second.c_str()));
    mapCopy.erase( it);
  }
  
  it = mapCopy.find( prefix + "framerate");
  if( it != mapCopy.end())
  {
    setFramerate( strtod( it->second.c_str(), NULL), true);
    mapCopy.erase( it);
  }
  
  for( it=mapCopy.begin(); it!=mapCopy.end(); ++it)
  {
    if( it->first.substr( 0, prefix.size()) == prefix)
    {
      if( it->first.size() > 11 && 
          it->first.substr( it->first.size()-11,
                            it->first.size()) == "_abs_switch")
      {
        LMBCamParam* parameter = param( it->first.substr( prefix.size()));
        if( parameter)
            parameter->setValue( atoi( it->second.c_str()));
        mapCopy.erase( it);
      }
    }
  }

  for( it=mapCopy.begin(); it!=mapCopy.end(); ++it)
  {
    if( it->first.substr( 0, prefix.size()) == prefix)
    {
      LMBCamParam* parameter = param( it->first.substr( prefix.size()));
      if( parameter)
          parameter->setValue( float(atof( it->second.c_str())));
      else
          p_extendedParameters[it->first.substr( prefix.size())] = it->second;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBCam::saveParametersToMap(
 *                       const std::map<std::string,std::string>& paramMap,
 *                       const std::string& prefix)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::LMBCam::saveParametersToMap(
    std::map<std::string,std::string>& paramMap,
    const std::string& prefix)
{
  paramMap[prefix+"mode"] = mode();

  paramMap[prefix+"colorcoding"] = colorCoding();

  {
    std::ostringstream stream;
    stream << framerate();
    paramMap[prefix+"framerate"] = stream.str();
  }
    
  {
    std::ostringstream stream;
    stream << left();
    paramMap[prefix+"left"] = stream.str();
  }
  
  {
    std::ostringstream stream;
    stream << top();
    paramMap[prefix+"top"] = stream.str();
  }
  
  {
    std::ostringstream stream;
    stream << width();
    paramMap[prefix+"width"] = stream.str();
  }
  
  {
    std::ostringstream stream;
    stream << height();
    paramMap[prefix+"height"] = stream.str();
  }
  
  for( std::map<std::string,LMBCamParam*>::const_iterator it=p_availableParameters.begin(); it!=p_availableParameters.end(); ++it)
  {
    std::ostringstream stream;
    stream << it->second->asFloat();
    paramMap[prefix+it->first] = stream.str();
  }

  for( std::map<std::string,std::string>::const_iterator it=p_extendedParameters.begin(); it!=p_extendedParameters.end(); ++it)
  {
    paramMap[prefix+it->first] = it->second;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void liblmbcam::LMBCam::getFrameStartMutex( pthread_mutex_t** m, 
                                            pthread_cond_t**  c)
{
  *c = &p_frameStartCond;
  *m = &p_frameStartMutex;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
size_t
liblmbcam::LMBCam::bytesUsed( long frameIndex) const
{
  return width()*height()*bytePerPixel();
}
