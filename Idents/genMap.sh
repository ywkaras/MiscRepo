# Generate unordered map from string to enum from list of identifers.

# $1 = name of map
# $2 = name of enum
# stdin = list
# stdout = map definition with initialization.

# A big step up from this would be to run the list of identifiers through GNU gperf.

rmComments ()
{
sed 's/[ \t]//g' | grep -v '^#' | grep -v '^$'
}

cat << END
#include <string>
#include <unordered_map>

#include "$2.h"

std::unordered_map<std::string, $2> $1 =
{
END
  
rmComments| sed 's/^\(.*\)$/{ "\1", \1 },/' | sed '$s/,$//'

echo '};'
