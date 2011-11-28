/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
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
    };

    void setHelpIndex(Help helpIndex);

private:
    Ui::HelpBox *ui;
};

#endif // HELPBOX_H
