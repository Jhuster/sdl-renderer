#!/bin/sh

astyle --style=linux -nHpUc --align-pointer=type -r "*.cpp" "*.hpp" "*.h"  --exclude=build-deps --exclude=deps --exclude=output