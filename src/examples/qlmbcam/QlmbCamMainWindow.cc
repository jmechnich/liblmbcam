/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamMainWindow.cc,v $
**   $Revision: 1.11 $$Name:  $
**       $Date: 2005/11/06 18:55:11 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamMainWindow.cc,v $
**  Revision 1.11  2005/11/06 18:55:11  mechnich
**  fixed segfault on exit (thanks, Christoph;)
**
**  Revision 1.10  2005/05/10 20:57:29  ckeller
**  Added progress bar
**
**  Revision 1.9  2005/05/04 14:47:49  ckeller
**  added some checks if still saving
**
**  Revision 1.8  2005/05/04 12:09:05  ckeller
**  Now using thread to save images
**
**  Revision 1.7  2005/05/03 14:22:53  ckeller
**  added support for grabbing image series
**
**  Revision 1.6  2004/10/19 05:51:14  mechnich
**  changed LMBError interface; lock certain camera controls (like DMA) while camera is running
**
**  Revision 1.5  2003/10/17 22:52:48  mechnich
**  - added geometric equalizer
**  - camera in bus widget is selected now when video widget gets focus
**  - fixed bug in update image
**
**  Revision 1.4  2003/10/07 12:56:45  mechnich
**  replaced calls of QMessageBox::question() by QMessageBox::information()
**  for qt backward compatibility.
**
**  Revision 1.3  2003/10/07 05:19:21  mechnich
**  - added menu bat
**  - added grab button and functions
**
**  Revision 1.2  2003/10/06 14:01:50  mechnich
**  - added rescanBus() function
**  - changed calls of static QMessageBox functions for Qt backward compatibility
**
**  Revision 1.1  2003/10/05 19:30:34  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include <qfiledialog.h>

#include "QlmbCamMainWindow.hh"

#include <fstream>
#include <sstream>

#include <QLayout>
#include <QApplication>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QSpinBox> 


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::QlmbCamMainWindow(
 *                                        QWidget* parent, const char* name,
 *                                        WFlags f)
 *  ==> see headerfile
 *=======================================================================*/
QlmbCamMainWindow::QlmbCamMainWindow( QWidget* parent)
        :QWidget( parent), _camera( 0 ), _saveThread(0), _save_progressDialog(0)
{
  setWindowTitle( "qlmbcam - Main Window");

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->setMargin( 5);
  mainLayout->setSpacing( -1);

  QMenuBar* menuBar = new QMenuBar( this);

  QMenu *file = new QMenu( "&File", this );
  file->addAction( "&Load camera settings...",
                   this, SLOT( loadSettings()), Qt::CTRL+Qt::Key_L );
  file->addAction( "&Save camera settings...",
                   this, SLOT( saveSettings()), Qt::CTRL+Qt::Key_S );
  file->addSeparator();
  file->addAction( "&Quit", qApp, SLOT( quit()), Qt::CTRL+Qt::Key_C );
  menuBar->addMenu( file );

  mainLayout->setMenuBar( menuBar);
  
  QHBoxLayout* viewLayout = new QHBoxLayout;
  _grab_spinbox = new QSpinBox( this);
  _grab_spinbox->setMinimum( 1);
  _grab_spinbox->setMaximum( 300);
  _grab_spinbox->setSingleStep( 1);
  _grab_spinbox->setValue(1);
  
  _busWidget = new QlmbCamBusWidget( this);
  _busWidget->rescan();
  viewLayout->addWidget( _busWidget);
  
  connect( _busWidget, SIGNAL( cameraSelectionChanged( liblmbcam::LMBCam*)),
           this, SLOT( setActiveCamera( liblmbcam::LMBCam*)));
  
  _camWidget = new QlmbCamWidget( 0, this);
  _camWidget->setMinimumWidth( 350);
  viewLayout->addWidget( _camWidget);

  connect( _camWidget, SIGNAL( startCamera()), this, SLOT( startCamera()));
  connect( _camWidget, SIGNAL( stopCamera()), this, SLOT( stopCamera()));
  mainLayout->addLayout( viewLayout);
  
  QHBoxLayout* buttonLayout = new QHBoxLayout;
 
  QPushButton* rescanButton = new QPushButton( "Rescan bus", this);
  connect( rescanButton, SIGNAL( clicked()), this, SLOT( rescanBus()));
  buttonLayout->addWidget( rescanButton);
  buttonLayout->addStretch();

  _startButton = new QPushButton( "Start", this);
  _stopButton = new QPushButton( "Stop", this);
  _ngrabButton = new QPushButton("Grab",this);
  
//  _save_progressBar = new QProgressBar("Save Images",this);
  
  
  _startButton->setEnabled( false);
  _stopButton->setEnabled( false);
  _ngrabButton->setEnabled(false);
  
  connect( _startButton, SIGNAL( clicked()),
           this, SLOT( startCamera()));
  connect( _stopButton, SIGNAL( clicked()),
           this, SLOT( stopCamera()));
  connect(_ngrabButton,SIGNAL( clicked()),this,SLOT(ngrabImage()));

  buttonLayout->addWidget( _startButton);
  buttonLayout->addWidget( _stopButton);
  buttonLayout->addSpacing( 10);
  buttonLayout->addWidget( _ngrabButton);
  buttonLayout->addWidget(_grab_spinbox);

  mainLayout->addLayout( buttonLayout);
  setLayout( mainLayout);
  
  resize( 600, 500);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::~QlmbCamMainWindow()
 *  ==> see headerfile
 *=======================================================================*/
QlmbCamMainWindow::~QlmbCamMainWindow()
{
  _videoWindows.clear();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::setActiveCamera( LMBCam* camera)
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::setActiveCamera( LMBCam* camera)
{
  _camera = camera;

  _camWidget->setCamera( camera);

  if( _camera->isRunning())
  {
    _startButton->setEnabled( false);
    _stopButton->setEnabled( true);
    _ngrabButton->setEnabled(true);
    _camWidget->lockControls();

  }
  else
  {
    _startButton->setEnabled( true);
    _stopButton->setEnabled( false);
    _ngrabButton->setEnabled(false);
    _camWidget->unlockControls();

  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::loadSettings()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::loadSettings()
{
  if( _camera == 0)
  {
    int ret = QMessageBox::information( this, "Load Settings ...",
                                        "Select a camera first !",
                                        QMessageBox::Ok);
  }
  else
  {
    QString filename = QFileDialog::getOpenFileName(
        this, "Load Settings ...",
        QString( _camera->guid().c_str()) + ".cam", "*.cam");
    if( filename != QString::null)
    {
      try
      {
        std::ifstream stream( filename.toLatin1().data());

        int stop =
            QMessageBox::information( this, "Load Settings ...",
                                      "Loading from a file will replace current "
                                      "camera settings. Proceed ?",
                                      QMessageBox::Yes, QMessageBox::No);
        if( stop == QMessageBox::Yes)
        {
          bool camWasRunning = false;
          if( _camera->isRunning())
          {
            stopCamera();
            camWasRunning = true;
          }
          
          _camera->readState( stream);

          if( camWasRunning)
          {
            startCamera();
          }
        }
      }
      catch( LMBCamError err)
      {
        std::cout << err.str() << std::endl;
      }
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::saveSettings()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::saveSettings()
{
  if( _camera == 0)
  {
    int ret = QMessageBox::information( this, "Load Settings ...",
                                        "Select a camera first !",
                                        QMessageBox::Ok);
  }
  else
  {
    QString filename = QFileDialog::getSaveFileName(
        this, "Save Settings ...",
        QString( _camera->guid().c_str()) + ".cam",  "*.cam");
    if( filename != QString::null)
    {
      std::ofstream stream( filename.toLatin1().data());
      _camera->writeState( stream);
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::rescanBus()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::rescanBus()
{
  for( std::map<LMBCam*,QlmbCamVideoWidget*>::iterator it =
           _videoWindows.begin(); it != _videoWindows.end(); ++it)
  {
    delete it->second;    
  }
  _videoWindows.clear();
  _busWidget->rescan();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::grabImage()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::grabImage()
{
  if( _camera->isRunning())
  {
    std::map<LMBCam*,QlmbCamVideoWidget*>::iterator it =
        _videoWindows.find( _camera);
    if( it == _videoWindows.end())
    {
      return;
    }
    
    it->second->freezeImage( true);
    int action = QMessageBox::information( it->second, "Grab image ...",
                                           "Save image ?",
                                           QMessageBox::Yes, QMessageBox::No);
    switch( action)
    {
    case(QMessageBox::Yes):
    {
      it->second->setFocus();
      QString filename = QFileDialog::getSaveFileName(
          this, "Save Image ...");
      if( filename != QString::null)
      {
        it->second->saveCurrentImage( filename);
      }
      
      it->second->freezeImage( false);
      break;
    }
    case(QMessageBox::No):
      it->second->freezeImage( false);
      break;
    default:
      break;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void 
QlmbCamMainWindow::ngrabImage()
{
  if ( !_saveThread )
  {
    int nframes = (int)_grab_spinbox->value();
    _ngrabButton->setEnabled(false);
    std::map<LMBCam*,QlmbCamVideoWidget*>::iterator it =
        _videoWindows.find( _camera);
    if( it == _videoWindows.end())
      {
        return;
      }
    it->second->setBuffer((unsigned int)nframes,this);
    _stopButton->setEnabled(false);
    }
  else
  {
    QMessageBox::information( this, 
                              " Saving!"," Please wait until finished") ;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
bool
QlmbCamMainWindow::event( QEvent* cevent)
{
  if( cevent->type() < QEvent::User)
      return QWidget::event( cevent);
  
  /*-------------------------------------------------------------------------
   * Start Thread for saving files to disk 
   *-------------------------------------------------------------------------*/
  if(cevent->type() == (QEvent::Type)buffer_full)
  {
    BufferFullEvent* e = dynamic_cast<BufferFullEvent*>(cevent);  
    QString filename = QFileDialog::getSaveFileName(
        this, "Save Images ...");
    
    //start thread to save images
    _saveThread = new QlmbCamSaveThread( e->buffer(), 
                                           filename, 
                                           e->frameSize(), 
                                           e->nFrames(),
                                           e->width(), 
                                           e->height(), 
                                           e->colorCoding(),
                                           this);
    
    _saveThread->start();
    _stopButton->setEnabled(true);
  }
  
 /*-------------------------------------------------------------------------
  * Show Progress Bar for memory Buffer   
  *-------------------------------------------------------------------------*/
  if ( cevent->type() == (QEvent::Type)save_to_mem )
  {
    SavetoMemEvent* e = dynamic_cast<SavetoMemEvent*>(cevent);  
    if ( _save_progressDialog == 0 )
    {
      QString caption = "Grabbing %1 Images";
      _save_progressDialog = new QProgressDialog(
          caption.arg(e->nFrames()), QString(), 0, e->nFrames());
      _save_progressDialog->setWindowTitle("Saving");
      _save_progressDialog->setValue(e->actual_frame());
    }
    else 
    {
      _save_progressDialog->setValue(e->actual_frame());
    }
    
     if (e->actual_frame() == e->nFrames()) 
     {
       delete _save_progressDialog;
       _save_progressDialog=0;
     }
    
  }

  
  /*-------------------------------------------------------------------------
   *  Show Progress Bar for Writing to disk 
   *-------------------------------------------------------------------------*/
  if( cevent->type() == (QEvent::Type)saveDoneE && !_startButton->isEnabled () )
  {    
    SaveDoneEvent* e = dynamic_cast<SaveDoneEvent*>(cevent);  

    if (_save_progressDialog == 0)
    {
      QString caption = "Writing %1 Images to Disk";
      _save_progressDialog = new QProgressDialog(
          caption.arg(e->nFrames()), QString(), 0, e->nFrames());
      _save_progressDialog->setValue( e->actual_frame());
      _save_progressDialog->setWindowTitle("Saving");
    }
    else
    {
      _save_progressDialog->setValue( e->actual_frame() );
    }
    
    if ( e->saveDone() )
    {
      delete _save_progressDialog;
      _save_progressDialog = 0;
      _ngrabButton->setEnabled(true);
      _saveThread = 0;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::startCamera()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::startCamera()
{
  try
  {
    std::map<LMBCam*,QlmbCamVideoWidget*>::iterator it =
        _videoWindows.find( _camera);
    if( it != _videoWindows.end())
    {
      return;
    }
    
    _camWidget->readSettings();
    QlmbCamVideoWidget* tmp = new QlmbCamVideoWidget( _camera);
    
    std::stringstream caption;
    caption << _camera->vendor() << " "
            << _camera->model() << " ("
            << _camera->mode() << ", "
            << _camera->framerate() << " fps)";
    tmp->setWindowTitle( QString( caption.str().c_str()));
        
    connect( tmp, SIGNAL( aboutToClose()),
             this, SLOT( removeWidgetAndStopCamera()));
    connect( tmp, SIGNAL( focused( const QString& )),
             _busWidget, SLOT( changeSelectedCamera( const QString& )));
    
    _videoWindows[_camera] = tmp;
    
    _camera->startCameraRunContinuous();
    
    _startButton->setEnabled( false);
    _stopButton->setEnabled( true);
    _ngrabButton->setEnabled( true);
    
    _camWidget->lockControls();
    
    _videoWindows[_camera]->startUpdate();
    _videoWindows[_camera]->show();
  }
  catch( LMBError& err)
  {
    stopCamera();
    QMessageBox::warning( this, "Start Camera ...",
                          "Caught LMBError: " + QString( err.str().c_str()),
                          QMessageBox::Ok, 0);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::stopCamera( bool deleteWidget)
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::stopCamera( bool deleteWidget)
{
  try
  {
    std::map<LMBCam*,QlmbCamVideoWidget*>::iterator it =
        _videoWindows.find( _camera);
    if( it != _videoWindows.end())
    {
      if( deleteWidget)
      {
        delete it->second;
      }
      
      _videoWindows.erase( _camera);
    }
          
    _camera->stopCamera();

    _startButton->setEnabled( true);
    _stopButton->setEnabled( false);
    _camWidget->unlockControls();
  }
  catch( LMBError& err)
  {
    QMessageBox::warning( this, "Stop Camera ...",
                          "Caught LMBError: " + QString( err.str().c_str()),
                          QMessageBox::Ok, 0);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWindow::removeWidgetAndStopCamera()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::removeWidgetAndStopCamera()
{
  const QlmbCamVideoWidget* tmp = dynamic_cast<const QlmbCamVideoWidget*>( sender());
  
  if( tmp == 0)
  {
    std::cout << "removeWidgetAndStopCamera: sender is not of type "
              << "'QlmbCamVideoWidget'" << std::endl;
  }
  else
  {
    for( std::map<LMBCam*,QlmbCamVideoWidget*>::iterator it =
             _videoWindows.begin(); it != _videoWindows.end(); ++it)
    {
      if( it->second == tmp)
      {
        if( _camera == it->first)
        {
          stopCamera( false);
          return;
        }
        else
        {
          it->first->stopCamera();
          _videoWindows.erase( it->first);
          return;
        }
      }
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWindow::closeEvent(QCloseEvent* e)
{
  if( _saveThread)
  {
    if ( _saveThread->isRunning() ) 
    {
      int answer = QMessageBox::question( this, "Quit",
                                          "Still saving Images! Really Quit?", 
                                          QMessageBox::Yes,
                                          QMessageBox::No);
      switch(answer)
      {
      case(QMessageBox::Yes):
        _saveThread->terminate();
        e->accept();
        break;
      case(QMessageBox::No):
        e->ignore();
        break;
      default:
        e->ignore();
        break;
      }

    }
  }
  else
  {
    e->accept();
  }
}

