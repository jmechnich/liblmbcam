/****************************************************************************
** QlmbCamModeControlWidget meta object code from reading C++ file 'QlmbCamModeControl.hh'
**
** Created: Mon Aug 28 21:22:58 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../src/libqlmbcamwid/QlmbCamModeControl.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QlmbCamModeControlWidget::className() const
{
    return "QlmbCamModeControlWidget";
}

QMetaObject *QlmbCamModeControlWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QlmbCamModeControlWidget( "QlmbCamModeControlWidget", &QlmbCamModeControlWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QlmbCamModeControlWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamModeControlWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QlmbCamModeControlWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamModeControlWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QlmbCamModeControlWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "mode", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"changeVideoMode", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "fps", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"changeFramerate", 1, param_slot_1 };
    static const QUMethod slot_2 = {"updateFormat7Geometry", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "changeVideoMode(const QString&)", &slot_0, QMetaData::Private },
	{ "changeFramerate(const QString&)", &slot_1, QMetaData::Private },
	{ "updateFormat7Geometry()", &slot_2, QMetaData::Private }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"videoModeChanged", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"framerateChanged", 1, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_ptr, "unsigned int", QUParameter::In }
    };
    static const QUMethod signal_2 = {"widthChanged", 1, param_signal_2 };
    static const QUParameter param_signal_3[] = {
	{ 0, &static_QUType_ptr, "unsigned int", QUParameter::In }
    };
    static const QUMethod signal_3 = {"heightChanged", 1, param_signal_3 };
    static const QMetaData signal_tbl[] = {
	{ "videoModeChanged(const QString&)", &signal_0, QMetaData::Public },
	{ "framerateChanged(double)", &signal_1, QMetaData::Public },
	{ "widthChanged(unsigned int)", &signal_2, QMetaData::Public },
	{ "heightChanged(unsigned int)", &signal_3, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"QlmbCamModeControlWidget", parentObject,
	slot_tbl, 3,
	signal_tbl, 4,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QlmbCamModeControlWidget.setMetaObject( metaObj );
    return metaObj;
}

void* QlmbCamModeControlWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QlmbCamModeControlWidget" ) )
	return this;
    return QWidget::qt_cast( clname );
}

// SIGNAL videoModeChanged
void QlmbCamModeControlWidget::videoModeChanged( const QString& t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

// SIGNAL framerateChanged
void QlmbCamModeControlWidget::framerateChanged( double t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 1, t0 );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL widthChanged
void QlmbCamModeControlWidget::widthChanged( unsigned int t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,&t0);
    activate_signal( clist, o );
}

// SIGNAL heightChanged
void QlmbCamModeControlWidget::heightChanged( unsigned int t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 3 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,&t0);
    activate_signal( clist, o );
}

bool QlmbCamModeControlWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: changeVideoMode((const QString&)static_QUType_QString.get(_o+1)); break;
    case 1: changeFramerate((const QString&)static_QUType_QString.get(_o+1)); break;
    case 2: updateFormat7Geometry(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QlmbCamModeControlWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: videoModeChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 1: framerateChanged((double)static_QUType_double.get(_o+1)); break;
    case 2: widthChanged((unsigned int)(*((unsigned int*)static_QUType_ptr.get(_o+1)))); break;
    case 3: heightChanged((unsigned int)(*((unsigned int*)static_QUType_ptr.get(_o+1)))); break;
    default:
	return QWidget::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool QlmbCamModeControlWidget::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool QlmbCamModeControlWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
