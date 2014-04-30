/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamBusWidget.cc,v $
**   $Revision: 1.9 $$Name:  $
**       $Date: 2006/06/08 11:57:18 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamBusWidget.cc,v $
**  Revision 1.9  2006/06/08 11:57:18  jhense
**  Added empty destructor.
**
**  Revision 1.8  2004/10/19 05:55:38  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.7  2004/02/20 22:42:30  mechnich
**  corrected error handling
**
**  Revision 1.6  2003/12/18 15:32:14  mechnich
**  on rescan up to 8 FireCamBusses are checked for existence
**
**  Revision 1.5  2003/12/02 16:36:57  mechnich
**  removed V4L support until problem with inclusion of <linux/videodev2.h> is fixed
**
**  Revision 1.4  2003/10/17 22:52:48  mechnich
**  - added geometric equalizer
**  - camera in bus widget is selected now when video widget gets focus
**  - fixed bug in update image
**
**  Revision 1.3  2003/10/10 15:14:31  mechnich
**  added V4L support
**
**  Revision 1.2  2003/10/05 19:28:35  mechnich
**  update
**
**  Revision 1.1  2003/10/02 15:35:03  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "QlmbCamBusWidget.hh"

#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>

#include <LMBCamBusIndex.hh>
#include <LMBError.hh>
#include <LMBCam.hh>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamBusWidget::QlmbCamBusWidget( QWidget* parent)
        :QTreeWidget( parent), _variousRoot( 0)
{
  setColumnCount( 4);
  QTreeWidgetItem* header = headerItem();
  header->setText( 0, "Camera Overview");
  header->setText( 1, "Vendor");
  header->setText( 2, "Model");
  header->setText( 3, "Node ID");
  setRootIsDecorated( true);
  setAllColumnsShowFocus( true);
  connect( this, SIGNAL( itemSelectionChanged()),
           this, SLOT( changeSelectedCamera()));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamBusWidget::~QlmbCamBusWidget()
{
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamBusWidget::rescan()
{
  clear();
  for( std::map<QString,liblmbcam::LMBCamBus*>::iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    delete it->second;
  }
  _busses.clear();
  
  /*-----------------------------------------------------------------------
   *  Scan for ieee1394 busses
   *-----------------------------------------------------------------------*/
  QTreeWidgetItem* ieee1394root = new QTreeWidgetItem( this);
  ieee1394root->setText( 0, "FireCamBus");
  ieee1394root->setExpanded( true);
  Qt::ItemFlags flags = ieee1394root->flags();
  flags &= ~Qt::ItemIsSelectable;
  ieee1394root->setFlags( flags);
  
//   QListViewItem* v4lroot = new QListViewItem( this, "V4LCamBus");
//   v4lroot->setOpen( true);
//   v4lroot->setSelectable( false);
  
  // int errorhandlerMode = liblmbcam::LMBErrorHandler::ErrorHandler()->mode();
  // liblmbcam::LMBErrorHandler::ErrorHandler()->setMode(
  //     liblmbcam::LMBErrorHandler::THROW);
  
  try
  {
    QString busName( "FireCamBus");
    _busses[busName] = new liblmbcam::LMBCamBusIndex(
        liblmbcam::LMBCamBusIndex::FireCam);
    liblmbcam::LMBCam* tmpCamera = 0;
    for( unsigned int i=0; i < _busses[busName]->nCameras(); ++i)
    {
      tmpCamera = _busses[busName]->cameraByIndex( i);
      QTreeWidgetItem* tmpCameraItem = new QTreeWidgetItem( ieee1394root);
      tmpCameraItem->setText( 0, "FireCam"+QString::number(i));
      tmpCameraItem->setText( 1, tmpCamera->vendor().c_str());
      tmpCameraItem->setText( 2, tmpCamera->model().c_str());
      tmpCameraItem->setText( 3, tmpCamera->guid().c_str());
    }
    
    if( ieee1394root->child(0) != 0)
    {
      ieee1394root->setExpanded( true);
    }
    
//     try
//     {
//       _busses["V4LCamBus0"] = new liblmbcam::V4LCamBus;
//       tmp = new QTreeWidgetItem( v4lroot, "0");
//       tmp->setSelectable( false);
//       for( unsigned int i=0; i < _busses["V4LCamBus0"]->nCameras(); ++i)
//       {
//         tmpCamera = _busses["V4LCamBus0"]->cameraByIndex( i);
//         new QTreeWidgetItem( tmp, "V4LCam"+QString::number(i),
//                            tmpCamera->vendor().c_str(),
//                            tmpCamera->model().c_str(),
//                            tmpCamera->guid().c_str());
//       }
      
//       if( tmp->firstChild() != 0)
//       {
//         tmp->setOpen( true);
//       }
//     }
//     catch(...)
//     {
//     }
    
//     liblmbcam::LMBErrorHandler::ErrorHandler()->setMode( errorhandlerMode);
  }
  catch( liblmbcam::LMBCamBusError& err)
  {
    QMessageBox::warning( this, "Rescan bus ...",
                          "Caught LMBCamBusError: " +
                          QString( err.str().c_str()),
                          QMessageBox::Ok, 0);
  }
  catch( ...)
  {
    QMessageBox::warning( this, "Rescan bus ...",
                          "Unknown error !!! ",
                          QMessageBox::Ok, 0);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamBusWidget::changeSelectedCamera()
{
  QList<QTreeWidgetItem*> selection = selectedItems();
  if( selection.size() == 0) return;
  QTreeWidgetItem* li = selection[0];
  QTreeWidgetItem* busIndexItem = li->parent();
  liblmbcam::LMBCamBus* bus = _busses[busIndexItem->text(0)];
  liblmbcam::LMBCam* cam = bus->cameraByGUID( li->text(3).toLatin1().data());
  emit( cameraSelectionChanged( cam));
}

  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamBusWidget::changeSelectedCamera( const QString& guid)
{
  QTreeWidgetItemIterator it( this);
  while( *it)
  {
    if( (*it)->text( 3) == guid)
    {
      if( !(*it)->isSelected())
      {
        (*it)->setSelected( true);
      }
          
      return;
    }
    
    ++it;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamBusWidget::addCamera( liblmbcam::LMBCam* cam)
{}
