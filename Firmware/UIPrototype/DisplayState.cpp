#include "DisplayState.h"

DisplayState::DisplayState() :
    lcd(0x27, 20, 4),
    state(UIState::IndependentStatus),
    outputMode(OutputMode::Independent),
    chAState(ChannelState::Disabled),
    chBState(ChannelState::Disabled)
{

}

void DisplayState::begin()
{
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Front Panel test");
    lcd.setCursor(1,1);
    lcd.print("(c) Pau Estalella");
}

void DisplayState::updateDisplay()
{
    switch (state) {
        case UIState::IndependentStatus:
            showIndependentStatus();
            break;
        case UIState::ParallelStatus:
            showParallelStatus();
            break;
        case UIState::SeriesStatus:
            showSeriesStatus();
            break;
        case UIState::ChASettings:
            showChASettings();
            break;
        case UIState::ChBSettings:
            showChBSettings();
            break;
        case UIState::ParallelSettings:
            showParallelSettings();
            break;
        case UIState::SeriesSettings:
            showSeriesSettings();
            break;
    }
}

void DisplayState::menuPressed()
{
    switch (state) {
        case UIState::IndependentStatus:
            state = UIState::ChASettings;
            break;
        case UIState::ParallelStatus:
            state = UIState::ParallelSettings;
            break;
        case UIState::SeriesStatus:
            state = UIState::SeriesSettings;
            break;
        case UIState::ChASettings:
            state = UIState::ChBSettings;
            break;
        case UIState::ChBSettings:
            state = UIState::IndependentStatus;
            break;
        case UIState::ParallelSettings:
            state = UIState::ParallelStatus;
            break;
        case UIState::SeriesSettings:
            state = UIState::SeriesStatus;
            break;
    }
}

void DisplayState::chAEnablePressed()
{
    if (chAState == ChannelState::Disabled) {
        chAState = ChannelState::Enabled;
    } else {
        chAState = ChannelState::Disabled;
    }
}

void DisplayState::chBEnablePressed()
{
    if (chBState == ChannelState::Disabled) {
        chBState = ChannelState::Enabled;
    } else {
        chBState = ChannelState::Disabled;
    }
}

void DisplayState::independentOutputPressed()
{
    outputMode = OutputMode::Independent;
    state = UIState::IndependentStatus;
}

void DisplayState::parallelOutputPressed()
{
    outputMode = OutputMode::Parallel;
    state = UIState::ParallelStatus;
}

void DisplayState::seriesOutputPressed()
{
    outputMode = OutputMode::Series;
    state = UIState::SeriesStatus;
}

void DisplayState::showIndependentStatus()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ch A");
    lcd.setCursor(5,0);
    lcd.print(chAState == ChannelState::Enabled? "ON":"OFF");
    lcd.setCursor(10,0);
    lcd.print("Ch B");
    lcd.setCursor(15,0);
    lcd.print(chBState == ChannelState::Enabled? "ON":"OFF");
    // Channel A data
    lcd.setCursor(0,1);
    lcd.print("V: 12V");
    lcd.setCursor(0,2);
    lcd.print("I: 100 mA");
    // Channel B data
    lcd.setCursor(10,1);
    lcd.print("V: 12V");
    lcd.setCursor(10,2);
    lcd.print("I: 100 mA");
}

void DisplayState::showParallelStatus()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Parallel output");
    // Output data
    lcd.setCursor(0,1);
    lcd.print("V: 2 V");
    lcd.setCursor(0,2);
    lcd.print("I: 5 A");
}

void DisplayState::showSeriesStatus()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Series output");
    // Output data
    lcd.setCursor(0,1);
    lcd.print("V: 59 V");
    lcd.setCursor(0,2);
    lcd.print("I: 100 mA");
}

void DisplayState::showChASettings()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ch A settings");
    lcd.setCursor(0,1);
    lcd.print("Voltage: 12 V");
    lcd.setCursor(0,2);
    lcd.print("Current: 100 mA");
}

void DisplayState::showChBSettings()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ch B settings");
    lcd.setCursor(0,1);
    lcd.print("Voltage: 5 V");
    lcd.setCursor(0,2);
    lcd.print("Current: 200 mA");
}

void DisplayState::showParallelSettings()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Parallel mode settings");
    lcd.setCursor(0,1);
    lcd.print("Voltage: 5 V");
    lcd.setCursor(0,2);
    lcd.print("Current: 4 A");
}

void DisplayState::showSeriesSettings()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Series mode settings");
    lcd.setCursor(0,1);
    lcd.print("Voltage: 56 V");
    lcd.setCursor(0,2);
    lcd.print("Current: 100 mA");
}
