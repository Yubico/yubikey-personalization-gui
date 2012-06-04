#!/bin/sh

set -e

VERSION=$1
USER=$2
PGP_KEYID=$3

if [ "x$PGP_KEYID" = "x" ]; then
  echo "try with $0 VERSION GOOGLECODE_USER PGP_KEYID"
  echo "example: $0 3.0.4 klas@yubico.com 9D1C0E79"
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
find $tmpdir -name '*.dll' -print0 | xargs -0 rm -f
tar -cz --directory=$tmpdir --file=${releasename}.tar.gz $releasename
gpg --detach-sign --default-key $PGP_KEYID ${releasename}.tar.gz
googlecode_upload.py -s "OpenPGP signature for ${releasename}." -p yubikey-personalization -u $USER ${releasename}.tar.gz.sig
googlecode_upload.py -s "$releasename" -p yubikey-personalization -u $USER ${releasename}.tar.gz
rm -rf $releasedir
