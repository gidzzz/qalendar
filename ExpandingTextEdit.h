#ifndef EXPANDINGTEXTEDIT_H
#define EXPANDINGTEXTEDIT_H

#include <QTextEdit>

class ExpandingTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    ExpandingTextEdit(QWidget *parent);

    QString toPlainText();

    void setPlainText(QString text);
    void setPlaceholderText(QString text);

private:
    QString placeholderText;

    bool inhibitAutoscroll;
    bool placeholderActive;

    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent *e);
    void inputMethodEvent(QInputMethodEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

private slots:
    void adjust();
};

#endif // EXPANDINGTEXTEDIT_H
