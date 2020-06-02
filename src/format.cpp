#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int hours = seconds / 60 / 60;
    int minutes = seconds / 60 - hours * 60;
    int sec = seconds - hours * 60 * 60 - minutes * 60;

    char str[9];

    sprintf(str, "%02d:%02d:%02d", hours, minutes, sec);

    return string(str);
}