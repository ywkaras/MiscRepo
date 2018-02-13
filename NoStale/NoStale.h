#pragma once

namespace ts
{

// This class helps ensure that users of a dynamically-created object don't use it after the owner has destroyed
// it.  This class has only type and static members.
//
class NoStale
{
public:

  template <typename T>
  class Own;

  template <typename T>
  class Access;

  // This class allows for thread-safe use of owned objects that may have been destroyed.  'Use' instances have
  // an N-to-1 relationship to owned objects.
  //
  template <typename T>
  class Use
  {
  public:
    Use();

    // Retruns true if the instance is not associated with an owned object.  Empty 'Use' objects should not be
    // used to construct 'Access' objects.
    //
    bool empty();

  private:
    friend class NoStale::Own<T>;
    friend class NoStale::Access<T>;

    // Implementation TBD.
  };

  // Safely access an owned object. These objects should only exist in function scope, and as briefly as possible.
  // They may block other threads while they exist, as it "locks" the owned object.  Instances are not
  // moveable/copyable.
  //
  template <typename T>
  class Access
  {
  public:
    // Construct with 'Use' object associated with the owned object to be accessed.
    //
    Access(Use<T> &);

    // Returns true if this object is in a useable state.  If it is usesable, it will stay useable while this
    // object exists, or release() is called.
    //
    bool useable() const;

    // Returns reference to owned object.  Must not be called if this Access object is not useable.
    T &obj();

    // Make this object unuseable, and release lock on owned object.
    //
    void release();

    ~Access();

    Access(const Access &) = delete;
    Access & operator = (const Access &) = delete;

  private:
    // Implementation TBD.
  };

  //
  template <typename T>
  class Own
  {
  public:
    // Construct and optionally specify the owned object.
    //
    Own(T * = nullptr);

    // Returns true if there is no owned object for this instance.  Must not call use/useConst/stopUse for
    // an empty instance.
    //
    bool empty();

    // Returns pointer to owned object for this instance, nullptr if none.  Note that this does not return
    // nullptr because stopUse() has been called.
    //
    T *get();

    // Returns a new 'Use' object that is associated with the owned object.
    //
    Use<T> use();

    // Returns a new  'Use' object that is associated with the owned object, that will only allow 'const' access to
    // the owned object.
    //
    Use<const T> useConst();

    // Stop use of owned object.  This call will block as long as 'Access' objects exist for the owned object.
    //
    void stopUse();

    // Returns true if there is an owned object and stopUse() has been called.
    //
    bool stopped() const;

    Own(const Own &) = delete;
    Own & operator = (const Own &) = delete;
    Own(Own &&) = default;
    Own & operator = (Own &&) = default;

    // Destructor does an ink_release_assert() if there is an owned object and stopUse() has not been called.
    //
    ~Own();

  private:
    // Implementation TBD.
  };

private:
  // Implementation TBD.
};

} // end namespace ts
