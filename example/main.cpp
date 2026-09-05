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
  DhConfigDialog::initDialog (DhConfig::self ());
  DhConfigDialog::instance ()->show ();
  return app.exec ();
}
