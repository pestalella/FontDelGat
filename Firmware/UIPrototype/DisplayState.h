#pragma once

#include <LiquidCrystal_I2C.h>

enum class UIState {
    IndependentStatus,
    ParallelStatus,
    SeriesStatus,
    ChASettings,
    ChBSettings,
    ParallelSettings,
    SeriesSettings
};

enum class ChannelState {
    Disabled,
    Enabled
};

enum class OutputMode {
    Independent,
    Parallel,
    Series
};

class DisplayState
{
public:
    DisplayState();

    void begin();
    void menuPressed();
    void chAEnablePressed();
    void chBEnablePressed();
    void updateDisplay();
    void independentOutputPressed();
    void parallelOutputPressed();
    void seriesOutputPressed();

private:
    void showIndependentStatus();
    void showParallelStatus();
    void showSeriesStatus();
    void showChASettings();
    void showChBSettings();
    void showParallelSettings();
    void showSeriesSettings();

private:
    UIState state;
    OutputMode outputMode;
    LiquidCrystal_I2C lcd;
    ChannelState chAState;
    ChannelState chBState;
};
