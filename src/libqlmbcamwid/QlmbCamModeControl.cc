/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamModeControl.cc,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2004/12/03 09:13:53 $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamModeControl.cc,v $
**  Revision 1.5  2004/12/03 09:13:53  ronneber
**  - updateFormat7Controls() now displays actual framerate of camera
**
**  Revision 1.4  2003/09/19 14:39:42  mechnich
**  - changed handling of Format7 image geometry controls
**
**  Revision 1.3  2003/08/12 16:58:58  mechnich
**  replaced STL strings by c-style strings in conjunction with Qt functions
**  for compatibility reasons
**
**  Revision 1.2  2003/08/05 15:35:54  mechnich
**  added lineedits for Format7 image position and changed layout
**
**  Revision 1.1  2003/05/22 08:25:56  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "QlmbCamModeControl.hh"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qcombobox.h>

#include <string>
#include <vector>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QlmbCamModeControlWidget::QlmbCamModeControlWidget( LMBCam* camera,
                                                    QWidget* parent)
        :QWidget( parent), p_camera( camera)
{
  QVBoxLayout* layout = new QVBoxLayout( this);
  QHBoxLayout* modeLayout = new QHBoxLayout( layout);
  modeLayout->addWidget( new QLabel( "Mode:", this));
  p_modeCombo = new QComboBox( this);
  p_modeCombo->setEditable( false);
  modeLayout->addWidget( p_modeCombo);
  
  QHBoxLayout* framerateLayout = new QHBoxLayout( layout);
  framerateLayout->addWidget( new QLabel( "Framerate:", this));
  p_framerateCombo = new QComboBox( this);
  p_framerateCombo->setEditable( false);
  framerateLayout->addWidget( p_framerateCombo);

  QHBoxLayout* leftLayout = new QHBoxLayout( layout);
  leftLayout->addWidget( new QLabel( "Left:", this));
  p_left = new QLineEdit( QString::number( p_camera->left()),
                          this);
  leftLayout->addWidget( p_left);

  QHBoxLayout* topLayout = new QHBoxLayout( layout);
  topLayout->addWidget( new QLabel( "Top:", this));
  p_top = new QLineEdit( QString::number( p_camera->top()),
                         this);
  topLayout->addWidget( p_top);

  QHBoxLayout* widthLayout = new QHBoxLayout( layout);
  widthLayout->addWidget( new QLabel( "Width:", this));
  p_width = new QLineEdit( QString::number( p_camera->width()),
                           this);
  widthLayout->addWidget( p_width);

  QHBoxLayout* maxWidthLayout = new QHBoxLayout( layout);
  maxWidthLayout->addWidget( new QLabel( "Max Width:", this));
  p_maxWidth = new QLabel( QString::number( p_camera->maxWidth()),
                           this);
  maxWidthLayout->addWidget( p_maxWidth);

  QHBoxLayout* heightLayout = new QHBoxLayout( layout);
  heightLayout->addWidget( new QLabel( "Height:", this));
  p_height = new QLineEdit( QString::number( p_camera->height()),
                            this);
  heightLayout->addWidget( p_height);
  
  QHBoxLayout* maxHeightLayout = new QHBoxLayout( layout);
  maxHeightLayout->addWidget( new QLabel( "Max Height:", this));
  p_maxHeight = new QLabel( QString::number( p_camera->maxHeight()),
                            this);
  maxHeightLayout->addWidget( p_maxHeight);

  int count = 0;
  std::vector<std::string> modes = p_camera->availableModes();
  for( std::vector<std::string>::const_iterator it = modes.begin();
       it != modes.end(); ++it)
  {
    p_modeCombo->insertItem( (*it).c_str());
    if( *it == p_camera->mode())
    {
      p_modeCombo->setCurrentItem( count);
      if( it->substr( 0, 7) == "Format7")
      {
        updateFormat7Controls();
      }
      else
      {
        updateFramerateCombo();
      }
    }
    ++count;
  }
  
  connect( p_modeCombo, SIGNAL( activated( const QString&)),
           this, SLOT( changeVideoMode( const QString&)));
  connect( p_framerateCombo, SIGNAL( activated( const QString&)),
           this, SLOT( changeFramerate( const QString&)));
  connect( p_left, SIGNAL( returnPressed()),
           this, SLOT( updateFormat7Geometry()));
  connect( p_top, SIGNAL( returnPressed()),
           this, SLOT( updateFormat7Geometry()));
  connect( p_width, SIGNAL( returnPressed()),
           this, SLOT( updateFormat7Geometry()));
  connect( p_height, SIGNAL( returnPressed()),
           this, SLOT( updateFormat7Geometry()));
  
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamModeControlWidget::updateFramerateCombo()
{
  p_left->setDisabled( true);
  p_top->setDisabled( true);
  p_width->setDisabled( true);
  p_height->setDisabled( true);

  p_framerateCombo->clear();
  
  int count = 0;
  std::vector<double> framerates = p_camera->availableFramerates();
  for( std::vector<double>::const_iterator it2 = framerates.begin();
       it2 != framerates.end(); ++it2)
  {
    p_framerateCombo->insertItem( QString::number( *it2));
    if( *it2 == p_camera->framerate())
    {
      p_framerateCombo->setCurrentItem( count);
    }
    ++count;
  }

  p_framerateCombo->setDisabled( false);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamModeControlWidget::updateFormat7Controls()
{
  p_framerateCombo->setDisabled( true);
  p_framerateCombo->setCurrentText( QString::number( p_camera->framerate()));
  p_left->setText( QString::number( p_camera->left()));
  p_top->setText( QString::number( p_camera->top()));
  p_width->setText( QString::number( p_camera->width()));
  p_maxWidth->setText( QString::number( p_camera->maxWidth()));
  p_height->setText( QString::number( p_camera->height()));
  p_maxHeight->setText( QString::number( p_camera->maxHeight()));

  p_left->setDisabled( false);
  p_top->setDisabled( false);
  p_width->setDisabled( false);
  p_height->setDisabled( false);
}

