/****************************************************************************
** Meta object code from reading C++ file 'appcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/appcontroller.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
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
struct qt_meta_tag_ZN13AppControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto AppController::qt_create_metaobjectdata<qt_meta_tag_ZN13AppControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppController",
        "currentScreenChanged",
        "",
        "currentScreen",
        "sessionLabel",
        "systemLabel",
        "walletLabel",
        "agentStatus",
        "appNames",
        "appRegistryModel",
        "AppRegistryModel*",
        "activityFeedModel",
        "ActivityFeedModel*",
        "quickActionsModel",
        "QuickActionsModel*",
        "approvalQueueModel",
        "ApprovalQueueModel*",
        "ghostRuntime",
        "GhostRuntime*"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'currentScreenChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'currentScreen'
        QtMocHelpers::PropertyData<QString>(3, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'sessionLabel'
        QtMocHelpers::PropertyData<QString>(4, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'systemLabel'
        QtMocHelpers::PropertyData<QString>(5, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'walletLabel'
        QtMocHelpers::PropertyData<QString>(6, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'agentStatus'
        QtMocHelpers::PropertyData<QString>(7, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'appNames'
        QtMocHelpers::PropertyData<QStringList>(8, QMetaType::QStringList, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'appRegistryModel'
        QtMocHelpers::PropertyData<AppRegistryModel*>(9, 0x80000000 | 10, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'activityFeedModel'
        QtMocHelpers::PropertyData<ActivityFeedModel*>(11, 0x80000000 | 12, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'quickActionsModel'
        QtMocHelpers::PropertyData<QuickActionsModel*>(13, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'approvalQueueModel'
        QtMocHelpers::PropertyData<ApprovalQueueModel*>(15, 0x80000000 | 16, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'ghostRuntime'
        QtMocHelpers::PropertyData<GhostRuntime*>(17, 0x80000000 | 18, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AppController, qt_meta_tag_ZN13AppControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AppController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13AppControllerE_t>.metaTypes,
    nullptr
} };

void AppController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->currentScreenChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::currentScreenChanged, 0))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ActivityFeedModel* >(); break;
        case 6:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< AppRegistryModel* >(); break;
        case 9:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ApprovalQueueModel* >(); break;
        case 10:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GhostRuntime* >(); break;
        case 8:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QuickActionsModel* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->currentScreen(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->sessionLabel(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->systemLabel(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->walletLabel(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->agentStatus(); break;
        case 5: *reinterpret_cast<QStringList*>(_v) = _t->appNames(); break;
        case 6: *reinterpret_cast<AppRegistryModel**>(_v) = _t->appRegistryModel(); break;
        case 7: *reinterpret_cast<ActivityFeedModel**>(_v) = _t->activityFeedModel(); break;
        case 8: *reinterpret_cast<QuickActionsModel**>(_v) = _t->quickActionsModel(); break;
        case 9: *reinterpret_cast<ApprovalQueueModel**>(_v) = _t->approvalQueueModel(); break;
        case 10: *reinterpret_cast<GhostRuntime**>(_v) = _t->ghostRuntime(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setCurrentScreen(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *AppController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void AppController::currentScreenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
