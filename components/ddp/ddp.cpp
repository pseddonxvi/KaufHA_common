#include "esphome/core/log.h"
#include "esphome/core/util.h"
#include "ddp.h"
#include "ddp_light_effect.h"

namespace esphome {
namespace ddp {

static const char *const TAG = "ddp";

DDPComponent *global_ddp_component = nullptr;

void DDPComponent::loop() {
  if (!this->udp_)
    return;

  while (uint16_t packet_size = this->udp_->parsePacket()) {
    std::vector<uint8_t> payload;
    payload.resize(packet_size);
    
    if (!this->udp_->read(&payload[0], payload.size())) {
      ESP_LOGW(TAG, "Error reading UDP packet!");
      continue;
    }

    for (auto *effect : this->effects_) {
      effect->on_ddp_data(payload);
    }
  }
}

void DDPComponent::add_effect(DDPLightEffectBase *effect) {
  this->effects_.push_back(effect);

  // Initialize UDP if this is the first effect
  if (this->effects_.size() == 1) {
    // Create UDP instance for RP2040 if not already created
    if (!this->udp_) { 
      this->udp_ = std::unique_ptr<LwipUDP>(new LwipUDP()); 
    }

    // Begin UDP on the DDP port
    if (!this->udp_->begin(PORT)) {
      ESP_LOGE(TAG, "Cannot bind to UDP port %d!", PORT);
      return;
    }

    ESP_LOGI(TAG, "DDP server started on port %d", PORT);
    
    // Set global component
    global_ddp_component = this;
  }
}

void DDPComponent::remove_effect(DDPLightEffectBase *effect) {
  auto it = std::find(this->effects_.begin(), this->effects_.end(), effect);
  if (it != this->effects_.end())
    this->effects_.erase(it);

  // Stop UDP if this was the last effect
  if (this->effects_.empty()) {
    this->udp_->stop();
    global_ddp_component = nullptr;
  }
}

}  // namespace ddp
}  // namespace esphome
