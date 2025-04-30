#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "ddp_light_effect.h"

// For RP2040, use WiFi.h instead of ESP8266WiFi.h and LwipUDP instead of WiFiUDP
#include <WiFi.h>
#include <LwipUdp.h>

namespace esphome {
namespace ddp {

static const char *const TAG = "ddp";
static const uint16_t PORT = 4048;

class DDPLightEffectBase;

class DDPComponent : public Component {
 public:
  float get_setup_priority() const override { return setup_priority::LATE; }
  void loop() override;
  void add_effect(DDPLightEffectBase *effect);
  void remove_effect(DDPLightEffectBase *effect);

 protected:
  // Changed from WiFiUDP to LwipUDP for RP2040
  std::unique_ptr<LwipUDP> udp_;
  std::vector<DDPLightEffectBase *> effects_;
};

extern DDPComponent *global_ddp_component;

}  // namespace ddp
}  // namespace esphome
#endif  // USE_ARDUINO
