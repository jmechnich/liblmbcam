/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamBusWidget.hh,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2006/06/08 11:57:18 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamBusWidget.hh,v $
**  Revision 1.5  2006/06/08 11:57:18  jhense
**  Added empty destructor.
**
**  Revision 1.4  2003/12/02 16:36:57  mechnich
**  removed V4L support until problem with inclusion of <linux/videodev2.h> is fixed
**
**  Revision 1.3  2003/10/17 22:52:48  mechnich
**  - added geometric equalizer
**  - camera in bus widget is selected now when video widget gets focus
**  - fixed bug in update image
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

#ifndef QLMBCAMBUSWIDGET_HH
#define QLMBCAMBUSWIDGET_HH

#include <map>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>

#include <LMBCamBus.hh>

namespace libqlmbcam
{
  class QlmbCamBusWidget : public QTreeWidget
  {
    Q_OBJECT
  public:
    QlmbCamBusWidget( QWidget* parent=0);

    virtual ~QlmbCamBusWidget();
  public slots:
    void rescan();
    void changeSelectedCamera( const QString& guid);
    void addCamera( liblmbcam::LMBCam* cam);
    
  signals:
    void cameraSelectionChanged( liblmbcam::LMBCam*);

  protected slots:
    void changeSelectedCamera();
    
  protected:
    std::map<QString,liblmbcam::LMBCamBus*> _busses;
    QTreeWidgetItem* _variousRoot;
  };
}

#endif
