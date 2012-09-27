#
# global definitions
#
VERSION         = "3.0.7"
APP_NAME        = $$quote(YubiKey Personalization Tool)

#
# common configuration
#
QT             += core gui
TEMPLATE        = app
TARGET          = YKPersonalization

DEFINES        += VERSION=\\\"$${VERSION}\\\"

CONFIG(debug, debug|release) {
    TARGET_DIR  = build/debug

    CONFIG     += console no_lflags_merge
} else {
    TARGET_DIR  = build/release

    DEFINES    += QT_NO_DEBUG_OUTPUT
}

UI_DIR          = ./src/ui
RCC_DIR         = "$$TARGET_DIR/RCCFiles"
MOC_DIR         = "$$TARGET_DIR/MOCFiles"
OBJECTS_DIR     = "$$TARGET_DIR/ObjFiles"
DESTDIR         = "$$TARGET_DIR"

DEPENDPATH     += . src src/ui
INCLUDEPATH    += . src src/ui

FORMS += \
    src/ui/toolpage.ui \
    src/ui/staticpage.ui \
    src/ui/settingpage.ui \
    src/ui/otppage.ui \
    src/ui/oathpage.ui \
    src/ui/mainwindow.ui \
    src/ui/helpbox.ui \
    src/ui/confirmbox.ui \
    src/ui/chalresppage.ui \
    src/ui/aboutpage.ui

HEADERS += \
    src/ui/toolpage.h \
    src/ui/staticpage.h \
    src/ui/settingpage.h \
    src/ui/scanedit.h \
    src/ui/otppage.h \
    src/ui/oathpage.h \
    src/ui/mainwindow.h \
    src/ui/helpbox.h \
    src/ui/confirmbox.h \
    src/ui/chalresppage.h \
    src/ui/aboutpage.h \
    src/yubikeywriter.h \
    src/yubikeyutil.h \
    src/yubikeylogger.h \
    src/yubikeyfinder.h \
    src/yubikeyconfig.h \
    src/version.h \
    src/otpdef.h \
    src/common.h

SOURCES += \
    src/main.cpp \
    src/ui/toolpage.cpp \
    src/ui/staticpage.cpp \
    src/ui/settingpage.cpp \
    src/ui/scanedit.cpp \
    src/ui/otppage.cpp \
    src/ui/oathpage.cpp \
    src/ui/mainwindow.cpp \
    src/ui/helpbox.cpp \
    src/ui/confirmbox.cpp \
    src/ui/chalresppage.cpp \
    src/ui/aboutpage.cpp \
    src/yubikeywriter.cpp \
    src/yubikeyutil.cpp \
    src/yubikeylogger.cpp \
    src/yubikeyfinder.cpp \
    src/yubikeyconfig.cpp

RESOURCES += \
    resources/resources.qrc

OTHER_FILES += \
    resources/win/resources.rc \
    resources/mac/Yubico.icns \
    resources/mac/Info.plist.in

!debian {
  HEADERS += \
      deps/libusb-1.0/libusb.h \
      deps/libykpers/ykpers.h \
      deps/libykpers/ykpbkdf2.h \
      deps/libykpers/ykcore/yktsd.h \
      deps/libykpers/ykcore/ykstatus.h \
      deps/libykpers/ykcore/ykdef.h \
      deps/libykpers/ykcore/ykcore_lcl.h \
      deps/libykpers/ykcore/ykcore_backend.h \
      deps/libykpers/ykcore/ykcore.h \
      deps/libykpers/rfc4634/sha.h \
      deps/libykpers/rfc4634/sha-private.h \
      deps/libyubikey/yubikey.h

  INCLUDEPATH    += . src src/ui deps/libusb-1.0 deps/libykpers deps/libykpers/ykcore deps/libyubikey
}

cross {
    message("Doing a cross platform build..")
    QMAKE_CXXFLAGS += $$(CXXFLAGS)
    QMAKE_LFLAGS += $$(LDFLAGS)

    # pickup compiler from environment
    _TARGET = $$(TARGET)
    isEmpty(_TARGET) {
        error("Cross compiling without a target is completely invalid.")
    }
    QMAKE_CC = $$(TARGET)-gcc
    QMAKE_CXX = $$(TARGET)-g++

    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_C = $$QMAKE_CC

    win32 {
        QMAKE_LIB = $$(TARGET)-ar -ru
        QMAKE_RC = $$(TARGET)-windres

        QMAKE_MOC = $$[QT_INSTALL_BINS]/moc
        QMAKE_UIC = $$[QT_INSTALL_BINS]/uic
        QMAKE_IDC = $$[QT_INSTALL_BINS]/idc
    }

    _INCDIR = $$(QT_INCDIR)
    !isEmpty (_INCDIR) {
        QMAKE_INCDIR_QT = $$(QT_INCDIR)
        macx {
            QMAKE_CPPFLAGS += -I$$(QT_INCDIR)/QtCore.Framework/Headers/
            QMAKE_CPPFLAGS += -I$$(QT_INCDIR)/QtGui.Framework/Headers/
        }
    }
    _LIBDIR = $$(QT_LIBDIR)
    !isEmpty (_LIBDIR) {
        QMAKE_LIBDIR_QT = $$(QT_LIBDIR)
        macx {
            QMAKE_LFLAGS += -L$$(QT_LIBDIR)/QtCore.Framework/
            QMAKE_LFLAGS += -L$$(QT_LIBDIR)/QtGui.Framework/
        }
    }
}

#
# Windows specific configuration
#
win32 {
    HEADERS += src/crandom.h
    SOURCES += src/crandom.cpp

    # The application icon
    RC_FILE = resources/win/resources.rc

    # The application dependencies
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("Windows x86 build")

        LIBS += $$quote(-L./libs/win32) -llibyubikey-0 -llibykpers-1-1
    } else {
        message("Windows x86_64 build")

        LIBS += $$quote(-L./libs/win64) -llibyubikey-0 -llibykpers-1-1
    }

    # Copy dependencies
    !contains(QMAKE_TARGET.arch, x86_64) {
        CONFIG(debug, debug|release) {
            LIB_FILES += \
                $(QTDIR)/bin/QtCored4.dll \
                $(QTDIR)/bin/QtGuid4.dll
        } else {
            LIB_FILES += \
                $(QTDIR)/bin/QtCore4.dll \
                $(QTDIR)/bin/QtGui4.dll
        }

        LIB_FILES += \
            $(QTDIR)/bin/libgcc_s_dw2-1.dll \
            $(QTDIR)/../mingw/bin/mingwm10.dll \
            libs/win32/libyubikey-0.dll \
            libs/win32/libykpers-1-1.dll
    } else {
        LIB_FILES += \
            libs/win64/libyubikey-0.dll \
            libs/win64/libykpers-1-1.dll
    }

    !cross {
        LIB_FILES_WIN = $${LIB_FILES}
        LIB_FILES_WIN ~= s,/,\\,g
        TARGET_DIR_WIN = $${DESTDIR}
        TARGET_DIR_WIN ~= s,/,\\,g
        for(FILE, LIB_FILES_WIN) {
            QMAKE_POST_LINK +=$$quote(cmd /c copy /y $${FILE} $${TARGET_DIR_WIN}$$escape_expand(\\n\\t))
        }
    }
}

#
# Non-windows specific configuration
#
!win32:!debian {
    SOURCES += \
        deps/libykpers/ykpers.c \
        deps/libykpers/ykpbkdf2.c \
        deps/libykpers/ykcore/ykstatus.c \
        deps/libykpers/ykcore/ykcore.c \
        deps/libykpers/rfc4634/usha.c \
        deps/libykpers/rfc4634/sha384-512.c \
        deps/libykpers/rfc4634/sha224-256.c \
        deps/libykpers/rfc4634/sha1.c \
        deps/libykpers/rfc4634/hmac.c \
        deps/libyubikey/yktoken.c \
        deps/libyubikey/ykmodhex.c \
        deps/libyubikey/ykhex.c \
        deps/libyubikey/ykcrc.c \
        deps/libyubikey/ykaes.c
}

#
# *nix specific configuration
#
unix:!macx {
  debian {
    message("Debian build")

    TARGET = yubikey-personalization-gui

    LIBS += -lyubikey

    CONFIG += link_pkgconfig
    PKGCONFIG += ykpers-1

    QMAKE_CXXFLAGS += $$(CXXFLAGS)
    QMAKE_LFLAGS += $$(LDFLAGS)

  } else {
    message("Linux build")

    SOURCES += deps/libykpers/ykcore/ykcore_libusb-1.0.c

    # The application dependencies
    LIBS += $$quote(-L./libs/lin) -lusb-1.0

    TARGET_LIN = $${APP_NAME}
    TARGET_LIN ~= s, ,\\ ,g

    # Copy dependencies and other resources
    LIB_FILES += \
        $(QTDIR)/lib/libQtGui.so.4 \
        $(QTDIR)/lib/libQtCore.so.4 \
        libs/lin/libusb-1.0.so.0 \
        resources/lin/$${TARGET_LIN}.sh

    for(FILE, LIB_FILES){
        QMAKE_POST_LINK += $$quote(cp $${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
    }

    # Create application tarball
    TARROOT = Yubico
    TARFILE = "$${TARGET_LIN}\\ Installer-lin.tgz"

    tarball.target   = tarball
    tarball.commands = \
        rm -rf "$${DESTDIR}/$${TARROOT}"; \
        mkdir "$${DESTDIR}/$${TARROOT}"; \
        (cd $${DESTDIR} && cp -R $${TARGET} *.sh *.so* "$${TARROOT}"); \
        rm -f "$${DESTDIR}/$${TARFILE}"; \
        (cd $${DESTDIR} && tar -czf "$${TARFILE}" $${TARROOT}); \
        rm -rf "$${DESTDIR}/$${TARROOT}";

    QMAKE_EXTRA_TARGETS += tarball
  }
}

#
# MacOS X specific configuration
#
macx {
    message("Mac build")

    CONFIG += x86_64

    SOURCES += deps/libykpers/ykcore/ykcore_osx.c

    DEFINES += QT_MAC_USE_COCOA

    cross {
        _SDK = $$(OSX_SDK)
        !isEmpty (_SDK) {
            # FIXME: this is prone to breaking with version numbers
            QT_INCDIR += $$(OSX_SDK)/use/include/c++/4.2.1
        }
    }
    # The application dependencies
    LIBS += $$_SDK/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += $$_SDK/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit

    # The application executable name
    TARGET = $$APP_NAME
    TARGET_MAC = $${TARGET}
    TARGET_MAC ~= s, ,\\ ,g

    # The application icon
    ICON = resources/mac/Yubico.icns

    # Copy required resources into the final app bundle and
    # put the current version number into Info.plist
    QMAKE_POST_LINK = $$quote(mkdir -p $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources && \
        cp -R resources/mac/Yubico.icns $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources/. && \
        sed -e \'s|@@version@@|$$VERSION|g\' \
        < resources/mac/Info.plist.in  > $${DESTDIR}/$${TARGET_MAC}.app/Contents/Info.plist)

    # Create application dmg
    shutup = ">/dev/null 2>&1"
    isEmpty(MACDEPLOYQT):MACDEPLOYQT = macdeployqt"
    !system($$MACDEPLOYQT $$shutup) {
        warning("macdeployqt utility '$$MACDEPLOYQT' not found \
                 will not create target for application bundling")
    } else {
        macdeploy.depends  = $${DESTDIR}/$${TARGET_MAC}.app/Contents/MacOS/$${TARGET_MAC}
        macdeploy.target   = macdeploy
        macdeploy.commands = \
            [ -f $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources/qt.conf ] || \
                $$MACDEPLOYQT $${DESTDIR}/$${TARGET_MAC}.app -no-strip;

        QMAKE_EXTRA_TARGETS += macdeploy
    }

    isEmpty(HDIUTIL):HDIUTIL = "hdiutil"
    !system($$HDIUTIL help $$shutup) {
        warning("hdiutil utility '$$HDIUTIL' not found \
                 will not create target for disk image creation")
    } else {
        contains(QMAKE_EXTRA_TARGETS, macdeploy) {
            IMAGEROOT = $${DESTDIR}/disk-image-root
            IMAGEFILE = $${DESTDIR}/$${TARGET_MAC}\\ Installer-mac.dmg

            #Note: Volume name for disk image should be passed without escaping quotes
            macdisk.depends  = macdeploy
            macdisk.target   = macdisk
            macdisk.commands = \
                rm -rf $${IMAGEROOT}; \
                mkdir $${IMAGEROOT}; \
                cp -R $${DESTDIR}/$${TARGET_MAC}.app $${IMAGEROOT}; \
                rm -f $${IMAGEFILE}; \
                $${HDIUTIL} create -srcfolder $${IMAGEROOT} -format UDBZ \
                    -volname \'$${TARGET} $${VERSION}\' $${IMAGEFILE}; \
                rm -rf $${IMAGEROOT}

            QMAKE_EXTRA_TARGETS += macdisk
        }
    }
}

#
# Additional cleanup to be performed
#
win32 {
    TARGET_DIR_WIN = $${DESTDIR}
    TARGET_DIR_WIN ~= s,/,\\,g

    QMAKE_CLEAN += $${TARGET_DIR_WIN}\\*.exe \
                   $${TARGET_DIR_WIN}\\*.dll
} else:macx {
    QMAKE_CLEAN += -r $${DESTDIR}/*.app
} else {
    QMAKE_CLEAN += -r $${DESTDIR}/*
}
