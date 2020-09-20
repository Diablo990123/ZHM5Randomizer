#pragma once
#include <queue>
#include <random>
#include <type_traits>
#include <unordered_map>

// TODO: Remove this?
#include "..\thirdparty\json.hpp"
#include "Repository.h"
#include "Scenario.h"
#include "Config.h"

class DefaultItemPool;

class RandomisationStrategy {
 protected:
  RandomDrawRepository& repo;

  std::shared_ptr<hitman_randomizer::Config> config_;

 public:
  RandomisationStrategy(std::shared_ptr<hitman_randomizer::Config> config);

  // Takes Repository ID and returns a new ID according to the internal
  // randomisation strategy Item IDs that don't have a corresponding item
  // configuration in the Repository should be skipped.
  virtual const RepositoryID* randomize(const RepositoryID* in_out_ID) = 0;

  // Called on SceneLoad. This function is intended for stateful randomisation
  // strategies which might require knowledge of the next scene and/or default
  // item pool of that scene to setup their internal state in preparation for
  // item randomisation.
  virtual void initialize(Scenario, const DefaultItemPool* const);
};

class IdentityRandomisation : public RandomisationStrategy {
 public:
  IdentityRandomisation(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};

// This randomisation strategy is intended to be used to randomize world items
// during the initial load of a level. It's desiged to be as undistruptive to
// the game flow as possible.
class WorldInventoryRandomisation : public RandomisationStrategy {
 protected:
  std::queue<const RepositoryID*> item_queue;

 public:
 WorldInventoryRandomisation(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override;
  void initialize(Scenario scen,
                  const DefaultItemPool* const default_pool) override;
};

class OopsAllExplosivesWorldInventoryRandomization
    : public WorldInventoryRandomisation {
 public:
 OopsAllExplosivesWorldInventoryRandomization(std::shared_ptr<hitman_randomizer::Config> config) : WorldInventoryRandomisation(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
  void initialize(Scenario scen,
                  const DefaultItemPool* const default_pool) override final;
};

class TreasureHuntWorldInventoryRandomization
    : public WorldInventoryRandomisation {
 public:
 TreasureHuntWorldInventoryRandomization(std::shared_ptr<hitman_randomizer::Config> config) : WorldInventoryRandomisation(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
  void initialize(Scenario scen,
                  const DefaultItemPool* const default_pool) override final;
};

class NoItemsWorldInventoryRandomization : public WorldInventoryRandomisation {
 public:
 NoItemsWorldInventoryRandomization(std::shared_ptr<hitman_randomizer::Config> config) : WorldInventoryRandomisation(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
  void initialize(Scenario scen,
                  const DefaultItemPool* const default_pool) override final;
};

class ActionWorldRandomization : public WorldInventoryRandomisation {
 public:
 ActionWorldRandomization(std::shared_ptr<hitman_randomizer::Config> config) : WorldInventoryRandomisation(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
  void initialize(Scenario scen,
                  const DefaultItemPool* const default_pool) override final;
};


class NPCItemRandomisation : public RandomisationStrategy {
 public:
 NPCItemRandomisation(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};

/*
This randomisation stratgy is intended to be used to randomize 47's starting
inventory. To preserve some intentionallity the randomizer will only randomize
items within their own category as defined by the InventoryCategoryIcon key in
the item repository. Those categories are: melee, key, explosives, questitem,
tool, sniperrifle, assaultrifle, remote, QuestItem, shotgun, suitcase, pistol,
distraction, poison, Container and smg.
*/
class HeroInventoryRandomisation : public RandomisationStrategy {
 public:
 HeroInventoryRandomisation(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};

class StashInventoryRandomisation : public RandomisationStrategy {
 public:
 StashInventoryRandomisation(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};

// Randomizes all NPC weapons without type restrictions and replaces flash
// grenades with frag grenades.
class UnrestrictedNPCRandomization : public RandomisationStrategy {
 public:
 UnrestrictedNPCRandomization(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};

class UnlimitedNPCItemRandomization : public RandomisationStrategy {
 public:
 UnlimitedNPCItemRandomization(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};


class SleepyNPCRandomization : public RandomisationStrategy {
 public:
 SleepyNPCRandomization(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
  bool exception_assigned;
};

class ChainReactionNPCRandomization : public RandomisationStrategy {
 public:
 ChainReactionNPCRandomization(std::shared_ptr<hitman_randomizer::Config> config) : RandomisationStrategy(config) {}
  const RepositoryID* randomize(const RepositoryID* in_out_ID) override final;
};

class Randomizer {
 private:
  bool enabled;
  std::unique_ptr<RandomisationStrategy> strategy;

 public:
  Randomizer(RandomisationStrategy*);
  const RepositoryID* randomize(const RepositoryID* id);
  void initialize(Scenario, const DefaultItemPool* const);
  void disable();
};
