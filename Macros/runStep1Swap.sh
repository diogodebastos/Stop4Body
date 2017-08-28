#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${SWAP_DIR} ]] ; then
  mkdir -p ${SWAP_DIR}
fi

python buildAndSubmitJobs.py -o ${SWAP_DIR} -s

echo "Run the following command to check the progress of the output and resubmit failed jobs:"
echo "  python checkJobs.py -o ${SWAP_DIR}"
