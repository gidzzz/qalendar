#ifndef WEEKPROFILE_H
#define WEEKPROFILE_H

#include <vector>

#include "ComponentInstance.h"

namespace Metrics
{
    namespace WeekProfile
    {
        const int FrameSize = 4;
    }
}

class WeekProfile
{
public:
    // Structure representing a single change in the profile
    struct ChangePoint
    {
        ComponentInstance *component;
        int stamp;
        int change; // +1 for start, -1 for end

        ChangePoint(ComponentInstance* component, int change) :
            component(component), change(change)
        {
            stamp = change == 1 ? component->stamp : component->end();
        }
    };

    // Structure representing state of the profile at a specific moment
    struct Frame
    {
        ComponentInstance *components[Metrics::WeekProfile::FrameSize];
        bool initial[Metrics::WeekProfile::FrameSize];

        int stamp;

        Frame(int stamp) :
            stamp(stamp)
        {
            using namespace Metrics::WeekProfile;

            for (int i = 0; i < FrameSize; i++) {
                components[i] = NULL;
                initial[i] = false;
            }
        }

        Frame(const Frame &other, int stamp) :
            stamp(stamp)
        {
            using namespace Metrics::WeekProfile;

            for (int i = 0; i < FrameSize; i++) {
                components[i] = other.components[i];
                initial[i] = false;
            }
        }

        // Take a change point into account
        int process(ChangePoint point)
        {
            using namespace Metrics::WeekProfile;

            if (point.change > 0) {
                // Add
                for (int i = 0; i < FrameSize; i++) {
                    if (components[i] == NULL) {
                        components[i] = point.component;
                        initial[i] = true;
                        return i;
                    }
                }
            } else {
                // Remove
                for (int i = 0; i < FrameSize; i++) {
                    if (components[i] == point.component) {
                        components[i] = NULL;
                        return i;
                    }
                }
            }
            return -1;
        }

        int indent()
        {
            using namespace Metrics::WeekProfile;

            for (int i = 0; i < FrameSize; i++)
                if (components[i])
                    return i;

            return FrameSize;
        }

        int width()
        {
            using namespace Metrics::WeekProfile;

            for (int i = FrameSize; i > 1; i--)
                if (components[i-1])
                    return i;

            return 0;
        }
    };

    std::vector<Frame> frames;

    // Try to add a component to the profile
    void add(ComponentInstance *event)
    {
        ChangePoint startPoint(event,  1);
        ChangePoint endPoint(event, -1);

        unsigned int i = 0;
        int slot = -1;

        // Insert start point
        for (; i < frames.size(); i++) {
            if (frames[i].stamp == startPoint.stamp) {
                // Modify the affected frame
                slot = frames[i].process(startPoint);
                if (slot < 0) {
                    // Insertion failed
                    return;
                } else {
                    break;
                }
            } else if (frames[i].stamp > startPoint.stamp) {
                // Insert a new frame at i
                Frame frame = Frame(frames[(int)i-1], startPoint.stamp);
                slot = frame.process(startPoint);
                if (slot < 0) {
                    // Insertion failed
                    return;
                } else {
                    frames.insert(frames.begin()+i, frame);
                    break;
                }
            }
        }
        if (i == frames.size()) {
            Frame frame(startPoint.stamp);
            slot = frame.process(startPoint);
            if (slot < 0) {
                // Insertion failed
                return;
            }
            frames.push_back(frame);
        }

        // Insert end point
        for (; i < frames.size(); i++) {
            if (frames[i].stamp == endPoint.stamp) {
                // End frame already exists
                break;
            } else if (frames[i].stamp > endPoint.stamp) {
                // Insert a new frame at i
                Frame frame = Frame(frames[(int)i-1], endPoint.stamp);
                frame.components[slot] = NULL;
                frames.insert(frames.begin()+i, frame);
                break;
            } else {
                // Update passed frames
                frames[i].components[slot] = startPoint.component;
            }
        }
        if (i == frames.size()) {
            Frame frame(endPoint.stamp);
            slot = frame.process(endPoint);
            frames.push_back(frame);
        }
    }
};

#endif // WEEKPROFILE_H
