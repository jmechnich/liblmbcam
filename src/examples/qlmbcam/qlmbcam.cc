/**************************************************************************
**       Title: 
**    $RCSfile: qlmbcam.cc,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2005/02/28 00:22:17 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: qlmbcam.cc,v $
**  Revision 1.5  2005/02/28 00:22:17  mechnich
**  changed include path for libcmdline headers
**
**  Revision 1.4  2004/10/19 05:51:14  mechnich
**  changed LMBError interface; lock certain camera controls (like DMA) while camera is running
**
**  Revision 1.3  2004/03/18 15:40:12  mechnich
**  *** empty log message ***
**
**  Revision 1.2  2004/01/26 21:37:34  mechnich
**  eat libdc1394 output
**
**  Revision 1.1  2003/10/05 19:30:34  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include <iostream>

#include <CmdArgs.hh>
#include <CmdLine.hh>
#include <ArgvIter.hh>

#include <qapplication.h>

#include "QlmbCamMainWindow.hh"

#include <LMBError.hh>
#include <LMBErrorHandler.hh>

using namespace liblmbcam;

int main( int argc, char** argv)
{
  freopen( "/dev/null", "w+", stdout);

  CmdLine cmd( argv[0], "", true, NULL);

  cmd.description( "qlmbcam - a common interface to all liblmbcam bus"
                   " and camera classes using the libqlmbcam GUI library");

  LMBErrorHandler::ErrorHandler()->setMode( LMBErrorHandler::THROW);
  
  try
  {
    /*---------------------------------------------------------------------
     *  Parse command line
     *---------------------------------------------------------------------*/
    ArgvIter arg_iter( --argc, ++argv);
    cmd.parse( arg_iter);

    /*---------------------------------------------------------------------
     *  Setup GUI
     *---------------------------------------------------------------------*/
    QApplication app( argc, argv);
    QlmbCamMainWindow widget;
    app.setMainWidget( &widget);
    widget.show();
    return app.exec();
  }
  catch( LMBCriticalError& err)
  {
    std::cerr << "Caught LMBCriticalError: "
              << err.str() << std::endl;
    return 1;
  }
  catch( LMBError& err)
  {
    std::cerr << "Caught LMBError: "
              << err.str() << std::endl;
  }
  catch (CmdLineUsageError e)
  {
    cmd.usage( std::cerr);
    return 0;
  }
  catch (CmdLineUsageHTMLError e)
  {
    cmd.usageHTML( std::cerr);
    return 0;
  }
  catch (CmdLineUsageXMLError e)
  {
    cmd.usageXML( std::cerr);
    return 0;
  }
  catch (CmdLineSyntaxError e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage( std::cerr);
    return 1;
  }
  catch( ...)
  {
    std::cerr << "Caught unknown error, bailing out." << std::endl;
    return 1;
  }
  
}
