/*!
  @file gob_json_element.hpp
  @brief Helper for retrieving elements.
 */
#ifndef GOB_JSON_ELEMENT_HPP
#define GOB_JSON_ELEMENT_HPP

#include "gob_json_typedef.hpp"
#include "gob_json_element_value.hpp"
#include "internal/gob_json_log.hpp"
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

// Integer type determination excluding bool.
template <typename T> struct is_integral_exclude_bool
{
    static constexpr bool value = !std::is_same<T,bool>::value && std::is_integral<T>::value;
};

/*!
  @class ElementBase
  @brief Base class
  Call virtual store() => each _store() (SFINAE)
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
  @brief Retriveing helper
  @tparam T Type of store value
 */
template<typename T> struct Element : public ElementBase
{
    T* value{}; // Pointer of store target.

    /*! @brief Constructor
      @param key JSON key
      @param p Pointer of object
     */
    Element(const char* key, T* p) : ElementBase(key), value(p) { assert(p); }
    /*!
      @param ev Value
      @param index <0: Object >=0:Array
     */
    virtual void store(const ElementValue& ev, const int index = -1) override { _store(ev, index); }

    ///@name Store functions for generic type.
    ///@{
    /*! @brief Integer */
    template<typename U = T,
             typename std::enable_if<is_integral_exclude_bool<U>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("int");
        *value = !ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10);
    }
    //! @brief Array or pointer of integers
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::decay<U>::type>::value &&
                                     is_integral_exclude_bool<typename std::remove_pointer<typename std::decay<U>::type>::type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("int a/p %d", index);
        assert(index >= 0);
        *(*value + index) = !ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10);
    }
    //! @brief std::array<integer, N>
    template<typename U = T,
             typename std::enable_if<is_std_array<U>::value &&
                                     is_integral_exclude_bool<typename U::value_type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("int a %d", index);
        assert(index >= 0);
        (*value)[index] = !ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10);
    }
    //! @brief vector<integer>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value &&
                                     is_integral_exclude_bool<typename U::value_type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("int v");
        value->emplace_back(!ev.isString() ? ev.getInt() : std::strtoumax(ev.toString().c_str(), nullptr, 10));
    }

    //! @brief Floating-point
    template<typename U = T,
             typename std::enable_if<std::is_floating_point<U>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("float");
        *value = !ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr);
    }
    //! @brief Array or pointer of floating-points
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::decay<U>::type>::value && 
                                     std::is_floating_point<typename std::remove_pointer<typename std::decay<U>::type>::type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("float a/p");
        assert(index >= 0);
        *(*value + index) = !ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr);
    }
    //! @brief std::array<floating-point, N>
    template<typename U = T,
             typename std::enable_if<is_std_array<U>::value &&
                                     std::is_floating_point<typename U::value_type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("float a");
        assert(index >= 0);
        (*value)[index] = !ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr);
    }
    //! @brief vector<floating-point>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value &&
                                     std::is_floating_point<typename U::value_type>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("float b");
        value->emplace_back(!ev.isString() ? ev.getFloat() : std::strtod(ev.toString().c_str(), nullptr));
    }

    //! @brief bool
    template<typename U = T,
             typename std::enable_if<std::is_same<U, bool>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("bool");        
        *value = !ev.isString() ? ev.getBool() : (ev.toString() == "true");
    }
    //! @brief Array or pointer of bool
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::decay<U>::type>::value && 
                                     std::is_same<typename std::remove_pointer<typename std::decay<U>::type>::type, bool>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("bool a/p %d", index);
        assert(index >= 0);
        *(*value + index) = !ev.isString() ? ev.getBool() : (ev.toString() == "true");
    }
    //! @brief std::array<bool,N>
    template<typename U = T,
             typename std::enable_if<is_std_array<U>::value &&
                                     std::is_same<typename U::value_type, bool>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("bool a %d", index);
        assert(index >= 0);
        (*value)[index] = !ev.isString() ? ev.getBool() : (ev.toString() == "true");
    }
    //! @brief std::vector<bool>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value &&
                                     std::is_same<typename U::value_type, bool>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("bool v");
        value->emplace_back(!ev.isString() ? ev.getBool() : (ev.toString() == "true"));
    }
    
    //@brief string_t (String or std::string)
    template<typename U = T,
             typename std::enable_if<std::is_same<U, string_t>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        //GOB_JSON_LOGD("str");        
        *value = ev.isString() ? ev.getString(): "";
    }
    //! @brief Array or pointer of string_t
    template<typename U = T,
             typename std::enable_if<std::is_pointer<typename std::decay<U>::type>::value && 
                                     std::is_same<typename std::remove_pointer<typename std::decay<U>::type>::type, string_t>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("str a/p %d", index);
        assert(index >= 0);
        *(*value + index) = ev.isString() ? ev.getString(): "";
    }
    //! @brief std::array<string_t, N>
    template<typename U = T,
             typename std::enable_if<is_std_array<U>::value &&
                                     std::is_same<typename U::value_type, string_t>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int index)
    {
        //GOB_JSON_LOGD("str a %d", index);
        assert(index >= 0);
        (*value)[index] = ev.isString() ? ev.getString() : "";
    }
    //!@brief vector<string_t>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value &&
                                     std::is_same<typename U::value_type, string_t>::value,
                                     std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev,const int)
    {
        //GOB_JSON_LOGD("str v");
        value->emplace_back(ev.isString() ? ev.getString(): "");
    }
    ///@}

    //
    template<std::nullptr_t U = nullptr>
    void _store(...)
    {
        assert(0 && "The value cannot be obtained because the appropriate TEMPLATE IS NOT PROVIDED");
    }
};
//
}}
#endif
