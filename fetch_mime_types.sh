#!/bin/bash

default_output="mime_types.txt"

if [ $# -ge 1 ]; then
    output_file="$1"
else
    output_file="$default_output"
fi

echo "Output will be saved to: $output_file"

echo "Fetching JSON data..."
curl -s https://raw.githubusercontent.com/patrickmccallum/mimetype-io/refs/heads/master/src/mimeData.json > mimedata_temp.json

if [ $? -ne 0 ]; then
    echo "Error: Failed to download JSON data"
    exit 1
fi

echo "Processing data..."
if command -v jq >/dev/null 2>&1; then
    jq -r '.[] | .fileTypes[] as $ext | ($ext | sub("^\\."; "")) + ": " + .name' mimedata_temp.json > "$output_file"
else
    echo "jq not found, using fallback method (install jq for better results)"

    grep -o '"fileTypes":\[.*\].*"name":"[^"]*"' mimedata_temp.json |
        sed -E 's/"fileTypes":\["\.([^"]*)(",[^]]*\])?.*"name":"([^"]*)"/\1: \3/g' |
        tr ',' '\n' |
        sed -E 's/"\.//g' |
        sed -E 's/"//g' > "$output_file"
fi

rm mimedata_temp.json

echo "Done! Results saved to $output_file"
echo "Found $(wc -l < "$output_file") mime type entries"