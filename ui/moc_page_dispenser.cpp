/****************************************************************************
** Meta object code from reading C++ file 'page_dispenser.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "page_dispenser.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_dispenser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_page_dispenser_t {
    QByteArrayData data[10];
    char stringdata0[219];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_page_dispenser_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_page_dispenser_t qt_meta_stringdata_page_dispenser = {
    {
QT_MOC_LITERAL(0, 0, 14), // "page_dispenser"
QT_MOC_LITERAL(1, 15, 22), // "fsmSendStartDispensing"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 21), // "fsmSendStopDispensing"
QT_MOC_LITERAL(4, 61, 18), // "onDispenseIdleTick"
QT_MOC_LITERAL(5, 80, 20), // "dispensing_end_admin"
QT_MOC_LITERAL(6, 101, 27), // "on_pushButton_abort_clicked"
QT_MOC_LITERAL(7, 129, 23), // "on_cancelButton_clicked"
QT_MOC_LITERAL(8, 153, 34), // "on_pushButton_debug_Button_cl..."
QT_MOC_LITERAL(9, 188, 30) // "on_pushButton_problems_clicked"

    },
    "page_dispenser\0fsmSendStartDispensing\0"
    "\0fsmSendStopDispensing\0onDispenseIdleTick\0"
    "dispensing_end_admin\0on_pushButton_abort_clicked\0"
    "on_cancelButton_clicked\0"
    "on_pushButton_debug_Button_clicked\0"
    "on_pushButton_problems_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_page_dispenser[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    0,   58,    2, 0x08 /* Private */,
       7,    0,   59,    2, 0x08 /* Private */,
       8,    0,   60,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
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

void page_dispenser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        page_dispenser *_t = static_cast<page_dispenser *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fsmSendStartDispensing(); break;
        case 1: _t->fsmSendStopDispensing(); break;
        case 2: _t->onDispenseIdleTick(); break;
        case 3: _t->dispensing_end_admin(); break;
        case 4: _t->on_pushButton_abort_clicked(); break;
        case 5: _t->on_cancelButton_clicked(); break;
        case 6: _t->on_pushButton_debug_Button_clicked(); break;
        case 7: _t->on_pushButton_problems_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject page_dispenser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_page_dispenser.data,
      qt_meta_data_page_dispenser,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *page_dispenser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *page_dispenser::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_page_dispenser.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int page_dispenser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
