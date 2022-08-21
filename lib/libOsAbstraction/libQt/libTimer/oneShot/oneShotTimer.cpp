

// includes
#include "oneShotTimer.h"

namespace libOsAbstraction {
namespace libQt {
namespace libTimer {
namespace oneShot {

//ctor
oneShotTimer::oneShotTimer()
             :timer_e(std::make_unique<QTimer>())
{
    // make timer one shot
    timer_e->setSingleShot(true);
    // Connect the function to be called during timeout
    connect(timer_e.get(), &QTimer::timeout, this, &oneShotTimer::Timeout);
}
//dtor
oneShotTimer::~oneShotTimer()
{

}

void oneShotTimer::RegisterEventHandler(TimerHandler timerHandler)
{
    timerHandler_e = timerHandler;
}

// start the timer
void oneShotTimer::Start(int msec)
{
    timer_e->start(msec);
}

// stop the timer
void oneShotTimer::Stop()
{
    timer_e->stop();
}

bool oneShotTimer::IsActive()
{
    return(timer_e->isActive());
}

// function called when time elapses
void oneShotTimer::Timeout()
{
    timerHandler_e();
}


} // oneShot
} // libTimer
} // libQt
} // libOsAbstraction
