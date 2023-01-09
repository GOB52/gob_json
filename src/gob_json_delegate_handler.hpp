/*!
  @file gob_json_delegate_handler.hpp
  @brief Derived handler using delegate object.
 */
#ifndef GOB_JSON_DELEGATE_HANDLER_HPP
#define GOB_JSON_DELEGATE_HANDLER_HPP

#include "gob_json_handler.hpp"
#include "gob_json_element.hpp"
#include <stack>

namespace goblib { namespace json {

/*!
  @class DelegateHandler
  @brief Handler using delegate object.
*/
class DelegateHandler : public Handler
{
  public:
    /*!
      @class Delegater
      @brief Object to delegate parsing
     */
    struct Delegater
    {
        virtual ~Delegater(){}
        ///@name Delegate functions
        ///@{
        virtual Delegater* startObject(const ElementPath& /*path*/) { return new Delegater(); }
        virtual void endObject(const ElementPath& /*path*/){}
        virtual void startArray(const ElementPath& /*path*/) {}
        virtual void endArray(const ElementPath& /*path*/) {}
        virtual void value(const ElementPath& /*path*/, const ElementValue& /*value*/) {}
        ///@}
    };

    virtual void startDocument() override {}
    virtual void endDocument() override;
    virtual void whitespace(const char /*ch*/) override {}
    virtual void startObject(const ElementPath& path) override;
    virtual void endObject(const ElementPath& path) override;
    virtual void startArray(const ElementPath& path) override;
    virtual void endArray(const ElementPath& path) override;
    virtual void value(const ElementPath& path, const ElementValue& value) override;

  protected:
    Delegater* currentDelegater() const { return !_delegaters.empty() ? _delegaters.top() : nullptr; }
    void pushDelegater(Delegater* o) { _delegaters.push(o); }

  private:
    std::stack<Delegater*> _delegaters;
};
//
}}
#endif
