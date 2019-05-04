#ifndef BUILD2_IN_RULE_HXX
#define BUILD2_IN_RULE_HXX

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/rule.hxx>

namespace build2
{
  namespace moc
  {
    class compile_rule: public build2::rule
    {
    public:
      virtual bool
      match (action, target&, const string& hint) const override;

      virtual recipe
      apply (action, target&) const override;
    };
  }
}

#endif // BUILD2_IN_RULE_HXX
