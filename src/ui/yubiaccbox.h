/*
Copyright (C) 2013-2014 Yubico AB.  All rights reserved.

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

#ifndef YUBIACCBOX_H
#define YUBIACCBOX_H

#include <QWidget>
#include <QLineEdit>

#include "common.h"

namespace Ui {
class YubiAccBox;
}

class YubiAccBox : public QWidget
{
    Q_OBJECT

public:
    explicit YubiAccBox(QWidget *parent = 0);
    ~YubiAccBox();
    void reset();
    bool checkConfirm();
    QString newAccessCode();
    QString currentAccessCode();
    AccMode newAccMode();

private:
    Ui::YubiAccBox *ui;
    QString m_serial;
    void setSerial(QLineEdit* line);

private slots:
    void keyFound(bool found, bool* featuresMatrix);
    void on_currentUseSerial_clicked(bool checked);
    void on_newUseSerial_clicked(bool checked);
    void on_configProtectionCombo_currentIndexChanged(int index);
    void on_currentAccessCodeTxt_editingFinished();
    void on_newAccessCodeTxt_editingFinished();
signals:
    void showHelp(int helpIndex);
};

#endif // YUBIACCBOX_H
