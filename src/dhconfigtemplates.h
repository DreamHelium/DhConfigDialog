#ifndef DHLRC_DHCONFIGTEMPLATES_H
#define DHLRC_DHCONFIGTEMPLATES_H

#include "dhconfigdialog.h"

class DhIntConfigTemplate : public DhConfigTemplate
{
public:
  DhIntConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                       DhConfigDialog *dialog);
  void initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) override;
  void applyChange () const override;
  [[nodiscard]] bool detect () const override;
  void setDefault () const override;
  void changeConfig () const override;
};

class DhStringConfigTemplate : public DhConfigTemplate
{
public:
  DhStringConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                          DhConfigDialog *dialog);
  void initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) override;
  void applyChange () const override;
  [[nodiscard]] bool detect () const override;
  void setDefault () const override;
  void changeConfig () const override;
};

class DhBoolConfigTemplate : public DhConfigTemplate
{
public:
  DhBoolConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                          DhConfigDialog *dialog);
  void initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) override;
  void applyChange () const override;
  [[nodiscard]] bool detect () const override;
  void setDefault () const override;
  void changeConfig () const override;
};

class DhColorConfigTemplate : public DhConfigTemplate
{
public:
  DhColorConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                          DhConfigDialog *dialog);
  void initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) override;
  void applyChange () const override;
  [[nodiscard]] bool detect () const override;
  void setDefault () const override;
  void changeConfig () const override;
};

#endif // DHLRC_DHCONFIGTEMPLATES_H
