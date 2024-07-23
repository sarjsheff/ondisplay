#include <Arduino.h>

#include <esp32_smartdisplay.h>
#include "ui/ui.h"
#include <LuaWrapper.h>
#include <WiFi.h>
#include "mqtt_client.h"
#include <map>
#include <ArduinoJson.h>

enum sensor_panel_types
{
  SP_TEMP,
  SP_HUMIDITY,
  SP_DEW
};

static const char *TAG = "ondisplay";
WiFiClient wifi;
LuaWrapper lua;
esp_mqtt_client_handle_t client;

std::map<std::string, double> dev_temp{};
std::map<std::string, double> dev_hum{};
std::map<std::string, double> dev_dew{};
std::map<std::string, lv_obj_t *> dev{};

void setSensorPanel(lv_obj_t *obj, sensor_panel_types tp, double value)
{
  lv_obj_t *p = lv_obj_get_child(obj, tp + 1);
  lv_obj_remove_flag(p, LV_OBJ_FLAG_HIDDEN);
  lv_obj_t *l = lv_obj_get_child(p, 1);
  lv_label_set_text_fmt(l, "%.1f", value);
}

void createSensorPanel(lv_obj_t *obj, sensor_panel_types tp)
{
  lv_obj_t *ui_tempPanel = lv_obj_create(obj);
  lv_obj_set_width(ui_tempPanel, lv_pct(30));       // LV_SIZE_CONTENT);
  lv_obj_set_height(ui_tempPanel, LV_SIZE_CONTENT); /// 1
  lv_obj_set_align(ui_tempPanel, LV_ALIGN_CENTER);
  lv_obj_set_flex_flow(ui_tempPanel, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(ui_tempPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_remove_flag(ui_tempPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
  lv_obj_add_flag(ui_tempPanel, LV_OBJ_FLAG_HIDDEN);        /// Flags
  lv_obj_set_style_pad_left(ui_tempPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_right(ui_tempPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(ui_tempPanel, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_bottom(ui_tempPanel, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_row(ui_tempPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_column(ui_tempPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_style_pad_left(ui_tempPanel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_right(ui_tempPanel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(ui_tempPanel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_bottom(ui_tempPanel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

  lv_obj_t *ui_tempImage = lv_image_create(ui_tempPanel);
  if (tp == SP_TEMP)
  {
    lv_image_set_src(ui_tempImage, &ui_img_thermometer_png);
  }
  if (tp == SP_HUMIDITY)
  {
    lv_image_set_src(ui_tempImage, &ui_img_humidity_png);
  }
  if (tp == SP_DEW)
  {
    lv_image_set_src(ui_tempImage, &ui_img_dew_png);
  }

  lv_obj_set_width(ui_tempImage, 20);  /// 24
  lv_obj_set_height(ui_tempImage, 20); /// 24
  // lv_obj_set_x(ui_tempImage, -64);
  // lv_obj_set_y(ui_tempImage, -109);
  lv_obj_set_align(ui_tempImage, LV_ALIGN_CENTER);
  lv_obj_add_flag(ui_tempImage, LV_OBJ_FLAG_CLICKABLE);     /// Flags
  lv_obj_remove_flag(ui_tempImage, LV_OBJ_FLAG_SCROLLABLE); /// Flags

  lv_obj_t *ui_tempLabel = lv_label_create(ui_tempPanel);
  lv_obj_set_width(ui_tempLabel, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(ui_tempLabel, LV_SIZE_CONTENT); /// 1
  lv_obj_set_align(ui_tempLabel, LV_ALIGN_CENTER);
  lv_label_set_text(ui_tempLabel, "10");
  lv_obj_set_style_text_font(ui_tempLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_left(ui_tempLabel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_right(ui_tempLabel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(ui_tempLabel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_bottom(ui_tempLabel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void addSensorData(esp_mqtt_event_handle_t event)
{
  char topic[event->topic_len] = "";
  memcpy(topic, event->topic, event->topic_len);
  topic[event->topic_len] = 0;
  char *mtype = strtok(topic, "/");
  char *name = strtok(NULL, "/");
  if (strncmp("tele", mtype, 4) == 0)
  {
    if (name != NULL)
    {

      if (dev.count(name) == 0)
      {
        printf("Not found device [%s]\n", name);
        dev[name] = lv_obj_create(ui_sensorPanel);
        lv_obj_set_width(dev[name], lv_pct(100));
        lv_obj_set_height(dev[name], LV_SIZE_CONTENT); /// 1
        lv_obj_set_align(dev[name], LV_ALIGN_CENTER);
        lv_obj_set_flex_flow(dev[name], LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_align(dev[name], LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_remove_flag(dev[name], LV_OBJ_FLAG_SCROLLABLE); /// Flags
        lv_obj_set_style_pad_left(dev[name], 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(dev[name], 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(dev[name], 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(dev[name], 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

        // lv_obj_get_child

        lv_obj_t *ui_title = lv_label_create(dev[name]);
        lv_obj_set_width(ui_title, lv_pct(80));       /// 1
        lv_obj_set_height(ui_title, LV_SIZE_CONTENT); /// 1
        lv_obj_set_align(ui_title, LV_ALIGN_CENTER);
        lv_label_set_text(ui_title, name);
        lv_obj_set_style_text_font(ui_title, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

        createSensorPanel(dev[name], SP_TEMP);
        createSensorPanel(dev[name], SP_HUMIDITY);
        createSensorPanel(dev[name], SP_DEW);
      }

      char *cmd = strtok(NULL, "/");
      if (cmd != NULL)
      {
        if (strncmp("SENSOR", cmd, 6) == 0)
        {
          char data[event->data_len] = "";
          memcpy(data, event->data, event->data_len);
          data[event->data_len] = 0;
          JsonDocument doc;
          DeserializationError error = deserializeJson(doc, data);
          if (error)
          {
            Serial.print("deserializeJson() returned ");
            Serial.println(error.c_str());
            return;
          }
          const char *tm = doc["Time"];
          lv_label_set_text(ui_lastUpdate, tm);

          String sensor_name = "";
          if (doc.containsKey("AM2301"))
          {
            sensor_name = "AM2301";
          }
          if (doc.containsKey("SI7021"))
          {
            sensor_name = "SI7021";
          }

          if (sensor_name.compareTo("") != 0)
          {
            dev_temp[name] = doc[sensor_name]["Temperature"];
            dev_hum[name] = doc[sensor_name]["Humidity"];
            dev_dew[name] = doc[sensor_name]["DewPoint"];
          }

          setSensorPanel(dev[name], SP_TEMP, dev_temp[name]);
          setSensorPanel(dev[name], SP_HUMIDITY, dev_hum[name]);
          setSensorPanel(dev[name], SP_DEW, dev_dew[name]);
        }
      }
    }
    // printf("\n[%s]\n",strtok(&event->topic[5],"/"));
  }
}

void addLogItem(esp_mqtt_event_handle_t event)
{
  lv_obj_t *ui_itemPanel = lv_obj_create(ui_mainPanel);
  lv_obj_set_width(ui_itemPanel, lv_pct(100));
  lv_obj_set_height(ui_itemPanel, LV_SIZE_CONTENT); /// 1
  lv_obj_set_align(ui_itemPanel, LV_ALIGN_CENTER);
  lv_obj_set_flex_flow(ui_itemPanel, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(ui_itemPanel, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_remove_flag(ui_itemPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags

  lv_obj_t *ui_Label2 = lv_label_create(ui_itemPanel);
  lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT); /// 1
  lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
  char name[event->topic_len] = "";
  memcpy(name, event->topic, event->topic_len);
  name[event->topic_len] = 0;
  lv_label_set_text(ui_Label2, name);

  lv_obj_t *ui_Label3 = lv_label_create(ui_itemPanel);
  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT); /// 1
  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  char value[event->data_len] = "";
  memcpy(value, event->data, event->data_len);
  value[event->data_len] = 0;
  lv_label_set_text(ui_Label3, value);
}

static void log_error_if_nonzero(const char *message, int error_code)
{
  if (error_code != 0)
  {
    ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
  }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;
  switch ((esp_mqtt_event_id_t)event_id)
  {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
    lv_label_set_text_fmt(ui_lbStatus, "IP: %s MQTT", WiFi.localIP().toString().c_str());

    msg_id = esp_mqtt_client_subscribe(client, "#", 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
    break;

  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
    break;
  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);

    addLogItem(event);
    addSensorData(event);

    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
    {
      log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
      log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
      log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
      ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
    }
    break;
  default:
    ESP_LOGI(TAG, "Other event id:%d", event->event_id);
    break;
  }
}

void mqtt_init()
{

#ifdef MQTT_URL
  printf(MQTT_URL);
  esp_mqtt_client_config_t mqtt_cfg = {
      .uri = MQTT_URL,
  };

  client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
  esp_mqtt_client_start(client);
#endif
}

void wifi_init()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  lv_label_set_text_fmt(ui_lbStatus, "IP: %s", WiFi.localIP().toString().c_str());
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  smartdisplay_init();

  auto disp = lv_disp_get_default();
  // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_270);

  // String script = String("print('Hello world!')");
  // Serial.println(lua.Lua_dostring(&script));
  ui_init();
  lv_label_set_text(ui_lbStatus, "Connecting to WiFi...");
  wifi_init();
  lv_label_set_text(ui_lbStatus, "Connecting to MQTT...");
  mqtt_init();
}

auto lv_last_tick = millis();

void loop()
{
  auto now = millis();
  // Update the ticker
  lv_tick_inc(now - lv_last_tick);
  lv_last_tick = now;
  // Update the UI
  lv_timer_handler();
}
