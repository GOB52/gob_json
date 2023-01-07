/*!
  @file gob_json_element_path.hpp
  @brief JSON key
 */
#ifndef GOB_JSON_ELEMENT_PATH_HPP
#define GOB_JSON_ELEMENT_PATH_HPP

#include <cstdint>
#include <cstddef>
#include "gob_json_typedef.hpp"

namespace goblib { namespace json {
/*
  @class ElementSelector
  @brief Unified element selector.
  Represents the handle associated to an element within either
  an object (key) or an array (index). 
*/
class ElementSelector
{
  public:
    inline int getIndex() const { return index; }
    inline const char* getKey() const { return key; }
    inline bool isObject() const { return index < 0; }
    /*! @brief Builds the string representation of this node position within  its parent. */
    string_t toString() const;

  private:    
    inline void reset() { index = -1; key[0] = '\0'; }
    void set(int index);
    void set(char* key);
    /*
      Advances to next index.
    */
    void step() { ++index; }

  private: 
    int index{-1};
    char key[32]{0,};
    friend class ElementPath;
    friend class StreamingParser;
};

/*
  @class ElementPath
  @brief Hierarchical path to currently parsed element.
  It eases element filtering, keeping track of the current node
  position.  
*/
class ElementPath
{
  public:
    int getCount() const { return count; }

    /*! @brief Gets current element selector. */
    inline const ElementSelector* getCurrent() const { return current; }
    /*! @brief Gets current element selector. */
    inline ElementSelector* getCurrent() { return current; }
    /*! @brief Gets the element selector at the given level. */
    inline const ElementSelector* get(int index) const
    {
        return (index >= count || (index < 0 && (index += count - 1) < 0)) ? nullptr : &selectors[index];
    }        
    /*! @brief Gets parent element selector. */
    const ElementSelector* getParent() const { return get(-1); }
    
    /*! @brief Gets current element's index (in case of array). */
    int getIndex() const { return getIndex(current); }
    int getIndex(const int index) const { return getIndex(get(index)); }

    /*! @brief Gets current element's key (in case of object). */
    const char* getKey() const { return (current != nullptr) ? current->key : "\0"; }
    const char* getKey(int index) const { return getKey(get(index)); }
    
    /*
      Builds the full path corresponding to the current node position.

      For example, "weather[0].id" corresponds to a 3-level hierarchy:
      {
      "weather" : [
      {
      "id" : ..., <===== HERE IT IS
      ... : ...
      },
      { ... }
      ],
      ...
      }
    */    
    string_t toString() const;

  protected:
    int getIndex(const ElementSelector* selector) const { return (selector != nullptr) ? selector->index : -1; }
    const char* getKey(const ElementSelector* selector) const { return (selector != nullptr) ? selector->key : "\0"; }
    void push();
    void pop();

  private:
    int count{0};
    ElementSelector* current{nullptr};
    ElementSelector selectors[20]{};
    friend class StreamingParser;
};

//
}}
#endif
