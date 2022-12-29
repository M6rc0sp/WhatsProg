/****************************************************************************
** Meta object code from reading C++ file 'whatsprog_thread.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../WhatsProg_interface/whatsprog_thread.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'whatsprog_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_WhatsProgThread_t {
    uint offsetsAndSizes[20];
    char stringdata0[16];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[4];
    char stringdata5[17];
    char stringdata6[10];
    char stringdata7[12];
    char stringdata8[16];
    char stringdata9[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_WhatsProgThread_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_WhatsProgThread_t qt_meta_stringdata_WhatsProgThread = {
    {
        QT_MOC_LITERAL(0, 15),  // "WhatsProgThread"
        QT_MOC_LITERAL(16, 12),  // "novaMensagem"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 12),  // "iterConversa"
        QT_MOC_LITERAL(43, 3),  // "itr"
        QT_MOC_LITERAL(47, 16),  // "confirmarLeitura"
        QT_MOC_LITERAL(64, 9),  // "fimThread"
        QT_MOC_LITERAL(74, 11),  // "desconectar"
        QT_MOC_LITERAL(86, 15),  // "confirmarThread"
        QT_MOC_LITERAL(102, 15)   // "finalizarThread"
    },
    "WhatsProgThread",
    "novaMensagem",
    "",
    "iterConversa",
    "itr",
    "confirmarLeitura",
    "fimThread",
    "desconectar",
    "confirmarThread",
    "finalizarThread"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_WhatsProgThread[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       5,    1,   53,    2, 0x06,    3 /* Public */,
       6,    0,   56,    2, 0x06,    5 /* Public */,
       7,    0,   57,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   58,    2, 0x08,    7 /* Private */,
       9,    0,   59,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject WhatsProgThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_WhatsProgThread.offsetsAndSizes,
    qt_meta_data_WhatsProgThread,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_WhatsProgThread_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WhatsProgThread, std::true_type>,
        // method 'novaMensagem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<iterConversa, std::false_type>,
        // method 'confirmarLeitura'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<iterConversa, std::false_type>,
        // method 'fimThread'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'desconectar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'confirmarThread'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'finalizarThread'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void WhatsProgThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WhatsProgThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->novaMensagem((*reinterpret_cast< std::add_pointer_t<iterConversa>>(_a[1]))); break;
        case 1: _t->confirmarLeitura((*reinterpret_cast< std::add_pointer_t<iterConversa>>(_a[1]))); break;
        case 2: _t->fimThread(); break;
        case 3: _t->desconectar(); break;
        case 4: _t->confirmarThread(); break;
        case 5: _t->finalizarThread(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WhatsProgThread::*)(iterConversa );
            if (_t _q_method = &WhatsProgThread::novaMensagem; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WhatsProgThread::*)(iterConversa );
            if (_t _q_method = &WhatsProgThread::confirmarLeitura; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WhatsProgThread::*)();
            if (_t _q_method = &WhatsProgThread::fimThread; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WhatsProgThread::*)();
            if (_t _q_method = &WhatsProgThread::desconectar; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *WhatsProgThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WhatsProgThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WhatsProgThread.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WhatsProgThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void WhatsProgThread::novaMensagem(iterConversa _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WhatsProgThread::confirmarLeitura(iterConversa _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WhatsProgThread::fimThread()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void WhatsProgThread::desconectar()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
