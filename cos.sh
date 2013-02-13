#!/bin/bash
if [ -n "$1" ]
then
	line="--------------------"
	echo -e "$line\nWysylamy: $1\n$line"
	IFS=" "
	boundary=$RANDOM
	content="--"$line$boundary"\nContent-Disposition: form-data; name=\"tresc\"\n\n"$1"\n--"$line$boundary"--\n"
	headers="`cat header`"$line$boundary"\nContent-Length: "$((${#content}-6))"\n\n"$content
	result=`echo -e "${headers}" | nc -nvv 156.17.4.3 80`
	echo -e "$line\n$result$line"
else
	echo "You have to enter a string to send!"
fi
