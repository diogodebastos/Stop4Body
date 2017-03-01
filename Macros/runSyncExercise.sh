#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ -d ${SYNC_DIR}]] ; then
  rm -Rf ${SYNC_DIR}
fi

mkdir -p ${SYNC_DIR}

cd SyncExercise

qsub SyncExercise.sh

cd -
