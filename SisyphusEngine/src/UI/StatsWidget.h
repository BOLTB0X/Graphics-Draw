#pragma once
#include "IWidget.h"


class Timer;
class Fps;


class StatsWidget : public IWidget {
public:
    StatsWidget(const std::string& , Timer*, Fps*);
    StatsWidget(const StatsWidget& other) = delete;
    
    virtual ~StatsWidget() = default;
    virtual void Frame() override;

private:
    Timer* m_Timer;
    Fps* m_Fps;
}; // StatsWidget