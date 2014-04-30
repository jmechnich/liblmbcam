/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamWidget.cc,v $
**   $Revision: 1.8 $$Name:  $
**       $Date: 2006/05/19 11:28:42 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamWidget.cc,v $
**  Revision 1.8  2006/05/19 11:28:42  jhense
**  Stopping camera when destroyed.
**
**  Revision 1.7  2006/05/18 15:29:12  jhense
**  Sliders can be adjusted with mouse wheel in steps of 1 (was 10).
**
**  Revision 1.6  2005/07/11 09:01:28  tschmidt
**  - Sliders for non-adjustable parameters of the camera are suppressed
**  - Multiple sliders for one parameter are now enabled/disabled by the
**    corresponding auto-checkbox... This is important for white balance,
**    for two sliders represent manual white balance adjustment:
**    white_balance_u and white_balance_v
**
**  Revision 1.5  2004/12/03 09:11:48  ronneber
**  - fixed order in writeSettings(). Now framerate in format7 works correctly
**  - read setings at the end of writes settings to ensure, that Widget is
**    in sync with actual camera settings
**
**  Revision 1.4  2004/10/19 05:55:38  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.3  2003/12/18 15:31:09  mechnich
**  sort framerates in descending order
**
**  Revision 1.2  2003/10/05 20:03:04  mechnich
**  added error handling in reset() function
**
**  Revision 1.1  2003/10/05 19:27:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "QlmbCamWidget.hh"

#include <QLayout>
#include <QPushButton>
#include <QFont>
#include <QMessageBox>
#include <QValidator>

#include <vector>

#include <LMBError.hh>
#ifdef HAVE_LIBDC1394
#include <FireCam.hh>
#endif

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Constructor
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamWidget::QlmbCamWidget( liblmbcam::LMBCam* camera,
                                          QWidget* parent)
        :QScrollArea( parent), _camera( camera), _controls( 0),
         _modeCombo( 0), _framerateCombo( 0)
{
  if( camera != 0)
  {
    createFireCamGUI();
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:Destructor
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamWidget::~QlmbCamWidget()
{
  if (_camera)
    _camera->stopCamera();
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION
 *  ==> see headerfile
 *=======================================================================*/

void
libqlmbcam::QlmbCamWidget::setCamera( liblmbcam::LMBCam* camera)
{
#ifdef HAVE_LIBDC1394
  if( dynamic_cast<liblmbcam::FireCam*>( camera))
  {
    _camera = camera;
    createFireCamGUI();
  }
#endif
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::updateGUI()
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::createFireCamGUI()
{
  /*-----------------------------------------------------------------------
   *  Reset everything
   *-----------------------------------------------------------------------*/
  if( _controls != 0)
  {
    delete _controls;
    _sliders.clear();
    _checkBoxes.clear();
    _lineEdits.clear();
  }
  
  /*-----------------------------------------------------------------------
   *  Create carrier widget
   *-----------------------------------------------------------------------*/
  _controls = new QWidget( viewport());
  _controls->setMinimumWidth( 300);
  QPalette pal1 = _controls->palette();
  QPalette pal2 = viewport()->palette();
  pal2.setColor( viewport()->backgroundRole(),
                 pal1.color( _controls->backgroundRole()));
  viewport()->setPalette(pal2);
  
  QLabel* label;
  QVBoxLayout* layout = new QVBoxLayout( _controls);
  layout->setMargin( 10);
  layout->setSpacing( 2);
  /*-----------------------------------------------------------------------
   *  General information section
   *-----------------------------------------------------------------------*/
  label = new QLabel( "Camera information", _controls);
  label->setAlignment( Qt::AlignCenter);
  QFont font( label->font());
  font.setUnderline( true);
  label->setFont( font);
  layout->addWidget( label);
  
  layout->addSpacing( 5);
  
  layout->addWidget( 
      new QLabel( "Vendor: " + QString( _camera->vendor().c_str()),
                  _controls));
  layout->addWidget( 
      new QLabel( "Model: " + QString( _camera->model().c_str()),
                  _controls));
  layout->addWidget( 
      new QLabel( "GUID: " + QString( _camera->guid().c_str()),
                  _controls));

  layout->addSpacing( 10);
  
  /*-----------------------------------------------------------------------
   *  Camera settings section
   *-----------------------------------------------------------------------*/
  label = new QLabel( "Camera settings", _controls);
  label->setAlignment( Qt::AlignCenter);
  label->setFont( font);
  layout->addWidget( label);

  layout->addSpacing( 5);
  
  QHBoxLayout* modeLayout = new QHBoxLayout( _controls);
  modeLayout->addWidget( new QLabel( "Mode:", _controls));
  _modeCombo = new QComboBox( _controls);
  _modeCombo->setEditable( false);
  modeLayout->addWidget( _modeCombo);
  layout->addLayout( modeLayout);
  
  QHBoxLayout* framerateLayout = new QHBoxLayout( _controls);
  framerateLayout->addWidget( new QLabel( "Framerate:", _controls));
  _framerateCombo = new QComboBox( _controls);
  _framerateCombo->setEditable( false);
  framerateLayout->addWidget( _framerateCombo);
  layout->addLayout( framerateLayout);
  
  connect( _modeCombo, SIGNAL( activated( const QString&)),
           this, SLOT( updateFramerateCombo( const QString&)));
  connect( _modeCombo, SIGNAL( activated( const QString&)),
           this, SLOT( updateLineEdits( const QString&)));
  
  QGridLayout* geometryLayout = new QGridLayout( _controls);
  geometryLayout->addWidget( new QLabel( "Left:", _controls), 0, 0);
  _left = new QLineEdit( QString::number( _camera->left()), _controls);
  _left->setAlignment( Qt::AlignRight);
  _left->setFixedWidth( 50);
  geometryLayout->addWidget( _left, 0, 1);

  geometryLayout->addWidget( new QLabel( "Top:", _controls), 0, 2);
  _top = new QLineEdit( QString::number( _camera->top()), _controls);
  _top->setAlignment( Qt::AlignRight);
  _top->setFixedWidth( 50);
  geometryLayout->addWidget( _top, 0, 3);

  geometryLayout->addWidget( new QLabel( "Width:", _controls), 1, 0);
  _width = new QLineEdit( QString::number( _camera->width()), _controls);
  _width->setAlignment( Qt::AlignRight);
  _width->setFixedWidth( 50);
  geometryLayout->addWidget( _width, 1, 1);

  geometryLayout->addWidget( new QLabel( "Height:", _controls), 1, 2);
  _height = new QLineEdit( QString::number( _camera->height()), _controls);
  _height->setAlignment( Qt::AlignRight);
  _height->setFixedWidth( 50);
  geometryLayout->addWidget( _height, 1, 3);

  geometryLayout->addWidget( new QLabel( "Max Width:", _controls), 2, 0);
  _maxWidth = new QLabel( QString::number( _camera->maxWidth()), _controls);
  _maxWidth->setAlignment( Qt::AlignRight);
  geometryLayout->addWidget( _maxWidth, 2, 1);

  geometryLayout->addWidget( new QLabel( "Max Height:", _controls), 2, 2);
  _maxHeight = new QLabel( QString::number( _camera->maxHeight()), _controls);
  _maxHeight->setAlignment( Qt::AlignRight);
  geometryLayout->addWidget( _maxHeight, 2, 3);
  
  layout->addLayout( geometryLayout);
  
  QHBoxLayout* buttonLayout = new QHBoxLayout( _controls);
  QPushButton* setButton = new QPushButton( "Set", _controls);
  buttonLayout->addWidget( setButton);
  QPushButton* readButton = new QPushButton( "Read", _controls);
  buttonLayout->addWidget( readButton);
  QPushButton* resetButton = new QPushButton( "Reset", _controls);
  buttonLayout->addWidget( resetButton);
  layout->addLayout( buttonLayout);
  
  connect( setButton, SIGNAL( clicked()), this, SLOT( writeSettings()));
  connect( readButton, SIGNAL( clicked()), this, SLOT( readSettings()));
  connect( resetButton, SIGNAL( clicked()), this, SLOT( resetCamera()));

  int count=0;
  std::vector<std::string> modes = _camera->availableModes();
  for( std::vector<std::string>::const_iterator it = modes.begin();
       it != modes.end(); ++it)
  {
    _modeCombo->addItem( (*it).c_str());
    if( *it == _camera->mode())
    {
      _modeCombo->setCurrentIndex( count);
    }
    ++count;
  }

  updateFramerateCombo( _camera->mode().c_str());
  _framerateCombo->setCurrentIndex(
      _framerateCombo->findText( QString::number( _camera->framerate())));

  updateLineEdits( _camera->mode().c_str());
  
  layout->addSpacing( 10);
  
  /*-----------------------------------------------------------------------
   *  Camera parameter section
   *-----------------------------------------------------------------------*/
  label = new QLabel( "Camera parameters", _controls);
  label->setAlignment( Qt::AlignCenter);
  label->setFont( font);
  layout->addWidget( label);

  layout->addSpacing( 5);
  
  QGridLayout* parameterGrid = new QGridLayout( _controls);
  parameterGrid->setSpacing( 5);
  
  /*-----------------------------------------------------------------------
   *  Get parameter map from camera and create widgets according to GUIHints
   *-----------------------------------------------------------------------*/
  std::map<std::string,liblmbcam::LMBCamParam*> parameters =
      _camera->parameterMap();
  for( std::map<std::string,liblmbcam::LMBCamParam*>::const_iterator it =
           parameters.begin(); it != parameters.end(); ++it)
  {
    if( it->second->GUIHint() == "slider")
    {
      createSlider( it->first.c_str(), it->second, parameterGrid);
    }
    
    else if( it->second->GUIHint() == "checkbox")
    {
      createCheckBox( it->first.c_str(), it->second, parameterGrid);
    }
    else if( it->second->GUIHint() == "lineedit")
    {
      createLineEdit( it->first.c_str(), it->second, parameterGrid);
    }
    else
    {
      parameterGrid->addWidget( new QLabel( it->first.c_str()));
      parameterGrid->addWidget( new QLabel);
      parameterGrid->addWidget(
          new QLabel( QString::number( it->second->asFloat())));
    }
  }
  
  /*-----------------------------------------------------------------------
   *  Find parameters with auto mode and connect the sliders'
   *  'setDisabled'-slot to the checkbox state
   *-----------------------------------------------------------------------*/
  for( std::map<std::string,QCheckBox*>::const_iterator boxIt
           = _checkBoxes.begin();
       boxIt != _checkBoxes.end(); ++boxIt)
  {
    if( QString( boxIt->first.c_str()).endsWith( "_auto"))
    {

      /*-------------------------------------------------------------------
       *  Search for corresponding sliders in the map... the fast search
       *  function from map had to be replaced by a slower one for we have to
       *  search for substrings and multiple occurences
       *-------------------------------------------------------------------*/
      std::string searchstring = boxIt->first.substr( 0, boxIt->first.size()-5);
      for (std::map<std::string,QSlider*>::const_iterator sl = _sliders.begin();
           sl != _sliders.end(); ++sl) 
      {
        if (sl->first.substr(0, boxIt->first.size()-5) == searchstring) 
        {
          sl->second->setDisabled( boxIt->second->isChecked());
          connect( boxIt->second, SIGNAL( toggled( bool)),
                   sl->second, SLOT( setDisabled( bool)));
        }
      }

      std::map<std::string,QLineEdit*>::const_iterator le =
          _lineEdits.find( boxIt->first.substr( 0, boxIt->first.size()-5) +
                           "_abs");
      if( le != _lineEdits.end())
      {
        le->second->setDisabled( boxIt->second->isChecked());
        connect( boxIt->second, SIGNAL( toggled( bool)),
                 le->second, SLOT( setDisabled( bool)));

        std::map<std::string,QCheckBox*>::const_iterator cb =
            _checkBoxes.find( le->first + "_switch");
        if( cb != _checkBoxes.end())
        {
          cb->second->setDisabled( boxIt->second->isChecked());
          connect( boxIt->second, SIGNAL( toggled( bool)),
                   cb->second, SLOT( setDisabled( bool)));
        }
      }
    }
    else if( QString( boxIt->first.c_str()).endsWith( "_abs_switch"))
    {
      std::map<std::string,QLineEdit*>::const_iterator le =
          _lineEdits.find( boxIt->first.substr( 0, boxIt->first.size()-11) +
                           "_abs");
      if( le != _lineEdits.end())
      {
        le->second->setEnabled( boxIt->second->isChecked());
        connect( boxIt->second, SIGNAL( toggled( bool)),
                 le->second, SLOT( setEnabled( bool)));

        std::map<std::string,QSlider*>::const_iterator sl =
            _sliders.find( boxIt->first.substr( 0, boxIt->first.size()-11));
        if( sl != _sliders.end())
        {
          sl->second->setDisabled( boxIt->second->isChecked());
          connect( boxIt->second, SIGNAL( toggled( bool)),
                   sl->second, SLOT( setDisabled( bool)));
        }
      }
    }
  }

  layout->addLayout( parameterGrid);
  
  //addChild( _controls);
  _controls->show();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::updateFramerateCombo(
 *                                                     const QString& mode)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::updateFramerateCombo( const QString& mode)
{
  if( _modeCombo == 0 || _framerateCombo == 0)
  {
    std::cout << "_modeCombo == 0 || _framerateCombo == 0: "
              << "This should not happen" << std::endl;
    return;
  }
  
  if( mode.mid( 0, 7) == "Format7")
  {
    _framerateCombo->clear();
    _framerateCombo->setEditable( true);
  }
  else
  {
    _framerateCombo->clear();
    _framerateCombo->setEditable( false);
    
    std::vector<double> framerates = _camera->availableFrameratesInMode(
        mode.toLatin1().data());
    for( std::vector<double>::const_iterator it = framerates.begin();
         it != framerates.end(); ++it)
    {
      _framerateCombo->insertItem( 0, QString::number( *it));
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::updateLineEdits(const QString& mode)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::updateLineEdits( const QString& mode)
{
  if( mode.startsWith( "Format7"))
  {
    _left->setEnabled( true);
    _top->setEnabled( true);
    _width->setEnabled( true);
    _height->setEnabled( true);
  }
  else
  {
    _left->setEnabled( false);
    _top->setEnabled( false);
    _width->setEnabled( false);
    _height->setEnabled( false);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::createLineEdit( const QString& name,
 *                              liblmbcam::LMBCamParam* param, QGrid* parent)
 *  ==> see headerfile
 *=======================================================================*/
QLineEdit*
libqlmbcam::QlmbCamWidget::createLineEdit( const QString& name,
                                         liblmbcam::LMBCamParam* param,
                                         QGridLayout* layout)
{
  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = layout->rowCount();
  layout->addWidget( new QLabel( name), row, 0);
  QLineEdit* lineedit = new QLineEdit( QString::number( param->asFloat()));
  lineedit->setObjectName( name);
  lineedit->setValidator( new QDoubleValidator( param->minValueAsFloat(),
                                                param->maxValueAsFloat(),
                                                10,
                                                lineedit));
  connect( lineedit, SIGNAL( returnPressed()),
           this, SLOT( changeAbsParamValue()));
  layout->addWidget( lineedit, row, 1);
  layout->addWidget( new QLabel, row, 2);

  _lineEdits[name.toLatin1().data()] = lineedit;
  
  return lineedit;
}

  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::createSlider( const QString& name,
 *                              liblmbcam::LMBCamParam* param, QGrid* parent)
 *  ==> see headerfile
 *=======================================================================*/
QSlider*
libqlmbcam::QlmbCamWidget::createSlider( const QString& name,
                                         liblmbcam::LMBCamParam* param,
                                         QGridLayout* layout)
{
  /*-----------------------------------------------------------------------
   *  Only create slider if min and max value differ... otherwise it
   *  makes no sense to provide one to the user
   *-----------------------------------------------------------------------*/
  if (param->minValueAsInt() == param->maxValueAsInt()) return 0;

  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = layout->rowCount();
  layout->addWidget( new QLabel( name), row, 0);
  QSlider* slider = new QSlider( Qt::Horizontal);
  slider->setMinimum( param->minValueAsInt());
  slider->setMaximum( param->maxValueAsInt());
  slider->setPageStep( 1);
  slider->setValue( param->asInt());
  slider->setObjectName( name);
  layout->addWidget( slider, row, 1);
  QLabel* label = new QLabel( QString::number( param->asInt()));
  layout->addWidget( label, row, 2);
  connect( slider, SIGNAL( valueChanged(int)),
           label, SLOT( setNum(int)));
  connect( slider, SIGNAL( valueChanged(int)),
           this, SLOT( changeParamValue(int)));
  
  _sliders[name.toLatin1().data()] = slider;
  
  return slider;
}

  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::createCheckBox( const QString& name,
 *                                liblmbcam::LMBCamParam* param, QGrid* parent)
  *  ==> see headerfile
 *=======================================================================*/
QCheckBox*
libqlmbcam::QlmbCamWidget::createCheckBox( const QString& name,
                                           liblmbcam::LMBCamParam* param,
                                           QGridLayout* layout)
{
  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = layout->rowCount();
  layout->addWidget( new QLabel( name), row, 0);
  QCheckBox* checkbox = new QCheckBox;
  checkbox->setObjectName( name);
  checkbox->setChecked( param->asInt());
  layout->addWidget( checkbox);
  layout->addWidget( new QLabel( name), row, 2);

  _checkBoxes[name.toLatin1().data()] = checkbox;
  connect( checkbox, SIGNAL( toggled(bool)),
           this, SLOT( changeAutoMode(bool)));

  return checkbox;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::changeAbsParamValue( const QString& value)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::changeAbsParamValue()
{
  std::map<std::string,QLineEdit*>::const_iterator le =
      _lineEdits.find( sender()->objectName().toLatin1().data());
  if( le != _lineEdits.end())
  {
    liblmbcam::LMBCamParam* param =
        _camera->param( le->first);
    if( !param)
    {
      std::cerr << __PRETTY_FUNCTION__ << ": unknown parameter " << le->first
                << std::endl;
    }
    param->setValue( le->second->text().toFloat());
    le->second->setText( QString::number( param->asFloat()));
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::changeParamValue( int value)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::changeParamValue( int value)
{
  QString parameter = sender()->objectName();
  
  _camera->param( parameter.toLatin1().data())->setValue( value);
  std::map<std::string,QLineEdit*>::const_iterator le =
      _lineEdits.find( (parameter + "_abs").toLatin1().data());
  if( le != _lineEdits.end())
  {
    le->second->setText( QString::number(
                             _camera->param( le->first)->asFloat()));
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::changeAutoMode( bool autoMode)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::changeAutoMode( bool autoMode)
{
  QString parameter = sender()->objectName();

  _camera->param( parameter.toLatin1().data())->setValue( autoMode ? 1 : 0);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::changeAutoMode( bool absMode)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::changeAbsControl( bool absMode)
{
  QString parameter = sender()->objectName();

  _camera->param( parameter.toLatin1().data())->setValue( absMode ? 1 : 0);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::changeAutoMode( bool absMode)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::lockControls()
{
  std::map<std::string,QCheckBox*>::const_iterator cb =
      _checkBoxes.find( "DMA");
  if( cb != _checkBoxes.end())
  {
    cb->second->setEnabled( false);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamWidget::changeAutoMode( bool absMode)
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::unlockControls()
{
  std::map<std::string,QCheckBox*>::const_iterator cb =
      _checkBoxes.find( "DMA");
  if( cb != _checkBoxes.end())
  {
    cb->second->setEnabled( true);
  }
}

 /*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::readSettings()
{
  _modeCombo->setCurrentIndex( 
      _modeCombo->findText( _camera->mode().c_str()));
  updateFramerateCombo( _modeCombo->currentText());
  _framerateCombo->setCurrentIndex(
      _framerateCombo->findText( QString::number( _camera->framerate())));
  _left->setText( QString::number( _camera->left()));
  _top->setText( QString::number( _camera->top()));
  _width->setText( QString::number( _camera->width()));
  _height->setText( QString::number( _camera->height()));
  _maxWidth->setText( QString::number( _camera->maxWidth()));
  _maxHeight->setText( QString::number( _camera->maxHeight()));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::writeSettings()
{
  try
  {
    bool camWasRunning = false;
    if( _camera->isRunning())
    {
      camWasRunning = true;
      emit( stopCamera());
    }

    _camera->setMode( _modeCombo->currentText().toLatin1().data());
    if( _modeCombo->currentText().startsWith( "Format7"))
    {
      _camera->setLeft( _left->text().toInt());
      _camera->setTop( _top->text().toInt());
      _camera->setWidth( _width->text().toInt());
      _camera->setHeight( _height->text().toInt());
    }
    _camera->setFramerate( _framerateCombo->currentText().toDouble(), true);

    if( camWasRunning)
        emit( startCamera());
  }
  catch( liblmbcam::LMBError& error)
  {
    QMessageBox::warning( this, "Write Settings ...", error.str().c_str(),
                          QMessageBox::Ok, 0);
  }
  readSettings();
  

}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamWidget::resetCamera()
{
  try
  {
    bool camWasRunning = false;
    if( _camera->isRunning())
    {
      camWasRunning = true;
      emit( stopCamera());
    }

    _camera->reset();
    readSettings();
    
    if( camWasRunning)
        emit( startCamera());
  }
  catch( liblmbcam::LMBError& error)
  {
    QMessageBox::warning( this, "Reset Settings ...", error.str().c_str(),
                          QMessageBox::Ok, 0);
  }
}
