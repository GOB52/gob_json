/*!
  @file gob_json_element_value.hpp
  @brief Value of JSON element.
 */
#ifndef GOB_JSON_ELEMENT_VALUE_HPP
#define GOB_JSON_ELEMENT_VALUE_HPP

#include <cstdint>
#include <cstddef>
#include "gob_json_typedef.hpp"

namespace goblib { namespace json {
/*!
  @struct ElementValue
  @brief Structure in which JSON values are stored.
 */
struct ElementValue
{
  public:
    /*! @enum Type Type of value */
    enum class Type : uint8_t
    {
        Null,
        Int,
        Float,
        String,
        Bool,
    };
    ///@name Setter
    ///@{
    /*! @brief From integral value */
    ElementValue with(const long long value)
    {
        data.numValue = value;
        type = Type::Int;
        return *this;
    }
    /*! @brief From floating-point value */
    ElementValue with(const double value)
    {
        data.numValue = value;
        type = Type::Float;
        return *this;
    }
    /*! @brief From boolean value */
    ElementValue with(const bool value)
    {
        data.boolValue = value;
        type = Type::Bool;
        return *this;
    }
    /*! @brief From string value */
    ElementValue with(const char* value)
    {
        data.stringValue = value;
        type = Type::String;
        return *this;
    }
    /*! @brief Null value */
    ElementValue with()
    {
        type = Type::Null;
        return *this;
    }
    ///@}

    ///@name Getter
    ///@warning Note that I have not checked to see if it is the correct type.
    ///@{
    /*! @brief Get the integer value*/
    inline long long  getInt() const     { return data.numValue; }
    /*! @brief Get the floating-point value*/
    inline double getFloat() const       { return data.numValue;}
    /*! @brief Get the boolean value*/
    inline bool getBool() const          { return data.boolValue; }
    /*! @brief Get the string value */
    inline const char* getString() const { return data.stringValue; }
    ///@}

    ///@name Detect type
    ///@{
    /*! @brief Check type */
    inline bool isType(const Type t) const { return type == t; }
    /*! @brief Integer value? */
    inline bool isInt() const    { return isType(Type::Int); }
    /*! @brief Floating-point value? */
    inline bool isFloat() const  { return isType(Type::Float); }
    /*! @brief Boolean value? */
    inline bool isBool() const   { return isType(Type::Bool); }
    /*! @brief String value? */
    inline bool isString() const { return isType(Type::String); }
    /*! @brief Null? */
    inline bool isNull() const   { return isType(Type::Null); }
    ///@}

    /*! @brief Stringify for debug */
    string_t toString() const
    {
        string_t s("?unknown?");
        switch(type)
        {
        case Type::Int:    s = formatString("%lld", getInt()); break;
        case Type::Float:  s = formatString("%lf", getFloat()); break;
        case Type::String: s = formatString("%s", getString()); break;
        case Type::Bool:   s = getBool() ? "true" : "false"; break;
        case Type::Null:   s = "null"; break;
        }
        return s;
    }

  private:
    union Variant
    {
        bool boolValue;
        double numValue{};
        const char* stringValue;
    };
    Variant data{};
    Type type{Type::Null};
};

//
}}
#endif
