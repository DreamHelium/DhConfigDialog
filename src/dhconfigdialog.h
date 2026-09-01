#ifndef DHLRC_DHCONFIGDIALOG_H
#define DHLRC_DHCONFIGDIALOG_H

#include <KConfigSkeleton>
#include <KConfigWatcher>
#include <KPageDialog>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>

class DhConfigTemplate;
class DhConfigDialog;
class DhHelpAssistant
{
public:
  virtual ~DhHelpAssistant () = default;
  virtual void applyHelp () const = 0;
};

using DhTemplateCreator = std::function<std::unique_ptr<DhConfigTemplate> (
    KConfigSkeletonItem *, QVBoxLayout *, DhConfigDialog *)>;

class DhConfigTemplate
{
public:
  DhConfigTemplate (KConfigSkeletonItem *item, QVBoxLayout *layout,
                    DhConfigDialog *dialog)
      : item (item), layout (layout), dialog (dialog)
  {
  }
  virtual ~DhConfigTemplate () = default;
  virtual void initWidget (QVBoxLayout *layout, DhConfigDialog *dialog) = 0;
  virtual void applyChange () const = 0;
  [[nodiscard]] virtual bool detect () const = 0;
  virtual void setDefault () const = 0;
  virtual void changeConfig () const = 0;
  KConfigSkeletonItem *item;
  QVBoxLayout *layout;
  DhConfigDialog *dialog;
  QWidget *widget = nullptr;
  /* Optional */
  QHBoxLayout *hLayout = nullptr;
};

class DhConfigDialog : public KPageDialog
{
  Q_OBJECT
public:
  /* If lazy loading is enabled, you must load pages later. */
  explicit DhConfigDialog (KConfigSkeleton *config, const QString &fileName,
                           bool lazyLoading = false,
                           QWidget *parent = nullptr);
  ~DhConfigDialog () override;
  void addAssistant (std::unique_ptr<DhHelpAssistant> &&assistant);
  void setIcon (const QString &group, const QIcon &icon);
  void addPages ();
  void addLongTextItems (const QString &str);
  QList<QString> longTextItems;
  void addTemplateByItem (KConfigSkeletonItem *item,
                          const DhTemplateCreator &creator);
  void addTemplateByType (int type, const DhTemplateCreator &creator);
  void show ();
  void show (const QString &group);
  std::vector<std::unique_ptr<DhConfigTemplate>> templates;

private:
  bool loaded = false;
  QString fileName;
  KConfigSkeleton *config;
  QString configFileName;
  QFileSystemWatcher *watcher;
  std::vector<std::unique_ptr<DhHelpAssistant>> assistants;
  QList<QString> groups;
  QList<QLabel *> labels;

  /* This comes first */
  QMap<KConfigSkeletonItem *, DhTemplateCreator> itemForTemplates;
  QMap<int, DhTemplateCreator> typeForTemplates;

  // QList<DhConfigTemplate *> templates;
  QList<KPageWidgetItem *> items;

  void addWidget (KConfigSkeletonItem *item, QWidget *widget);

public:
  Q_SLOT void detect () const;

private Q_SLOTS:
  void apply ();
  void configChanged ();
  void setDefaults ();
  void changeDir (const QString &path);
};

#endif // DHLRC_DHCONFIGDIALOG_H
