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
    ComponentInstance *components[Metrics::MonthLayoutWindow::WindowSize][Metrics::MonthLayoutWindow::WindowColumns];

    // Create an empty window
    MonthLayoutWindow()
    {
        using namespace Metrics::MonthLayoutWindow;

        for (int i = 0; i < WindowSize; i++)
            for (int j = 0; j < WindowColumns; j++)
                components[i][j] = NULL;
    }

    // Try to add a component to the window
    void add(ComponentInstance *component)
    {
        using namespace Metrics::MonthLayoutWindow;

        if (QDateTime::fromTime_t(component->stamp).date() != QDateTime::fromTime_t(component->end()).date()) {
            // Multi-day event, fits only in a whole row in a multi-day slot
            for (int i = 0; i < MultiDayWindoSize; i++) {
                if (!components[i][0] && !components[i][1]) {
                    components[i][0] = components[i][1] = component;
                    return;
                }
            }
        } else if (component->component->getAllDay()) {
            // All-day event, fits only in a whole row
            for (int i = 0; i < WindowSize; i++) {
                if (!components[i][0] && !components[i][1]) {
                    components[i][0] = components[i][1] = component;
                    return;
                }
            }
        } else {
            // Minor event, fits everywhere
            for (int j = 0; j < WindowColumns; j++) {
                for (int i = 0; i < WindowSize; i++) {
                    if (!components[i][j]) {
                        components[i][j] = component;
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

        for (int i = 0; i < WindowSize; i++)
            for (int j = 0; j < WindowColumns; j++)
                if (components[i][j] && (components[i][j]->end() < start))
                    components[i][j] = NULL;
    }
};

#endif // MONTHRENDERWINDOW_H
