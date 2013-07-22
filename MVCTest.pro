#-------------------------------------------------
#
# Project created by QtCreator 2011-07-26T01:04:32
#
#-------------------------------------------------

QT       += core gui sql network script widgets printsupport

CONFIG += c++11
TARGET = myprojects

TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    db.cpp \
    model.cpp \
    dbexception.cpp \
    clientwindow.cpp \
    settings.cpp \
    sync.cpp \
    qupfile.cpp \
    settingswindow.cpp \
    counter.cpp \
    projectcompleter.cpp \
    clientmodel.cpp \
    projectmodelitem.cpp \
    metamodel.cpp \
    paymentmodel.cpp \
    projectmodel.cpp \
    projectproxymodel.cpp \
    projectview.cpp \
    paymenteditwidget.cpp \
    projecteditwidget.cpp \
    reportwindow.cpp \
    report.cpp \
    exception.cpp \
    reportexception.cpp \
    invoicemodel.cpp \
    invoicedialog.cpp \
    invoiceprintdialog.cpp \
    delegates.cpp

HEADERS  += mainwindow.h \
    db.h \
    model.h \
    dbexception.h \
    clientwindow.h \
    settings.h \
    sync.h \
    qupfile.h \
    settingswindow.h \
    counter.h \
    projectcompleter.h \
    clientmodel.h \
    projectmodelitem.h \
    metamodel.h \
    paymentmodel.h \
    projectmodel.h \
    projectproxymodel.h \
    projectview.h \
    paymenteditwidget.h \
    projecteditwidget.h \
    report.h \
    reportwindow.h \
    exception.h \
    reportexception.h \
    invoicemodel.h \
    invoicedialog.h \
    invoiceprintdialog.h \
    delegates.h

FORMS    += mainwindow.ui \
    clientwindow.ui \
    settingswindow.ui \
    widgetpaymentedit.ui \
    reportwindow.ui \
    invoicedialog.ui \
    invoiceprintdialog.ui

RESOURCES += icons.qrc \
    icons.qrc


QMAKE_CXXFLAGS_DEBUG += -g
QMAKE_CXXFLAGS_RELEASE += -O3

OTHER_FILES += \
    android/version.xml \
    android/AndroidManifest.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/res/values-rs/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-it/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/values-ro/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-ja/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-pl/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-nl/strings.xml

