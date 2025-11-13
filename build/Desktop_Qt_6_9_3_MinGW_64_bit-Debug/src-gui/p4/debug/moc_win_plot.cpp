/****************************************************************************
** Meta object code from reading C++ file 'win_plot.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src-gui/p4/win_plot.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_plot.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8QPlotWndE_t {};
} // unnamed namespace

template <> constexpr inline auto QPlotWnd::qt_create_metaobjectdata<qt_meta_tag_ZN8QPlotWndE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QPlotWnd",
        "signalEvaluating",
        "",
        "signalEvaluated",
        "signalChanged",
        "onBtnClose",
        "onBtnRefresh",
        "onBtnLegend",
        "onBtnOrbits",
        "onBtnIntParams",
        "onBtnView",
        "onBtnGCF",
        "onBtnCurve",
        "onBtnIsoclines",
        "onBtnPlotSep",
        "onBtnPlotAllSeps",
        "onBtnLimitCycles",
        "onBtnPrint",
        "close",
        "openZoomWindow",
        "closeZoomWindow",
        "id",
        "configure",
        "customEvent",
        "QEvent*",
        "e",
        "hideEvent",
        "QHideEvent*",
        "h",
        "getDlgData",
        "adjustHeight",
        "onSaveSignal",
        "onLoadSignal"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'signalEvaluating'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'signalEvaluated'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'signalChanged'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnClose'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnRefresh'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnLegend'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnOrbits'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnIntParams'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnView'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnGCF'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnCurve'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnIsoclines'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnPlotSep'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnPlotAllSeps'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnLimitCycles'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBtnPrint'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'close'
        QtMocHelpers::SlotData<bool()>(18, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'openZoomWindow'
        QtMocHelpers::SlotData<void(double, double, double, double)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 2 }, { QMetaType::Double, 2 }, { QMetaType::Double, 2 }, { QMetaType::Double, 2 },
        }}),
        // Slot 'closeZoomWindow'
        QtMocHelpers::SlotData<void(int)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Slot 'configure'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'customEvent'
        QtMocHelpers::SlotData<void(QEvent *)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Slot 'hideEvent'
        QtMocHelpers::SlotData<void(QHideEvent *)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 27, 28 },
        }}),
        // Slot 'getDlgData'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'adjustHeight'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSaveSignal'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onLoadSignal'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QPlotWnd, qt_meta_tag_ZN8QPlotWndE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QPlotWnd::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QPlotWndE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QPlotWndE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8QPlotWndE_t>.metaTypes,
    nullptr
} };

void QPlotWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QPlotWnd *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->signalEvaluating(); break;
        case 1: _t->signalEvaluated(); break;
        case 2: _t->signalChanged(); break;
        case 3: _t->onBtnClose(); break;
        case 4: _t->onBtnRefresh(); break;
        case 5: _t->onBtnLegend(); break;
        case 6: _t->onBtnOrbits(); break;
        case 7: _t->onBtnIntParams(); break;
        case 8: _t->onBtnView(); break;
        case 9: _t->onBtnGCF(); break;
        case 10: _t->onBtnCurve(); break;
        case 11: _t->onBtnIsoclines(); break;
        case 12: _t->onBtnPlotSep(); break;
        case 13: _t->onBtnPlotAllSeps(); break;
        case 14: _t->onBtnLimitCycles(); break;
        case 15: _t->onBtnPrint(); break;
        case 16: { bool _r = _t->close();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 17: _t->openZoomWindow((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[4]))); break;
        case 18: _t->closeZoomWindow((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->configure(); break;
        case 20: _t->customEvent((*reinterpret_cast< std::add_pointer_t<QEvent*>>(_a[1]))); break;
        case 21: _t->hideEvent((*reinterpret_cast< std::add_pointer_t<QHideEvent*>>(_a[1]))); break;
        case 22: _t->getDlgData(); break;
        case 23: _t->adjustHeight(); break;
        case 24: _t->onSaveSignal(); break;
        case 25: _t->onLoadSignal(); break;
        default: ;
        }
    }
}

const QMetaObject *QPlotWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QPlotWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8QPlotWndE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int QPlotWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 26;
    }
    return _id;
}
QT_WARNING_POP
