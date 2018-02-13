#if INLINE_FUNC_SINGLETON

struct A
{
  static A & getSingleton()
  {
    static A onlyInstance;
    return onlyInstance;
  }

  int dummy;
};

#else

struct A
{
  static A & getSingleton()
  {
    return onlyInstance;
  }

  int dummy;

private:
  static A onlyInstance;
};

#endif

A * inLib();
