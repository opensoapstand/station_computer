/****************************************************************************
** Meta object code from reading C++ file 'page_sendFeedback.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "page_sendFeedback.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_sendFeedback.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_page_sendFeedback_t {
    QByteArrayData data[12];
    char stringdata0[271];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_page_sendFeedback_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_page_sendFeedback_t qt_meta_stringdata_page_sendFeedback = {
    {
QT_MOC_LITERAL(0, 0, 17), // "page_sendFeedback"
QT_MOC_LITERAL(1, 18, 12), // "paymentTotal"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6), // "string"
QT_MOC_LITERAL(4, 39, 29), // "on_pushButton_to_idle_clicked"
QT_MOC_LITERAL(5, 69, 19), // "onSelectTimeoutTick"
QT_MOC_LITERAL(6, 89, 26), // "on_pushButton_send_clicked"
QT_MOC_LITERAL(7, 116, 35), // "on_pushButton_previous_page_c..."
QT_MOC_LITERAL(8, 152, 33), // "on_pushButton_start_input_cli..."
QT_MOC_LITERAL(9, 186, 30), // "on_feedback_Text_Input_clicked"
QT_MOC_LITERAL(10, 217, 21), // "keyboardButtonPressed"
QT_MOC_LITERAL(11, 239, 31) // "on_pushButton_help_page_clicked"

    },
    "page_sendFeedback\0paymentTotal\0\0string\0"
    "on_pushButton_to_idle_clicked\0"
    "onSelectTimeoutTick\0on_pushButton_send_clicked\0"
    "on_pushButton_previous_page_clicked\0"
    "on_pushButton_start_input_clicked\0"
    "on_feedback_Text_Input_clicked\0"
    "keyboardButtonPressed\0"
    "on_pushButton_help_page_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_page_sendFeedback[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    1,   72,    2, 0x08 /* Private */,
      11,    0,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3,    2,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void page_sendFeedback::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        page_sendFeedback *_t = static_cast<page_sendFeedback *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->paymentTotal((*reinterpret_cast< string(*)>(_a[1])),(*reinterpret_cast< string(*)>(_a[2])),(*reinterpret_cast< string(*)>(_a[3]))); break;
        case 1: _t->on_pushButton_to_idle_clicked(); break;
        case 2: _t->onSelectTimeoutTick(); break;
        case 3: _t->on_pushButton_send_clicked(); break;
        case 4: _t->on_pushButton_previous_page_clicked(); break;
        case 5: _t->on_pushButton_start_input_clicked(); break;
        case 6: _t->on_feedback_Text_Input_clicked(); break;
        case 7: _t->keyboardButtonPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_pushButton_help_page_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (page_sendFeedback::*_t)(string , string , string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&page_sendFeedback::paymentTotal)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject page_sendFeedback::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_page_sendFeedback.data,
      qt_meta_data_page_sendFeedback,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *page_sendFeedback::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *page_sendFeedback::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_page_sendFeedback.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int page_sendFeedback::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void page_sendFeedback::paymentTotal(string _t1, string _t2, string _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
