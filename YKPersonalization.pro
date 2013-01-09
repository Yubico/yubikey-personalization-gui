#
# global definitions
#
VERSION         = "3.1.4"
APP_NAME        = $$quote(YubiKey Personalization Tool)

#
# common configuration
#
QT             += core gui
DEPLOYMENT_PLUGIN += qmng
TEMPLATE        = app
TARGET          = yubikey-personalization-gui

DEFINES        += VERSION=\\\"$${VERSION}\\\"

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

!debian:!fedora {
  HEADERS += \
      deps/libykpers/ykpers.h \
      deps/libykpers/ykpers-version.h \
      deps/libykpers/ykcore/yktsd.h \
      deps/libykpers/ykcore/ykstatus.h \
      deps/libykpers/ykcore/ykdef.h \
      deps/libykpers/ykcore/ykcore_backend.h \
      deps/libykpers/ykcore/ykcore.h \
      deps/libyubikey/yubikey.h

  INCLUDEPATH    += . src src/ui deps/libusb-1.0 deps/libykpers deps/libykpers/ykcore deps/libyubikey
}

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
        libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}libyubikey-0.dll \
        libs$${DIR_SEPARATOR}win32$${DIR_SEPARATOR}libykpers-1-1.dll

    isEmpty(TIMESTAMP_URL):TIMESTAMP_URL = 'http://timestamp.verisign.com/scripts/timstamp.dll'

    LIB_FILES_WIN = $${LIB_FILES}
    TARGET_DIR_WIN = $${DESTDIR}
    for(FILE, LIB_FILES_WIN) {
        QMAKE_POST_LINK +=$$quote($$QMAKE_COPY $${FILE} $${TARGET_DIR_WIN}$$escape_expand(\\n\\t))
    }
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
# Non-windows specific configuration
#
!win32:!debian:!fedora {
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

    LIBS += -lyubikey

    CONFIG += link_pkgconfig
    PKGCONFIG += ykpers-1

    QMAKE_CXXFLAGS += $$(CXXFLAGS)
    QMAKE_LFLAGS += $$(LDFLAGS)

  } else:fedora {
    message("Fedora build")

    LIBS += -lyubikey

    CONFIG += link_pkgconfig
    PKGCONFIG += ykpers-1

    QMAKE_CXXFLAGS += $$(CXXFLAGS)
    QMAKE_LFLAGS += $$(LDFLAGS)

  } else {
    message("Generic Linux build")

    HEADERS += deps/libusb-1.0/libusb.h
    SOURCES += deps/libykpers/ykcore/ykcore_libusb-1.0.c

    # The application dependencies
    LIBS += $$quote(-L./libs/lin) -lusb-1.0

    TARGET_LIN = $${APP_NAME}
    TARGET_LIN ~= s, ,\\ ,g

    # Copy dependencies and other resources
    LIB_FILES += \
        $$[QT_INSTALL_LIBS]/libQtGui.so.4 \
        $$[QT_INSTALL_LIBS]/libQtCore.so.4 \
        $$[QT_INSTALL_PLUGINS]/imageformats/libqmng.so \
        libs/lin/libusb-1.0.so.0 \
        resources/lin/$${TARGET_LIN}.sh

    for(FILE, LIB_FILES){
        QMAKE_POST_LINK += $$quote(cp $${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
    }

    # Create application tarball
    TARROOT = "$${TARGET_LIN}-linux-$${VERSION}"
    TARFILE = "$${TARGET_LIN}-linux-$${VERSION}.tgz"

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
        CONFIG += qt_framework
        _SDK = $$(OSX_SDK)
        !isEmpty (_SDK) {
            # FIXME: this is prone to breaking with version numbers
            INCLUDEPATH += $$(OSX_SDK)/usr/include/c++/4.2.1
        }
        QMAKE_CFLAGS_X86_64 -= -arch
        QMAKE_CFLAGS_X86_64 -= x86_64
        QMAKE_CXXFLAGS_X86_64 -= -arch
        QMAKE_CXXFLAGS_X86_64 -= x86_64
    } else {
        _QT_LIBDIR = $$QMAKE_LIBDIR_QT
        _QT_PLUGINDIR = $$[QT_INSTALL_PLUGINS]

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
        mv $$_FRAMEWORKDIR/QtGui.framework/Versions/4/Resources/qt_menu.nib $$_BASEDIR/Resources/qt_menu.nib && \
        rmdir $$_FRAMEWORKDIR/QtGui.framework/Versions/4/Resources && \
        mkdir -p $$_PLUGINDIR/imageformats && \
        cp -R $$_QT_PLUGINDIR/imageformats/libqmng.dylib $$_PLUGINDIR/imageformats)

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
        
    cross {
        build_installer {
            QMAKE_POST_LINK += $$quote( && mkdir -p $${DESTDIR}/temp/ && \
                cp -R $${DESTDIR}/$${TARGET_MAC}.app $${DESTDIR}/temp/ && \
                ln -s /Applications $${DESTDIR}/temp/Applications && \
                mkdir $${DESTDIR}/temp/.background/ && \
                cp resources/mac/yubico-logo.png $${DESTDIR}/temp/.background/background.png && \
                genisoimage -V "$$TARGET_MAC" -r -apple --hfs-bless "/$${TARGET_MAC}.app" -o $${DESTDIR}/ykpers-pre.dmg  $${DESTDIR}/temp && \
                rm -rf $${DESTDIR}/temp && \
                dmg dmg $${DESTDIR}/ykpers-pre.dmg $${DESTDIR}/$${TARGET_MAC}-$${VERSION}.dmg)
        }
    } else {
        build_installer {
            QMAKE_POST_LINK += $$quote( && codesign -s \'$$PACKAGE_SIGN_IDENTITY\' $${DESTDIR}/$${TARGET_MAC}.app && \
                rm -rf $${DESTDIR}/temp && \
                mkdir -p $${DESTDIR}/temp/ && \
                cp -R $${DESTDIR}/$${TARGET_MAC}.app $${DESTDIR}/temp/ && \
                pkgbuild --sign \'$$INSTALLER_SIGN_IDENTITY\' --version $${VERSION} --root $${DESTDIR}/temp/ --component-plist resources/mac/installer.plist --install-location '/Applications/' $${DESTDIR}/$${TARGET_MAC}-$${VERSION}.pkg)
        }

        # Create application dmg
        shutup = ">/dev/null 2>&1"

        isEmpty(MACDEPLOYQT):MACDEPLOYQT = macdeployqt
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
                IMAGEFILE = $${DESTDIR}/$${TARGET_MAC}-$${VERSION}.dmg

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
    QMAKE_CLEAN += -r $${DESTDIR}/*.app $${DESTDIR}/*.pkg $${DESTDIR}/*.dmg $${DESTDIR}/temp
} else {
    QMAKE_CLEAN += -r $${DESTDIR}/*
}
