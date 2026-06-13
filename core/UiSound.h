#pragma once

class UiSound
{
public:
    static void click();
    static void toggle();
    static void success();
    static void close();
    static void setEnabled(bool enabled);
    static bool isEnabled();
};
