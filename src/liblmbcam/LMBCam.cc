/**************************************************************************
**       Title: 
**    $RCSfile: LMBCam.cc,v $
**   $Revision: 1.12 $$Name:  $
**       $Date: 2006/07/13 12:51:57 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCam.cc,v $
**  Revision 1.12  2006/07/13 12:51:57  jhense
**  Added new interface getFrameStartMutex(). Other threads can be notified, when
**  frame transmission started.
**
**  Revision 1.11  2006/03/09 14:55:32  jhense
**  New function measureRelativeStarttimes to get the timing of a
**  sequence.
**
**  Revision 1.10  2005/12/14 16:11:25  jhense
**  Additional functions: applySettingsToSequenceRegister and selectSequenceRegister for better support of Sequence-Feature.
**
**  Revision 1.9  2005/10/28 12:41:46  jhense
**  SUBDIRS = etc scripts
**
**  AM_CXXFLAGS = -Wall -g -O2 -D_REENTRANT -DLIBLMBCAMDEBUG
**
**  lib_LTLIBRARIES = liblmbcam.la
**
**  liblmbcam_la_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@
**  liblmbcam_la_LIBADD = -ldc1394_control -lraw1394 -lpthread
**
**  liblmbcam_la_SOURCES =	FileCam.cc \
**  			FireCam.cc \
**  			FireCamBusRegistry.cc \
**  			FireCamBus.cc \
**  			FireCamParam.cc \
**  			LMBCam.cc \
**  			LMBCamBusIndex.cc \
**  			LMBCamFileIO.cc \
**  			LMBErrorHandler.cc
**  #			V4LCam.cc \
**  #			V4LCamBus.cc \
**  #			V4LCamParam.cc
**
**  pkginclude_HEADERS =	AtomicCounter.hh \
**  			AtomicCounter.icc \
**  			FileCam.hh \
**  			FileCam.icc \
**  			FileCamError.hh \
**  			FireCam.hh \
**  			FireCam.icc \
**  			FireCamBusRegistry.hh \
**  			FireCamBus.hh \
**  			FireCamBus.icc \
**  			FireCamError.hh \
**  			FireCamParam.hh \
**  			FireCamParam.icc \
**  			LMBCam.hh \
**  			LMBCam.icc \
**  			LMBCamBus.hh \
**  			LMBCamBusIndex.hh \
**  			LMBCamError.hh \
**  			LMBCamFileIO.hh \
**  			LMBCamParam.hh \
**  			LMBError.hh \
**  			LMBErrorHandler.hh \
**  			MutexLocker.hh
**  #			V4LCam.hh \
**  #			V4LCam.icc \
**  #			V4LCamBus.hh \
**  #			V4LCamBus.icc \
**  #			V4LCamError.hh \
**  #			V4LCamParam.hh \
**  #			V4LCamParam.icc
**
**  Revision 1.8  2004/12/03 09:09:00  ronneber
**  - fixed order in loadParametersFromMap(). Now framerate in format7
**    works correctly
**
**  Revision 1.7  2004/11/05 15:49:40  mechnich
**  fixed bug with std::string::substr()
**
**  Revision 1.6  2004/10/19 05:54:46  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.5  2004/05/28 12:09:00  mechnich
**  added extended parameters
**
**  Revision 1.4  2004/05/09 17:03:59  mechnich
**  corrected errors in saveParametersToMap()
**
**  Revision 1.3  2004/05/09 16:26:11  mechnich
**  added saveParametersToMap() ( after adding loadParametersFromMap())
**
**  Revision 1.2  2004/05/09 16:02:04  mechnich
**  added loadParameterMap() function
**
**  Revision 1.1  2003/10/05 19:30:02  mechnich
**  initial revision
**
**
**
**************************************************************************/

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
      is >> eatWsAndComments >> buf;
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
void liblmbcam::LMBCam::getFrameStartMutex(pthread_mutex_t**, pthread_cond_t**)
{}

