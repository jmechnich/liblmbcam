/****************************************************************************
** QlmbCamMainWindow meta object code from reading C++ file 'QlmbCamMainWindow.hh'
**
** Created: Mon Aug 28 21:23:47 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../../src/examples/qlmbcam/QlmbCamMainWindow.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QlmbCamMainWindow::className() const
{
    return "QlmbCamMainWindow";
}

QMetaObject *QlmbCamMainWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QlmbCamMainWindow( "QlmbCamMainWindow", &QlmbCamMainWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QlmbCamMainWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamMainWindow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QlmbCamMainWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamMainWindow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QlmbCamMainWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "camera", &static_QUType_ptr, "liblmbcam::LMBCam", QUParameter::In }
    };
    static const QUMethod slot_0 = {"setActiveCamera", 1, param_slot_0 };
    static const QUMethod slot_1 = {"loadSettings", 0, 0 };
    static const QUMethod slot_2 = {"saveSettings", 0, 0 };
    static const QUMethod slot_3 = {"rescanBus", 0, 0 };
    static const QUMethod slot_4 = {"grabImage", 0, 0 };
    static const QUMethod slot_5 = {"ngrabImage", 0, 0 };
    static const QUMethod slot_6 = {"startCamera", 0, 0 };
    static const QUMethod slot_7 = {"stopCamera", 0, 0 };
    static const QUParameter param_slot_8[] = {
	{ "deleteWidget", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_8 = {"stopCamera", 1, param_slot_8 };
    static const QUMethod slot_9 = {"removeWidgetAndStopCamera", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "setActiveCamera(liblmbcam::LMBCam*)", &slot_0, QMetaData::Private },
	{ "loadSettings()", &slot_1, QMetaData::Private },
	{ "saveSettings()", &slot_2, QMetaData::Private },
	{ "rescanBus()", &slot_3, QMetaData::Private },
	{ "grabImage()", &slot_4, QMetaData::Private },
	{ "ngrabImage()", &slot_5, QMetaData::Private },
	{ "startCamera()", &slot_6, QMetaData::Private },
	{ "stopCamera()", &slot_7, QMetaData::Private },
	{ "stopCamera(bool)", &slot_8, QMetaData::Private },
	{ "removeWidgetAndStopCamera()", &slot_9, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"QlmbCamMainWindow", parentObject,
	slot_tbl, 10,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QlmbCamMainWindow.setMetaObject( metaObj );
    return metaObj;
}

void* QlmbCamMainWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QlmbCamMainWindow" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool QlmbCamMainWindow::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setActiveCamera((liblmbcam::LMBCam*)static_QUType_ptr.get(_o+1)); break;
    case 1: loadSettings(); break;
    case 2: saveSettings(); break;
    case 3: rescanBus(); break;
    case 4: grabImage(); break;
    case 5: ngrabImage(); break;
    case 6: startCamera(); break;
    case 7: stopCamera(); break;
    case 8: stopCamera((bool)static_QUType_bool.get(_o+1)); break;
    case 9: removeWidgetAndStopCamera(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QlmbCamMainWindow::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QlmbCamMainWindow::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool QlmbCamMainWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
