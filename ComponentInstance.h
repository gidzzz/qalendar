#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H

#include <QMetaType>

#include <CComponent.h>
#include <CEvent.h>
#include <CTodo.h>

struct ComponentInstance
{
    ComponentInstance(CComponent *component, time_t stamp) :
        component(component), stamp(stamp)
    {
    }

    union
    {
        CComponent *component;
        CEvent *event;
        CTodo *todo;
    };

    time_t stamp;

    time_t duration()
    {
        return component->getDateEnd() - component->getDateStart();
    }

    time_t end()
    {
        return stamp + duration();
    }
};

Q_DECLARE_METATYPE (ComponentInstance*)

#endif // COMPONENTINSTANCE_H
