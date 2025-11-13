/****************************************************************************
** Meta object code from reading C++ file 'win_print.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../win_print.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_print.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9QPrintDlgE_t {};
} // unnamed namespace

template <> constexpr inline auto QPrintDlg::qt_create_metaobjectdata<qt_meta_tag_ZN9QPrintDlgE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QPrintDlg",
        "onDefaultPrinter",
        "",
        "onEpsImagePrinter",
        "onXfigImagePrinter",
        "onJpegImagePrinter",
        "onCancel",
        "readDialog",
        "readFloatField",
        "QLineEdit*",
        "edt",
        "double*",
        "presult",
        "defvalue",
        "minvalue",
        "maxvalue",
        "markBad",
        "getChosenResolution",
        "getChosenLineWidth",
        "getChosenSymbolSize"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onDefaultPrinter'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onEpsImagePrinter'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onXfigImagePrinter'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onJpegImagePrinter'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onCancel'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'readDialog'
        QtMocHelpers::SlotData<bool()>(7, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'readFloatField'
        QtMocHelpers::SlotData<bool(QLineEdit *, double *, double, double, double)>(8, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 9, 10 }, { 0x80000000 | 11, 12 }, { QMetaType::Double, 13 }, { QMetaType::Double, 14 },
            { QMetaType::Double, 15 },
        }}),
        // Slot 'markBad'
        QtMocHelpers::SlotData<void(QLineEdit *)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Slot 'getChosenResolution'
        QtMocHelpers::SlotData<int()>(17, 2, QMC::AccessPublic, QMetaType::Int),
        // Slot 'getChosenLineWidth'
        QtMocHelpers::SlotData<double()>(18, 2, QMC::AccessPublic, QMetaType::Double),
        // Slot 'getChosenSymbolSize'
        QtMocHelpers::SlotData<double()>(19, 2, QMC::AccessPublic, QMetaType::Double),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QPrintDlg, qt_meta_tag_ZN9QPrintDlgE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QPrintDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QPrintDlgE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QPrintDlgE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9QPrintDlgE_t>.metaTypes,
    nullptr
} };

void QPrintDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QPrintDlg *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onDefaultPrinter(); break;
        case 1: _t->onEpsImagePrinter(); break;
        case 2: _t->onXfigImagePrinter(); break;
        case 3: _t->onJpegImagePrinter(); break;
        case 4: _t->onCancel(); break;
        case 5: { bool _r = _t->readDialog();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->readFloatField((*reinterpret_cast< std::add_pointer_t<QLineEdit*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->markBad((*reinterpret_cast< std::add_pointer_t<QLineEdit*>>(_a[1]))); break;
        case 8: { int _r = _t->getChosenResolution();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 9: { double _r = _t->getChosenLineWidth();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 10: { double _r = _t->getChosenSymbolSize();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLineEdit* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QLineEdit* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *QPrintDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QPrintDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9QPrintDlgE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int QPrintDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
