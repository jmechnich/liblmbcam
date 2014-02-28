/****************************************************************************
** libqlmbcam::QlmbCamVideoWidget meta object code from reading C++ file 'QlmbCamVideoWidget.hh'
**
** Created: Mon Aug 28 21:23:29 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../src/libqlmbcam/QlmbCamVideoWidget.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *libqlmbcam::QlmbCamVideoWidget::className() const
{
    return "libqlmbcam::QlmbCamVideoWidget";
}

QMetaObject *libqlmbcam::QlmbCamVideoWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_libqlmbcam__QlmbCamVideoWidget( "libqlmbcam::QlmbCamVideoWidget", &libqlmbcam::QlmbCamVideoWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString libqlmbcam::QlmbCamVideoWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "libqlmbcam::QlmbCamVideoWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString libqlmbcam::QlmbCamVideoWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "libqlmbcam::QlmbCamVideoWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* libqlmbcam::QlmbCamVideoWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QScrollView::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "id", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"setBayerMode", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "lambda", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setLambda", 1, param_slot_1 };
    static const QUMethod slot_2 = {"resetLambda", 0, 0 };
    static const QUMethod slot_3 = {"resetEqualizerWidget", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "setBayerMode(int)", &slot_0, QMetaData::Protected },
	{ "setLambda(int)", &slot_1, QMetaData::Protected },
	{ "resetLambda()", &slot_2, QMetaData::Protected },
	{ "resetEqualizerWidget()", &slot_3, QMetaData::Protected }
    };
    static const QUMethod signal_0 = {"aboutToClose", 0, 0 };
    static const QUParameter param_signal_1[] = {
	{ "cameraGUID", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"focused", 1, param_signal_1 };
    static const QMetaData signal_tbl[] = {
	{ "aboutToClose()", &signal_0, QMetaData::Public },
	{ "focused(const QString&)", &signal_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"libqlmbcam::QlmbCamVideoWidget", parentObject,
	slot_tbl, 4,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_libqlmbcam__QlmbCamVideoWidget.setMetaObject( metaObj );
    return metaObj;
}

void* libqlmbcam::QlmbCamVideoWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "libqlmbcam::QlmbCamVideoWidget" ) )
	return this;
    return QScrollView::qt_cast( clname );
}

// SIGNAL aboutToClose
void libqlmbcam::QlmbCamVideoWidget::aboutToClose()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

// SIGNAL focused
void libqlmbcam::QlmbCamVideoWidget::focused( const QString& t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 1, t0 );
}

bool libqlmbcam::QlmbCamVideoWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setBayerMode((int)static_QUType_int.get(_o+1)); break;
    case 1: setLambda((int)static_QUType_int.get(_o+1)); break;
    case 2: resetLambda(); break;
    case 3: resetEqualizerWidget(); break;
    default:
	return QScrollView::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool libqlmbcam::QlmbCamVideoWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: aboutToClose(); break;
    case 1: focused((const QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QScrollView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool libqlmbcam::QlmbCamVideoWidget::qt_property( int id, int f, QVariant* v)
{
    return QScrollView::qt_property( id, f, v);
}

bool libqlmbcam::QlmbCamVideoWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
