/****************************************************************************
** Meta object code from reading C++ file 'dfuicommthread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dfuicommthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dfuicommthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DfUiCommThread_t {
    QByteArrayData data[28];
    char stringdata0[470];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DfUiCommThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DfUiCommThread_t qt_meta_stringdata_DfUiCommThread = {
    {
QT_MOC_LITERAL(0, 0, 14), // "DfUiCommThread"
QT_MOC_LITERAL(1, 15, 5), // "error"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 23), // "QTcpSocket::SocketError"
QT_MOC_LITERAL(4, 46, 11), // "socketerror"
QT_MOC_LITERAL(5, 58, 20), // "transactionEndSignal"
QT_MOC_LITERAL(6, 79, 16), // "resetTimerSignal"
QT_MOC_LITERAL(7, 96, 15), // "targetHitSignal"
QT_MOC_LITERAL(8, 112, 17), // "noFlowAbortSignal"
QT_MOC_LITERAL(9, 130, 18), // "updateVolumeSignal"
QT_MOC_LITERAL(10, 149, 9), // "dispensed"
QT_MOC_LITERAL(11, 159, 32), // "updateFinalVolumeDispensedSignal"
QT_MOC_LITERAL(12, 192, 18), // "dispenseRateSignal"
QT_MOC_LITERAL(13, 211, 8), // "flowrate"
QT_MOC_LITERAL(14, 220, 20), // "dispenseStatusSignal"
QT_MOC_LITERAL(15, 241, 6), // "status"
QT_MOC_LITERAL(16, 248, 19), // "printerStatusSignal"
QT_MOC_LITERAL(17, 268, 8), // "isOnline"
QT_MOC_LITERAL(18, 277, 8), // "hasPaper"
QT_MOC_LITERAL(19, 286, 15), // "initReadySignal"
QT_MOC_LITERAL(20, 302, 17), // "temperatureSignal"
QT_MOC_LITERAL(21, 320, 13), // "temperature_1"
QT_MOC_LITERAL(22, 334, 13), // "temperature_2"
QT_MOC_LITERAL(23, 348, 34), // "dispenseButtonPressedPosEdgeS..."
QT_MOC_LITERAL(24, 383, 34), // "dispenseButtonPressedNegEdgeS..."
QT_MOC_LITERAL(25, 418, 28), // "messageHandlerFinishedSignal"
QT_MOC_LITERAL(26, 447, 9), // "readyRead"
QT_MOC_LITERAL(27, 457, 12) // "disconnected"

    },
    "DfUiCommThread\0error\0\0QTcpSocket::SocketError\0"
    "socketerror\0transactionEndSignal\0"
    "resetTimerSignal\0targetHitSignal\0"
    "noFlowAbortSignal\0updateVolumeSignal\0"
    "dispensed\0updateFinalVolumeDispensedSignal\0"
    "dispenseRateSignal\0flowrate\0"
    "dispenseStatusSignal\0status\0"
    "printerStatusSignal\0isOnline\0hasPaper\0"
    "initReadySignal\0temperatureSignal\0"
    "temperature_1\0temperature_2\0"
    "dispenseButtonPressedPosEdgeSignal\0"
    "dispenseButtonPressedNegEdgeSignal\0"
    "messageHandlerFinishedSignal\0readyRead\0"
    "disconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DfUiCommThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      15,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   99,    2, 0x06 /* Public */,
       5,    0,  102,    2, 0x06 /* Public */,
       6,    0,  103,    2, 0x06 /* Public */,
       7,    0,  104,    2, 0x06 /* Public */,
       8,    0,  105,    2, 0x06 /* Public */,
       9,    1,  106,    2, 0x06 /* Public */,
      11,    1,  109,    2, 0x06 /* Public */,
      12,    1,  112,    2, 0x06 /* Public */,
      14,    1,  115,    2, 0x06 /* Public */,
      16,    2,  118,    2, 0x06 /* Public */,
      19,    0,  123,    2, 0x06 /* Public */,
      20,    2,  124,    2, 0x06 /* Public */,
      23,    0,  129,    2, 0x06 /* Public */,
      24,    0,  130,    2, 0x06 /* Public */,
      25,    0,  131,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      26,    0,  132,    2, 0x0a /* Public */,
      27,    0,  133,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,   17,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   21,   22,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::QByteArray,
    QMetaType::Void,

       0        // eod
};

void DfUiCommThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DfUiCommThread *_t = static_cast<DfUiCommThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->error((*reinterpret_cast< QTcpSocket::SocketError(*)>(_a[1]))); break;
        case 1: _t->transactionEndSignal(); break;
        case 2: _t->resetTimerSignal(); break;
        case 3: _t->targetHitSignal(); break;
        case 4: _t->noFlowAbortSignal(); break;
        case 5: _t->updateVolumeSignal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->updateFinalVolumeDispensedSignal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->dispenseRateSignal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->dispenseStatusSignal((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->printerStatusSignal((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 10: _t->initReadySignal(); break;
        case 11: _t->temperatureSignal((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 12: _t->dispenseButtonPressedPosEdgeSignal(); break;
        case 13: _t->dispenseButtonPressedNegEdgeSignal(); break;
        case 14: _t->messageHandlerFinishedSignal(); break;
        case 15: { QByteArray _r = _t->readyRead();
            if (_a[0]) *reinterpret_cast< QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->disconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (DfUiCommThread::*_t)(QTcpSocket::SocketError );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::error)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::transactionEndSignal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::resetTimerSignal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::targetHitSignal)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::noFlowAbortSignal)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::updateVolumeSignal)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::updateFinalVolumeDispensedSignal)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::dispenseRateSignal)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::dispenseStatusSignal)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)(bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::printerStatusSignal)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::initReadySignal)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)(double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::temperatureSignal)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::dispenseButtonPressedPosEdgeSignal)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::dispenseButtonPressedNegEdgeSignal)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (DfUiCommThread::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DfUiCommThread::messageHandlerFinishedSignal)) {
                *result = 14;
                return;
            }
        }
    }
}

const QMetaObject DfUiCommThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_DfUiCommThread.data,
      qt_meta_data_DfUiCommThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DfUiCommThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DfUiCommThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DfUiCommThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int DfUiCommThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void DfUiCommThread::error(QTcpSocket::SocketError _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DfUiCommThread::transactionEndSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DfUiCommThread::resetTimerSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DfUiCommThread::targetHitSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DfUiCommThread::noFlowAbortSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void DfUiCommThread::updateVolumeSignal(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void DfUiCommThread::updateFinalVolumeDispensedSignal(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void DfUiCommThread::dispenseRateSignal(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void DfUiCommThread::dispenseStatusSignal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void DfUiCommThread::printerStatusSignal(bool _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void DfUiCommThread::initReadySignal()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void DfUiCommThread::temperatureSignal(double _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void DfUiCommThread::dispenseButtonPressedPosEdgeSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void DfUiCommThread::dispenseButtonPressedNegEdgeSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void DfUiCommThread::messageHandlerFinishedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
