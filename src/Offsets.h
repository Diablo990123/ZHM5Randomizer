#pragma once
#include "Version.h"


namespace hitman_randomizer {

// TODO: The naming here is a bit all over the place
class GameOffsets {
 private:
  struct Offsets {
    void* pPushItem;

    void* pPushWorldInventoryDetour;
    void* pPushNPCInventoryDetour;
    void* pPushHeroInventoryDetour;
    void* pPushStashInventoryDetour;
    void** pZEntitySceneContext_LoadScene;
  } offsets;

  GameOffsets();

 public:
  static const GameOffsets* instance();

  void* getPushItem() const;
  void* getPushWorldInventoryDetour() const;
  void* getPushNPCInventoryDetour() const;
  void* getPushHeroInventoryDetour() const;
  void* getPushStashInventoryDetour() const;
  void** getZEntitySceneContext_LoadScene() const;
};

}  // namespace hitman_randomizer