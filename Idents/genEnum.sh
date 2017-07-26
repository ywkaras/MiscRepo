# Generate enum from list of identifers.

# $1 = name of enum
# stdin = list
# stdout = enum definition

rmComments ()
{
sed 's/[ \t]//g' | grep -v '^#' | grep -v '^$'
}

cat << END
enum $1
{
END
  
rmComments| sed 's/$/,/' | sed '$s/,//'

echo '};'
