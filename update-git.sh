#!/bin/bash

echo "adding root"
git add .

echo "commit message: "
echo $1
git commit -m "$1"

echo "pushing to origin"
git push
