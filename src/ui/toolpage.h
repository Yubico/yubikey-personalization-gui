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

#ifndef TOOLPAGE_H
#define TOOLPAGE_H

#include <QStackedWidget>
#include <QLineEdit>

namespace Ui {
    class ToolPage;
}

class ToolPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit ToolPage(QWidget *parent = 0);
    ~ToolPage();
    static void setImportFilename(QString filename);
    static QString defaultImportFilename(void);

    enum Page {
        Page_Base,
        Page_Converter,
        Page_ChalResp,
        Page_Ndef,
        Page_Zap,
        Page_Import,
    };

private:
    Ui::ToolPage *ui;

    int m_currentPage;
    QString m_serial;

    static QString m_filename;

    void setSerial(QLineEdit *line);

public slots:
    void loadSettings();
    void setCurrentPage(int pageIndex);

private slots:
    void connectPages();
    void connectHelpButtons();

    // Converter Page
    void resetConverterPage();
    void convert(int updatedIndex, QString txt);

    void on_converterHexTxt_editingFinished();
    void on_converterModhexTxt_editingFinished();
    void on_converterDecTxt_editingFinished();

    void copyToClipboard(const QString &str);
    void on_converterHexCopyBtn_clicked();
    void on_converterModhexCopyBtn_clicked();
    void on_converterDecCopyBtn_clicked();

    // Challenge Response Page
    void resetChalRespPage();
    void performChallengeResponse();
    void on_chalRespChallenge_editingFinished();

    // ndef page
    void resetNdefPage();
    void programNdef();
    void on_ndefTextRadio_toggled(bool checked);
    void ndefWritten(bool written, const QString &msg);
    void on_ndefAccCodeCheckbox_toggled(bool checked);
    void on_ndefUseSerial_toggled(bool checked);

    // zap page
    void on_zapPerformBtn_clicked();
    void on_zapAccCodeCheckbox_toggled(bool checked);
    void on_zapUseSerial_toggled(bool checked);
    void zapDone(bool written, const QString &msg);

    // import page
    void on_importPerformBtn_clicked();

    void keyFound(bool found, bool* featuresMatrix);
signals:
    void showStatusMessage(const QString &text, int status = 0);
    void reloadSettings();
    void switchPage(int page, int tab, int slot);
    void showHelp(int helpIndex);
};

#endif // TOOLPAGE_H
