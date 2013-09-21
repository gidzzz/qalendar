#ifndef RULEPICKDIALOG_H
#define RULEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_RulePickDialog.h"

#include <vector>
#include <string>

class RulePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    RulePickDialog(QWidget *parent, std::vector<std::string> &rules, unsigned int &currentRule);
    ~RulePickDialog();

signals:
    void rulesUpdated();

public slots:
    void accept();

private:
    Ui::RulePickDialog *ui;

    std::vector<std::string> &rules;
    unsigned int &currentRule;

    void resizeEvent(QResizeEvent *e);

private slots:
    void newRule();
    void deleteRule();
};

#endif // RULEPICKDIALOG_H
