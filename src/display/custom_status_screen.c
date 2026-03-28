/*
 * Custom status screen for 128x32 SSD1306 OLED
 * Shows: dual battery (L+R), BT profile, layer name
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>
#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
#include <zmk/display/widgets/output_status.h>
static struct zmk_widget_output_status output_status_widget;
#endif

/* --- Dual battery widget --- */

static lv_obj_t *battery_label;
static uint8_t central_level;
static uint8_t peripheral_level;
static bool peripheral_connected;

static void update_battery_label(struct k_work *work) {
    char text[20];
    if (peripheral_connected) {
        snprintf(text, sizeof(text), "L:%d%% R:%d%%", central_level, peripheral_level);
    } else {
        snprintf(text, sizeof(text), "%d%%", central_level);
    }
    lv_label_set_text(battery_label, text);
}

static K_WORK_DEFINE(battery_work, update_battery_label);

static int central_battery_cb(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    central_level = ev ? ev->state_of_charge : zmk_battery_state_of_charge();
    if (zmk_display_is_initialized()) {
        k_work_submit_to_queue(zmk_display_work_q(), &battery_work);
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(custom_central_battery, central_battery_cb);
ZMK_SUBSCRIPTION(custom_central_battery, zmk_battery_state_changed);

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)

static int peripheral_battery_cb(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev =
        as_zmk_peripheral_battery_state_changed(eh);
    if (ev) {
        peripheral_level = ev->state_of_charge;
        peripheral_connected = true;
    }
    if (zmk_display_is_initialized()) {
        k_work_submit_to_queue(zmk_display_work_q(), &battery_work);
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(custom_peripheral_battery, peripheral_battery_cb);
ZMK_SUBSCRIPTION(custom_peripheral_battery, zmk_peripheral_battery_state_changed);

#endif /* CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING */

/* --- Layer widget --- */

static lv_obj_t *layer_label;

static void update_layer_label(struct k_work *work) {
    uint8_t index = zmk_keymap_highest_layer_active();
    const char *name = zmk_keymap_layer_name(index);
    char text[16];
    if (name) {
        snprintf(text, sizeof(text), "%s", name);
    } else {
        snprintf(text, sizeof(text), "Layer %d", index);
    }
    lv_label_set_text(layer_label, text);
}

static K_WORK_DEFINE(layer_work, update_layer_label);

static int layer_cb(const zmk_event_t *eh) {
    if (zmk_display_is_initialized()) {
        k_work_submit_to_queue(zmk_display_work_q(), &layer_work);
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(custom_layer, layer_cb);
ZMK_SUBSCRIPTION(custom_layer, zmk_layer_state_changed);

/* --- Screen setup --- */

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /* Battery: top right */
    battery_label = lv_label_create(screen);
    central_level = zmk_battery_state_of_charge();
    lv_label_set_text(battery_label, "");
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    update_battery_label(NULL);

    /* Output status (BT profile): top left */
#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);
#endif

    /* Layer: bottom left */
    layer_label = lv_label_create(screen);
    lv_obj_set_style_text_font(layer_label, lv_theme_get_font_small(screen), LV_PART_MAIN);
    lv_label_set_text(layer_label, "");
    lv_obj_align(layer_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    update_layer_label(NULL);

    return screen;
}
