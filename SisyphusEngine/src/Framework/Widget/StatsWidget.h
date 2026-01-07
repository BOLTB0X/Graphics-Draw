// Framework/Widget/StatsWidget.h
#pragma once
#include "IWidget.h"

#include "Base/Fps.h"
#include "Base/Timer.h"

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