#include "notification.h"
#include "data/definitions.h"
#include "data/colors.h"
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
            int rx = Config()->flipnotifications ? GetScreenWidth() - UITextWidth(g_notifications.data[i].message) - 20 : 10;
            int ry = g_anchor_y + (g_section_height * i);
            int rw = UITextWidth(g_notifications.data[i].message) + 10 + 20;
            if (i == 0 && g_pulse_end_timer > g_notification_read_speed)
                rx += ((g_pulse_end_timer - g_notification_read_speed) / SWIPE_TIME) * (rw + 10) * (Config()->flipnotifications ? 1.0f : -1.0f);
            MessageLevel ml = g_notifications.data[i].level;
            Color ncol = ml == LEVEL_WARN ? MappedColor(NOTIFICATION_WARN_COLOR) :
                (ml == LEVEL_INFO ? MappedColor(NOTIFICATION_INFO_COLOR) :
                (ml == LEVEL_ERROR ? MappedColor(NOTIFICATION_ERROR_COLOR) : 
                MappedColor(NOTIFICATION_TRACE_COLOR)));
            Color ngcol = ncol;
            ngcol.a = 50;
            Color subtle = MappedColor(NOTIFICATION_TEXT_COLOR);
            subtle.g *= 0.8f;
            subtle.r = subtle.g;
            subtle.b = subtle.g;
            if (Config()->enablenotifications) {
                DrawRectangle(
                    rx - 1, ry - 1, rw + 2,
                    20 + 10 + 2,
                    ncol);
                DrawRectangle(
                    rx, ry, rw,
                    20 + 10,
                    MappedColor(NOTIFICATION_BG_COLOR));
                DrawRectangleGradientH(
                    rx + (rw / 2.0f) + 1, ry, rw / 2.0f,
                    20 + 10,
                    (Color){ 0, 0, 0, 0},
                    ngcol);
                DrawTextEx(
                    FontAsset(),
                    g_notifications.data[i].message, (Vector2){rx + 5 + 20, ry + 5}, 20, 0,
                    MappedColor(NOTIFICATION_TEXT_COLOR));
                if (ml == LEVEL_INFO || ml == LEVEL_TRACE) {
                    DrawCircle(rx + 12, ry + 15, 7, subtle);
                    DrawCircle(rx + 12, ry + 15, 6, ncol);
                    DrawRectangle(rx + 11, ry + 11, 2, 5, subtle);
                    DrawRectangle(rx + 11, ry + 17, 2, 2, subtle);
                } else {
                    DrawTriangle(
                        (Vector2){rx + 12, ry + 7},
                        (Vector2){rx + 4, ry + 22},
                        (Vector2){rx + 20, ry + 22},
                        subtle);
                    DrawTriangle(
                        (Vector2){rx + 12, ry + 9},
                        (Vector2){rx + 6, ry + 21},
                        (Vector2){rx + 18, ry + 21},
                        ncol);
                    DrawRectangle(rx + 11, ry + 13, 2, 4, subtle);
                    DrawRectangle(rx + 11, ry + 18, 2, 2, subtle);
                }
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
