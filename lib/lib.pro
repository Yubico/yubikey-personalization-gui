TEMPLATE        = lib
CONFIG         += exceptions staticlib
!nosilent {
    CONFIG         += silent
}

CONFIG(debug, debug|release) {
    message("Debug build")
    CONFIG     += console no_lflags_merge warn_on
    DESTDIR = debug/
} else {
    message("Release build")
    DEFINES    += QT_NO_DEBUG_OUTPUT
    DESTDIR = release/
}

HEADERS += \
    scanedit.h \
    us-scanedit.h \
    de-scanedit.h \
    bepo-scanedit.h \
    fr-scanedit.h \
    common.h \
    otpdef.h \
    yubikeyutil.h

SOURCES += \
    scanedit.cpp \
    us-scanedit.cpp \
    de-scanedit.cpp \
    bepo-scanedit.cpp \
    fr-scanedit.cpp \
    yubikeyutil.cpp

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
}

#
# Windows specific configuration
#
win32 {
    HEADERS += crandom.h
    SOURCES += crandom.cpp

    INCLUDEPATH += ../libs/win32/include
}

macx:!force_pkgconfig {
    INCLUDEPATH += ../libs/macx/include
    CONFIG += x86_64
}

