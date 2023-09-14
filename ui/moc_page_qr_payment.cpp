/****************************************************************************
** Meta object code from reading C++ file 'page_qr_payment.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "page_qr_payment.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_qr_payment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_page_qr_payment_t {
    QByteArrayData data[9];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_page_qr_payment_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_page_qr_payment_t qt_meta_stringdata_page_qr_payment = {
    {
QT_MOC_LITERAL(0, 0, 15), // "page_qr_payment"
QT_MOC_LITERAL(1, 16, 35), // "on_pushButton_previous_page_c..."
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 19), // "proceed_to_dispense"
QT_MOC_LITERAL(4, 73, 13), // "onTimeoutTick"
QT_MOC_LITERAL(5, 87, 18), // "idlePaymentTimeout"
QT_MOC_LITERAL(6, 106, 29), // "on_pushButton_refresh_clicked"
QT_MOC_LITERAL(7, 136, 26), // "qrProcessedPeriodicalCheck"
QT_MOC_LITERAL(8, 163, 18) // "showErrorTimerPage"

    },
    "page_qr_payment\0on_pushButton_previous_page_clicked\0"
    "\0proceed_to_dispense\0onTimeoutTick\0"
    "idlePaymentTimeout\0on_pushButton_refresh_clicked\0"
    "qrProcessedPeriodicalCheck\0"
    "showErrorTimerPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_page_qr_payment[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void page_qr_payment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        page_qr_payment *_t = static_cast<page_qr_payment *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_previous_page_clicked(); break;
        case 1: _t->proceed_to_dispense(); break;
        case 2: _t->onTimeoutTick(); break;
        case 3: _t->idlePaymentTimeout(); break;
        case 4: _t->on_pushButton_refresh_clicked(); break;
        case 5: _t->qrProcessedPeriodicalCheck(); break;
        case 6: _t->showErrorTimerPage(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject page_qr_payment::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_page_qr_payment.data,
      qt_meta_data_page_qr_payment,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *page_qr_payment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *page_qr_payment::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_page_qr_payment.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int page_qr_payment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
