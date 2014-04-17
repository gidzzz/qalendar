#ifndef WEEKHINTPROFILE_H
#define WEEKHINTPROFILE_H

#include <deque>
#include <vector>
#include <algorithm>
#include <limits>

#include "ComponentWidget.h"

class WeekHintProfile
{
public:
    struct Hint
    {
        int left;
        int right;
        int color;

        Hint(int left, int right, int color) : left(left), right(right), color(color) { }
    };

    class Frame
    {
    public:
        int y;
        std::vector<Hint> hints;

        void add(int left, int right, int color)
        {
            for (unsigned int i = 0; i < hints.size(); i++) {
                // A disjoint hint on the left
                if (hints[i].right < left)
                    continue;

                // A disjoint hint on the right
                if (right < hints[i].left)
                    continue;

                // Completely covered a hint
                if (left <= hints[i].left && hints[i].right <= right) {
                    // Remove the covered hint
                    hints.erase(hints.begin() + i--);
                    continue;
                }

                // In the middle of a hint
                if (hints[i].left < left && right < hints[i].right) {
                    if (hints[i].color != color) {
                        // Separate the right part
                        hints.push_back(Hint(right+1, hints[i].right, hints[i].color));
                        // Adjust the left part
                        hints[i].right = left-1;
                        // Insert the new hint in the middle
                        hints.push_back(Hint(left, right, color));
                    }
                    return;
                }

                // Overlapping left part of a hint
                if (left <= hints[i].left && right < hints[i].right) {
                    if (hints[i].color == color) {
                        // Inherit the right edge
                        right = hints[i].right;
                        // Remove the overlapped hint
                        hints.erase(hints.begin() + i--);
                    } else {
                        hints[i].left = right+1;
                    }
                    continue;
                }

                // Overlapping right part of a hint
                if (hints[i].left < left && hints[i].right <= right) {
                    if (hints[i].color == color) {
                        // Inherit the left edge
                        left = hints[i].left;
                        // Remove the overlapped hint
                        hints.erase(hints.begin() + i--);
                    } else {
                        hints[i].right = left-1;
                    }
                    continue;
                }
            }

            hints.push_back(Hint(left, right, color));
        }
    };

    std::deque<Frame> upView; // Frames for the top edge
    std::deque<Frame> dnView; // Frames for the bottom edge

    // Remove all frames
    void clear()
    {
        upView.clear();
        dnView.clear();
    }

    static bool widgetDnComparator(ComponentWidget *w1, ComponentWidget *w2)
    {
        return w1->frameGeometry().top() < w2->frameGeometry().top();
    }

    static bool widgetUpComparator(ComponentWidget *w1, ComponentWidget *w2)
    {
        return w1->frameGeometry().bottom() > w2->frameGeometry().bottom();
    }

    void populate(std::vector<ComponentWidget*> &widgets)
    {
        Frame frame;
        frame.y = std::numeric_limits<int>::max();

        // Sort the widgets ascending by y coordinate of the top edge, as if we
        // were looking at them from above.
        std::sort(widgets.begin(), widgets.end(), widgetDnComparator);

        // Add a hint for each widget
        for (int w = widgets.size()-1; w >= 0; w--) {
            // Extract geometry info
            const QRect geometry = widgets[w]->frameGeometry();
            const int top   = geometry.top();
            const int left  = geometry.left() + 4;
            const int right = geometry.left() + geometry.width() - 4;

            // Check if a new frame should be started
            if (frame.y != top) {
                dnView.push_front(frame);
                frame.y = top;
            }

            // Add a hint of the widget to the frame
            frame.add(left, right, widgets[w]->color);
        }

        // Add the final frame
        dnView.push_front(frame);

        // Sort the widgets descending by y coordinate of the bottom edge, as if
        // we were looking at them from below.
        std::sort(widgets.begin(), widgets.end(), widgetUpComparator);

        frame.hints.clear();
        frame.y = std::numeric_limits<int>::min();

        // Add a hint for each widget
        for (int w = widgets.size()-1; w >= 0; w--) {
            // Extract geometry info
            const QRect geometry = widgets[w]->frameGeometry();
            const int bottom = geometry.bottom();
            const int left   = geometry.left() + 4;
            const int right  = geometry.right() - 4;


            // Check if a new frame should be started
            if (frame.y != bottom) {
                upView.push_front(frame);
                frame.y = bottom;
            }

            // Add a hint of the widget to the frame
            frame.add(left, right, widgets[w]->color);
        }

        // Add the final frame
        upView.push_front(frame);
    }
};

#endif // WEEKHINTPROFILE_H
