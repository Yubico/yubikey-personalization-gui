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

asciidoc -s -o $README_FILE README
cd $YUBICO_GITHUB_REPO/yubikey-personalization-gui
cat index.html.in $README_FILE > index.html
echo "</div></body></html>" >> index.html
rm -f $README_FILE
git add index.html
git commit -m "updated page with new README"
echo "Commited new readme, not pushed."
