#include <OneButton.h>

#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

#include <AXP192.h>
#include <M5Display.h>
#include <M5StickCPlus.h>
#include <RTC.h>
#include <string>

#include "lib/CountdownTimer.h"
void draw_widgets();
void count_down_and_draw();
void button_tick();
void callback_button_a();
void callback_button_b();

enum WorkMode {
	Stopped = 0,
	Timing = 1,
	Setting = 2
};
WorkMode mode = WorkMode::Stopped;
int setting_position = 0;
char time_str[10];
Scheduler sched;
CountdownTimer timer("00:02:00");
OneButton ButtonA(37);
OneButton ButtonB(39);
Task task_draw_widgets(1000, TASK_FOREVER, &draw_widgets);
Task task_count_down_and_draw(1000, TASK_FOREVER, &count_down_and_draw);
Task task_button_tick(1, TASK_FOREVER, &button_tick);

void count_down_and_draw() {
	if (!timer.cound_down()) {
        reset();
    }
	draw_widgets();
}
void draw_widgets() {
	M5.Lcd.setTextFont(2);
	M5.Lcd.setTextSize(2);
	M5.Lcd.drawString("timer", 40, 9);
	M5.Rtc.GetBm8563Time();
	M5.Lcd.setCursor(12, 99, 2);
	// M5.Lcd.printf("%02d:%02d:%02d", M5.Rtc.Hour, M5.Rtc.Minute, M5.Rtc.Second);
	timer.to_c_str(time_str);
	M5.Lcd.printf("%s", time_str);
	M5.Lcd.setCursor(12, 160, 2);
	switch (mode) {
		case WorkMode::Timing:
			M5.Lcd.printf(" Timing      ");
			break;
		case WorkMode::Stopped:
			M5.Lcd.printf("Stopped     ");
			break;
		case WorkMode::Setting:
			M5.Lcd.printf("Setting     ");
			break;
	}
}
void start_and_pause() {
	switch (mode) {
		case WorkMode::Timing:
			mode = WorkMode::Stopped;
			task_count_down_and_draw.disable();
			break;
		case WorkMode::Stopped:
			mode = WorkMode::Timing;
			task_count_down_and_draw.enable();
			break;
		case WorkMode::Setting:
            if (setting_position % 3 == 0) {
                time_str[setting_position] = (time_str[setting_position] - '0' + 1) % 6 + '0';
            } else {
                time_str[setting_position] = (time_str[setting_position] - '0' + 1) % 10 + '0';
            }
            timer = CountdownTimer(time_str);
			break;
	}
	draw_widgets();
}
void reset() {
	if (mode != WorkMode::Setting) {
		mode = WorkMode::Stopped;
		task_count_down_and_draw.disable();
		timer = CountdownTimer(10);
	} else {
        setting_position = (setting_position + (setting_position % 3 == 1 ? 2 : 1)) % 8;
	}
	draw_widgets();
}

void setting_mode() {
	if (mode == WorkMode::Stopped) {
		mode = WorkMode::Setting;
		setting_position = 0;
	} else {
		mode = WorkMode::Stopped;
	}
	draw_widgets();
}
void button_tick() {
	ButtonA.tick();
	ButtonB.tick();
}
void setup() {
	M5.begin();

	// M5.Lcd.print("Hello World");
	M5.Lcd.fillScreen(0x0);
	M5.Beep.beep();
	M5.Beep.end();
	draw_widgets();

	sched.init();
	sched.addTask(task_draw_widgets);
	sched.addTask(task_count_down_and_draw);
	sched.addTask(task_button_tick);

	ButtonA.attachClick(start_and_pause);
	ButtonB.attachClick(reset);
	ButtonB.attachDoubleClick(setting_mode);
	task_button_tick.enable();
}
void loop() {
	sched.execute();
}
