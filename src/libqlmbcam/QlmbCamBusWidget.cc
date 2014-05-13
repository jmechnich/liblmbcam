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
  setSelectionMode( QAbstractItemView::SingleSelection);
  
  connect( this, SIGNAL( itemSelectionChanged()),
           this, SLOT( changeSelectedCamera()));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamBusWidget::~QlmbCamBusWidget()
{
  clearBusses();
}

void
libqlmbcam::QlmbCamBusWidget::clearBusses()
{
  for( std::map<QString,liblmbcam::LMBCamBus*>::iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    delete it->second;
  }
  _busses.clear();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamBusWidget::rescan()
{
  clear();
  clearBusses();
  
  /*-----------------------------------------------------------------------
   *  Scan for ieee1394 busses
   *-----------------------------------------------------------------------*/
  QTreeWidgetItem* ieee1394root = new QTreeWidgetItem( this);
  ieee1394root->setText( 0, "FireCamBus");
  ieee1394root->setExpanded( true);
  Qt::ItemFlags flags = ieee1394root->flags();
  flags &= ~Qt::ItemIsSelectable;
  ieee1394root->setFlags( flags);
  
  QTreeWidgetItem* v4lroot = new QTreeWidgetItem( this);
  v4lroot->setText( 0, "V4LCamBus");
  v4lroot->setExpanded( true);
  v4lroot->setFlags( flags);
  
  int errorhandlerMode = liblmbcam::LMBErrorHandler::ErrorHandler()->mode();
  liblmbcam::LMBErrorHandler::ErrorHandler()->setMode(
      liblmbcam::LMBErrorHandler::THROW);
  
  liblmbcam::LMBCam* tmpCamera = 0;
  QString busName;
  try
  {
    busName = ieee1394root->text(0);
    _busses[busName] = new liblmbcam::LMBCamBusIndex(
        liblmbcam::LMBCamBusIndex::FireCam);
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
    
    busName = v4lroot->text(0);
    _busses[busName] = new liblmbcam::LMBCamBusIndex(
        liblmbcam::LMBCamBusIndex::V4LCam);
    for( unsigned int i=0; i < _busses[busName]->nCameras(); ++i)
    {
      tmpCamera = _busses[busName]->cameraByIndex( i);
      QTreeWidgetItem* tmpCameraItem = new QTreeWidgetItem( v4lroot);
      tmpCameraItem->setText( 0, "V4LCam"+QString::number(i));
      tmpCameraItem->setText( 1, tmpCamera->vendor().c_str());
      tmpCameraItem->setText( 2, tmpCamera->model().c_str());
      tmpCameraItem->setText( 3, tmpCamera->guid().c_str());
    }
    
    if( v4lroot->child(0) != 0)
    {
      v4lroot->setExpanded( true);
    }
    
  }
  catch( liblmbcam::LMBCamBusError& err)
  {
    QMessageBox::warning( this, "Rescan bus ...",
                          "Caught LMBCamBusError: " +
                          QString( err.str().c_str()),
                          QMessageBox::Ok, 0);
  }
  catch( liblmbcam::LMBCamError& err)
  {
    QMessageBox::warning( this, "Rescan bus ...",
                          "Caught LMBCamError: " +
                          QString( err.str().c_str()),
                          QMessageBox::Ok, 0);
  }
  catch( liblmbcam::LMBError& err)
  {
    QMessageBox::warning( this, "Rescan bus ...",
                          "Caught LMBError: " +
                          QString( err.str().c_str()),
                          QMessageBox::Ok, 0);
  }
  catch( ...)
  {
    QMessageBox::warning( this, "Rescan bus ...",
                          "Unknown error !!! ",
                          QMessageBox::Ok, 0);
  }

  liblmbcam::LMBErrorHandler::ErrorHandler()->setMode( errorhandlerMode);
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
        setCurrentItem( *it);
        //(*it)->setSelected( true);
      }
          
      return;
    }
    
    ++it;
  }
}
