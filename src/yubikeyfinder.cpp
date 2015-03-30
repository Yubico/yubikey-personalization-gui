/*
Copyright (C) 2011-2014 Yubico AB.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "yubikeyfinder.h"

#include <ykcore.h>
#include <ykdef.h>

#include <QApplication>
#include <QDebug>

YubiKeyFinder* YubiKeyFinder::_instance = 0;

// first version is inclusive, second is exclusive
const unsigned int YubiKeyFinder::FEATURE_MATRIX[][2] = {
    { YK_VERSION(2,0,0), 0 },   //Feature_MultipleConfigurations
    { YK_VERSION(2,0,0), 0 },   //Feature_ProtectConfiguration2
    { YK_VERSION(1,3,0), 0 },   //Feature_StaticPassword
    { YK_VERSION(2,0,0), 0 },   //Feature_ScanCodeMode
    { YK_VERSION(2,0,0), 0 },   //Feature_ShortTicket
    { YK_VERSION(2,0,0), 0 },   //Feature_StrongPwd
    { YK_VERSION(2,1,0), 0 },   //Feature_OathHotp
    { YK_VERSION(2,2,0), 0 },   //Feature_ChallengeResponse
    { YK_VERSION(2,1,4), 0 },   //Feature_SerialNumber
    { YK_VERSION(2,1,7), 0 },   //Feature_MovingFactor
    { YK_VERSION(2,3,0), 0 },   //Feature_ChallengeResponseFixed
    { YK_VERSION(2,3,0), 0 },   //Feature_Updatable
    { YK_VERSION(2,1,4), 0 },   //Feature_Ndef
    { YK_VERSION(2,4,0), 0 },   //Feature_LedInvert
    { YK_VERSION(3,3,0), 0 },   //Feature_U2F
};

// when a featureset should be excluded from versions (NEO, I'm looking at you.)
const unsigned int YubiKeyFinder::FEATURE_MATRIX_EXCLUDE[][4] = {
    { YK_VERSION(2,1,4), YK_VERSION(2,2,0), 0, 0 }, //Feature_MultipleConfigurations
    { YK_VERSION(2,1,4), YK_VERSION(2,2,0), 0, 0 }, //Feature_ProtectConfiguration2
    { YK_VERSION(2,1,4), YK_VERSION(2,1,8), 0, 0 }, //Feature_StaticPassword
    { YK_VERSION(2,1,4), YK_VERSION(2,1,8), 0, 0 }, //Feature_ScanCodeMode
    { 0, 0, 0, 0 },                                 //Feature_ShortTicket
    { YK_VERSION(2,1,4), YK_VERSION(2,1,8), 0, 0 }, //Feature_StrongPwd
    { 0, 0, 0, 0 },                                 //Feature_OathHotp
    { 0, 0, 0, 0 },                                 //Feature_ChallengeResponse
    { 0, 0, 0, 0 },                                 //Feature_SerialNumber
    { 0, 0, 0, 0 },                                 //Feature_MovingFactor
    { 0, 0, 0, 0 },                                 //Feature_ChallengeResponseFixed
    { 0, 0, 0, 0 },                                 //Feature_Updatable
    { YK_VERSION(2,2,0), YK_VERSION(3,0,0), YK_VERSION(4,0,0), YK_VERSION(4,9,0) }, //Feature_Ndef
    { YK_VERSION(3,0,0), YK_VERSION(3,1,0), 0, 0 }, //Feature_LedInvert
    { 0, 0, 0, 0 },                                 //Feature_U2F
};

YubiKeyFinder::YubiKeyFinder() {
    // init the ykpers library
    yk_init();

    //Initialize fields
    init();

    //Create timer
    m_timer = new QTimer( this );
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(findKey()));
}

YubiKeyFinder::~YubiKeyFinder() {
    yk_release();

    if(m_timer != 0) {
        delete m_timer;
        m_timer = 0;
    }

    if(_instance) {
        delete _instance;
    }

    if(m_ykds) {
      ykds_free(m_ykds);
    }
}

YubiKeyFinder* YubiKeyFinder::getInstance() {
    if(_instance == NULL) {
        _instance = new YubiKeyFinder();
    }
    return _instance;
}

QString YubiKeyFinder::versionStr() {
    if(m_version > 0) {
        return tr("%1.%2.%3").
                arg(m_versionMajor).
                arg(m_versionMinor).
                arg(m_versionBuild);
    }
    return "";
}

bool YubiKeyFinder::checkFeatureSupport(Feature feature) {
    if(m_version > 0 &&
       (unsigned int) feature < sizeof(FEATURE_MATRIX)/sizeof(FEATURE_MATRIX[0])) {
        bool supported = (
                m_version >= FEATURE_MATRIX[feature][0] &&
                (FEATURE_MATRIX[feature][1] == 0 || m_version < FEATURE_MATRIX[feature][1])
                );
        if(supported) {
            if(FEATURE_MATRIX_EXCLUDE[feature][0] != 0)
                if(m_version >= FEATURE_MATRIX_EXCLUDE[feature][0])
                    if(m_version < FEATURE_MATRIX_EXCLUDE[feature][1])
                        return false;
            if(FEATURE_MATRIX_EXCLUDE[feature][2] != 0)
                if(m_version >= FEATURE_MATRIX_EXCLUDE[feature][2])
                    if(m_version < FEATURE_MATRIX_EXCLUDE[feature][3])
                        return false;
        }

        return supported;
    }
    return false;
}

void YubiKeyFinder::init() {
    m_state = State_Absent;
    m_yk = 0;
    m_version = 0;
    m_versionMajor = 0;
    m_versionMinor = 0;
    m_versionBuild = 0;
    m_serial = 0;
    m_touchLevel = 0;

    m_ykds = ykds_alloc();
}

void YubiKeyFinder::start() {
    //Start timer
    init();
    if(m_timer && !m_timer->isActive()) {
        m_timer->start(TIMEOUT_FINDER);
    }
}

void YubiKeyFinder::stop() {
    //Stop timer
    if(m_timer && m_timer->isActive()) {
        m_timer->stop();
        // doing closeKey() here might look out of place and may cause findKey()
        // to fail unexpectedly, but it's needed to not leak file descriptors
        // when writing the key.
        closeKey();
    }
}

bool YubiKeyFinder::openKey() {
    bool flag = true;
    if (!(m_yk = yk_open_first_key())) {
        flag = false;
    }

    return flag;
}

bool YubiKeyFinder::closeKey() {
    bool flag = true;
    if(m_yk != 0) {
        if (!yk_close_key(m_yk)) {
            flag = false;
        }
        m_yk = 0;
    }

    return flag;
}

void YubiKeyFinder::findKey() {
    if(QApplication::activeWindow() == 0) {
        //No focus, avoid locking the YubiKey.
        m_state = State_NoFocus;
        return;
    }

    bool error = false;

    //qDebug() << "-------------------------";
    //qDebug() << "Starting key search";
    //qDebug() << "-------------------------";

    try {
        if(m_yk == 0 && !openKey()) {
            throw 0;
        }

        if (!yk_get_status(m_yk, m_ykds)) {
            throw 0;
        }

        //qDebug() << "Key found";

        //Check pervious state
        if(m_state == State_Absent || m_state == State_NoFocus) {

            m_state = State_Present;

            //Get version
            m_versionMajor = ykds_version_major(m_ykds);
            m_versionMinor = ykds_version_minor(m_ykds);
            m_versionBuild = ykds_version_build(m_ykds);
            m_version = YK_VERSION(m_versionMajor,
                                   m_versionMinor,
                                   m_versionBuild);

            m_touchLevel = ykds_touch_level(m_ykds);

            //Get serial number
            if(checkFeatureSupport(Feature_SerialNumber)) {
                if (!yk_get_serial(m_yk, 0, 0, &m_serial)) {
                    qDebug() << "Failed to read serial number (serial-api-visible disabled?).";
                } else {
                    qDebug() << "Serial number: " << m_serial;
                }
            }

            //Get supported features
            size_t featuresCount = sizeof(FEATURE_MATRIX)/sizeof(FEATURE_MATRIX[0]);
            bool featuresMatrix[featuresCount];
            for(size_t i = 0; i < featuresCount; i++) {
                featuresMatrix[i] = checkFeatureSupport((Feature)i);
            }

            int error = ERR_NOERROR;
            if(!yk_check_firmware_version2(m_ykds)) {
                error = ERR_UNKNOWN_FIRMWARE;
            }

            emit keyFound(true, featuresMatrix, error);
            emit diagnostics(QString("Found key with version %1, serial %2 and touch %3.")
                .arg(versionStr(), QString::number(m_serial), QString::number(m_touchLevel)));
        }
    }
    catch(...) {
        error = true;
    }

    closeKey();

    if(error) {
        init();
        m_state = State_Absent;
        int error = ERR_OTHER;
        if(yk_errno == YK_EMORETHANONE) {
            error = ERR_MORETHANONE;
        } else if(yk_errno == YK_ENOKEY) {
            error = ERR_NOKEY;
        } else if(yk_errno == YK_EUSBERR) {
            emit diagnostics(QString("USB Error: %1").arg(yk_usb_strerror()));
        } else if(yk_errno) {
            emit diagnostics(yk_strerror(yk_errno));
        }
        yk_errno = 0;
        ykp_errno = 0;
        emit keyFound(false, NULL, error);
    }

    //qDebug() << "-------------------------";
    //qDebug() << "Stopping key search";
    //qDebug() << "-------------------------";
}
