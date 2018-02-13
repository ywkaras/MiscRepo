#if defined(_cpp_lib_hardware_interference_size)
std::size_t const CACHE_LINE_SIZE_LCM{std::hardware_desctructive_interference_size};
#else
// Least common multiple of cache line size of architectures ATS will run on.
//
std::size_t const CACHE_LINE_SIZE_LCM{128};
#endif

template <typename T>
class CacheAligned
{
public:
  template <typename ...Arg_type>
  explicit CacheAligned(Arg_type&&... arg) : _v{std::forward<Arg_type>(arg)...} {}

  operator T & () { return _v; }
  T & operator () () { return _v; }
  operator T const & () const { return _v; }
  T const & operator () () const { return _v; }

private:
  static std::size_t const _Cls{CACHE_LINE_SIZE_LCM};
  alignas(_Cls) T _v;
  char _pad[sizeof(T) + ((sizeof(T) % _Cls) ? (_Cls - (sizeof(T) % _Cls)) : 0)];
};
