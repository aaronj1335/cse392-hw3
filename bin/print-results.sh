#!/bin/bash

for f in results/*; do
  echo `basename $f .txt | tr _ ','`,`sed 's/^.*: \(.*\)/\1/' $f`
done
