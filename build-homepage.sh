#!/bin/sh

if [ "x$YUBICO_GITHUB_REPO" = "x" ]; then
  echo "YUBICO_GITHUB_REPO not defined"
  exit
fi

if [ ! -d $YUBICO_GITHUB_REPO/yubikey-personalization-gui ]; then
  echo "$YUBICO_GITHUB_REPO/yubikey-personalization-gui does not exist"
  exit
fi

README_FILE=`mktemp '/tmp/README.XXXXXX'`
NEWS_FILE=`mktemp '/tmp/NEWS.XXXXXX'`

asciidoc -s -o $README_FILE README
asciidoc -s -o $NEWS_FILE NEWS
cd $YUBICO_GITHUB_REPO/yubikey-personalization-gui
cat index.html.in $README_FILE > index.html
echo "</div></body></html>" >> index.html
cat news.html.in $NEWS_FILE > news.html
echo "</div></body></html>" >> news.html
rm -f $README_FILE $NEWS_FILE
git add index.html
git add news.html
git commit -m "updated page with new README and release notes"
echo "Commited new readme and release notes, not pushed."
