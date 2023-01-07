/*!
@file gob_json_element_path.cpp
  @brief JSON key
*/

#include "gob_json_element_path.hpp"
#include <cstring>

namespace goblib { namespace json {

// ElementSelector
void ElementSelector::set(int index)
{
  this->index = index;
  this->key[0] = '\0';
}

void ElementSelector::set(char* key)
{
    std::strncpy(this->key, key, sizeof(this->key));
    this->key[sizeof(this->key)-1] = '\0';
    this->index = -1;
}

string_t ElementSelector::toString() const
{
    return (index >= 0) ?
            formatString("%s[%d]", key, index) :
            string_t(key);
}


// ElementPath

void ElementPath::push()
{
    (current = &selectors[count++])->reset();
}


void ElementPath::pop()
{
    if(count > 0)
    {
        current = --count > 0 ? &selectors[count - 1] : nullptr;
    }
}

#if 0
void ElementPath::toString(char* buffer) {
    if (count <= 0)
    return;

  for(int index = 0; index < count; index++) {
    if(index > 0 && selectors[index].isObject()) {
      strcat(buffer, "."); 
    }
    selectors[index].toString(buffer);
  }
}
#else
string_t ElementPath::toString() const
{
    string_t s;
    if(count <= 0) { return s; }

    for(int index = 0; index < count; index++)
    {
        if(index > 0 && selectors[index].isObject())
        {
            s += '.';
        }
        s += selectors[index].toString();
    }
    return s;
}
#endif
//
}}
