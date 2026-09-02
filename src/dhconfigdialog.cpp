#include "dhconfigdialog.h"

#include "dhconfigtemplates.h"

#include <KLocalizedString>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDir>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdesktopservices.h>
#include <qfilesystemwatcher.h>
#include <qlabel.h>
#include <qobject.h>
#include <qpushbutton.h>

DhConfigDialog::DhConfigDialog (KConfigSkeleton *config,
                                const QString &fileName, bool lazyLoading,
                                QWidget *parent)
    : KPageDialog (parent), config (config), fileName (fileName),
      lazyLoading (lazyLoading)
{
  init ();
}

DhConfigDialog::DhConfigDialog (KConfigSkeleton *config, bool lazyLoading,
                                QWidget *parent)
    : KPageDialog (parent), config (config), lazyLoading (lazyLoading)
{
  fileName = qApp->applicationName () + "rc";
  init ();
}

void
DhConfigDialog::init ()
{
  configFileName = QStandardPaths::locate (
      QStandardPaths::GenericConfigLocation, fileName);
  if (!configFileName.isEmpty ())
    watcher = new QFileSystemWatcher ({ configFileName }, this);
  else
    watcher = new QFileSystemWatcher (this);
  connect (watcher, &QFileSystemWatcher::fileChanged, this,
           &DhConfigDialog::configChanged);
  connect (config, &KConfigSkeleton::configChanged, this,
           &DhConfigDialog::configChanged);
  setStandardButtons (QDialogButtonBox::RestoreDefaults
                      | QDialogButtonBox::Apply | QDialogButtonBox::Ok
                      | QDialogButtonBox::Cancel);
  button (QDialogButtonBox::Apply)->setEnabled (false);
  connect (button (QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
           this, &DhConfigDialog::setDefaults);
  connect (button (QDialogButtonBox::Apply), &QPushButton::clicked, this,
           &DhConfigDialog::apply);
  connect (button (QDialogButtonBox::Ok), &QPushButton::clicked, this,
           &DhConfigDialog::apply);
  connect (button (QDialogButtonBox::Cancel), &QPushButton::clicked, this,
           [&]
             {
               for (const auto &i : templates)
                 i->changeConfig ();
             });
  if (!lazyLoading)
    {
      addPages ();
      loaded = true;
    }
}

DhConfigDialog::~DhConfigDialog () {}

void
DhConfigDialog::addAssistant (std::unique_ptr<DhHelpAssistant> &&assistant)
{
  assistant->applyHelp ();
  assistants.emplace_back (std::move (assistant));
}

void
DhConfigDialog::setIcon (const QString &group, const QIcon &icon)
{
  for (const auto &item : items)
    {
      if (item->name () == group)
        {
          item->setIcon (icon);
          break;
        }
    }
}

void
DhConfigDialog::addPages ()
{
  if (loaded)
    return;
  QList<std::pair<QWidget *, QString>> internalWidgets;
  for (const auto &item : config->items ())
    {
      bool added = false;
      for (const auto &widget : internalWidgets)
        {
          if (get<QString> (widget) == item->group ())
            {
              addWidget (item, get<QWidget *> (widget));
              added = true;
            }
        }
      if (!added)
        {
          auto qwidget = new QWidget;
          internalWidgets.append (std::make_pair (qwidget, item->group ()));
          auto layout = new QVBoxLayout (qwidget);
          addWidget (item, qwidget);
        }
    }
  for (const auto &widget : internalWidgets)
    {
      auto realWidget = get<QWidget *> (widget);
      auto realString = get<QString> (widget);
      auto label = new QLabel;
      if (!configFileName.isEmpty ())
        label->setText (configFileName);
      else
        {
          label->setText ("Unknown!");
          labels.append (label);
        }
      label->setStyleSheet ("color:gray;");
      auto btn = new QPushButton ();
      btn->setIcon (QIcon::fromTheme ("folder-open"));
      connect (btn, &QPushButton::clicked, this,
               [this]
                 {
                   if (!configFileName.isEmpty ())
                     QDesktopServices::openUrl (configFileName);
                 });
      auto layout = new QHBoxLayout ();
      layout->addWidget (label);
      layout->addStretch ();
      layout->addWidget (btn);
      qobject_cast<QVBoxLayout *> (realWidget->layout ())->addStretch ();
      qobject_cast<QVBoxLayout *> (realWidget->layout ())->addLayout (layout);
      items.append (addPage (realWidget, i18n (realString.toUtf8 ())));
    }
  loaded = true;
}

void
DhConfigDialog::changeDir (const QString &path)
{
  for (const auto &i : labels)
    i->setText (path);
}

void
DhConfigDialog::addLongTextItems (const QString &str)
{
  longTextItems.append (str);
}

void
DhConfigDialog::addTemplateByItem (KConfigSkeletonItem *item,
                                   const DhTemplateCreator &creator)
{
  itemForTemplates.insert (item, creator);
}

void
DhConfigDialog::addTemplateByType (int type, const DhTemplateCreator &creator)
{
  typeForTemplates.insert (type, creator);
}

void
DhConfigDialog::show ()
{
  if (!loaded)
    addPages ();
  QWidget::show ();
}

void
DhConfigDialog::show (const QString &group)
{
  if (!loaded)
    addPages ();
  for (const auto &item : items)
    if (item->name () == group)
      setCurrentPage (item);
  show ();
}

void
DhConfigDialog::addWidget (KConfigSkeletonItem *item, QWidget *widget)
{
  auto layout = qobject_cast<QVBoxLayout *> (widget->layout ());
  if (itemForTemplates.contains (item))
    {
      auto creator = itemForTemplates.value (item);
      templates.emplace_back (creator (item, layout, this));
    }
  else if (typeForTemplates.contains (item->property ().userType ()))
    {
      auto creator = typeForTemplates.value (item->property ().userType ());
      templates.emplace_back (creator (item, layout, this));
    }
  else
    {
      switch (item->property ().userType ())
        {
        case QMetaType::Int:
          templates.push_back (
              std::make_unique<DhIntConfigTemplate> (item, layout, this));
          break;
        case QMetaType::QString:
          templates.push_back (
              std::make_unique<DhStringConfigTemplate> (item, layout, this));
          break;
        case QMetaType::Bool:
          templates.push_back (
              std::make_unique<DhBoolConfigTemplate> (item, layout, this));
          break;
        case QMetaType::QColor:
          templates.push_back (
              std::make_unique<DhColorConfigTemplate> (item, layout, this));
          break;
        default:
          break;
        }
    }
}

void
DhConfigDialog::apply ()
{
  for (const auto &i : templates)
    i->applyChange ();
  config->save ();
  for (const auto &assistant : assistants)
    assistant->applyHelp ();
  detect ();
}

void
DhConfigDialog::detect () const
{
  bool enable = false;
  for (const auto &i : templates)
    {
      enable = i->detect () ? true : enable;
      if (enable)
        break;
    }
  button (QDialogButtonBox::Apply)->setEnabled (enable);
}

void
DhConfigDialog::configChanged ()
{
  config->load ();
  if (configFileName.isEmpty ())
    {
      configFileName = QStandardPaths::locate (
          QStandardPaths::GenericConfigLocation, fileName);
      if (!configFileName.isEmpty ())
        changeDir (configFileName);
    }
  if (!watcher->files ().contains (configFileName)
      && !configFileName.isEmpty ())
    watcher->addPath (configFileName);
  for (const auto &i : templates)
    i->changeConfig ();
  for (const auto &assistant : assistants)
    assistant->applyHelp ();
}

void
DhConfigDialog::setDefaults ()
{
  for (const auto &i : templates)
    i->setDefault ();
}
