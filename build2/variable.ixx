// file      : build2/variable.ixx -*- C++ -*-
// copyright : Copyright (c) 2014-2016 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <type_traits> // is_same

namespace build2
{
  // value
  //
  inline value& value::
  operator= (reference_wrapper<value> v)
  {
    return *this = v.get ();
  }

  inline value& value::
  operator= (reference_wrapper<const value> v)
  {
    return *this = v.get ();
  }

  template <typename T>
  inline value& value::
  operator= (T v)
  {
    assert (type == &value_traits<T>::value_type || type == nullptr);

    // Prepare the receiving value.
    //
    if (type == nullptr)
    {
      if (!null ())
        *this = nullptr;

      type = &value_traits<T>::value_type;
    }

    state = value_traits<T>::assign (*this, move (v))
      ? value_state::filled
      : value_state::empty;

    return *this;
  }

  template <typename T>
  inline value& value::
  operator+= (T v)
  {
    assert (type == &value_traits<T>::value_type ||
            (type == nullptr && null ()));

    // Prepare the receiving value.
    //
    if (type == nullptr)
      type = &value_traits<T>::value_type;

    state = value_traits<T>::append (*this, move (v))
      ? value_state::filled
      : value_state::empty;

    return *this;
  }

  inline bool
  operator!= (const value& x, const value& y)
  {
    return !(x == y);
  }

  template <>
  inline const names&
  cast (const value& v)
  {
    // Note that it can still be a typed vector<names>.
    //
    assert (!v.null () &&
            (v.type == nullptr || v.type == &value_traits<names>::value_type));
    return v.as<names> ();
  }

  template <typename T>
  inline const T&
  cast (const value& v)
  {
    assert (!v.null () && v.type == &value_traits<T>::value_type);
    return *static_cast<const T*> (v.type->cast == nullptr
                                   ? static_cast<const void*> (&v.data_)
                                   : v.type->cast (v));
  }

  template <typename T>
  inline T&
  cast (value& v)
  {
    return const_cast<T&> (cast<T> (static_cast<const value&> (v)));
  }

  template <typename T>
  inline T&&
  cast (value&& v)
  {
    return move (cast<T> (v)); // Forward to T&.
  }

  template <typename T>
  inline T&
  cast (const lookup<value>& l)
  {
    return cast<T> (*l);
  }

  template <typename T>
  inline const T&
  cast (const lookup<const value>& l)
  {
    return cast<T> (*l);
  }

  template <typename T>
  inline void
  typify (value& v, const variable& var)
  {
    value_type& t (value_traits<T>::value_type);

    if (v.type != &t)
      typify (v, t, var);
  }

  inline names_view
  reverse (const value& v, names& storage)
  {
    assert (!v.null () &&
            storage.empty () &&
            (v.type == nullptr || v.type->reverse != nullptr));
    return v.type == nullptr ? v.as<names> () : v.type->reverse (v, storage);
  }

  // value_traits
  //
  template <typename T>
  inline T
  convert (name&& n)
  {
    return value_traits<T>::convert (move (n), nullptr);
  }

  template <typename T>
  inline T
  convert (name&& l, name&& r)
  {
    return value_traits<T>::convert (move (l), &r);
  }

  // bool value
  //
  inline bool value_traits<bool>::
  assign (value& v, bool x)
  {
    if (v.null ())
      new (&v.data_) bool (x);
    else
      v.as<bool> () = x;

    return true;
  }

  inline bool value_traits<bool>::
  append (value& v, bool x)
  {
    // Logical OR.
    //
    if (v.null ())
      new (&v.data_) bool (x);
    else
      v.as<bool> () = v.as<bool> () || x;

    return true;
  }

  inline int value_traits<bool>::
  compare (bool l, bool r)
  {
    return l < r ? -1 : (l > r ? 1 : 0);
  }

  // string value
  //
  inline bool value_traits<string>::
  assign (value& v, string&& x)
  {
    string* p;

    if (v.null ())
      p = new (&v.data_) string (move (x));
    else
      p = &(v.as<string> () = move (x));

    return !p->empty ();
  }

  inline bool value_traits<string>::
  append (value& v, string&& x)
  {
    string* p;

    if (v.null ())
      p = new (&v.data_) string (move (x));
    else
    {
      p = &v.as<string> ();

      if (p->empty ())
        p->swap (x);
      else
        *p += x;
    }

    return !p->empty ();
  }

  inline bool value_traits<string>::
  prepend (value& v, string&& x)
  {
    string* p;

    if (v.null ())
      new (&v.data_) string (move (x));
    else
    {
      p = &v.as<string> ();

      if (!p->empty ())
        x += *p;

      p->swap (x);
    }

    return !p->empty ();
  }

  inline int value_traits<string>::
  compare (const string& l, const string& r)
  {
    return l.compare (r);
  }

  // dir_path value
  //
  inline bool value_traits<dir_path>::
  assign (value& v, dir_path&& x)
  {
    dir_path* p;

    if (v.null ())
      p = new (&v.data_) dir_path (move (x));
    else
      p = &(v.as<dir_path> () = move (x));

    return !p->empty ();
  }

  inline bool value_traits<dir_path>::
  append (value& v, dir_path&& x)
  {
    dir_path* p;

    if (v.null ())
      p = new (&v.data_) dir_path (move (x));
    else
    {
      p = &v.as<dir_path> ();

      if (p->empty ())
        p->swap (x);
      else
        *p /= x;
    }

    return !p->empty ();
  }

  inline bool value_traits<dir_path>::
  prepend (value& v, dir_path&& x)
  {
    dir_path* p;

    if (v.null ())
      new (&v.data_) dir_path (move (x));
    else
    {
      p = &v.as<dir_path> ();

      if (!p->empty ())
        x /= *p;

      p->swap (x);
    }

    return !p->empty ();
  }

  inline int value_traits<dir_path>::
  compare (const dir_path& l, const dir_path& r)
  {
    return l.compare (r);
  }

  // name value
  //
  inline bool value_traits<name>::
  assign (value& v, name&& x)
  {
    name* p;

    if (v.null ())
      p = new (&v.data_) name (move (x));
    else
      p = &(v.as<name> () = move (x));

    return !p->empty ();
  }

  inline int value_traits<name>::
  compare (const name& l, const name& r)
  {
    return l.compare (r);
  }

  // vector<T> value
  //
  template <typename T>
  inline bool value_traits<vector<T>>::
  assign (value& v, vector<T>&& x)
  {
    vector<T>* p;

    if (v.null ())
      p = new (&v.data_) vector<T> (move (x));
    else
      p = &(v.as<vector<T>> () = move (x));

    return !p->empty ();
  }

  template <typename T>
  inline bool value_traits<vector<T>>::
  append (value& v, vector<T>&& x)
  {
    vector<T>* p;

    if (v.null ())
      p = new (&v.data_) vector<T> (move (x));
    else
    {
      p = &v.as<vector<T>> ();

      if (p->empty ())
        p->swap (x);
      else
        p->insert (p->end (),
                   make_move_iterator (x.begin ()),
                   make_move_iterator (x.end ()));
    }

    return !p->empty ();
  }

  template <typename T>
  inline bool value_traits<vector<T>>::
  prepend (value& v, vector<T>&& x)
  {
    vector<T>* p;

    if (v.null ())
      new (&v.data_) vector<T> (move (x));
    else
    {
      p = &v.as<vector<T>> ();

      if (!p->empty ())
        x.insert (x.end (),
                  make_move_iterator (p->begin ()),
                  make_move_iterator (p->end ()));

      p->swap (x);
    }

    return !p->empty ();
  }

  // map<K, V> value
  //
  template <typename K, typename V>
  inline bool value_traits<std::map<K, V>>::
  assign (value& v, map<K, V>&& x)
  {
    map<K, V>* p;

    if (v.null ())
      p = new (&v.data_) map<K, V> (move (x));
    else
      p = &(v.as<map<K, V>> () = move (x));

    return !p->empty ();
  }

  template <typename K, typename V>
  inline bool value_traits<std::map<K, V>>::
  append (value& v, map<K, V>&& x)
  {
    map<K, V>* p;

    if (v.null ())
      p = new (&v.data_) map<K, V> (move (x));
    else
    {
      p = &v.as<map<K, V>> ();

      if (p->empty ())
        p->swap (x);
      else
        // Note that this will only move values. Keys (being const) are still
        // copied.
        //
        p->insert (p->end (),
                   make_move_iterator (x.begin ()),
                   make_move_iterator (x.end ()));
    }

    return !p->empty ();
  }

  inline const variable& variable_pool::
  find (string n, const variable_visibility* vv, const build2::value_type* t)
  {
    auto r (
      insert (
        variable {
          move (n),
          t,
          vv != nullptr ? *vv : variable_visibility::normal}));
    const variable& v (*r.first);

    // Update type?
    //
    if (!r.second && t != nullptr && v.type != t)
    {
      assert (v.type == nullptr);
      const_cast<variable&> (v).type = t; // Not changing the key.
    }

    // Change visibility? While this might at first seem like a bad idea,
    // it can happen that the variable lookup happens before any values
    // were set, in which case the variable will be entered with the
    // default visibility.
    //
    if (!r.second && vv != nullptr && v.visibility != *vv)
    {
      assert (v.visibility == variable_visibility::normal); // Default.
      const_cast<variable&> (v).visibility = *vv; // Not changing the key.
    }

    return v;
  }

  // variable_map::iterator_adapter
  //
  template <typename I>
  inline typename I::reference variable_map::iterator_adapter<I>::
  operator* () const
  {
    auto& r (I::operator* ());
    const variable& var (r.first);
    auto& val (r.second);

    // First access after being assigned a type?
    //
    if (var.type != nullptr && val.type != var.type)
      typify (const_cast<value&> (val), *var.type, var);

    return r;
  }

  template <typename I>
  inline typename I::pointer variable_map::iterator_adapter<I>::
  operator-> () const
  {
    auto p (I::operator-> ());
    const variable& var (p->first);
    auto& val (p->second);

    // First access after being assigned a type?
    //
    if (var.type != nullptr && val.type != var.type)
      typify (const_cast<value&> (val), *var.type, var);

    return p;
  }
}
