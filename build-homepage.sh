#!/bin/sh

set -e

README_FILE=`mktemp '/tmp/README.XXXXXX'`
NEWS_FILE=`mktemp '/tmp/NEWS.XXXXXX'`

asciidoc -s -o $README_FILE README
asciidoc -s -o $NEWS_FILE NEWS
git checkout gh-pages
cat index.html.in $README_FILE > index.html
echo "</div></body></html>" >> index.html
cat news.html.in $NEWS_FILE > news.html
echo "</div></body></html>" >> news.html
rm $README_FILE
git add index.html
git commit -m "updated page with new README"
git add news.html
git commit -m "updated page with new release notes"
git checkout master
