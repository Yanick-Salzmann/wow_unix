set(MIME_DATA_URL "https://raw.githubusercontent.com/patrickmccallum/mimetype-io/refs/heads/master/src/mimeData.json")
set(TEMP_JSON_FILE "${OUTPUT_FILE}.tmp.json")

message(STATUS "Fetching MIME types data...")
file(DOWNLOAD "${MIME_DATA_URL}" "${TEMP_JSON_FILE}" STATUS DOWNLOAD_STATUS)

list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
if(NOT STATUS_CODE EQUAL 0)
    list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
    message(FATAL_ERROR "Failed to download MIME data: ${ERROR_MESSAGE}")
endif()

message(STATUS "Processing MIME types...")
file(READ "${TEMP_JSON_FILE}" JSON_CONTENT)

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_FOUND)
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "-c" "
import json
import sys

with open(sys.argv[1], 'r', encoding='utf-8') as f:
    data = json.load(f)

lines = []
for entry in data:
    mime_name = entry.get('name', '')
    file_types = entry.get('fileTypes', [])
    for ext in file_types:
        ext_clean = ext.lstrip('.')
        lines.append(f'{ext_clean}: {mime_name}')

with open(sys.argv[2], 'w', encoding='utf-8') as f:
    f.write('\\n'.join(lines) + '\\n')

print(f'Found {len(lines)} mime type entries')
" "${TEMP_JSON_FILE}" "${OUTPUT_FILE}"
        RESULT_VARIABLE PYTHON_RESULT
        OUTPUT_VARIABLE PYTHON_OUTPUT
        ERROR_VARIABLE PYTHON_ERROR
    )
    if(NOT PYTHON_RESULT EQUAL 0)
        message(FATAL_ERROR "Python processing failed: ${PYTHON_ERROR}")
    endif()
else()
    message(FATAL_ERROR "Python3 is required to process MIME types")
endif()

file(REMOVE "${TEMP_JSON_FILE}")
message(STATUS "Done! Results saved to ${OUTPUT_FILE}")

