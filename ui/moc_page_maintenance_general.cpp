/****************************************************************************
** Meta object code from reading C++ file 'page_maintenance_general.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "page_maintenance_general.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_maintenance_general.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_page_maintenance_general_t {
    QByteArrayData data[18];
    char stringdata0[518];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_page_maintenance_general_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_page_maintenance_general_t qt_meta_stringdata_page_maintenance_general = {
    {
QT_MOC_LITERAL(0, 0, 24), // "page_maintenance_general"
QT_MOC_LITERAL(1, 25, 26), // "on_pushButton_back_clicked"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 30), // "on_pushButton_minimize_clicked"
QT_MOC_LITERAL(4, 84, 28), // "on_pushButton_reboot_clicked"
QT_MOC_LITERAL(5, 113, 30), // "on_pushButton_shutdown_clicked"
QT_MOC_LITERAL(6, 144, 39), // "on_checkBox_enable_pump_rampi..."
QT_MOC_LITERAL(7, 184, 7), // "checked"
QT_MOC_LITERAL(8, 192, 42), // "on_checkBox_enable_empty_cont..."
QT_MOC_LITERAL(9, 235, 28), // "on_test_lineEdit_textChanged"
QT_MOC_LITERAL(10, 264, 4), // "arg1"
QT_MOC_LITERAL(11, 269, 40), // "button_connect_to_specifiic_w..."
QT_MOC_LITERAL(12, 310, 21), // "keyboardButtonPressed"
QT_MOC_LITERAL(13, 332, 35), // "on_pushButton_wifi_networks_c..."
QT_MOC_LITERAL(14, 368, 37), // "on_pushButton_rtunnel_restart..."
QT_MOC_LITERAL(15, 406, 36), // "on_pushButton_network_status_..."
QT_MOC_LITERAL(16, 443, 32), // "on_pushButton_restart_UI_clicked"
QT_MOC_LITERAL(17, 476, 41) // "on_pushButton_restart_electro..."

    },
    "page_maintenance_general\0"
    "on_pushButton_back_clicked\0\0"
    "on_pushButton_minimize_clicked\0"
    "on_pushButton_reboot_clicked\0"
    "on_pushButton_shutdown_clicked\0"
    "on_checkBox_enable_pump_ramping_clicked\0"
    "checked\0on_checkBox_enable_empty_container_clicked\0"
    "on_test_lineEdit_textChanged\0arg1\0"
    "button_connect_to_specifiic_wifi_network\0"
    "keyboardButtonPressed\0"
    "on_pushButton_wifi_networks_clicked\0"
    "on_pushButton_rtunnel_restart_clicked\0"
    "on_pushButton_network_status_clicked\0"
    "on_pushButton_restart_UI_clicked\0"
    "on_pushButton_restart_electronics_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_page_maintenance_general[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08 /* Private */,
       3,    0,   85,    2, 0x08 /* Private */,
       4,    0,   86,    2, 0x08 /* Private */,
       5,    0,   87,    2, 0x08 /* Private */,
       6,    1,   88,    2, 0x08 /* Private */,
       8,    1,   91,    2, 0x08 /* Private */,
       9,    1,   94,    2, 0x08 /* Private */,
      11,    0,   97,    2, 0x08 /* Private */,
      12,    1,   98,    2, 0x08 /* Private */,
      13,    0,  101,    2, 0x08 /* Private */,
      14,    0,  102,    2, 0x08 /* Private */,
      15,    0,  103,    2, 0x08 /* Private */,
      16,    0,  104,    2, 0x08 /* Private */,
      17,    0,  105,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void page_maintenance_general::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        page_maintenance_general *_t = static_cast<page_maintenance_general *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_back_clicked(); break;
        case 1: _t->on_pushButton_minimize_clicked(); break;
        case 2: _t->on_pushButton_reboot_clicked(); break;
        case 3: _t->on_pushButton_shutdown_clicked(); break;
        case 4: _t->on_checkBox_enable_pump_ramping_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->on_checkBox_enable_empty_container_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_test_lineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->button_connect_to_specifiic_wifi_network(); break;
        case 8: _t->keyboardButtonPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_pushButton_wifi_networks_clicked(); break;
        case 10: _t->on_pushButton_rtunnel_restart_clicked(); break;
        case 11: _t->on_pushButton_network_status_clicked(); break;
        case 12: _t->on_pushButton_restart_UI_clicked(); break;
        case 13: _t->on_pushButton_restart_electronics_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject page_maintenance_general::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_page_maintenance_general.data,
      qt_meta_data_page_maintenance_general,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *page_maintenance_general::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *page_maintenance_general::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_page_maintenance_general.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int page_maintenance_general::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
