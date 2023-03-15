/****************************************************************************
** Meta object code from reading C++ file 'mygl.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../assignment_package/src/mygl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mygl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MyGL_t {
    const uint offsetsAndSize[18];
    char stringdata0[130];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MyGL_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MyGL_t qt_meta_stringdata_MyGL = {
    {
QT_MOC_LITERAL(0, 4), // "MyGL"
QT_MOC_LITERAL(5, 17), // "sig_sendPlayerPos"
QT_MOC_LITERAL(23, 0), // ""
QT_MOC_LITERAL(24, 17), // "sig_sendPlayerVel"
QT_MOC_LITERAL(42, 17), // "sig_sendPlayerAcc"
QT_MOC_LITERAL(60, 18), // "sig_sendPlayerLook"
QT_MOC_LITERAL(79, 19), // "sig_sendPlayerChunk"
QT_MOC_LITERAL(99, 25), // "sig_sendPlayerTerrainZone"
QT_MOC_LITERAL(125, 4) // "tick"

    },
    "MyGL\0sig_sendPlayerPos\0\0sig_sendPlayerVel\0"
    "sig_sendPlayerAcc\0sig_sendPlayerLook\0"
    "sig_sendPlayerChunk\0sig_sendPlayerTerrainZone\0"
    "tick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyGL[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x106,    1 /* Public | MethodIsConst  */,
       3,    1,   59,    2, 0x106,    3 /* Public | MethodIsConst  */,
       4,    1,   62,    2, 0x106,    5 /* Public | MethodIsConst  */,
       5,    1,   65,    2, 0x106,    7 /* Public | MethodIsConst  */,
       6,    1,   68,    2, 0x106,    9 /* Public | MethodIsConst  */,
       7,    1,   71,    2, 0x106,   11 /* Public | MethodIsConst  */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   74,    2, 0x08,   13 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void MyGL::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyGL *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sig_sendPlayerPos((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->sig_sendPlayerVel((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->sig_sendPlayerAcc((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->sig_sendPlayerLook((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->sig_sendPlayerChunk((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->sig_sendPlayerTerrainZone((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->tick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyGL::*)(QString ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyGL::sig_sendPlayerPos)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyGL::sig_sendPlayerVel)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyGL::sig_sendPlayerAcc)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyGL::sig_sendPlayerLook)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyGL::sig_sendPlayerChunk)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyGL::sig_sendPlayerTerrainZone)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject MyGL::staticMetaObject = { {
    QMetaObject::SuperData::link<OpenGLContext::staticMetaObject>(),
    qt_meta_stringdata_MyGL.offsetsAndSize,
    qt_meta_data_MyGL,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MyGL_t
, QtPrivate::TypeAndForceComplete<MyGL, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *MyGL::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyGL::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyGL.stringdata0))
        return static_cast<void*>(this);
    return OpenGLContext::qt_metacast(_clname);
}

int MyGL::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OpenGLContext::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void MyGL::sig_sendPlayerPos(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyGL::sig_sendPlayerVel(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MyGL::sig_sendPlayerAcc(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MyGL::sig_sendPlayerLook(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MyGL::sig_sendPlayerChunk(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MyGL::sig_sendPlayerTerrainZone(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
