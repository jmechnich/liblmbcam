/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamParamControl.cc,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2006/05/23 12:02:09 $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamParamControl.cc,v $
**  Revision 1.5  2006/05/23 12:02:09  ronneber
**  - made DMA button available again
**
**  Revision 1.4  2003/08/12 16:58:58  mechnich
**  replaced STL strings by c-style strings in conjunction with Qt functions
**  for compatibility reasons
**
**  Revision 1.3  2003/05/22 08:53:44  mechnich
**  added comments and made short functions inline
**
**  Revision 1.2  2003/05/22 08:27:08  mechnich
**  added signal-and-slot mechanism for controlling the cameras parameters
**
**  Revision 1.1  2003/05/21 08:58:39  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "QlmbCamParamControl.hh"

#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

#include <map>
#include <string>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QlmbCamParamControlWidget::QlmbCamParamControlWidget( LMBCam* camera,
                                                      QWidget* parent)
        : QWidget( parent), p_camera( camera), p_ncols( 3)
        , p_grid( new QGridLayout( this))
{
  /*-----------------------------------------------------------------------
   *  Get paramter map from camera and create widgets according to GUIHints
   *-----------------------------------------------------------------------*/
  std::map<std::string,LMBCamParam*> parameters = p_camera->parameterMap();
  for( std::map<std::string,LMBCamParam*>::const_iterator it =
           parameters.begin(); it != parameters.end(); ++it)
  {
    if( it->second->GUIHint() == "slider")
    {
      createSlider( it->first.c_str(), it->second, this);
    }
    
    else if( it->second->GUIHint() == "checkbox")
    {
      createCheckBox( it->first.c_str(), it->second, this);
    }
    else
    {
    }
  }
  
  /*-----------------------------------------------------------------------
   *  Find parameters with auto mode and connect the sliders'
   *  'setDisabled'-slot to the checkbox state
   *-----------------------------------------------------------------------*/
  for( std::vector<QSlider*>::const_iterator sliderIt = p_sliders.begin();
       sliderIt != p_sliders.end(); ++sliderIt)
  {
    for( std::vector<QCheckBox*>::const_iterator boxIt = p_autoBoxes.begin();
         boxIt != p_autoBoxes.end(); ++boxIt)
    {
      QString paramName( (*boxIt)->objectName());
      paramName.truncate( paramName.length() - 5);
      
      if( QString( (*sliderIt)->objectName()).startsWith( paramName))
      {
        (*sliderIt)->setDisabled( (*boxIt)->isChecked());
        connect( (*boxIt), SIGNAL( toggled( bool)),
                 (*sliderIt), SLOT( setDisabled( bool)));
      }
    }
  }
  setLayout( p_grid);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QSlider*
QlmbCamParamControlWidget::createSlider( const QString& name,
                                         LMBCamParam* param,
                                         QWidget* parent)
{
  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = p_grid->rowCount();
  p_grid->addWidget( new QLabel( name, parent), row, 0);
  QSlider* slider = new QSlider( Qt::Horizontal, parent);
  slider->setMinimum( param->minValueAsInt());
  slider->setMaximum( param->maxValueAsInt());
  slider->setPageStep( 10);
  slider->setValue( param->asInt());
  slider->setObjectName( name);
  
  p_grid->addWidget( slider, row, 1);
  QLabel* label = new QLabel( QString::number( param->asInt()), parent);
  p_grid->addWidget( label, row, 2);
  connect( slider, SIGNAL( valueChanged(int)),
           label, SLOT( setNum(int)));
  connect( slider, SIGNAL( valueChanged(int)),
           this, SLOT( changeParamValue(int)));

  p_sliders.push_back( slider);
  
  return slider;
}

  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QCheckBox*
QlmbCamParamControlWidget::createCheckBox( const QString& name,
                                           LMBCamParam* param,
                                           QWidget* parent)
{
  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = p_grid->rowCount();
  p_grid->addWidget( new QLabel( name, parent), row, 0);
  QCheckBox* checkbox = new QCheckBox( parent);
  checkbox->setObjectName( name);
  checkbox->setChecked( param->asInt());
  p_grid->addWidget( checkbox, row, 1);
  p_grid->addWidget( new QLabel( parent), row, 2);

  if( name.endsWith( "auto"))
  {
    p_autoBoxes.push_back( checkbox);
    connect( checkbox, SIGNAL( toggled(bool)),
             this, SLOT( changeAutoMode(bool)));
  }

  /*-----------------------------------------------------------------------
   *  Disable DMA checkbox as long as there is no special handling for it
   *-----------------------------------------------------------------------*/
//  if( name.startsWith( "DMA"))
//  {
//    checkbox->setDisabled( true);
//  }
    
  return checkbox;
}

