// UserInterface/Widget/StatsWidget.h
#pragma once
// Framework
#include "Widget/IWidget.h"
// Application
#include "Fps/Fps.h"
#include "Timer/Timer.h"

class StatsWidget : public IWidget {
public:
    StatsWidget(Fps*, Timer*);
	StatsWidget(const StatsWidget&) = delete;
	virtual ~StatsWidget() = default;

    virtual void Frame() override;

private:
    Fps* m_fpsPtr;
    Timer* m_timerPtr;
}; // StatsWidget