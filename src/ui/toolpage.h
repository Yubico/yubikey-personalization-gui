/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef TOOLPAGE_H
#define TOOLPAGE_H

#include <QStackedWidget>
#include "yubikeyconfig.h"

namespace Ui {
    class ToolPage;
}

class ToolPage : public QStackedWidget {
    Q_OBJECT

public:
    explicit ToolPage(QWidget *parent = 0);
    ~ToolPage();

private:
    Ui::ToolPage *ui;

    enum Page {
        Page_Base,
        Page_Converter,
    };
    int m_currentPage;

private slots:
    void connectPages();
    void connectHelpButtons();
    void setCurrentPage(int pageIndex);
    void helpBtn_pressed(int helpIndex);

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

signals:
    void showStatusMessage(const QString &text, int status = 0);
};

#endif // TOOLPAGE_H
