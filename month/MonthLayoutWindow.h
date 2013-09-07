#ifndef MONTHRENDERWINDOW_H
#define MONTHRENDERWINDOW_H

#define MDAY_WINDOW_SIZE 4
#define WINDOW_SIZE 9
#define WINDOW_COLUMNS 2

namespace Metrics
{
    namespace MonthLayoutWindow
    {
        const int MultiDayWindoSize = 4;
        const int WindowSize        = 9;
        const int WindowColumns     = 2;
    }
}

class MonthLayoutWindow
{
public:
    ComponentInstance *instances[Metrics::MonthLayoutWindow::WindowSize][Metrics::MonthLayoutWindow::WindowColumns];

    // Create an empty window
    MonthLayoutWindow()
    {
        using namespace Metrics::MonthLayoutWindow;

        for (int i = 0; i < WindowSize; i++)
            for (int j = 0; j < WindowColumns; j++)
                instances[i][j] = NULL;
    }

    // Try to add a component to the window
    void add(ComponentInstance *instance)
    {
        using namespace Metrics::MonthLayoutWindow;

        if (QDateTime::fromTime_t(instance->stamp).date() < QDateTime::fromTime_t(instance->end()).addSecs(-1).date()) {
            // Multi-day event, fits only in a whole row in a multi-day slot
            for (int i = 0; i < MultiDayWindoSize; i++) {
                if (!instances[i][0] && !instances[i][1]) {
                    instances[i][0] = instances[i][1] = instance;
                    return;
                }
            }
        } else if (instance->component->getAllDay()) {
            // All-day event, fits only in a whole row
            for (int i = 0; i < WindowSize; i++) {
                if (!instances[i][0] && !instances[i][1]) {
                    instances[i][0] = instances[i][1] = instance;
                    return;
                }
            }
        } else {
            // Minor event, fits everywhere
            for (int j = 0; j < WindowColumns; j++) {
                for (int i = 0; i < WindowSize; i++) {
                    if (!instances[i][j]) {
                        instances[i][j] = instance;
                        return;
                    }
                }
            }
        }
    }

    // Discard events which do not occur on the specified date
    void cleanup(QDate date)
    {
        using namespace Metrics::MonthLayoutWindow;

        time_t start = QDateTime(date).toTime_t();

        for (int i = 0; i < WindowSize; i++) {
            for (int j = 0; j < WindowColumns; j++) {
                if (instances[i][j]) {
                    if (instances[i][j]->end() < start
                    ||  instances[i][j]->end() == start && instances[i][j]->duration() > 0)
                    {
                        instances[i][j] = NULL;
                    }
                }
            }
        }
    }
};

#endif // MONTHRENDERWINDOW_H
