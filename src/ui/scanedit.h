/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef SCANEDIT_H
#define SCANEDIT_H

#include <QLineEdit>

class ScanEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ScanEdit(QWidget *parent = 0);

    void injectText(const QString &txt);
    int len() const;

    void injectScanCode(unsigned char code);
    void clearScanCodeText();
    QString scanCodeText() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);

private:
    QString m_scanCodeText;
signals:

public slots:

};

#endif // SCANEDIT_H
