/****************************************************************************
** libqlmbcam::QlmbCamBusWidget meta object code from reading C++ file 'QlmbCamBusWidget.hh'
**
** Created: Mon Aug 28 21:23:26 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../src/libqlmbcam/QlmbCamBusWidget.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *libqlmbcam::QlmbCamBusWidget::className() const
{
    return "libqlmbcam::QlmbCamBusWidget";
}

QMetaObject *libqlmbcam::QlmbCamBusWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_libqlmbcam__QlmbCamBusWidget( "libqlmbcam::QlmbCamBusWidget", &libqlmbcam::QlmbCamBusWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString libqlmbcam::QlmbCamBusWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "libqlmbcam::QlmbCamBusWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString libqlmbcam::QlmbCamBusWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "libqlmbcam::QlmbCamBusWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* libqlmbcam::QlmbCamBusWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QListView::staticMetaObject();
    static const QUMethod slot_0 = {"rescan", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ "guid", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"changeSelectedCamera", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "cam", &static_QUType_ptr, "liblmbcam::LMBCam", QUParameter::In }
    };
    static const QUMethod slot_2 = {"addCamera", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "li", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_3 = {"changeSelectedCamera", 1, param_slot_3 };
    static const QMetaData slot_tbl[] = {
	{ "rescan()", &slot_0, QMetaData::Public },
	{ "changeSelectedCamera(const QString&)", &slot_1, QMetaData::Public },
	{ "addCamera(liblmbcam::LMBCam*)", &slot_2, QMetaData::Public },
	{ "changeSelectedCamera(QListViewItem*)", &slot_3, QMetaData::Protected }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_ptr, "liblmbcam::LMBCam", QUParameter::In }
    };
    static const QUMethod signal_0 = {"cameraSelectionChanged", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "cameraSelectionChanged(liblmbcam::LMBCam*)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"libqlmbcam::QlmbCamBusWidget", parentObject,
	slot_tbl, 4,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_libqlmbcam__QlmbCamBusWidget.setMetaObject( metaObj );
    return metaObj;
}

void* libqlmbcam::QlmbCamBusWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "libqlmbcam::QlmbCamBusWidget" ) )
	return this;
    return QListView::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL cameraSelectionChanged
void libqlmbcam::QlmbCamBusWidget::cameraSelectionChanged( liblmbcam::LMBCam* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

bool libqlmbcam::QlmbCamBusWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: rescan(); break;
    case 1: changeSelectedCamera((const QString&)static_QUType_QString.get(_o+1)); break;
    case 2: addCamera((liblmbcam::LMBCam*)static_QUType_ptr.get(_o+1)); break;
    case 3: changeSelectedCamera((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QListView::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool libqlmbcam::QlmbCamBusWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: cameraSelectionChanged((liblmbcam::LMBCam*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QListView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool libqlmbcam::QlmbCamBusWidget::qt_property( int id, int f, QVariant* v)
{
    return QListView::qt_property( id, f, v);
}

bool libqlmbcam::QlmbCamBusWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
