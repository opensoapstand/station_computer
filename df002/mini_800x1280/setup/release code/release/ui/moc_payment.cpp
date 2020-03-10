/****************************************************************************
** Meta object code from reading C++ file 'payment.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Desktop/mini_800x1280/drinkfill_ui/ev2/payment.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'payment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_payment_t {
    QByteArrayData data[11];
    char stringdata0[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_payment_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_payment_t qt_meta_stringdata_payment = {
    {
QT_MOC_LITERAL(0, 0, 7), // "payment"
QT_MOC_LITERAL(1, 8, 18), // "declineTimer_start"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 23), // "on_goBackButton_clicked"
QT_MOC_LITERAL(4, 52, 19), // "progressStatusLabel"
QT_MOC_LITERAL(5, 72, 16), // "updatePageNumber"
QT_MOC_LITERAL(6, 89, 20), // "on_payButton_clicked"
QT_MOC_LITERAL(7, 110, 14), // "readTimer_loop"
QT_MOC_LITERAL(8, 125, 6), // "goBack"
QT_MOC_LITERAL(9, 132, 29), // "on_priceVolume1Button_clicked"
QT_MOC_LITERAL(10, 162, 29) // "on_priceVolume2Button_clicked"

    },
    "payment\0declineTimer_start\0\0"
    "on_goBackButton_clicked\0progressStatusLabel\0"
    "updatePageNumber\0on_payButton_clicked\0"
    "readTimer_loop\0goBack\0"
    "on_priceVolume1Button_clicked\0"
    "on_priceVolume2Button_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_payment[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    0,   63,    2, 0x08 /* Private */,
       7,    0,   64,    2, 0x08 /* Private */,
       8,    0,   65,    2, 0x08 /* Private */,
       9,    0,   66,    2, 0x08 /* Private */,
      10,    0,   67,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void payment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<payment *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->declineTimer_start(); break;
        case 1: _t->on_goBackButton_clicked(); break;
        case 2: _t->progressStatusLabel(); break;
        case 3: _t->updatePageNumber(); break;
        case 4: _t->on_payButton_clicked(); break;
        case 5: _t->readTimer_loop(); break;
        case 6: _t->goBack(); break;
        case 7: _t->on_priceVolume1Button_clicked(); break;
        case 8: _t->on_priceVolume2Button_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject payment::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_payment.data,
    qt_meta_data_payment,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *payment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *payment::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_payment.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int payment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
