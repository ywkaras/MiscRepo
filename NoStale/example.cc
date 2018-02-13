// This is a (very artificial) example of use of NoStale.

#include "NoStale.h"

template <typename T> using Own = ts::NoStale::Own<T>;
template <typename T> using Use = ts::NoStale::Use<T>;
template <typename T> using Access = ts::NoStale::Access<T>;

class Transaction;

class Session
{
public:
  Session() : _own_self(this)
  {
    // Create new session.
  }

  Transaction *startNewTransaction();

  void doSomethingForTransaction(Transaction &);

  void prepareToDeallocate()
  {
    _own_self.stopUse();

    // Do other cleanup stuff.
  }

private:
  Own<Session> _own_self;
};

class Transaction
{
public:
  Transaction(Use<Session> sess) : _sess(sess)
  {
    // Do initialization stuff.
  }

  void handleSomeHook()
  {
    // Do stuff.

    {
      Access<Session> a(_sess);

      if (a.useable()) {
        a.obj().doSomethingForTransaction(*this);
      } else {
        _abort();
        return;
      }
    }

    // Do more stuff.
  }

private:
  Use<Session> _sess;

  void _abort();
};

Transaction *Session::startNewTransaction()
{
  return new Transaction(_own_self.use());
}

void endSession(Session &sess)
{
  sess.prepareToDeallocate();

  // Return session object to heap, class allocator, or wherever it was allocated from.
}
