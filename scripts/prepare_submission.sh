#!/bin/bash

set -euo pipefail

TARGETS=(
  "README.md"
  "Makefile"
  "src"
  "inc"
  "libft"
)

for TARGET in ${TARGETS[@]}; do
  cp -r submit/${TARGET} submit-real/
done
