#!/bin/bash
#
# Run every fixture in test/ through pplatex and compare against the expected
# output committed alongside it.
#
#   test/run.sh [<path-to-pplatex>]     check against test/expected/
#   test/run.sh --update [<pplatex>]    rewrite test/expected/ from the binary
#
# Regenerating is expected when a change is meant to alter output. Read the
# resulting diff line by line before committing it: every changed line is either
# the improvement you intended or a regression.

set -u

testdir=$(cd "$(dirname "$0")" && pwd)
expected="$testdir/expected"

update=false
if [ "${1:-}" = "--update" ]; then
    update=true
    shift
fi

# Default to the build directory used by the README, then fall back to PATH.
pplatex=${1:-}
if [ -z "$pplatex" ]; then
    if [ -x "$testdir/../build/src/pplatex" ]; then
	pplatex="$testdir/../build/src/pplatex"
    else
	pplatex=$(command -v pplatex)
    fi
fi

if [ ! -x "$pplatex" ]; then
    echo "no pplatex binary found; build one or pass a path" >&2
    exit 2
fi

mkdir -p "$expected"

failed=0
checked=0

for log in "$testdir"/*.log; do
    name=$(basename "$log" .log)
    want="$expected/$name.txt"

    # pplatex exits non-zero when the log contained errors, which is not a
    # failure of the run itself.
    got=$("$pplatex" -i "$log" 2>&1)

    if $update; then
	printf '%s\n' "$got" > "$want"
	echo "updated $name"
	continue
    fi

    checked=$((checked + 1))

    if [ ! -f "$want" ]; then
	echo "FAIL $name: no expected output, run '$0 --update'"
	failed=$((failed + 1))
    elif ! printf '%s\n' "$got" | diff -u "$want" - > /tmp/pplatex-$name.diff; then
	echo "FAIL $name:"
	sed 's/^/    /' /tmp/pplatex-$name.diff
	failed=$((failed + 1))
    else
	echo "ok   $name"
    fi
done

$update && exit 0

echo
if [ $failed -eq 0 ]; then
    echo "$checked fixtures, all matching"
else
    echo "$checked fixtures, $failed failing"
fi

exit $((failed > 0))
