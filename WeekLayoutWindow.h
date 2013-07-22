#ifndef WEEKRENDERWINDOW_H
#define WEEKRENDERWINDOW_H

class WeekLayoutWindow
{
public:
    std::vector<ComponentInstance*> instances;
    std::vector<bool> initial;

    // Add a component to the window
    void add(ComponentInstance *instance)
    {
        // Try to find an unoccupied slot
        for (unsigned int i = 0; i < instances.size(); i++) {
            if (!instances[i]) {
                instances[i] = instance;
                initial[i] = true;
                return;
            }
        }

        // There are no free slots, add a new one
        instances.push_back(instance);
        initial.push_back(true);
    }

    // Discard events which do not occur on the specified date, mark the rest as old
    void move(QDate date)
    {
        time_t start = QDateTime(date).toTime_t();

        int lastUsedSlot = -1;
        for (unsigned int i = 0; i < instances.size(); i++) {
            if (instances[i]) {
                if (instances[i]->end() < start) {
                    instances[i] = NULL;
                } else {
                    lastUsedSlot = i;
                }
                initial[i] = false;
            }
        }

        // Discard trailing empty slots
        instances.resize(lastUsedSlot+1);
        initial.resize(lastUsedSlot+1);
    }
};

#endif // WEEKRENDERWINDOW_H
