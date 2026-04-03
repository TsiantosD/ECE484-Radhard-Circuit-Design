#!/bin/bash

echo "========================================"
echo "Zipping Project..."
echo "========================================"

# Define the name of the output zip file
ARCHIVE_NAME="Tsiantos_Kalousis_Tsimponidis-Project1_PartA.zip"

# Remove the old archive if it already exists so we don't double-zip
rm -f "$ARCHIVE_NAME"

# Run the zip command
# -r means recursively grab directories
# -x means exclude the following files/patterns
zip -r "$ARCHIVE_NAME" . \
    -x "./Project1_partA.pdf" \
    -x "./zip.sh" \
    -x "./$ARCHIVE_NAME" \
    -x "./outputs/*" \
    -x "./visualizer/venv/*" \
    -x "./visualizer/node_modules/*" \
    -x "*/.git/*"

echo ""
echo "✅ Successfully created $ARCHIVE_NAME"
