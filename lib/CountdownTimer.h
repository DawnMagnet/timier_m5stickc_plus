#include <stdio.h>
using ull = unsigned long long;
class CountdownTimer {
public:
	CountdownTimer();
	CountdownTimer(ull hours, ull minutes, ull seconds) {
		second = seconds;
		hour = hours;
		minute = minutes;
	}
	CountdownTimer(ull seconds) {
		hour = seconds / 3600ull;
		minute = (seconds / 60ull) % 60ull;
		second = seconds % 60ull;
	}
	// format: XX:XX:XX
	CountdownTimer(const char * s) {
        sscanf(s, " %llu:%llu:%llu", &hour, &minute, &second);
	}

	const char *to_c_str() {
        sprintf(view, "%02llu:%02llu:%02llu", hour, minute, second);
        return view;
	}
    void to_c_str(char * s) {
        sprintf(s, "%02llu:%02llu:%02llu", hour, minute, second);
	}

	bool cound_down() {
		if (second > 0) {
			second--;
		} else if (minute > 0) {
			second = 59;
			minute--;
		} else if (hour > 0) {
			minute = second = 59;
			hour--;
		} else {
			return false;
		}
		return true;
	}

private:
	ull second, hour, minute;
	char view[10];
};