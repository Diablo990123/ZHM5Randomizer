#include "ZHM5Randomizer/src/RandomizationMan.h"

#include <filesystem>
#include <map>
#include <memory>

#include "ZHM5Randomizer/src/Config.h"
#include "ZHM5Randomizer/src/Console.h"
#include "ZHM5Randomizer/src/MemoryUtils.h"
#include "ZHM5Randomizer/src/Offsets.h"
#include "ZHM5Randomizer/src/RNG.h"
#include "ZHM5Randomizer/src/SSceneInitParameters.h"
#include "ZHM5Randomizer/src/randomizers/Randomizer.h"
#include "ZHM5Randomizer/src/randomizers/custom.h"

#include "spdlog/spdlog.h"


namespace hitman_randomizer {

std::unique_ptr<Randomizer> RandomizationMan::world_inventory_randomizer =
    nullptr;
std::unique_ptr<Randomizer> RandomizationMan::npc_item_randomizer = nullptr;
std::unique_ptr<Randomizer> RandomizationMan::hero_inventory_randomizer =
    nullptr;
std::unique_ptr<Randomizer> RandomizationMan::stash_item_randomizer = nullptr;

template <typename T>
RandomizationStrategy *
createInstance(std::shared_ptr<hitman_randomizer::Config> config) {
  return new T(config);
}

std::unordered_map<std::string, RandomizationStrategy *(*)(std::shared_ptr<hitman_randomizer::Config>)> worldRandomizers{
    {"NONE", &createInstance<IdentityRandomization>},
    {"DEFAULT", &createInstance<DefaultWorldRandomization>},
    {"OOPS_ALL_EXPLOSIVES",
     &createInstance<OopsAllExplosivesWorldInventoryRandomization>},
    {"TREASURE_HUNT", &createInstance<TreasureHuntWorldInventoryRandomization>},
    {"NO_ITEMS", &createInstance<NoItemsWorldInventoryRandomization>},
    {"ACTION", &createInstance<ActionWorldRandomization>},
    {"CUSTOM", &createInstance<CustomWorldStrategy>},
};

std::unordered_map<std::string, RandomizationStrategy *(*)(std::shared_ptr<hitman_randomizer::Config>)> npcRandomizers{
    {"NONE", &createInstance<IdentityRandomization>},
    {"DEFAULT", &createInstance<DefaultNPCRandomization>},
    {"UNLIMITED", &createInstance<UnlimitedNPCRandomization>},
    {"HARD", &createInstance<HardNPCRandomization>},
    {"SLEEPY", &createInstance<SleepyNPCRandomization>},
    {"CHAIN_REACTION", &createInstance<ChainReactionNPCRandomization>},
    {"CUSTOM", &createInstance<CustomNPCStrategy>},
};

std::unordered_map<std::string, RandomizationStrategy *(*)(std::shared_ptr<hitman_randomizer::Config>)> heroRandomizers{
    {"NONE", &createInstance<IdentityRandomization>},
    {"DEFAULT", &createInstance<DefaultHeroRandomization>},
};

std::unordered_map<std::string, RandomizationStrategy *(*)(std::shared_ptr<hitman_randomizer::Config>)> stashRandomizers{
    {"NONE", &createInstance<IdentityRandomization>},
    {"DEFAULT", &createInstance<DefaultStashRandomization>},
};

void RandomizationMan::configureRandomizerCollection() {
  registerRandomizer(
      RandomizerSlot::WorldInventory,
      std::make_unique<Randomizer>(
          worldRandomizers[config_->world_inventory_randomizer()](config_)));
  registerRandomizer(
      RandomizerSlot::NPCInventory,
      std::make_unique<Randomizer>(
          npcRandomizers[config_->npc_inventory_randomizer()](config_)));
  registerRandomizer(
      RandomizerSlot::HeroInventory,
      std::make_unique<Randomizer>(
          heroRandomizers[config_->hero_inventory_randomizer()](config_)));
  registerRandomizer(
      RandomizerSlot::StashInventory,
      std::make_unique<Randomizer>(
          stashRandomizers[config_->stash_inventory_randomizer()](config_)));
}

RandomizationMan::RandomizationMan(std::shared_ptr<hitman_randomizer::Config> config) : config_(config) {
  default_item_pool_repo = std::make_unique<DefaultItemPoolRepository>(
      "..\\HITMAN2\\Retail\\DefaultItemPools.json");

  world_inventory_randomizer =
      std::make_unique<Randomizer>(new IdentityRandomization(config_));
  npc_item_randomizer = std::make_unique<Randomizer>(new IdentityRandomization(config_));
  hero_inventory_randomizer =
      std::make_unique<Randomizer>(new IdentityRandomization(config_));
  stash_item_randomizer =
      std::make_unique<Randomizer>(new IdentityRandomization(config_));

  MemoryUtils::DetourCall(
      GameOffsets::instance()->getPushWorldInventoryDetour(),
      reinterpret_cast<const void *>(
          &pushWorldItem<&world_inventory_randomizer>));
  MemoryUtils::DetourCall(
      GameOffsets::instance()->getPushNPCInventoryDetour(),
      reinterpret_cast<const void *>(&pushWorldItem<&npc_item_randomizer>));
  MemoryUtils::DetourCall(GameOffsets::instance()->getPushHeroInventoryDetour(),
                          reinterpret_cast<const void *>(
                              &pushWorldItem<&hero_inventory_randomizer>));
  MemoryUtils::DetourCall(
      GameOffsets::instance()->getPushStashInventoryDetour(),
      reinterpret_cast<const void *>(&pushWorldItem<&stash_item_randomizer>));
}

void RandomizationMan::registerRandomizer(RandomizerSlot slot,
                                          std::unique_ptr<Randomizer> rng) {
  switch (slot) {
  case RandomizerSlot::WorldInventory:
    world_inventory_randomizer = std::move(rng);
    break;
  case RandomizerSlot::NPCInventory:
    npc_item_randomizer = std::move(rng);
    break;
  case RandomizerSlot::HeroInventory:
    hero_inventory_randomizer = std::move(rng);
    break;
  case RandomizerSlot::StashInventory:
    stash_item_randomizer = std::move(rng);
    break;
  }
}

void RandomizationMan::initializeRandomizers(const SSceneInitParameters *sip) {
  auto scenario = Scenario::from_SceneInitParams(*sip);
  if (scenario.string() == "NONE") {
    return;
  }
  
  log::info("Loading Scenario: {}", scenario.string());

  configureRandomizerCollection();

  auto seed = config_->rng_seed();
  if (seed == 0)
    seed = std::random_device{}();
  RNG::inst().seed(seed);

  auto default_pool = default_item_pool_repo->getDefaultPool(scenario);
  if (default_pool != nullptr) {
    world_inventory_randomizer->initialize(scenario, default_pool);
    npc_item_randomizer->initialize(scenario, default_pool);
    hero_inventory_randomizer->initialize(scenario, default_pool);
    stash_item_randomizer->initialize(scenario, default_pool);
  } else {
    world_inventory_randomizer->disable();
    npc_item_randomizer->disable();
    hero_inventory_randomizer->disable();
    stash_item_randomizer->disable();
  }
  log::info("RandomizationMan::initializeRandomizers complete.");
}

}  // namespace hitman_randomizer