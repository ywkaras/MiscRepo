#include <iostream>
#include <cctype>
#include <vector>

namespace
{
enum class State
{
  Skipping,
  Func_name,
  White_pre_paren,
  White_post_paren,
  In_id
};

char const Prefix[] = "DBG_";

char const Func[] = "TSDebug";

State state{State::Skipping};

unsigned func_idx;

std::vector<char> post_white, id;

bool is_white(char c)
{
  return (' ' == c) or ('\t' == c) or ('\n' == c);
}

void add_id(char c)
{
  if (isalnum(c)) {
    id.push_back(toupper(c));

  } else {
    char b{id.back()};
    if ((id.size() > 0) and (b != '_')) {
      id.push_back('_');
    }
  }
}

} // end anonymous namespace

int main()
{
  char c;

  post_white.reserve(128);
  id.reserve(128);

  for (;;)
  {
    std::cin.get(c);
    if (std::cin.eof()) {
      break;
    }
    switch (state)
    {
    case State::Skipping:
    {
      if (Func[0] == c) {
        func_idx = 1;
        state = State::Func_name;

      } else {
        std::cout.put(c);
      }
    }
    break;

    case State::Func_name:
    {
      if (Func[func_idx] == c) {
        ++func_idx;
        if (!Func[func_idx]) {
          state = State::White_pre_paren;
        }
      } else {
        std::cout.write(Func, func_idx);
        state = State::Skipping;
      }
    }
    break;

    case State::White_pre_paren:
    {
      if ('(' == c) {
        post_white.clear();
        state = State::White_post_paren;

      } else if (!is_white(c)) {
        return 1;
      }
    }
    break;

    case State::White_post_paren:
    {
      if (is_white(c)) {
        post_white.push_back(c);

      } else {
        id.clear();
        add_id(c);
        state = State::In_id;
      }
    }
    break;

    case State::In_id:
    {
      if (')' == c) {
        return 1;

      } else if (',' == c) {
        if (id.back() == '_') {
          id.pop_back();
        }
        std::cout.write(Prefix, sizeof(Prefix) - 1);
        std::cout.write(id.data(), id.size());
        std::cout.put('(');

        state = State::Skipping;

      } else {
        add_id(c);
      }
    }
    break;

    } // end switch
  } // end for
  return 0;
}
