#ifndef RULEPICKSELECTOR_H
#define RULEPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

#include <vector>
#include <string>

class RulePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    RulePickSelector(std::vector<std::string> &rules, unsigned int &currentRule, QObject *parent = 0);

    QString currentValueText() const;

    QWidget* widget(QWidget *parent);

signals:
    void creatingWidget();
    void rulesUpdated();

private:
    std::vector<std::string> &rules;
    unsigned int &currentRule;

private slots:
    void onRulesUpdated();
};

#endif // RULEPICKSELECTOR_H
