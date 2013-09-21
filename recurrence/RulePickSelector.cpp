#include "RulePickSelector.h"

#include "RulePickDialog.h"

RulePickSelector::RulePickSelector(std::vector<std::string> &rules, unsigned int &currentRule, QObject *parent) :
    QMaemo5AbstractPickSelector(parent),
    rules(rules),
    currentRule(currentRule)
{
}

QString RulePickSelector::currentValueText() const
{
    return tr("Rule %1 of %2")
           .arg(currentRule+1)
           .arg(rules.size());
}

QWidget* RulePickSelector::widget(QWidget *parent)
{
    // Send a notification so that the recurrence dialog can export the state of
    // its controls to the current rule before it is shown in the rule picker.
    emit creatingWidget();

    RulePickDialog *dialog = new RulePickDialog(parent, rules, currentRule);

    connect(dialog, SIGNAL(rulesUpdated()), this, SLOT(onRulesUpdated()));

    return dialog;
}

void RulePickSelector::onRulesUpdated()
{
    // Use a separate signal for this notification, because selected() would
    // arrive to the parent button first instead of the recurrence dialog
    // which might need to do some processing before the button can be updated.
    emit rulesUpdated();

    emit selected(currentValueText());
}
