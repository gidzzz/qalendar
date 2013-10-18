#include "LinkLabel.h"

#include <QRegExp>
#include <QDBusInterface>

// Patterns borrowed from libcontentaction
#define PHONE_PATTERN "(callto:|sms:|tel:)?(\\+ ?)?(\\(\\d+\\) ?|\\d[-.pwxPWX# ]?){2,19}\\d(?!\\d)"
#define EMAIL_PATTERN "(mailto:)?[\\w!#$%&amp;'*+/=?^`{|}~-]+(\\.[\\w!#$%&amp;'*+/=?^`{|}~-]+)*@([\\w-]+\\.)+[\\w-]+"
#define LINK_PATTERN  "(https?://|www\\.)(\\w+((:\\w+)?)@)?([\\w\\-]+\\.)+[\\w\\-]+(:\\d+)?" \
                      "(/([\\w/?%:;@&amp;=+$,\\-.!~*']*[\\w/?%:;@&amp;=+$,\\-!~*']|\\([\\w/?%:;@&amp;=+$,\\-.!~*']*\\))*)?"

LinkLabel::LinkLabel(QWidget *parent) :
    QLabel(parent),
    linksEnabled(false)
{
    connect(this, SIGNAL(linkActivated(QString)), this, SLOT(onLinkActivated(QString)));
}

void LinkLabel::setText(QString text)
{
    originalText = text;

    // Disable HTML tags
    text.replace("<", "&lt;");

    if (linksEnabled) {
        // One expression to rule them all
        QRegExp pattern("(" LINK_PATTERN ")|(" EMAIL_PATTERN ")|(" PHONE_PATTERN ")");

        // Transform each matched string into a hyperlink
        int index = 0;
        while ((index = pattern.indexIn(text, index)) != -1) {
            QString link = QString("<a href=\"%1\">%1</a>").arg(text.mid(index, pattern.matchedLength()));
            text.replace(index, pattern.matchedLength(), link);
            index += link.length();
        }
    }

    QLabel::setText(text);
}

void LinkLabel::enableLinks(bool enable)
{
    linksEnabled = enable;

    setText(originalText);
}

void LinkLabel::onLinkActivated(QString link)
{
    // Compose an email
    if (QRegExp(EMAIL_PATTERN).exactMatch(link)) {
        // Without this prefix Modest will not fill the address field
        if (!link.startsWith("mailto:"))
            link.prepend("mailto:");

        QDBusInterface("com.nokia.modest",
                       "/com/nokia/modest",
                       "com.nokia.modest",
                       QDBusConnection::sessionBus())
        .call("MailTo", link);
    } else

    // Compose an SMS or prepare a call
    if (QRegExp(PHONE_PATTERN).exactMatch(link)) {
        if (link.startsWith("sms:")) {
            QDBusInterface("com.nokia.MessagingUI",
                           "/com/nokia/MessagingUI",
                           "com.nokia.MessagingUI",
                           QDBusConnection::sessionBus())
            .call("messaging_ui_interface_start_sms", link);
        } else {
            // Without a prefix the dialer will not appear
            if (!link.startsWith("tel:") && !link.startsWith("callto:"))
                link.prepend("tel:");

            QDBusInterface("com.nokia.CallUI",
                           "/com/nokia/CallUI",
                           "com.nokia.CallUI",
                           QDBusConnection::sessionBus())
            .call("CreateCall", link);
        }
    }

    // Open the web browser. Do not match with the pattern to allow manual links
    // with an arbitrary URL.
    else {
        QDBusInterface("com.nokia.osso_browser",
                       "/com/nokia/osso_browser",
                       "com.nokia.osso_browser",
                       QDBusConnection::sessionBus())
        .call("open_new_window", link);
    }
}
