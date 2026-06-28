#include "notification.h"
#include "data/definitions.h"
#include "core/config.h"
#include "data/fonts.h"
#include "ui/ui.h"

#define SWIPE_TIME 0.1f

static ARRLIST_Notification g_notifications = { 0 };
static float g_pulse_end_timer = 0.0f;
static float g_anchor_y = 0;
static const float g_section_height = 40.0f;
static const float g_notification_read_speed = 3.0f;

void DrawNotifications() {
	float true_y = GetScreenHeight() - (g_section_height * g_notifications.size);
	if (true_y < -g_section_height) g_pulse_end_timer = (g_notification_read_speed + SWIPE_TIME);
    g_anchor_y += (true_y - g_anchor_y) / 2.0f;
    if (g_notifications.size > 0) {
        g_pulse_end_timer += GetFrameTime();
        for (size_t i = 0; i < g_notifications.size; i++) {
            int rx = GetScreenWidth() - UITextWidth(g_notifications.data[i].message) - 20;
            int ry = g_anchor_y + (g_section_height * i);
            int rw = UITextWidth(g_notifications.data[i].message) + 10;
            if (i == 0 && g_pulse_end_timer > g_notification_read_speed)
                rx += ((g_pulse_end_timer - g_notification_read_speed) / SWIPE_TIME) * (rw + 10);
            if (Config()->enablenotifications) {
                DrawRectangle(
                    rx, ry, rw,
                    20 + 10,
                    RED);
                DrawTextEx(
                    FontAsset(),
                    g_notifications.data[i].message, (Vector2){rx + 5, ry + 5}, 20, 0,
                    WHITE);
            }
        }
        if (g_pulse_end_timer > (g_notification_read_speed + SWIPE_TIME)) {
            g_pulse_end_timer = 0.0f;
            ARRLIST_Notification_remove(&g_notifications, 0);
            g_anchor_y += g_section_height;
        }
    } else {
        g_pulse_end_timer = 0.0f;
    }
}

void CleanNotifications() {
    ARRLIST_Notification_clear(&g_notifications);
}

void Notify(MessageLevel level, char* message, ...) {
    if (!Config()->enablenotifications || level < Config()->notificationfilter) return;
    va_list args;
    va_start(args, message);
	char _b[MAX_NOTIFICATION_SIZE] = { 0 };
    vsnprintf(_b, MAX_NOTIFICATION_SIZE - 1, message, args);
    Notification n = { 0 };
    strncpy(n.message, _b, MAX_NOTIFICATION_SIZE);
    n.level = level;
    ARRLIST_Notification_add(&g_notifications, n);
}
