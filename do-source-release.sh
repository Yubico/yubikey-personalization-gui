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

if ! head -10 src/src.pro | grep -q "VERSION_MAJOR.*= `echo $VERSION | cut -d. -f1`"; then
  echo "You need to update version in src/src.pro"
  exit
fi
if ! head -10 src/src.pro | grep -q "VERSION_MINOR.*= `echo $VERSION | cut -d. -f2`"; then
  echo "You need to update version in src/src.pro"
  exit
fi
if ! head -10 src/src.pro | grep -q "VERSION_BUILD.*= `echo $VERSION | cut -d. -f3`"; then
  echo "You need to update version in src/src.pro"
  exit
fi

if [ "x$YUBICO_GITHUB_REPO" = "x" ]; then
  echo "you need to define YUBICO_GITHUB_REPO"
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
$YUBICO_GITHUB_REPO/publish yubikey-personalization-gui $VERSION ${releasename}.tar.gz*
rm -rf $tmpdir
rm -rf $stagedir
