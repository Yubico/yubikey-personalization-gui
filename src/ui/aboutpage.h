/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QWidget>

namespace Ui {
    class AboutPage;
}

class AboutPage : public QWidget {
    Q_OBJECT

public:
    explicit AboutPage(QWidget *parent = 0);
    ~AboutPage();

private:
    Ui::AboutPage *ui;

private slots:
    void connectButtons();
    void keyFound(bool found, bool* featuresMatrix);
    void btn_pressed(int pageIndex);
    void on_supportBtn_clicked() ;

signals:
    void changePage(int pageIndex);
};

#endif // ABOUTPAGE_H
