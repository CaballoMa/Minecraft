/****************************************************************************
** Meta object code from reading C++ file 'playerinfo.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../assignment_package/src/playerinfo.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playerinfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlayerInfo_t {
    const uint offsetsAndSize[16];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PlayerInfo_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PlayerInfo_t qt_meta_stringdata_PlayerInfo = {
    {
QT_MOC_LITERAL(0, 10), // "PlayerInfo"
QT_MOC_LITERAL(11, 15), // "slot_setPosText"
QT_MOC_LITERAL(27, 0), // ""
QT_MOC_LITERAL(28, 15), // "slot_setVelText"
QT_MOC_LITERAL(44, 15), // "slot_setAccText"
QT_MOC_LITERAL(60, 16), // "slot_setLookText"
QT_MOC_LITERAL(77, 17), // "slot_setChunkText"
QT_MOC_LITERAL(95, 16) // "slot_setZoneText"

    },
    "PlayerInfo\0slot_setPosText\0\0slot_setVelText\0"
    "slot_setAccText\0slot_setLookText\0"
    "slot_setChunkText\0slot_setZoneText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayerInfo[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x0a,    1 /* Public */,
       3,    1,   53,    2, 0x0a,    3 /* Public */,
       4,    1,   56,    2, 0x0a,    5 /* Public */,
       5,    1,   59,    2, 0x0a,    7 /* Public */,
       6,    1,   62,    2, 0x0a,    9 /* Public */,
       7,    1,   65,    2, 0x0a,   11 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void PlayerInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayerInfo *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->slot_setPosText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->slot_setVelText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->slot_setAccText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->slot_setLookText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->slot_setChunkText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->slot_setZoneText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject PlayerInfo::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PlayerInfo.offsetsAndSize,
    qt_meta_data_PlayerInfo,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PlayerInfo_t
, QtPrivate::TypeAndForceComplete<PlayerInfo, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>


>,
    nullptr
} };


const QMetaObject *PlayerInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayerInfo.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlayerInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
