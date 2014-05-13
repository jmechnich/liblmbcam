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

#include <iostream>

#include <CmdArgs.hh>
#include <CmdLine.hh>
#include <ArgvIter.hh>

#include <QApplication>

#include "QlmbCamMainWindow.hh"

#include <LMBError.hh>
#include <LMBErrorHandler.hh>

using namespace liblmbcam;

int main( int argc, char** argv)
{
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

    //freopen( "/dev/null", "w+", stdout);

    /*---------------------------------------------------------------------
     *  Setup GUI
     *---------------------------------------------------------------------*/
    QApplication app( argc, argv);
    QlmbCamMainWindow widget;
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
