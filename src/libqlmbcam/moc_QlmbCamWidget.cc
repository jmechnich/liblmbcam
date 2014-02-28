/****************************************************************************
** libqlmbcam::QlmbCamWidget meta object code from reading C++ file 'QlmbCamWidget.hh'
**
** Created: Mon Aug 28 21:23:32 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../src/libqlmbcam/QlmbCamWidget.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *libqlmbcam::QlmbCamWidget::className() const
{
    return "libqlmbcam::QlmbCamWidget";
}

QMetaObject *libqlmbcam::QlmbCamWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_libqlmbcam__QlmbCamWidget( "libqlmbcam::QlmbCamWidget", &libqlmbcam::QlmbCamWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString libqlmbcam::QlmbCamWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "libqlmbcam::QlmbCamWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString libqlmbcam::QlmbCamWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "libqlmbcam::QlmbCamWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* libqlmbcam::QlmbCamWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QScrollView::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "camera", &static_QUType_ptr, "liblmbcam::LMBCam", QUParameter::In }
    };
    static const QUMethod slot_0 = {"setCamera", 1, param_slot_0 };
    static const QUMethod slot_1 = {"readSettings", 0, 0 };
    static const QUMethod slot_2 = {"writeSettings", 0, 0 };
    static const QUMethod slot_3 = {"resetCamera", 0, 0 };
    static const QUMethod slot_4 = {"lockControls", 0, 0 };
    static const QUMethod slot_5 = {"unlockControls", 0, 0 };
    static const QUParameter param_slot_6[] = {
	{ "mode", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_6 = {"updateFramerateCombo", 1, param_slot_6 };
    static const QUParameter param_slot_7[] = {
	{ "mode", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_7 = {"updateLineEdits", 1, param_slot_7 };
    static const QUMethod slot_8 = {"changeAbsParamValue", 0, 0 };
    static const QUParameter param_slot_9[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"changeParamValue", 1, param_slot_9 };
    static const QUParameter param_slot_10[] = {
	{ "autoMode", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_10 = {"changeAutoMode", 1, param_slot_10 };
    static const QUParameter param_slot_11[] = {
	{ "absMode", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_11 = {"changeAbsControl", 1, param_slot_11 };
    static const QMetaData slot_tbl[] = {
	{ "setCamera(liblmbcam::LMBCam*)", &slot_0, QMetaData::Public },
	{ "readSettings()", &slot_1, QMetaData::Public },
	{ "writeSettings()", &slot_2, QMetaData::Public },
	{ "resetCamera()", &slot_3, QMetaData::Public },
	{ "lockControls()", &slot_4, QMetaData::Public },
	{ "unlockControls()", &slot_5, QMetaData::Public },
	{ "updateFramerateCombo(const QString&)", &slot_6, QMetaData::Protected },
	{ "updateLineEdits(const QString&)", &slot_7, QMetaData::Protected },
	{ "changeAbsParamValue()", &slot_8, QMetaData::Protected },
	{ "changeParamValue(int)", &slot_9, QMetaData::Protected },
	{ "changeAutoMode(bool)", &slot_10, QMetaData::Protected },
	{ "changeAbsControl(bool)", &slot_11, QMetaData::Protected }
    };
    static const QUMethod signal_0 = {"startCamera", 0, 0 };
    static const QUMethod signal_1 = {"stopCamera", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "startCamera()", &signal_0, QMetaData::Public },
	{ "stopCamera()", &signal_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"libqlmbcam::QlmbCamWidget", parentObject,
	slot_tbl, 12,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_libqlmbcam__QlmbCamWidget.setMetaObject( metaObj );
    return metaObj;
}

void* libqlmbcam::QlmbCamWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "libqlmbcam::QlmbCamWidget" ) )
	return this;
    return QScrollView::qt_cast( clname );
}

// SIGNAL startCamera
void libqlmbcam::QlmbCamWidget::startCamera()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

// SIGNAL stopCamera
void libqlmbcam::QlmbCamWidget::stopCamera()
{
    activate_signal( staticMetaObject()->signalOffset() + 1 );
}

bool libqlmbcam::QlmbCamWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setCamera((liblmbcam::LMBCam*)static_QUType_ptr.get(_o+1)); break;
    case 1: readSettings(); break;
    case 2: writeSettings(); break;
    case 3: resetCamera(); break;
    case 4: lockControls(); break;
    case 5: unlockControls(); break;
    case 6: updateFramerateCombo((const QString&)static_QUType_QString.get(_o+1)); break;
    case 7: updateLineEdits((const QString&)static_QUType_QString.get(_o+1)); break;
    case 8: changeAbsParamValue(); break;
    case 9: changeParamValue((int)static_QUType_int.get(_o+1)); break;
    case 10: changeAutoMode((bool)static_QUType_bool.get(_o+1)); break;
    case 11: changeAbsControl((bool)static_QUType_bool.get(_o+1)); break;
    default:
	return QScrollView::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool libqlmbcam::QlmbCamWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: startCamera(); break;
    case 1: stopCamera(); break;
    default:
	return QScrollView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool libqlmbcam::QlmbCamWidget::qt_property( int id, int f, QVariant* v)
{
    return QScrollView::qt_property( id, f, v);
}

bool libqlmbcam::QlmbCamWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
