/*!
  @file gob_json_element.hpp
 */
#ifndef GOB_JSON_ELEMENT_HPP
#define GOB_JSON_ELEMENT_HPP

#include "gob_json_element_value.hpp"
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <array>
#include <vector>
#include <type_traits>
#include <cassert>

namespace goblib { namespace json {

// Type is std::vector?
template <class T> struct is_std_vector : public std::false_type { };
template <class T> struct is_std_vector<std::vector<T> > : public std::true_type { };
// Type is std::array?
template <class T> struct is_std_array : public std::false_type { };
template <class T, size_t N> struct is_std_array<std::array<T, N> > : public std::true_type { };

/*!
  @class ElementBase
 */
struct ElementBase
{
    const char* key{};

    ElementBase(const char* s) : key(s) {}
    bool operator==(const char* k) { return strcmp(k, key) == 0; } //!< Compare key

    virtual void store(const ElementValue& s, const int index = -1) = 0;
};

/*!
  @class Element
  @tparam T Type of store value
 */
template<typename T> struct Element : public ElementBase
{
    T* value{}; // Pointer of store target.

    Element(const char* key, T* p) : ElementBase(key), value(p) { assert(p); }
    virtual void store(const ElementValue& ev, const int index = -1) override { _store(ev, index); }

    ///@name Store functions for generic type.
    ///@{
    //! @brief Integer
    template<typename U = T,
             typename std::enable_if<!std::is_same<U, bool>::value && std::is_integral<U>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        *value = !ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10);
    }
    //! @brief Array or pointer of integers
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::decay<U>::type>::value && 
                                     std::is_integral<typename std::remove_pointer<typename std::decay<U>::type>::type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        assert(index >= 0);
        *(*value + index) = !ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10);
    }
    //! @brief std::array<integer, N>
    template<typename U = T,
             typename std::enable_if<is_std_array<U>::value && std::is_integral<typename U::value_type>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        assert(index >= 0);
        (*value)[index] = !ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10);
    }
    //! @brief vector<integer>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_integral<typename U::value_type>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        value->emplace_back(!ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10));
    }
    //! @brief Floating-point
    template<typename U = T,
             typename std::enable_if<std::is_floating_point<U>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev)
    {
        *value = !ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr);
    }
    //! @brief Array or pointer of floating-points */
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::decay<U>::type>::value && 
                                     std::is_floating_point<typename std::remove_pointer<typename std::decay<U>::type>::type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        assert(index >= 0);
        *(*value + index) = !ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr);
    }
    //! @brief std::array<floating-point, N>
    template<typename U = T,
             typename std::enable_if<is_std_array<U>::value && std::is_floating_point<typename U::value_type>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        assert(index >= 0);
        (*value)[index] = !ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr);
    }
    //! @brief vector<floating-point>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_floating_point<typename U::value_type>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        value->emplace_back(!ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr));
    }
    //! @brief Boolean
    template<typename U = T,
             typename std::enable_if<std::is_same<U, bool>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev) { *value = !ev.isString() ? ev.getBool() : (ev.toString() == "true"); }
    //@brief string_t (String or std::string)
    template<typename U = T,
             typename std::enable_if<std::is_same<U, string_t>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev) { *value = ev.getString(); }
    //!@brief  vector<string_t>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_same<typename U::value_type, string_t>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev) { value->emplace_back(ev.getString()); }
    ///@}

    template<std::nullptr_t U = nullptr>
    void _store(...) {}
};
//
}}
#endif
