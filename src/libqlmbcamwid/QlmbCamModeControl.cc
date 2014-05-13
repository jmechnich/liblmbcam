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

#include "QlmbCamModeControl.hh"

#include <QLayout>
#include <QPushButton>
#include <QComboBox>

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
  QVBoxLayout* layout = new QVBoxLayout;
  QHBoxLayout* modeLayout = new QHBoxLayout;
  modeLayout->addWidget( new QLabel( "Mode:", this));
  p_modeCombo = new QComboBox( this);
  p_modeCombo->setEditable( false);
  modeLayout->addWidget( p_modeCombo);
  layout->addLayout( modeLayout);
  
  QHBoxLayout* framerateLayout = new QHBoxLayout;
  framerateLayout->addWidget( new QLabel( "Framerate:", this));
  p_framerateCombo = new QComboBox( this);
  p_framerateCombo->setEditable( false);
  framerateLayout->addWidget( p_framerateCombo);
  layout->addLayout( framerateLayout);

  QHBoxLayout* leftLayout = new QHBoxLayout;
  leftLayout->addWidget( new QLabel( "Left:", this));
  p_left = new QLineEdit( QString::number( p_camera->left()),
                          this);
  leftLayout->addWidget( p_left);
  layout->addLayout( leftLayout);

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->addWidget( new QLabel( "Top:", this));
  p_top = new QLineEdit( QString::number( p_camera->top()),
                         this);
  topLayout->addWidget( p_top);
  layout->addLayout( topLayout);

  QHBoxLayout* widthLayout = new QHBoxLayout;
  widthLayout->addWidget( new QLabel( "Width:", this));
  p_width = new QLineEdit( QString::number( p_camera->width()),
                           this);
  widthLayout->addWidget( p_width);
  layout->addLayout( widthLayout);

  QHBoxLayout* maxWidthLayout = new QHBoxLayout;
  maxWidthLayout->addWidget( new QLabel( "Max Width:", this));
  p_maxWidth = new QLabel( QString::number( p_camera->maxWidth()),
                           this);
  maxWidthLayout->addWidget( p_maxWidth);
  layout->addLayout( maxWidthLayout);

  QHBoxLayout* heightLayout = new QHBoxLayout;
  heightLayout->addWidget( new QLabel( "Height:", this));
  p_height = new QLineEdit( QString::number( p_camera->height()),
                            this);
  heightLayout->addWidget( p_height);
  layout->addLayout( heightLayout);
  
  QHBoxLayout* maxHeightLayout = new QHBoxLayout;
  maxHeightLayout->addWidget( new QLabel( "Max Height:", this));
  p_maxHeight = new QLabel( QString::number( p_camera->maxHeight()),
                            this);
  maxHeightLayout->addWidget( p_maxHeight);
  layout->addLayout( maxHeightLayout);

  int count = 0;
  std::vector<std::string> modes = p_camera->availableModes();
  for( std::vector<std::string>::const_iterator it = modes.begin();
       it != modes.end(); ++it)
  {
    p_modeCombo->addItem( (*it).c_str());
    if( *it == p_camera->mode())
    {
      p_modeCombo->setCurrentIndex( count);
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
    p_framerateCombo->addItem( QString::number( *it2));
    if( *it2 == p_camera->framerate())
    {
      p_framerateCombo->setCurrentIndex( count);
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
  p_framerateCombo->setEditText( QString::number( p_camera->framerate()));
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

