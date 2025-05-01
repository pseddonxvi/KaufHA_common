#include "ddp.h"
#include "ddp_light_effect_base.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace ddp {

static const char *const TAG = "ddp";
static const int PORT = 4048;

DDPComponent::DDPComponent() {}
DDPComponent::~DDPComponent() {}

void DDPComponent::setup() {
  this->udp_ = make_unique<esphome::network::UDP>();
  if (!this->udp_->listen(PORT)) {
    ESP_LOGE(TAG, "Failed to start UDP listener on port %d.", PORT);
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Started UDP listener on port %d.", PORT);
}

void DDPComponent::loop() {
  if (!this->udp_) {
    return;
  }

  uint8_t payload[512];
  int packet_size = this->udp_->recv(payload, sizeof(payload));
  if (packet_size <= 0) {
    return;
  }

  if (!this->process_(payload, packet_size)) {
    ESP_LOGW(TAG, "Failed to process DDP packet.");
  }
}

void DDPComponent::add_effect(DDPLightEffectBase *light_effect) {
  if (this->light_effects_.count(light_effect)) {
    return;
  }
  this->light_effects_.insert(light_effect);
}

void DDPComponent::remove_effect(DDPLightEffectBase *light_effect) {
  this->light_effects_.erase(light_effect);
}

bool DDPComponent::process_(const uint8_t *payload, uint16_t size) {
  if (size < 13) {
    ESP_LOGE(TAG, "Invalid DDP packet received, too short (size=%d)", size);
    return false;
  }

  if (payload[4] || payload[5] || payload[6] || payload[7]) {
    ESP_LOGE(TAG, "Ignoring DDP Packet with non-zero data offset.");
    return false;
  }

  uint16_t used = 10;

  for (auto *light_effect : this->light_effects_) {
    if (used >= size) {
      return false;
    }
    uint16_t new_used = light_effect->process_(payload, size, used);
    if (new_used == 0) {
      return false;
    }
    used += new_used;
  }

  return true;
}

}  // namespace ddp
}  // namespace esphome
