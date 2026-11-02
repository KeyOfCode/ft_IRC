#!/bin/sh



# current_branch=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')
# message="$1"
# branch="$2"

message="${1:-Update files}"

# if [ -z "$message" ]; then
# 	echo "Usage: $0 \"commit message\"" >&2
# 	exit 1
# fi

current_branch=$(git symbolic-ref --quiet --short HEAD)

if [ -z "$current_branch" ]; then
	echo "Error: HEAD is detached; no branch to push." >&2
	exit 1
fi

git add .
git commit -m "$message"
git push origin "$current_branch"
