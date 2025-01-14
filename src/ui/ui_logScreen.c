// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 9.1.0
// Project name: ondisplay

#include "ui.h"

void ui_logScreen_screen_init(void)
{
ui_logScreen = lv_obj_create(NULL);
lv_obj_remove_flag( ui_logScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_Container1 = lv_obj_create(ui_logScreen);
lv_obj_remove_style_all(ui_Container1);
lv_obj_set_height( ui_Container1, 20);
lv_obj_set_width( ui_Container1, lv_pct(100));
lv_obj_set_align( ui_Container1, LV_ALIGN_BOTTOM_LEFT );
lv_obj_remove_flag( ui_Container1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Container1, lv_color_hex(0xBCBCBC), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Container1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_lbStatus = lv_label_create(ui_Container1);
lv_obj_set_width( ui_lbStatus, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_lbStatus, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_lbStatus, LV_ALIGN_LEFT_MID );
lv_label_set_text(ui_lbStatus,"-");
lv_obj_set_style_pad_left(ui_lbStatus, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_lbStatus, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_lbStatus, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_lbStatus, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_mainPanel = lv_obj_create(ui_logScreen);
lv_obj_remove_style_all(ui_mainPanel);
lv_obj_set_height( ui_mainPanel, 297);
lv_obj_set_width( ui_mainPanel, lv_pct(100));
lv_obj_set_align( ui_mainPanel, LV_ALIGN_TOP_MID );
lv_obj_set_flex_flow(ui_mainPanel,LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(ui_mainPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
lv_obj_remove_flag( ui_mainPanel, LV_OBJ_FLAG_CLICKABLE );    /// Flags
lv_obj_set_style_pad_left(ui_mainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_right(ui_mainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_top(ui_mainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_bottom(ui_mainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_row(ui_mainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(ui_mainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_set_style_pad_row(ui_mainPanel, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
lv_obj_set_style_pad_column(ui_mainPanel, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_logScreen, ui_event_logScreen, LV_EVENT_ALL, NULL);

}
