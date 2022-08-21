#ifndef ONESHOTTIMER_H
#define ONESHOTTIMER_H


#include "oneShotTimer_Types.h"

namespace libOsAbstraction {
namespace libQt {
namespace libTimer {
namespace oneShot {



class oneShotTimer : public QObject
{
    public:
        //ctor
        oneShotTimer();
        //dtor
        virtual ~oneShotTimer();
        // Function to register event handler
        void RegisterEventHandler(TimerHandler timerHandler);
        // Start timer
        void Start(int msec);
        // Stop Timer
        void Stop();
        // Function to check whether timer is active or not
        bool IsActive();
    private:
        // timer
        std::unique_ptr<QTimer> timer_e;
        // handler
        TimerHandler timerHandler_e;
        // timeout funtion
        void Timeout();

};
} // oneShot
} // libTimer
} // libQt
} // libOsAbstraction


#endif // ONESHOTTIMER_H
