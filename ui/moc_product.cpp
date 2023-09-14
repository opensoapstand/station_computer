/****************************************************************************
** Meta object code from reading C++ file 'product.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "product.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'product.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_product_t {
    QByteArrayData data[8];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_product_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_product_t qt_meta_stringdata_product = {
    {
QT_MOC_LITERAL(0, 0, 7), // "product"
QT_MOC_LITERAL(1, 8, 15), // "orderSlotChange"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 6), // "newOpt"
QT_MOC_LITERAL(4, 32, 11), // "priceChange"
QT_MOC_LITERAL(5, 44, 8), // "newPrice"
QT_MOC_LITERAL(6, 53, 10), // "sizeChange"
QT_MOC_LITERAL(7, 64, 7) // "newSize"

    },
    "product\0orderSlotChange\0\0newOpt\0"
    "priceChange\0newPrice\0sizeChange\0newSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_product[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,
       6,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    7,

       0        // eod
};

void product::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        product *_t = static_cast<product *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->orderSlotChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->priceChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->sizeChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (product::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&product::orderSlotChange)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (product::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&product::priceChange)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (product::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&product::sizeChange)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject product::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_product.data,
      qt_meta_data_product,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *product::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *product::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_product.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int product::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void product::orderSlotChange(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void product::priceChange(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void product::sizeChange(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
