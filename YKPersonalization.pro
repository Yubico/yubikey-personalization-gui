#
# global definitions
#
VERSION_MAJOR   = 3
VERSION_MINOR   = 1
VERSION_BUILD   = 10
VERSION         = "$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}"
APP_NAME        = $$quote(YubiKey Personalization Tool)

#
# common configuration
#
QT             += core gui
DEPLOYMENT_PLUGIN += qmng
TEMPLATE        = app
TARGET          = yubikey-personalization-gui

DEFINES        += VERSION_MAJOR=\\\"$${VERSION_MAJOR}\\\" VERSION_MINOR=\\\"$${VERSION_MINOR}\\\" VERSION_BUILD=\\\"$${VERSION_BUILD}\\\"

CONFIG         += exceptions

CONFIG         += silent

CONFIG(debug, debug|release) {
    message("Debug build")
    TARGET_DIR  = build$${DIR_SEPARATOR}debug

    QT += testlib

    CONFIG     += console no_lflags_merge
} else {
    message("Release build")
    TARGET_DIR  = build$${DIR_SEPARATOR}release

    DEFINES    += QT_NO_DEBUG_OUTPUT
}

UI_DIR          = ./src/ui
RCC_DIR         = "$$TARGET_DIR/RCCFiles"
MOC_DIR         = "$$TARGET_DIR/MOCFiles"
OBJECTS_DIR     = "$$TARGET_DIR/ObjFiles"
DESTDIR         = "$$TARGET_DIR"

DEPENDPATH     += . src src/ui
INCLUDEPATH    += . src src/ui

LICENSEFILES    = json-c.txt \
                  libyubikey.txt \
                  yubikey-personalization.txt

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
    resources/mac/Info.plist.in \
    resources/mac/qt.conf

cross {
    message("Doing a cross platform build..")
    QMAKE_CXXFLAGS += $$(CXXFLAGS)
    QMAKE_LFLAGS += $$(LDFLAGS)

    # pickup compiler from environment
    _TARGET_ARCH = $$(TARGET_ARCH)
    isEmpty(_TARGET_ARCH) {
        error("Cross compiling without a target is completely invalid, set TARGET_ARCH")
    }
    QMAKE_CC = $$(TARGET_ARCH)-gcc
    QMAKE_CXX = $$(TARGET_ARCH)-g++

    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_C = $$QMAKE_CC

    win32 {
        QMAKE_LIB = $$(TARGET_ARCH)-ar -ru
        QMAKE_RC = $$(TARGET_ARCH)-windres

        QMAKE_MOC = $$[QT_INSTALL_BINS]/moc
        QMAKE_UIC = $$[QT_INSTALL_BINS]/uic
        QMAKE_IDC = $$[QT_INSTALL_BINS]/idc
        QMAKE_RCC = $$[QT_INSTALL_BINS]/rcc

        QMAKE_LFLAGS += -static-libstdc++ -static-libgcc
    }

    _QTDIR = $$(QTDIR)
    !isEmpty (_QTDIR) {
      _QT_INCDIR = $$(QTDIR)$${DIR_SEPARATOR}include
      _QT_LIBDIR = $$(QTDIR)$${DIR_SEPARATOR}lib
      _QT_BINDIR = $$(QTDIR)$${DIR_SEPARATOR}bin
      _QT_PLUGINDIR = $$(QTDIR)$${DIR_SEPARATOR}plugins
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

#
# Windows specific configuration
#
win32 {
    INCLUDEPATH += libs/win32/include libs/win32/include/ykpers-1
    HEADERS += src/crandom.h
    SOURCES += src/crandom.cpp

    # The application icon
    RC_FILE = resources/win/resources.rc

    # The application dependencies
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("Windows x86 build")

        LIBS += $$quote(-L./libs/win32/bin) -llibyubikey-0 -llibykpers-1-1
    } else {
        message("Windows x86_64 build")

        LIBS += $$quote(-L./libs/win64/bin) -llibyubikey-0 -llibykpers-1-1
    }

    CONFIG(debug, debug|release) {
        LIB_FILES += \
             $$_QT_BINDIR$${DIR_SEPARATOR}QtCored4.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}QtGuid4.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}QtTestd4.dll \
             $$_QT_PLUGINDIR$${DIR_SEPARATOR}imageformats$${DIR_SEPARATOR}qmngd4.dll
    } else {
        LIB_FILES += \
             $$_QT_BINDIR$${DIR_SEPARATOR}QtCore4.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}QtGui4.dll \
             $$_QT_PLUGINDIR$${DIR_SEPARATOR}imageformats$${DIR_SEPARATOR}qmng4.dll
    }

    LIB_FILES += \
        $$_QT_BINDIR$${DIR_SEPARATOR}libgcc_s_dw2-1.dll \
        $$_QT_BINDIR$${DIR_SEPARATOR}mingwm10.dll \
        libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}bin$${DIR_SEPARATOR}libjson-c-2.dll \
        libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}bin$${DIR_SEPARATOR}libyubikey-0.dll \
        libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}bin$${DIR_SEPARATOR}libykpers-1-1.dll

    isEmpty(TIMESTAMP_URL):TIMESTAMP_URL = 'http://timestamp.verisign.com/scripts/timstamp.dll'

    LIB_FILES_WIN = $${LIB_FILES}
    TARGET_DIR_WIN = $${DESTDIR}
    for(FILE, LIB_FILES_WIN) {
        QMAKE_POST_LINK +=$$quote($$QMAKE_COPY $${FILE} $${TARGET_DIR_WIN}$$escape_expand(\\n\\t))
    }
    LICENSE_DIR = $${TARGET_DIR_WIN}$${DIR_SEPARATOR}licenses
    QMAKE_POST_LINK += $$quote($$QMAKE_MKDIR $${LICENSE_DIR}$$escape_expand(\\n\\t))
    BASEDIR = libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}licenses$${DIR_SEPARATOR}
    for(FILE, LICENSEFILES) {
        QMAKE_POST_LINK += $$quote($$QMAKE_COPY $${BASEDIR}$${FILE} $${LICENSE_DIR}$$escape_expand(\\n\\t))
    }
    QMAKE_POST_LINK += $$quote($$QMAKE_COPY COPYING $${LICENSE_DIR}$${DIR_SEPARATOR}yubikey-personalization-gui.txt$$escape_expand(\\n\\t))
    sign_binaries {
        _PVK_FILE = $$(PVK_FILE)
        _SPC_FILE = $$(SPC_FILE)
        isEmpty(_PVK_FILE) {
            error("Must have a pvk file to sign (PVK_FILE env variable).")
        }
        isEmpty(_SPC_FILE) {
            error("Must have a spc file to sign (SPC_FILE env variable).")
        }

        # sign all Yubico binaries
        SIGN_FILES = $${TARGET}.exe \
            libyubikey-0.dll \
            libykpers-1-1.dll

        for(FILE, SIGN_FILES) {
            QMAKE_POST_LINK += $$quote("signcode -spc $$(SPC_FILE) -v $$(PVK_FILE) -a sha1 -$ commercial -n '$${APP_NAME}' -i 'http://www.yubico.com' -t $${TIMESTAMP_URL} $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${FILE}"$$escape_expand(\\n\\t))
        }
    }
    build_installer {
        QMAKE_POST_LINK += $$quote("makensis -DYKPERS_VERSION=$${VERSION} installer/win-nsis/ykpers.nsi"$$escape_expand(\\n\\t))
        sign_binaries {
            QMAKE_POST_LINK += $$quote("signcode -spc $$(SPC_FILE) -v $$(PVK_FILE) -a sha1 -$ commercial -n '$${APP_NAME} Installer' -i 'http://www.yubico.com' -t '$${TIMESTAMP_URL}' $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${TARGET}-$${VERSION}.exe"$$escape_expand(\\n\\t))
        }
    }
}

#
# *nix specific configuration
#
unix:!macx {
    message("Unix build")

    LIBS += -lyubikey

    CONFIG += link_pkgconfig
    PKGCONFIG += ykpers-1

    QMAKE_CXXFLAGS += $$(CXXFLAGS)
    QMAKE_LFLAGS += $$(LDFLAGS)
}

#
# MacOS X specific configuration
#
macx {
    message("Mac build")

    INCLUDEPATH += libs/macx/include libs/macx/include/ykpers-1
    LIBS += libs/macx/lib/libykpers-1.dylib libs/macx/lib/libyubikey.dylib

    CONFIG += x86_64

    DEFINES += QT_MAC_USE_COCOA

    _QT_LIBDIR = $$QMAKE_LIBDIR_QT
    _QT_PLUGINDIR = $$[QT_INSTALL_PLUGINS]

    for_store {
        QMAKE_CFLAGS += -gdwarf-2
        QMAKE_CXXFLAGS += -gdwarf-2

        isEmpty(PACKAGE_SIGN_IDENTITY):PACKAGE_SIGN_IDENTITY = '3rd Party Mac Developer Application'
        isEmpty(INSTALLER_SIGN_IDENTITY):INSTALLER_SIGN_IDENTITY = '3rd Party Mac Developer Installer'
    } else {
        isEmpty(PACKAGE_SIGN_IDENTITY):PACKAGE_SIGN_IDENTITY = 'Developer ID Application'
        isEmpty(INSTALLER_SIGN_IDENTITY):INSTALLER_SIGN_IDENTITY = 'Developer ID Installer'
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
    QMAKE_POST_LINK += $$quote(mkdir -p $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources && \
        cp -R resources/mac/Yubico.icns $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources/. && \
        cp resources/mac/qt.conf $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources/. && \
        sed -e \'s|@@version@@|$$VERSION|g\' \
        < resources/mac/Info.plist.in  > $${DESTDIR}/$${TARGET_MAC}.app/Contents/Info.plist)

    # copy the QT libraries into our bundle
    _BASEDIR = $${DESTDIR}/$${TARGET_MAC}.app/Contents
    _FRAMEWORKDIR = $${_BASEDIR}/Frameworks
    _PLUGINDIR = $${_BASEDIR}/PlugIns
    QMAKE_POST_LINK += $$quote( && mkdir -p $$_FRAMEWORKDIR && \
        cp -R $$_QT_LIBDIR/QtCore.framework $$_FRAMEWORKDIR/QtCore.framework && \
        rm -rf $$_FRAMEWORKDIR/QtCore.framework/Versions/4/Headers && \
        cp -R $$_QT_LIBDIR/QtGui.framework $$_FRAMEWORKDIR/QtGui.framework && \
        rm -rf $$_FRAMEWORKDIR/QtGui.framework/Versions/4/Headers && \
        find $$_FRAMEWORKDIR -type l -print0 | xargs -0 rm -f  && \
        test -d $$_BASEDIR/Resources/qt_menu.nib || \
        mv $$_FRAMEWORKDIR/QtGui.framework/Versions/4/Resources/qt_menu.nib $$_BASEDIR/Resources/qt_menu.nib && \
        test ! -d $$_FRAMEWORKDIR/QtGui.framework/Versions/4/Resources || \
        rmdir $$_FRAMEWORKDIR/QtGui.framework/Versions/4/Resources && \
        mkdir -p $$_PLUGINDIR/imageformats && \
        cp -R $$_QT_PLUGINDIR/imageformats/libqmng.dylib $$_PLUGINDIR/imageformats)

    # copy libykpers and friends
    _LIBDIR = $${_BASEDIR}/lib
    QMAKE_POST_LINK += $$quote( && mkdir -p $$_LIBDIR && \
        cp libs/macx/lib/libyubikey.0.dylib $$_LIBDIR && \
        cp libs/macx/lib/libykpers-1.1.dylib $$_LIBDIR && \
        cp libs/macx/lib/libjson-c.2.dylib $$_LIBDIR)

    _LICENSEDIR = $${_BASEDIR}/licenses
    QMAKE_POST_LINK += $$quote(&& mkdir -p $$_LICENSEDIR && \
        cp COPYING $$_LICENSEDIR/yubikey-personalization-gui.txt)
    for(FILE, LICENSEFILES) {
        QMAKE_POST_LINK += $$quote(&& cp libs/macx/licenses/$${FILE} $$_LICENSEDIR)
    }


    # fixup all library paths..
    _BASE = $$quote(@executable_path/../Frameworks)
    _QTCORE = $$quote(QtCore.framework/Versions/4/QtCore)
    _QTGUI = $$quote(QtGui.framework/Versions/4/QtGui)
    isEmpty(_TARGET_ARCH) {
        _INSTALL_NAME_TOOL = install_name_tool
    } else {
        _INSTALL_NAME_TOOL = $$(TARGET_ARCH)-install_name_tool
    }
    QMAKE_POST_LINK += $$quote( && $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/$$_QTCORE $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/$$_QTGUI $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/$$_QTCORE $$_FRAMEWORKDIR/$$_QTGUI && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/$$_QTCORE $$_PLUGINDIR/imageformats/libqmng.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/$$_QTGUI $$_PLUGINDIR/imageformats/libqmng.dylib)

    build_installer {
        QMAKE_POST_LINK += $$quote( && codesign -s \'$$PACKAGE_SIGN_IDENTITY\' $${DESTDIR}/$${TARGET_MAC}.app \
            --entitlements resources/mac/Entitlements.plist && \
            productbuild --sign \'$$INSTALLER_SIGN_IDENTITY\' --component $${DESTDIR}/$${TARGET_MAC}.app /Applications $${DESTDIR}/$${TARGET_MAC}-$${VERSION}.pkg)
    }
}

#
# Additional cleanup to be performed
#
win32 {
    TARGET_DIR_WIN = $${DESTDIR}

    QMAKE_CLEAN += $${TARGET_DIR_WIN}$${DIR_SEPARATOR}*.exe \
                   $${TARGET_DIR_WIN}$${DIR_SEPARATOR}*.dll \
                   $${TARGET_DIR_WIN}$${DIR_SEPARATOR}*.exe.bak
} else:macx {
    QMAKE_CLEAN += -r $${DESTDIR}/*.app $${DESTDIR}/*.pkg $${DESTDIR}/*.dmg
} else {
    QMAKE_CLEAN += -r $${DESTDIR}/*
}
