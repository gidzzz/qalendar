QT += core gui maemo5

TARGET = qalendar
TEMPLATE = app

INCLUDEPATH += month week recurrence settings

SOURCES += main.cpp \
    MainWindow.cpp \
    \
    TemporalPlug.cpp \
    AgendaPlug.cpp \
    JournalsPlug.cpp \
    TodosPlug.cpp \
    \
    month/MonthPlug.cpp \
    month/WeekButton.cpp \
    month/MonthWidget.cpp \
    \
    week/WeekPlug.cpp \
    week/WeekdayButton.cpp \
    week/WeekArea.cpp \
    week/WeekHintWidget.cpp \
    week/WeekWidget.cpp \
    week/ComponentWidget.cpp \
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
    JournalEditDialog.cpp \
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
    JournalsPlug.h \
    \
    month/MonthPlug.h \
    month/WeekButton.h \
    month/MonthWidget.h \
    month/MonthLayoutWindow.h \
    \
    week/WeekPlug.h \
    week/WeekdayButton.h \
    week/WeekArea.h \
    week/WeekHintWidget.h \
    week/WeekHintProfile.h \
    week/WeekWidget.h \
    week/WeekProfile.h \
    week/WeekLayoutWindow.h \
    week/ComponentWidget.h \
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
    Metrics.h \
    Roles.h \
    Theme.h \
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
    JournalEditDialog.h \
    JournalDelegate.h \
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

TRANSLATIONS =

isEmpty(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
lrelease.input = TRANSLATIONS
lrelease.output = ${QMAKE_FILE_BASE}.qm
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

isEmpty(PREFIX) {
    PREFIX = /usr
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
PKGDATADIR = $$DATADIR/$$TARGET

DEFINES += PKGDATADIR=\\\"$$PKGDATADIR\\\"

INSTALLS += target desktop translations

target.path = $$BINDIR

desktop.path = $$DATADIR/applications/hildon
desktop.files += ../extra/$${TARGET}.desktop

for(TSFILE, TRANSLATIONS) {
    QMFILE = $$replace(TSFILE, ".ts", ".qm")
    translations.files += $$OUT_PWD/$$basename(QMFILE)
}
translations.CONFIG += no_check_exist
translations.path = $$PKGDATADIR/translations

CONFIG += link_pkgconfig
PKGCONFIG += calendar-backend
