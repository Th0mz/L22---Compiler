#ifndef __L22_TARGETS_SYMBOL_H__
#define __L22_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace l22 {

  class symbol {
    
    // REVIEW : are this properties needed? (_qualifier and _initialized)
    int _qualifier;
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    bool _initialized;
    int _offset = 0;
    long _value; // hack!

  public:
    // REVIEW : contructor with value or without it? 
    symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized, long value) :
      _qualifier(qualifier), _type(type), _name(name), _initialized(initialized), _value(value) {
    }

    
    symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized) :
        _qualifier(qualifier), _type(type), _name(name), _initialized(initialized) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }

    
    int qualifier() const {
      return _qualifier;
    }
    bool initialized() const {
      return _initialized;
    }
  };
  

  inline auto make_symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized, long value) {
    return std::make_shared<symbol>(qualifier, type, name, initialized, value);
  }

  inline auto make_symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized) {
    return std::make_shared<symbol>(qualifier, type, name, initialized);
  }


} // l22

#endif
