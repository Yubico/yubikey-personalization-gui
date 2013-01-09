#!/bin/sh

set -e

VERSION=$1
PGP_KEYID=$2

if [ "x$PGP_KEYID" = "x" ]; then
  echo "try with $0 VERSION PGP_KEYID"
  echo "example: $0 3.0.4 9D1C0E79"
  exit
fi

if ! head -3 NEWS  | grep -q "Version $VERSION .released `date -I`"; then
  echo "You need to update date/version in NEWS"
  exit
fi

if ! head -5 YKPersonalization.pro | grep -q "VERSION.*= \"$VERSION\""; then
  echo "You need to update version in YKPersonalization.pro"
  exit
fi

if ! cat src/version.h | grep -q "#define VER_PRODUCTVERSION.*$VERSION"; then
  echo "You need to update version in src/version.h"
  exit
fi

releasename=yubikey-personalization-gui-${VERSION}

git push
git tag -u ${PGP_KEYID}! -m $VERSION $releasename
git push --tags
tmpdir=`mktemp -d /tmp/release.XXXXXX`
releasedir=${tmpdir}/${releasename}
mkdir -p $releasedir
git archive $releasename --format=tar | tar -xC $releasedir
git2cl > $releasedir/ChangeLog
tar -cz --directory=$tmpdir --file=${releasename}.tar.gz $releasename
gpg --detach-sign --default-key $PGP_KEYID ${releasename}.tar.gz
stagedir=`mktemp -d`
mv ${releasename}.tar.gz.sig $stagedir
mv ${releasename}.tar.gz $stagedir
git checkout gh-pages
mv $stagedir/${releasename}.tar.gz.sig releases/
mv $stagedir/${releasename}.tar.gz releases/
git add releases/${releasename}.tar.gz.sig
git add releases/${releasename}.tar.gz
x=`ls -1 releases/*.tar.gz | awk -F\- '{print $4}' | sed 's/.tar.gz/,/' | paste -sd ' ' - | sed 's/,$//'`; sed -i -e "2s|\[.*\]|[$x]|" releases.html
git add releases.html
git commit -m "Added release $VERSION"
git checkout master
git push origin gh-pages
rm -rf $tmpdir
rm -rf $stagedir
