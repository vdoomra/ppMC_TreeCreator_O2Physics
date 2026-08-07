#!/bin/bash
FileIn="$1"
JSON="$2"

Base=$(basename "$FileIn" .root)
OutName="EECTree_${Base}"

JobJSON="dpl-config-${Base}.json"
sed "s#\"aod-file-private\": \"[^\"]*\"#\"aod-file-private\": \"${FileIn}\"#" "$JSON" > "$JobJSON"

o2-analysis-trackselection -b --configuration json://$JobJSON | \
o2-analysis-propagationservice -b --configuration json://$JobJSON | \
o2-analysis-multcenttable -b --configuration json://$JobJSON | \
o2-analysis-event-selection-service -b --configuration json://$JobJSON | \
o2-analysis-ft0-corrected-table -b --configuration json://$JobJSON | \
o2-analysis-je-emcal-correction-task   -b --configuration json://$JobJSON | \
o2-analysis-je-emcal-cluster-hadronic-correction-task   -b --configuration json://$JobJSON | \
o2-analysis-je-jet-deriveddata-producer -b --configuration json://$JobJSON | \
o2-analysis-je-jet-finder-mcd-charged  -b --configuration json://$JobJSON | \
o2-analysis-je-jet-finder-mcp-charged  -b --configuration json://$JobJSON | \
o2-analysis-je-jet-matching-mc-ch  -b --configuration json://$JobJSON | \
o2-analysis-je-jet-finder-mcd-full  -b --configuration json://$JobJSON | \
o2-analysis-je-jet-finder-mcp-full  -b --configuration json://$JobJSON | \
o2-analysis-je-jet-luminosity-producer  -b --configuration json://$JobJSON | \
o2-analysis-je-jet-luminosity-calculator  -b --configuration json://$JobJSON | \
o2-analysis-je-eec-tree-creator-mc -b --configuration json://$JobJSON --aod-writer-json OutputDirector.json --aod-writer-resfile $OutName  --aod-file $FileIn --shm-segment-size 16000000000 --min-failure-level error
