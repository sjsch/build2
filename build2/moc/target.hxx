#ifndef BUILD2_MOC_TARGET_HXX
#define BUILD2_MOC_TARGET_HXX

#include <build2/types.hxx>
#include <build2/utility.hxx>

#include <build2/target.hxx>

#include <build2/cxx/target.hxx>

namespace build2
{
  namespace moc
  {
    class moc_cxx: public cxx::cxx
    {
    public:
      using cxx::cxx;

    public:
      static const target_type static_type;

      virtual const target_type&
      dynamic_type () const override { return static_type; }
    };
  }
}

#endif // BUILD2_MOC_TARGET_HXX
