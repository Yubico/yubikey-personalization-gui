/*
Copyright (C) 2011-2013 Yubico AB.  All rights reserved.

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

#ifndef HELPBOX_H
#define HELPBOX_H

#include <QDialog>

namespace Ui {
    class HelpBox;
}

class HelpBox : public QDialog {
    Q_OBJECT

public:
    explicit HelpBox(QWidget *parent = 0);
    ~HelpBox();

    enum Help {
        Help_ConfigurationSlot,
        Help_PublicID,
        Help_PrivateID,
        Help_SecretKey,
        Help_ParameterGeneration,
        Help_ConfigurationProtection,
        Help_OutputFormat,
        Help_OutputSpeed,
        Help_SrNoVisibility,
        Help_OathPublicID,
        Help_HotpLen,
        Help_HotpParam,
        Help_StaticScanCode,
        Help_ChalRespYubico,
        Help_ChalRespHmac,
        Help_ChalRespOption,
        Help_AllowUpdate,
        Help_Swap,
        Help_ManUpdate,
        Help_LogFormat,
    };

    void setHelpIndex(Help helpIndex);

private:
    Ui::HelpBox *ui;
};

#endif // HELPBOX_H
