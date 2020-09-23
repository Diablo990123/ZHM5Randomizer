#include "DefaultItemPool.h"

#include "..\thirdparty\json.hpp"
#include "Console.h"
#include "Repository.h"

DefaultItemPool::DefaultItemPool(json &json) {
  for (json::iterator item = json.begin(); item != json.end(); ++item) {
    RepositoryID id(item.value().get<std::string>());

    if (RandomDrawRepository::inst().contains(id))
      ids.push_back(id);
  }
}

size_t DefaultItemPool::size() const { return ids.size(); }

void DefaultItemPool::get(std::vector<const RepositoryID *> &out,
                          bool (Item::*fn)() const) const {
  auto repo = RandomDrawRepository::inst();
  for (const auto &id : ids) {
    if ((repo.getItem(id)->*fn)())
      out.push_back(&id);
  }
}

void DefaultItemPool::getL(std::vector<const RepositoryID*> &out,
                          std::function<bool(const Item&)> fn) const {
  auto repo = RandomDrawRepository::inst();
  for (const auto &id : ids) {
    if (fn(*(repo.getItem(id)))) {
      out.push_back(&id);
    }
  }
}

void DefaultItemPool::getPosition(std::vector<int> &out,
                                  bool (Item::*fn)() const) const {
  auto repo = RandomDrawRepository::inst();
  int cnt = 0;
  for (const auto &id : ids) {
    if ((repo.getItem(id)->*fn)())
      out.push_back(cnt);
    ++cnt;
  }
}

size_t DefaultItemPool::getCount(bool (Item::*fn)() const) const {
  auto repo = RandomDrawRepository::inst();
  int cnt = 0;
  for (const auto &id : ids) {
    if ((repo.getItem(id)->*fn)())
      ++cnt;
  }
  return cnt;
}

size_t DefaultItemPool::getCount(const RepositoryID &id) const {
  return std::count(ids.begin(), ids.end(), id);
}

void DefaultItemPool::print() const {
  auto repo = RandomDrawRepository::inst();
  Console::log("DefaultPool report:\n");
  for (const auto &id : ids) {
    Console::log("\t");
    repo.getItem(id)->print();
  }
}

void DefaultItemPool::getIdAt(RepositoryID &repoId, int position) const {
  repoId = ids[position];
}

//
// void DefaultItemPool::getPosition(std::vector<int> out,
// std::function<bool(const Item&)> fn); int
// DefaultItemPool::getCount(std::function<bool(const Item&)> fn);