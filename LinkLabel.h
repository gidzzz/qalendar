#ifndef LINKLABEL_H
#define LINKLABEL_H

#include <QLabel>

class LinkLabel : public QLabel
{
    Q_OBJECT

public:
    LinkLabel(QWidget *parent);

    void setText(QString text);
    void enableLinks(bool enable);

private:
    QString originalText;
    bool linksEnabled;

private slots:
    void onLinkActivated(QString link);
};

#endif // LINKLABEL_H
