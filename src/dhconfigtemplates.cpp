#include "dhconfigtemplates.h"

#include <KColorButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>

DhIntConfigTemplate::DhIntConfigTemplate (KConfigSkeletonItem *item,
                                          QVBoxLayout *layout,
                                          DhConfigDialog *dialog)
    : DhConfigTemplate (item, layout, dialog)
{
  DhIntConfigTemplate::initWidget (layout, dialog);
}

void
DhIntConfigTemplate::initWidget (QVBoxLayout *layout, DhConfigDialog *dialog)
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
DhIntConfigTemplate::applyChange () const
{
  int value = qobject_cast<QSpinBox *> (widget)->value ();
  item->setProperty (value);
}

[[nodiscard]] bool
DhIntConfigTemplate::detect () const
{
  auto spinBox = qobject_cast<QSpinBox *> (widget);
  auto boxValue = spinBox->value ();
  auto itemValue = item->property ().toInt ();
  if (boxValue != itemValue)
    return true;
  return false;
}

void
DhIntConfigTemplate::setDefault () const
{
  int value = item->getDefault ().toInt ();
  qobject_cast<QSpinBox *> (widget)->setValue (value);
}

void
DhIntConfigTemplate::changeConfig () const
{
  int value = item->property ().toInt ();
  qobject_cast<QSpinBox *> (widget)->setValue (value);
}

DhStringConfigTemplate::DhStringConfigTemplate (KConfigSkeletonItem *item,
                                                QVBoxLayout *layout,
                                                DhConfigDialog *dialog)
    : DhConfigTemplate (item, layout, dialog)
{
  DhStringConfigTemplate::initWidget (layout, dialog);
}

void
DhStringConfigTemplate::initWidget (QVBoxLayout *layout,
                                    DhConfigDialog *dialog)
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
DhStringConfigTemplate::applyChange () const
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
DhStringConfigTemplate::detect () const
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
DhStringConfigTemplate::setDefault () const
{
  bool useTextEdit = dialog->longTextItems.contains (item->key ());
  QString value = item->getDefault ().toString ();
  if (useTextEdit)
    qobject_cast<QTextEdit *> (widget)->setPlainText (value);
  else
    qobject_cast<QLineEdit *> (widget)->setText (value);
}

void
DhStringConfigTemplate::changeConfig () const
{
  bool useTextEdit = dialog->longTextItems.contains (item->key ());
  QString value = item->property ().toString ();
  if (useTextEdit)
    qobject_cast<QTextEdit *> (widget)->setPlainText (value);
  else
    qobject_cast<QLineEdit *> (widget)->setText (value);
}

DhBoolConfigTemplate::DhBoolConfigTemplate (KConfigSkeletonItem *item,
                                            QVBoxLayout *layout,
                                            DhConfigDialog *dialog)
    : DhConfigTemplate (item, layout, dialog)
{
  DhBoolConfigTemplate::initWidget (layout, dialog);
}

void
DhBoolConfigTemplate::initWidget (QVBoxLayout *layout, DhConfigDialog *dialog)
{
  bool value = item->property ().toBool ();

  QString label = item->label ();
  QString toolTip = item->toolTip ();
  QCheckBox *checkBox = new QCheckBox (label);
  checkBox->setToolTip (toolTip);

  checkBox->setChecked (value);

  layout->addWidget (checkBox);
  widget = checkBox;
  QObject::connect (checkBox, &QCheckBox::checkStateChanged, dialog,
                    [dialog] { dialog->detect (); });
}

void
DhBoolConfigTemplate::applyChange () const
{
  bool value = qobject_cast<QCheckBox *> (widget)->isChecked ();
  item->setProperty (value);
}

[[nodiscard]] bool
DhBoolConfigTemplate::detect () const
{
  auto checkBox = qobject_cast<QCheckBox *> (widget);
  auto boxValue = checkBox->isChecked ();
  auto itemValue = item->property ().toBool ();
  if (boxValue != itemValue)
    return true;
  return false;
}

void
DhBoolConfigTemplate::setDefault () const
{
  bool value = item->getDefault ().toBool ();
  qobject_cast<QCheckBox *> (widget)->setChecked (value);
}

void
DhBoolConfigTemplate::changeConfig () const
{
  bool value = item->property ().toBool ();
  qobject_cast<QCheckBox *> (widget)->setChecked (value);
}

DhColorConfigTemplate::DhColorConfigTemplate (KConfigSkeletonItem *item,
                                              QVBoxLayout *layout,
                                              DhConfigDialog *dialog)
    : DhConfigTemplate (item, layout, dialog)
{
  DhColorConfigTemplate::initWidget (layout, dialog);
}

void
DhColorConfigTemplate::initWidget (QVBoxLayout *layout, DhConfigDialog *dialog)
{
  auto color = item->property ().value<QColor> ();
  QString label = item->label ();
  auto labelWidget = new QLabel (label);
  QString toolTip = item->toolTip ();
  KColorButton *colorButton = new KColorButton (color);
  colorButton->setDefaultColor (item->getDefault ().value<QColor> ());
  colorButton->setAlphaChannelEnabled (true);

  colorButton->setToolTip (toolTip);
  auto hLayout = new QHBoxLayout();

  hLayout->addWidget (labelWidget);
  hLayout->addWidget (colorButton);
  layout->addLayout (hLayout);
  widget = colorButton;
  QObject::connect (colorButton, &KColorButton::changed, dialog,
                    [dialog] { dialog->detect (); });
}

void
DhColorConfigTemplate::applyChange () const
{
  auto color = qobject_cast<KColorButton *> (widget)->color ();
  item->setProperty (color);
}

bool
DhColorConfigTemplate::detect () const
{
  auto color = qobject_cast<KColorButton *> (widget)->color ();
  auto itemColor = item->property ().value<QColor> ();
  if (color != itemColor)
    return true;
  return false;
}

void
DhColorConfigTemplate::setDefault () const
{
  auto color = item->getDefault ().value<QColor> ();
  qobject_cast<KColorButton *> (widget)->setColor (color);
}

void
DhColorConfigTemplate::changeConfig () const
{
  auto color = item->property ().value<QColor> ();
  qobject_cast<KColorButton *> (widget)->setColor (color);
}
