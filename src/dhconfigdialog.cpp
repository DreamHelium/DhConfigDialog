#include "dhconfigdialog.h"
#include <QDir>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <libintl.h>

class DhIntConfigTemplate : public DhConfigTemplate
{
public:
  DhIntConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                       DhConfigDialog *dialog)
      : DhConfigTemplate (item, layout, dialog)
  {
    DhIntConfigTemplate::initWidget (layout, dialog);
  }
  void
  initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) override
  {
    int value = item->property ().toInt ();

    QString label = item->label ();
    QString toolTip = item->toolTip ();
    QLabel *labelWidget = new QLabel (label);
    QSpinBox *spinBox = new QSpinBox ();
    spinBox->setToolTip (toolTip);

    if (item->minValue ().isValid ())
      spinBox->setMinimum (item->minValue ().toInt ());
    if (item->maxValue ().isValid ())
      spinBox->setMaximum (item->maxValue ().toInt ());
    else
      /* The default maximum number is 99 */
      spinBox->setMaximum (INT_MAX);
    spinBox->setValue (value);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget (labelWidget);
    hlayout->addWidget (spinBox);
    layout->addLayout (hlayout);
    widget = spinBox;
    QObject::connect (spinBox, &QSpinBox::valueChanged, dialog,
                      [dialog] { dialog->detect (); });
  }
  void
  applyChange () const override
  {
    int value = qobject_cast<QSpinBox *> (widget)->value ();
    item->setProperty (value);
  }
  [[nodiscard]] bool
  detect () const override
  {
    auto spinBox = qobject_cast<QSpinBox *> (widget);
    auto boxValue = spinBox->value ();
    auto itemValue = item->property ().toInt ();
    if (boxValue != itemValue)
      return true;
    return false;
  }
  void
  setDefault () const override
  {
    int value = item->getDefault ().toInt ();
    qobject_cast<QSpinBox *> (widget)->setValue (value);
  }
  void
  changeConfig () const override
  {
    int value = item->property ().toInt ();
    qobject_cast<QSpinBox *> (widget)->setValue (value);
  }
};

class DhStringConfigTemplate : public DhConfigTemplate
{
public:
  DhStringConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                          DhConfigDialog *dialog)
      : DhConfigTemplate (item, layout, dialog)
  {
    DhStringConfigTemplate::initWidget (layout, dialog);
  }
  void
  initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) override
  {
    auto value = item->property ().toString ();

    QString label = item->label ();
    QString toolTip = item->toolTip ();

    QLabel *labelWidget = new QLabel (label);
    bool useTextEdit = dialog->longTextItems.contains (item->key ());
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget (labelWidget);

    if (useTextEdit)
      {
        auto edit = new QTextEdit (value);
        edit->setToolTip (toolTip);
        hlayout->addWidget (edit);
        widget = edit;
        QObject::connect (edit, &QTextEdit::textChanged, dialog,
                          [dialog] { dialog->detect (); });
      }
    else
      {
        auto edit = new QLineEdit (value);
        edit->setToolTip (toolTip);
        hlayout->addWidget (edit);
        widget = edit;
        QObject::connect (edit, &QLineEdit::textChanged, dialog,
                          [dialog] { dialog->detect (); });
      }
    layout->addLayout (hlayout);
  }
  void
  applyChange () const override
  {
    bool useTextEdit = dialog->longTextItems.contains (item->key ());
    QString value;
    if (useTextEdit)
      value = qobject_cast<QTextEdit *> (widget)->toPlainText ();
    else
      value = qobject_cast<QLineEdit *> (widget)->text ();
    item->setProperty (value);
  }
  [[nodiscard]] bool
  detect () const override
  {
    bool useTextEdit = dialog->longTextItems.contains (item->key ());
    QString value;
    if (useTextEdit)
      value = qobject_cast<QTextEdit *> (widget)->toPlainText ();
    else
      value = qobject_cast<QLineEdit *> (widget)->text ();
    if (item->property ().toString () != value)
      return true;
    return false;
  }
  void
  setDefault () const override
  {
    bool useTextEdit = dialog->longTextItems.contains (item->key ());
    QString value = item->getDefault ().toString ();
    if (useTextEdit)
      qobject_cast<QTextEdit *> (widget)->setPlainText (value);
    else
      qobject_cast<QLineEdit *> (widget)->setText (value);
  }
  void
  changeConfig () const override
  {
    bool useTextEdit = dialog->longTextItems.contains (item->key ());
    QString value = item->property ().toString ();
    if (useTextEdit)
      qobject_cast<QTextEdit *> (widget)->setPlainText (value);
    else
      qobject_cast<QLineEdit *> (widget)->setText (value);
  }
};

DhConfigDialog::DhConfigDialog (KConfigSkeleton *config,
                                const QString &fileName, bool lazyLoading,
                                QWidget *parent)
    : KPageDialog (parent), config (config)
{
  configFileName = QStandardPaths::locate (
      QStandardPaths::GenericConfigLocation, fileName);
  watcher = new QFileSystemWatcher ({ configFileName }, this);
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
  if (!lazyLoading)
    addPages ();
}

DhConfigDialog::~DhConfigDialog () {}

void
DhConfigDialog::addAssistant (const DhHelpAssistant &&assistant)
{
  assistants.append (assistant);
  assistant.applyHelp ();
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
      items.append (addPage (realWidget, gettext (realString.toUtf8 ())));
    }
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
    assistant.applyHelp ();
}

void
DhConfigDialog::detect () const
{
  bool enable = false;
  for (const auto &i : templates)
    enable = i->detect () ? true : enable;
  button (QDialogButtonBox::Apply)->setEnabled (enable);
}

void
DhConfigDialog::configChanged ()
{
  config->load ();
  if (!watcher->files ().contains (configFileName))
    watcher->addPath (configFileName);
  for (const auto &i : templates)
    i->changeConfig ();
}

void
DhConfigDialog::setDefaults ()
{
  for (const auto &i : templates)
    i->setDefault ();
}