/****************************************************************************
** Meta object code from reading C++ file 'dfuiserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dfuiserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dfuiserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DfUiServer_t {
    QByteArrayData data[24];
    char stringdata0[342];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DfUiServer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DfUiServer_t qt_meta_stringdata_DfUiServer = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DfUiServer"
QT_MOC_LITERAL(1, 11, 13), // "printerStatus"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 8), // "isOnline"
QT_MOC_LITERAL(4, 35, 8), // "hasPaper"
QT_MOC_LITERAL(5, 44, 21), // "controllerFinishedAck"
QT_MOC_LITERAL(6, 66, 11), // "pleaseReset"
QT_MOC_LITERAL(7, 78, 18), // "signalUpdateVolume"
QT_MOC_LITERAL(8, 97, 9), // "dispensed"
QT_MOC_LITERAL(9, 107, 6), // "isFull"
QT_MOC_LITERAL(10, 114, 20), // "finalVolumeDispensed"
QT_MOC_LITERAL(11, 135, 18), // "signalDispenseRate"
QT_MOC_LITERAL(12, 154, 8), // "flowrate"
QT_MOC_LITERAL(13, 163, 20), // "signalDispenseStatus"
QT_MOC_LITERAL(14, 184, 6), // "status"
QT_MOC_LITERAL(15, 191, 9), // "targetHit"
QT_MOC_LITERAL(16, 201, 34), // "dispenseButtonPressedPosEdgeS..."
QT_MOC_LITERAL(17, 236, 34), // "dispenseButtonPressedNegEdgeS..."
QT_MOC_LITERAL(18, 271, 11), // "noFlowAbort"
QT_MOC_LITERAL(19, 283, 17), // "temperatureSignal"
QT_MOC_LITERAL(20, 301, 13), // "temperature_1"
QT_MOC_LITERAL(21, 315, 13), // "temperature_2"
QT_MOC_LITERAL(22, 329, 9), // "initReady"
QT_MOC_LITERAL(23, 339, 2) // "MM"

    },
    "DfUiServer\0printerStatus\0\0isOnline\0"
    "hasPaper\0controllerFinishedAck\0"
    "pleaseReset\0signalUpdateVolume\0dispensed\0"
    "isFull\0finalVolumeDispensed\0"
    "signalDispenseRate\0flowrate\0"
    "signalDispenseStatus\0status\0targetHit\0"
    "dispenseButtonPressedPosEdgeSignal\0"
    "dispenseButtonPressedNegEdgeSignal\0"
    "noFlowAbort\0temperatureSignal\0"
    "temperature_1\0temperature_2\0initReady\0"
    "MM"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DfUiServer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   84,    2, 0x06 /* Public */,
       5,    0,   89,    2, 0x06 /* Public */,
       6,    0,   90,    2, 0x06 /* Public */,
       7,    2,   91,    2, 0x06 /* Public */,
      10,    1,   96,    2, 0x06 /* Public */,
      11,    1,   99,    2, 0x06 /* Public */,
      13,    1,  102,    2, 0x06 /* Public */,
      15,    0,  105,    2, 0x06 /* Public */,
      16,    0,  106,    2, 0x06 /* Public */,
      17,    0,  107,    2, 0x06 /* Public */,
      18,    0,  108,    2, 0x06 /* Public */,
      19,    2,  109,    2, 0x06 /* Public */,
      22,    0,  114,    2, 0x06 /* Public */,
      23,    0,  115,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   20,   21,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DfUiServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DfUiServer *_t = static_cast<DfUiServer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->printerStatus((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->controllerFinishedAck(); break;
        case 2: _t->pleaseReset(); break;
        case 3: _t->signalUpdateVolume((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->finalVolumeDispensed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->signalDispenseRate((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->signalDispenseStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->targetHit(); break;
        case 8: _t->dispenseButtonPressedPosEdgeSignal(); break;
        case 9: _t->dispenseButtonPressedNegEdgeSignal(); break;
        case 10: _t->noFlowAbort(); break;
        case 11: _t->temperatureSignal((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 12: _t->initReady(); break;
        case 13: _t->MM(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DfUiServer::*_t)(bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::printerStatus)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::controllerFinishedAck)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::pleaseReset)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)(double , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::signalUpdateVolume)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::finalVolumeDispensed)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::signalDispenseRate)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::signalDispenseStatus)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::targetHit)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::dispenseButtonPressedPosEdgeSignal)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::dispenseButtonPressedNegEdgeSignal)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::noFlowAbort)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)(double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::temperatureSignal)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::initReady)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (DfUiServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiServer::MM)) {
                *result = 13;
                return;
            }
        }
    }
}

const QMetaObject DfUiServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_DfUiServer.data,
      qt_meta_data_DfUiServer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DfUiServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DfUiServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DfUiServer.stringdata0))
        return static_cast<void*>(this);
    return QTcpServer::qt_metacast(_clname);
}

int DfUiServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void DfUiServer::printerStatus(bool _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DfUiServer::controllerFinishedAck()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DfUiServer::pleaseReset()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DfUiServer::signalUpdateVolume(double _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DfUiServer::finalVolumeDispensed(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DfUiServer::signalDispenseRate(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DfUiServer::signalDispenseStatus(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DfUiServer::targetHit()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void DfUiServer::dispenseButtonPressedPosEdgeSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void DfUiServer::dispenseButtonPressedNegEdgeSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void DfUiServer::noFlowAbort()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void DfUiServer::temperatureSignal(double _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void DfUiServer::initReady()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void DfUiServer::MM()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
