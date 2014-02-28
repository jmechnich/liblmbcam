/****************************************************************************
** QlmbCamMainWidget meta object code from reading C++ file 'QlmbCamMainWidget.hh'
**
** Created: Mon Aug 28 21:22:55 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../src/libqlmbcamwid/QlmbCamMainWidget.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QlmbCamMainWidget::className() const
{
    return "QlmbCamMainWidget";
}

QMetaObject *QlmbCamMainWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QlmbCamMainWidget( "QlmbCamMainWidget", &QlmbCamMainWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QlmbCamMainWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamMainWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QlmbCamMainWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamMainWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QlmbCamMainWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "mode", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"changeVideoMode", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "fps", &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"changeFramerate", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "width", &static_QUType_ptr, "unsigned int", QUParameter::In }
    };
    static const QUMethod slot_2 = {"changeWidth", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "height", &static_QUType_ptr, "unsigned int", QUParameter::In }
    };
    static const QUMethod slot_3 = {"changeHeight", 1, param_slot_3 };
    static const QMetaData slot_tbl[] = {
	{ "changeVideoMode(const QString&)", &slot_0, QMetaData::Public },
	{ "changeFramerate(double)", &slot_1, QMetaData::Public },
	{ "changeWidth(unsigned int)", &slot_2, QMetaData::Public },
	{ "changeHeight(unsigned int)", &slot_3, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"QlmbCamMainWidget", parentObject,
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QlmbCamMainWidget.setMetaObject( metaObj );
    return metaObj;
}

void* QlmbCamMainWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QlmbCamMainWidget" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool QlmbCamMainWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: changeVideoMode((const QString&)static_QUType_QString.get(_o+1)); break;
    case 1: changeFramerate((double)static_QUType_double.get(_o+1)); break;
    case 2: changeWidth((unsigned int)(*((unsigned int*)static_QUType_ptr.get(_o+1)))); break;
    case 3: changeHeight((unsigned int)(*((unsigned int*)static_QUType_ptr.get(_o+1)))); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QlmbCamMainWidget::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QlmbCamMainWidget::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool QlmbCamMainWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
