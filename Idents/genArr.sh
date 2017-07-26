# Generate string array from list of identifers.

# $1 = name of array
# stdin = list
# stdout = array defintion with initialization

rmComments ()
{
sed 's/[ \t]//g' | grep -v '^#' | grep -v '^$'
}

cat << END
const char * const $1[]
{
END
  
rmComments| sed 's/^/"/' | sed 's/$/",/' | sed '$s/,//'

echo '};'
