#! /bin/sh
TEST="interpolation from sigma-hybrid from pressure"
echo "testing $TEST"
TEST_SRCDIR=$(dirname $0)
OUT_NC="out$$.nc"
./fimex.sh \
    --input.file "${TEST_SRCDIR}/verticalSigmaHybridPressure.nc" \
    --input.config "${TEST_SRCDIR}/verticalSigmaHybrid_tohybrid.ncml" \
    --verticalInterpolate.type=pressure \
    --verticalInterpolate.method=log \
    --verticalInterpolate.templateVar "to_hybrid_template" \
    --output.file="$OUT_NC"
if [ $? != 0 ]; then
  echo "failed $TEST"
  rm -f "$OUT_NC"
  exit 1
fi

EXP_NC="${TEST_SRCDIR}/verticalSigmaHybridFromPressure.nc"
if ./nccmp.sh "$EXP_NC" "$OUT_NC" ; then
  echo "success"
  E=0
else
  echo "failed diff for $TEST"
  E=1
fi
rm -f "$OUT_NC"
exit $E
