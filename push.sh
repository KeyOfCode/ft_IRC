#!/bin/sh

current_branch=$(git branch | sed -n -e 's/^\* \(.*\)/\1/p')
message="$1"
branch="$2"


git add .
git commit -m "$message"
git push origin "$current_branch"
