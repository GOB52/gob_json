/*!
  @file gob_json_delegate_handler.cpp
  @brief Derived handler using delegate object.
 */
#include "gob_json_delegate_handler.hpp"
#include <cassert>

namespace goblib { namespace json {

void DelegateHandler::startObject(const ElementPath& path)
{
    //    printf("%s:[%s]\n", __func__, path.toString().c_str());

    // Delegate processing
    auto del = currentDelegater();
    if(del)
    {
        auto ndel = del->startObject(path);
        assert(ndel);
        _delegaters.push(ndel);
        return;
    }
    _delegaters.push(new Delegater());
}

void DelegateHandler::endObject(const ElementPath& path)
{
    //    printf("%s:[%s]\n", __func__, path.toString().c_str());

    // Delegate processing
    auto del = currentDelegater();
    if(del)
    {
        del->endObject(path);
        delete _delegaters.top();
        _delegaters.pop();
    }
}

void DelegateHandler::startArray(const ElementPath& path)
{
    //    printf("%s:[%s]\n", __func__, path.toString().c_str());

    // Delegate processing
    auto del = currentDelegater();
    if(del) { del->startArray(path); }
}

void DelegateHandler::endArray(const ElementPath& path)
{
    //    printf("%s:[%s]\n", __func__, path.toString().c_str());

    // Delegate processing
    auto del = currentDelegater();
    if(del) { del->endArray(path); }
}

void DelegateHandler::value(const ElementPath& path, const ElementValue& value)
{
    //    printf("%s:[%s]\n", __func__, path.toString().c_str());

    // Delegate processing
    auto del = currentDelegater();
    if(del) { del->value(path, value); }
}

void DelegateHandler::endDocument()
{
    assert(_delegaters.empty());
}

//
}}
