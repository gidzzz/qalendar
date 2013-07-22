QT += core gui maemo5

TARGET = qalendar
TEMPLATE = app

INCLUDEPATH += recurrence settings

SOURCES += main.cpp \
    MainWindow.cpp \
    \
    TemporalPlug.cpp \
    AgendaPlug.cpp \
    MonthPlug.cpp \
    WeekPlug.cpp \
    JournalsPlug.cpp \
    TodosPlug.cpp \
    \
    recurrence/RecurrencePickSelector.cpp \
    recurrence/RecurrencePickDialog.cpp \
    recurrence/ByDayWidget.cpp \
    recurrence/ByDayCoreWidget.cpp \
    recurrence/ByWeekdayWidget.cpp \
    recurrence/ByWeekdayCoreWidget.cpp \
    recurrence/ByWeekdayColumnWidget.cpp \
    recurrence/ByMonthWidget.cpp \
    \
    settings/SettingsDialog.cpp \
    settings/CalendarsConfigSelector.cpp \
    settings/CalendarsConfigDialog.cpp \
    settings/CalendarEditDialog.cpp \
    \
    ChangeManager.cpp \
    DayWindow.cpp \
    TodoEditDialog.cpp \
    EventEditDialog.cpp \
    AlarmPickSelector.cpp \
    AlarmPickDialog.cpp \
    CalendarPickSelector.cpp \
    CalendarPickDialog.cpp \
    ColorPickSelector.cpp \
    ColorPickDialog.cpp \
    DatePickSelector.cpp \
    DatePickDialog.cpp \
    MonthWidget.cpp \
    JournalEditDialog.cpp \
    WeekButton.cpp \
    WeekdayButton.cpp \
    WeekArea.cpp \
    WeekWidget.cpp \
    WeekHintWidget.cpp \
    ComponentWidget.cpp \
    EventWindow.cpp \
    TodoWindow.cpp \
    ComponentListWidget.cpp \
    ExpandingTextEdit.cpp \
    Rotator.cpp

HEADERS += \
    MainWindow.h \
    \
    Plug.h \
    TemporalPlug.h \
    AgendaPlug.h \
    TodosPlug.h \
    MonthPlug.h \
    WeekPlug.h \
    JournalsPlug.h \
    \
    recurrence/RecurrencePickSelector.h \
    recurrence/RecurrencePickDialog.h \
    recurrence/ByDayWidget.h \
    recurrence/ByDayCoreWidget.h \
    recurrence/ByWeekdayWidget.h \
    recurrence/ByWeekdayCoreWidget.h \
    recurrence/ByWeekdayColumnWidget.h \
    recurrence/ByMonthWidget.h \
    recurrence/DayNumberDialog.h \
    recurrence/RecurrenceSectionLabel.h \
    \
    settings/SettingsDialog.h \
    settings/CalendarsConfigSelector.h \
    settings/CalendarsConfigDialog.h \
    settings/CalendarEditDialog.h \
    settings/DetailedCalendarDelegate.h \
    \
    Roles.h \
    \
    ChangeManager.h \
    ChangeClient.h \
    DayWindow.h \
    ColorCheckDelegate.h \
    TodoEditDialog.h \
    TodoDelegate.h \
    EventEditDialog.h \
    AlarmPickSelector.h \
    AlarmPickDialog.h \
    CalendarPickSelector.h \
    CalendarPickDialog.h \
    ColorPickSelector.h \
    ColorPickDialog.h \
    CalendarDelegate.h \
    DateDayDelegate.h \
    DatePickSelector.h \
    DatePickDialog.h \
    ComponentListDelegate.h \
    DateHeadingWidget.h \
    CWrapper.h \
    MonthWidget.h \
    JournalEditDialog.h \
    JournalDelegate.h \
    MonthLayoutWindow.h \
    WeekLayoutWindow.h \
    WeekProfile.h \
    WeekHintProfile.h \
    Theme.h \
    Metrics.h \
    WeekButton.h \
    WeekdayButton.h \
    WeekArea.h \
    WeekWidget.h \
    WeekHintWidget.h \
    ComponentWidget.h \
    EventWindow.h \
    TodoWindow.h \
    ComponentListWidget.h \
    ChainedSpinBox.h \
    ExpandingTextEdit.h \
    ComponentInstance.h \
    VerticalScrollArea.h \
    ExpandingListWidget.h \
    RotatingDialog.h \
    ComponentEditDialog.h \
    Rotator.h \
    Painter.h

FORMS += \
    AgendaPlug.ui \
    JournalsPlug.ui \
    TodosPlug.ui \
    \
    recurrence/RecurrencePickDialog.ui \
    \
    settings/SettingsDialog.ui \
    settings/CalendarsConfigDialog.ui \
    settings/CalendarEditDialog.ui \
    \
    DayWindow.ui \
    TodoEditDialog.ui \
    EventEditDialog.ui \
    JournalEditDialog.ui \
    AlarmPickDialog.ui \
    CalendarPickDialog.ui \
    ColorPickDialog.ui \
    DatePickDialog.ui \
    EventWindow.ui \
    TodoWindow.ui

isEmpty(PREFIX) {
    PREFIX = /usr
}

BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share

DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

INSTALLS += target desktop

target.path = $$BINDIR

desktop.path = $$DATADIR/applications/hildon
desktop.files += ../extra/$${TARGET}.desktop

CONFIG += link_pkgconfig
PKGCONFIG += calendar-backend
