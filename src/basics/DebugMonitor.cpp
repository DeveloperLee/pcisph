#include "DebugMonitor.h"

namespace cs224 {

std::vector<DebugMonitor::Item> DebugMonitor::_items;

void DebugMonitor::clear() {
    _items.clear();
}

void DebugMonitor::addItem(const std::string &name, const std::string &value) {
    _items.emplace_back(Item({name, value}));
}

} 
