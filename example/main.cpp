#include "../src/dhconfigdialog.h"
#include "settings.h"
#include <QApplication>

int
main (int argc, char *argv[])
{
  QApplication app (argc, argv);
  DhConfigDialog dialog (DhConfig::self (), "example_dhcdrc");
  dialog.show ();
  return app.exec ();
}
