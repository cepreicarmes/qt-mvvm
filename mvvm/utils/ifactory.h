// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#ifndef MVVM_IFACTORY_H
#define MVVM_IFACTORY_H

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace ModelView
{

//! Base for factories.

template <class Key, class Value> class IFactory
{
public:
    using function_t = std::function<Value*()>;
    using map_t = std::map<Key, function_t>;

    bool contains(const Key& item_key) const { return m_data.find(item_key) != m_data.end(); }

    std::unique_ptr<Value> create(const Key& item_key) const
    {
        auto it = m_data.find(item_key);
        if (it == m_data.end()) {
            std::ostringstream message;
            message << "IFactory::createItem() -> Error. Unknown item key '" << item_key << "'";
            throw std::runtime_error(message.str());
        }
        return std::unique_ptr<Value>((it->second)());
    }

    bool add(const Key& key, function_t func)
    {
        if (m_data.find(key) != m_data.end()) {
            std::ostringstream message;
            message << "IFactory::createItem() -> Already registered item key '" << key << "'";
            throw std::runtime_error(message.str());
        }
        return m_data.insert(make_pair(key, func)).second;
    }

    size_t size() const { return m_data.size(); }

private:
    map_t m_data;
};

} // namespace ModelView

#endif // MVVM_IFACTORY_H
