#include "counterframe.h"

CounterFrame::CounterFrame()
{

}

CounterFrame::~CounterFrame()
{
    clear();
}

void CounterFrame::clear()
{
    detections.clear();
}
