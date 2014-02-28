/****************************************************************************
** QlmbCamParamControlWidget meta object code from reading C++ file 'QlmbCamParamControl.hh'
**
** Created: Mon Aug 28 21:22:57 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../../../src/libqlmbcamwid/QlmbCamParamControl.hh"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QlmbCamParamControlWidget::className() const
{
    return "QlmbCamParamControlWidget";
}

QMetaObject *QlmbCamParamControlWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QlmbCamParamControlWidget( "QlmbCamParamControlWidget", &QlmbCamParamControlWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QlmbCamParamControlWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamParamControlWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QlmbCamParamControlWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QlmbCamParamControlWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QlmbCamParamControlWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGrid::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "value", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"changeParamValue", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "autoMode", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"changeAutoMode", 1, param_slot_1 };
    static const QMetaData slot_tbl[] = {
	{ "changeParamValue(int)", &slot_0, QMetaData::Private },
	{ "changeAutoMode(bool)", &slot_1, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"QlmbCamParamControlWidget", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QlmbCamParamControlWidget.setMetaObject( metaObj );
    return metaObj;
}

void* QlmbCamParamControlWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QlmbCamParamControlWidget" ) )
	return this;
    return QGrid::qt_cast( clname );
}

bool QlmbCamParamControlWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: changeParamValue((int)static_QUType_int.get(_o+1)); break;
    case 1: changeAutoMode((bool)static_QUType_bool.get(_o+1)); break;
    default:
	return QGrid::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QlmbCamParamControlWidget::qt_emit( int _id, QUObject* _o )
{
    return QGrid::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QlmbCamParamControlWidget::qt_property( int id, int f, QVariant* v)
{
    return QGrid::qt_property( id, f, v);
}

bool QlmbCamParamControlWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
