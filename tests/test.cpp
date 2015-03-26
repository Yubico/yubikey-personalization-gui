#include "scanedittest.h"
#include "yubikeyutiltest.h"

int main(int argc, char** argv) {
  int status = 0;

  TestScanEdit scanEdit;
  TestYubikeyUtil yubikeyUtil;

  status |= QTest::qExec(&scanEdit, argc, argv);
  status |= QTest::qExec(&yubikeyUtil, argc, argv);

  return status;
}
