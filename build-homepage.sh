#!/bin/sh

set -e

README_FILE=`mktemp '/tmp/README.XXXXXX'`
asciidoc -s -o $README_FILE README
git checkout gh-pages
cat index.html.in $README_FILE > index.html
echo "</div></body></html>" >> index.html
rm $README_FILE
git add index.html
git commit -m "updated page with new README"
git checkout master
