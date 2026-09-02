#include "../src/dhconfigdialog.h"
#include "settings.h"
#include <KLocalizedString>
#include <QApplication>

int
main (int argc, char *argv[])
{
  QApplication app (argc, argv);
  /* Without this example will complain */
  KLocalizedString::setApplicationDomain ("exampledhcd");
  DhConfigDialog dialog (DhConfig::self ());
  dialog.show ();
  return app.exec ();
}
