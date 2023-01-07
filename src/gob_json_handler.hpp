/*!
  @file gob_json_handler.hpp
  @brief Abstract interface class for handling.
 */
#ifndef GOB_JSON_HANDLER_HPP
#define GOB_JSON_HANDLER_HPP

#include "gob_json_element_path.hpp"
#include "gob_json_element_value.hpp"

namespace goblib { namespace json {

/*!
  @class Handler
  @brief Abstract interface class.
  @brief Handling action during parsing.
 */
class Handler
{
  public:
    virtual void startDocument() = 0; //!< Start of JSON document.
    virtual void endDocument() = 0;   //!< End of JSON document.

    virtual void startObject(const ElementPath& path) = 0; //!< Start of JSON object.
    virtual void endObject(const ElementPath& path) = 0;   //!< End of JSON object

    virtual void startArray(const ElementPath& path) = 0; //!< Start of JSON array.
    virtual void endArray(const ElementPath& path) = 0;   //!< End of JSON array.

    virtual void value(const ElementPath& path, const ElementValue& value) = 0; //!< Get the JSON value.
    virtual void whitespace(const char ch) = 0; //!< Whitespace
};
//
}}
#endif
