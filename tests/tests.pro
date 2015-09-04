QT += testlib
QT -= gui
SOURCES += test.cpp scanedittest.cpp yubikeyutiltest.cpp versiontest.cpp
HEADERS += scanedittest.h yubikeyutiltest.h versiontest.h
CONFIG += testcase no_testcase_installs
CONFIG += console
CONFIG -= app_bundle
INCLUDEPATH += ../lib

!nosilent {
    CONFIG         += silent
}

CONFIG(debug, debug|release) {
    TARGET_DIR  = ..$${DIR_SEPARATOR}build$${DIR_SEPARATOR}debug
    CONFIG     += console no_lflags_merge
    LIBS += $$quote(-L../lib/debug) -llib
} else {
    TARGET_DIR  = ..$${DIR_SEPARATOR}build$${DIR_SEPARATOR}release
    DEFINES    += QT_NO_DEBUG_OUTPUT
    LIBS += $$quote(-L../lib/release) -llib
}

DESTDIR         = "$$TARGET_DIR"

QMAKE_CXXFLAGS += $$CXXFLAGS $$CPPFLAGS
QMAKE_LFLAGS += $$LDFLAGS

cross {
    message("Doing a cross platform build..")

    # pickup compiler from environment
    isEmpty(TARGET_ARCH) {
        error("Cross compiling without a target is completely invalid, set TARGET_ARCH")
    }
    QMAKE_CC = $${TARGET_ARCH}-gcc
    QMAKE_CXX = $${TARGET_ARCH}-g++

    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_C = $$QMAKE_CC

    win32 {
        QMAKE_LIB = $${TARGET_ARCH}-ar -ru
        QMAKE_RC = $${TARGET_ARCH}-windres $$quote(-DVERSION_WIN_STR=\'\\\"$${VERSION}\\0\\\"\')

        QMAKE_MOC = $$[QT_INSTALL_BINS]/moc
        QMAKE_UIC = $$[QT_INSTALL_BINS]/uic
        QMAKE_IDC = $$[QT_INSTALL_BINS]/idc
        QMAKE_RCC = $$[QT_INSTALL_BINS]/rcc

        QMAKE_LFLAGS += -static-libstdc++ -static-libgcc
    }

    !isEmpty (QTDIR) {
      _QT_INCDIR = $${QTDIR}$${DIR_SEPARATOR}include
      _QT_LIBDIR = $${QTDIR}$${DIR_SEPARATOR}lib
      _QT_BINDIR = $${QTDIR}$${DIR_SEPARATOR}bin
      _QT_PLUGINDIR = $${QTDIR}$${DIR_SEPARATOR}plugins
    } else {
      _QT_INCDIR = $$(QT_INCDIR)
      _QT_LIBDIR = $$(QT_LIBDIR)
      _QT_BINDIR = $$(QT_BINDIR)
      _QT_PLUGINDIR = $$(QT_PLUGINDIR)
    }
    !isEmpty (_QT_INCDIR) {
        QMAKE_INCDIR_QT = $$_QT_INCDIR
    }
    !isEmpty (_QT_LIBDIR) {
        QMAKE_LIBDIR_QT = $$_QT_LIBDIR
    }
}

unix:!macx|force_pkgconfig {
    LIBS += -lyubikey
}


macx:!force_pkgconfig {
    INCLUDEPATH += libs/macx/include
    LIBS += ../libs/macx/lib/libyubikey.dylib
}

win32 {
    LIBS += $$quote(-L$$_QT_BINDIR) $$quote(-L$$_QT_LIBDIR)
    !contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += $$quote(-L../libs/win32/bin) -llibyubikey-0
    } else {
        LIBS += $$quote(-L../libs/win64/bin) -llibyubikey-0
    }
}
