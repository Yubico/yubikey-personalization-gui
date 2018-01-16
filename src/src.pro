#
# global definitions
#
VERSION_MAJOR   = 3
VERSION_MINOR   = 1
VERSION_BUILD   = 26
VERSION         = "$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}"
APP_NAME        = $$quote(YubiKey Personalization Tool)

#
# common configuration
#
QT             += core gui
TEMPLATE        = app
TARGET          = yubikey-personalization-gui

DEFINES        += VERSION_MAJOR=\\\"$${VERSION_MAJOR}\\\" VERSION_MINOR=\\\"$${VERSION_MINOR}\\\" VERSION_BUILD=\\\"$${VERSION_BUILD}\\\" VERSION=\\\"$${VERSION}\\\"

CONFIG         += exceptions

# if this is qt5, add widgets
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    macx {
        QT += printsupport
        QTPLUGIN.platforms = qcocoa
    }
}

!nosilent {
    CONFIG         += silent
}

CONFIG(debug, debug|release) {
    message("Debug build")
    TARGET_DIR  = ..$${DIR_SEPARATOR}build$${DIR_SEPARATOR}debug

    CONFIG     += console no_lflags_merge warn_on
    LIBS += $$quote(-L../lib/debug) -llib
} else {
    message("Release build")
    TARGET_DIR  = ..$${DIR_SEPARATOR}build$${DIR_SEPARATOR}release

    DEFINES    += QT_NO_DEBUG_OUTPUT
    LIBS += $$quote(-L../lib/release) -llib
}

UI_DIR          = ./ui
RCC_DIR         = "$$TARGET_DIR/RCCFiles"
MOC_DIR         = "$$TARGET_DIR/MOCFiles"
OBJECTS_DIR     = "$$TARGET_DIR/ObjFiles"
DESTDIR         = "$$TARGET_DIR"

DEPENDPATH     += . ui
INCLUDEPATH    += ../lib . ui

LICENSEFILES    = json-c.txt \
                  libyubikey.txt \
                  yubikey-personalization.txt

FORMS += \
    ui/toolpage.ui \
    ui/staticpage.ui \
    ui/settingpage.ui \
    ui/otppage.ui \
    ui/oathpage.ui \
    ui/mainwindow.ui \
    ui/helpbox.ui \
    ui/confirmbox.ui \
    ui/chalresppage.ui \
    ui/aboutpage.ui \
    ui/diagnostics.ui \
    ui/yubiaccbox.ui

HEADERS += \
    ui/toolpage.h \
    ui/staticpage.h \
    ui/settingpage.h \
    ui/otppage.h \
    ui/oathpage.h \
    ui/mainwindow.h \
    ui/helpbox.h \
    ui/confirmbox.h \
    ui/chalresppage.h \
    ui/aboutpage.h \
    ui/diagnostics.h \
    ui/yubiaccbox.h \
    yubikeywriter.h \
    yubikeylogger.h \
    yubikeyfinder.h \
    yubikeyconfig.h \
    version.h \
    help.h

SOURCES += \
    main.cpp \
    ui/toolpage.cpp \
    ui/staticpage.cpp \
    ui/settingpage.cpp \
    ui/otppage.cpp \
    ui/oathpage.cpp \
    ui/mainwindow.cpp \
    ui/helpbox.cpp \
    ui/confirmbox.cpp \
    ui/chalresppage.cpp \
    ui/aboutpage.cpp \
    ui/yubiaccbox.cpp \
    ui/diagnostics.cpp \
    yubikeywriter.cpp \
    yubikeylogger.cpp \
    yubikeyfinder.cpp \
    yubikeyconfig.cpp

RESOURCES += \
    ../resources/resources.qrc

OTHER_FILES += \
    ../resources/win/resources.rc \
    ../resources/mac/Yubico.icns \
    ../resources/mac/Info.plist.in \
    ../resources/mac/qt.conf

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

#
# Windows specific configuration
#
win32 {
    INCLUDEPATH += ../libs/win32/include ../libs/win32/include/ykpers-1

    # The application icon
    RC_FILE = ../resources/win/resources.rc

    # The application dependencies
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("Windows x86 build")

        LIBS += $$quote(-L../libs/win32/bin) -llibyubikey-0 -llibykpers-1-1
    } else {
        message("Windows x86_64 build")

        LIBS += $$quote(-L../libs/win64/bin) -llibyubikey-0 -llibykpers-1-1
    }

    LIBS += $$quote(-L$$_QT_BINDIR) $$quote(-L$$_QT_LIBDIR)

    CONFIG(debug, debug|release) {
        LIB_FILES += \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Cored.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Guid.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Widgetsd.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Testd.dll \
             $$_QT_PLUGINDIR$${DIR_SEPARATOR}platforms$${DIR_SEPARATOR}qwindowsd.dll \
             $$_QT_PLUGINDIR$${DIR_SEPARATOR}imageformats$${DIR_SEPARATOR}qmngd.dll
    } else {
        LIB_FILES += \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Core.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Gui.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Widgets.dll \
             $$_QT_BINDIR$${DIR_SEPARATOR}Qt5Test.dll \
             $$_QT_PLUGINDIR$${DIR_SEPARATOR}platforms$${DIR_SEPARATOR}qwindows.dll \
             $$_QT_PLUGINDIR$${DIR_SEPARATOR}imageformats$${DIR_SEPARATOR}qmng.dll
    }

    LIB_FILES += \
        $$_QT_BINDIR$${DIR_SEPARATOR}libgcc_s_dw2-1.dll \
        $$_QT_BINDIR$${DIR_SEPARATOR}libwinpthread-1.dll \
        $$_QT_BINDIR$${DIR_SEPARATOR}libstdc++-6.dll \
        $$_QT_BINDIR$${DIR_SEPARATOR}icuin54.dll \
        $$_QT_BINDIR$${DIR_SEPARATOR}icuuc54.dll \
        $$_QT_BINDIR$${DIR_SEPARATOR}icudt54.dll \
        ..$${DIR_SEPARATOR}libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}bin$${DIR_SEPARATOR}libjson-c-2.dll \
        ..$${DIR_SEPARATOR}libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}bin$${DIR_SEPARATOR}libyubikey-0.dll \
        ..$${DIR_SEPARATOR}libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}bin$${DIR_SEPARATOR}libykpers-1-1.dll

    isEmpty(TIMESTAMP_URL):TIMESTAMP_URL = 'http://tsa.starfieldtech.com'

    LIB_FILES_WIN = $${LIB_FILES}
    TARGET_DIR_WIN = $${DESTDIR}
    for(FILE, LIB_FILES_WIN) {
        QMAKE_POST_LINK +=$$quote($$QMAKE_COPY $${FILE} $${TARGET_DIR_WIN}$$escape_expand(\\n\\t))
    }
    LICENSE_DIR = $${TARGET_DIR_WIN}$${DIR_SEPARATOR}licenses
    QMAKE_POST_LINK += $$quote($$QMAKE_MKDIR $${LICENSE_DIR}$$escape_expand(\\n\\t))
    BASEDIR = ..$${DIR_SEPARATOR}libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}licenses$${DIR_SEPARATOR}
    for(FILE, LICENSEFILES) {
        QMAKE_POST_LINK += $$quote($$QMAKE_COPY $${BASEDIR}$${FILE} $${LICENSE_DIR}$$escape_expand(\\n\\t))
    }
    QMAKE_POST_LINK += $$quote($$QMAKE_COPY ..$${DIR_SEPARATOR}COPYING $${LICENSE_DIR}$${DIR_SEPARATOR}yubikey-personalization-gui.txt$$escape_expand(\\n\\t))
    sign_binaries {
        isEmpty(CERT_FILE) {
            error("Must have a cert file to sign (CERT_FILE env variable).")
        }

        # sign all Yubico binaries
        SIGN_FILES = $${TARGET}.exe \
            libyubikey-0.dll \
            libykpers-1-1.dll

        for(FILE, SIGN_FILES) {
            QMAKE_POST_LINK += $$quote("osslsigncode sign -pkcs11engine /usr/lib/engines/engine_pkcs11.so -pkcs11module /usr/lib/x86_64-linux-gnu/opensc-pkcs11.so -key slot_1-id_2 -certs $${CERT_FILE} -h sha256 -comm -t $${TIMESTAMP_URL} -n '$${APP_NAME}' -i 'https://www.yubico.com' $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${FILE} $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${FILE}-sign"$$escape_expand(\\n\\t))
            QMAKE_POST_LINK += $$quote("mv $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${FILE}-sign $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${FILE}"$$escape_expand(\\n\\t))
        }
    }
    build_installer {
        QMAKE_POST_LINK += $$quote("makensis -DYKPERS_VERSION=$${VERSION} ../installer/win-nsis/ykpers.nsi"$$escape_expand(\\n\\t))
        sign_binaries {
            QMAKE_POST_LINK += $$quote("osslsigncode sign -pkcs11engine /usr/lib/engines/engine_pkcs11.so -pkcs11module /usr/lib/x86_64-linux-gnu/opensc-pkcs11.so -key slot_1-id_2 -certs $${CERT_FILE} -h sha256 -comm -t $${TIMESTAMP_URL} -n '$${APP_NAME} Installer' -i 'https://www.yubico.com' $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${TARGET}-$${VERSION}.exe $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${TARGET}-$${VERSION}.exe-sign"$$escape_expand(\\n\\t))
            QMAKE_POST_LINK += $$quote("mv $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${TARGET}-$${VERSION}.exe-sign $${TARGET_DIR_WIN}$${DIR_SEPARATOR}$${TARGET}-$${VERSION}.exe"$$escape_expand(\\n\\t))
        }
    }
}

#
# *nix specific configuration
#
unix:!macx|force_pkgconfig {
    message("Unix build")

    LIBS += -lyubikey

    CONFIG += link_pkgconfig
    QT_CONFIG -= no-pkg-config
    PKGCONFIG += ykpers-1
    macx {
        QMAKE_INFO_PLIST = ../resources/mac/Info.plist.in
    }
}

#
# MacOS X specific configuration
#
macx {
    ICON = ../resources/mac/Yubico.icns
}
macx:!force_pkgconfig {
    message("Mac build")

    INCLUDEPATH += ../libs/macx/include ../libs/macx/include/ykpers-1
    LIBS += ../libs/macx/lib/libykpers-1.dylib ../libs/macx/lib/libyubikey.dylib

    CONFIG += x86_64

    DEFINES += QT_MAC_USE_COCOA

    _QT_LIBDIR = $$[QT_INSTALL_LIBS]
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

    BUILD = $$(BUILD)
    isEmpty(BUILD) {
        BUILD = 1
    }

    # Copy required resources into the final app bundle and
    # put the current version number into Info.plist
    QMAKE_POST_LINK += $$quote(mkdir -p $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources && \
        cp -R ../resources/mac/Yubico.icns $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources/. && \
        cp ../resources/mac/qt.conf $${DESTDIR}/$${TARGET_MAC}.app/Contents/Resources/. && \
        sed -e \'s|@SHORT_VERSION@|$$VERSION|g\' -e \'s|@FULL_VERSION@|$$VERSION|g\' \
        < ../resources/mac/Info.plist.in  > $${DESTDIR}/$${TARGET_MAC}.app/Contents/Info.plist)

    # copy the QT libraries into our bundle
    _BASEDIR = $${DESTDIR}/$${TARGET_MAC}.app/Contents
    _LIBDIR = $${_BASEDIR}/Frameworks
    _PLUGINDIR = $${_BASEDIR}/PlugIns
    QMAKE_POST_LINK += $$quote( && mkdir -p $$_LIBDIR && \
        cp $$_QT_LIBDIR/QtCore.framework/Versions/5/QtCore $$_LIBDIR && \
        cp $$_QT_LIBDIR/QtGui.framework/Versions/5/QtGui $$_LIBDIR && \
        cp $$_QT_LIBDIR/QtWidgets.framework/Versions/5/QtWidgets $$_LIBDIR && \
	cp $$_QT_LIBDIR/QtPrintSupport.framework/Versions/5/QtPrintSupport $$_LIBDIR && \
	cp $$_QT_LIBDIR/QtDBus.framework/Versions/5/QtDBus $$_LIBDIR && \
        mkdir -p $$_PLUGINDIR/imageformats && \
        cp -R $$_QT_PLUGINDIR/imageformats/libqmng.dylib $$_PLUGINDIR/imageformats && \
	mkdir -p $$_PLUGINDIR/platforms && \
	cp -R $$_QT_PLUGINDIR/platforms/libqcocoa.dylib $$_PLUGINDIR/platforms)

    # copy libykpers and friends
    QMAKE_POST_LINK += $$quote( && mkdir -p $$_LIBDIR && \
        cp ../libs/macx/lib/libyubikey.0.dylib $$_LIBDIR && \
        cp ../libs/macx/lib/libykpers-1.1.dylib $$_LIBDIR && \
        cp ../libs/macx/lib/libjson-c.2.dylib $$_LIBDIR)

    _LICENSEDIR = $${_BASEDIR}/licenses
    QMAKE_POST_LINK += $$quote(&& mkdir -p $$_LICENSEDIR && \
        cp ../COPYING $$_LICENSEDIR/yubikey-personalization-gui.txt)
    for(FILE, LICENSEFILES) {
        QMAKE_POST_LINK += $$quote(&& cp ../libs/macx/licenses/$${FILE} $$_LICENSEDIR)
    }

    # fixup all library paths..
    _BASE = $$quote(@executable_path/../Frameworks)
    _LIBBASE = $$quote(@executable_path/../lib)
    _QTCORE = $$quote(@rpath/QtCore.framework/Versions/5/QtCore)
    _QTGUI = $$quote(@rpath/QtGui.framework/Versions/5/QtGui)
    _QTWIDGETS = $$quote(@rpath/QtWidgets.framework/Versions/5/QtWidgets)
    _QTPRINTSUPPORT = $$quote(@rpath/QtPrintSupport.framework/Versions/5/QtPrintSupport)
    _QTDBUS = $$quote(@rpath/QtDBus.framework/Versions/5/QtDBus)
    isEmpty(TARGET_ARCH) {
        _INSTALL_NAME_TOOL = install_name_tool
    } else {
        _INSTALL_NAME_TOOL = $${TARGET_ARCH}-install_name_tool
    }
    QMAKE_POST_LINK += $$quote( && $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/QtGui $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_QTWIDGETS $$_BASE/QtWidgets $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_QTPRINTSUPPORT $$_BASE/QtPrintSupport $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_LIBBASE/libykpers-1.1.dylib $$_BASE/libykpers-1.1.dylib $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -change $$_LIBBASE/libyubikey.0.dylib $$_BASE/libyubikey.0.dylib $$_BASEDIR/MacOS/$$TARGET_MAC && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/libjson-c.2.dylib $$_LIBDIR/libjson-c.2.dylib && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/libyubikey.0.dylib $$_LIBDIR/libyubikey.0.dylib && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/libykpers-1.1.dylib $$_LIBDIR/libykpers-1.1.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_LIBBASE/libyubikey.0.dylib $$_BASE/libyubikey.0.dylib $$_LIBDIR/libykpers-1.1.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_LIBBASE/libjson-c.2.dylib $$_BASE/libjson-c.2.dylib $$_LIBDIR/libykpers-1.1.dylib && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/QtCore $$_LIBDIR/QtCore && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_LIBDIR/QtGui && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/QtGui $$_LIBDIR/QtGui && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_LIBDIR/QtWidgets && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/QtGui $$_LIBDIR/QtWidgets && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/QtWidgets $$_LIBDIR/QtWidgets && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_LIBDIR/QtPrintSupport && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/QtGui $$_LIBDIR/QtPrintSupport && \
        $$_INSTALL_NAME_TOOL -change $$_QTWIDGETS $$_BASE/QtWidgets $$_LIBDIR/QtPrintSupport && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/QtPrintSupport $$_LIBDIR/QtPrintSupport && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_LIBDIR/QtDBus && \
        $$_INSTALL_NAME_TOOL -id $$_BASE/QtDBus $$_LIBDIR/QtDBus && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_PLUGINDIR/imageformats/libqmng.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/QtGui $$_PLUGINDIR/imageformats/libqmng.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTCORE $$_BASE/QtCore $$_PLUGINDIR/platforms/libqcocoa.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTWIDGETS $$_BASE/QtWidgets $$_PLUGINDIR/platforms/libqcocoa.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTGUI $$_BASE/QtGui $$_PLUGINDIR/platforms/libqcocoa.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTDBUS $$_BASE/QtDBus $$_PLUGINDIR/platforms/libqcocoa.dylib && \
        $$_INSTALL_NAME_TOOL -change $$_QTPRINTSUPPORT $$_BASE/QtPrintSupport $$_PLUGINDIR/platforms/libqcocoa.dylib)

    QMAKE_POST_LINK += $$quote( && \
        if otool -L $$_LIBDIR/* $$_PLUGINDIR/*/* $$_BASEDIR/MacOS/$$TARGET_MAC | grep -e '$$_QT_LIBDIR' -e '$$_LIBBASE' | grep -q compatibility; then \
            echo "Something is incorrectly linked!"; \
            exit 1; \
        fi)

    build_installer {
        # the productbuild path doesn't work pre 10.8
        for_store {
            _INSTALLER_CMD = "productbuild --sign \'$$INSTALLER_SIGN_IDENTITY\' --component $${DESTDIR}/$${TARGET_MAC}.app /Applications/ $${DESTDIR}/yubikey-personalization-gui-$${VERSION}.pkg"
        } else {
            _INSTALLER_CMD = "rm -rf $${DESTDIR}/temp && \
                mkdir -p $${DESTDIR}/temp/ && \
                cp -R $${DESTDIR}/$${TARGET_MAC}.app $${DESTDIR}/temp && \
                pkgbuild --sign \'$$INSTALLER_SIGN_IDENTITY\' --root ${DESTDIR}/temp/ --component-plist ../resources/mac/installer.plist --install-location '/Applications/' $${DESTDIR}/yubikey-personalization-gui-$${VERSION}.pkg"
        }
        QMAKE_POST_LINK += $$quote( && \
            codesign --deep -s \'$$PACKAGE_SIGN_IDENTITY\' $${DESTDIR}/$${TARGET_MAC}.app \
            --entitlements ../resources/mac/Entitlements.plist && \
            $$_INSTALLER_CMD)
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
