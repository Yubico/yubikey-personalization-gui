#include "scanedittest.h"
#include "yubikeyutiltest.h"
#include "versiontest.h"

int main(int argc, char** argv) {
  int status = 0;

  TestScanEdit scanEdit;
  TestYubikeyUtil yubikeyUtil;
  TestVersion version;

  status |= QTest::qExec(&scanEdit, argc, argv);
  status |= QTest::qExec(&yubikeyUtil, argc, argv);
  status |= QTest::qExec(&version, argc, argv);

  return status;
}
