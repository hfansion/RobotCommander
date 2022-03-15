#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "settings.h"
#include "updater.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialog; }
class QAbstractButton;
class QIntValidator;
class QTranslator;
QT_END_NAMESPACE

// 设置窗口类，同时管理设置信息的存储
class SettingsDialog : public QDialog {
Q_OBJECT

public:
    enum class Page {
        General=0, SerialPort=1, Appearance=2, Update=3
    };
    static QMap<Page, QString> PAGE_NAME;

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

    // 返回一个指向设置内容的常量指针
    [[nodiscard]] const Settings *settings() const;
    void showPage(Page page);
    void checkUpdateResult(Updater::Result result);

signals:
    void needUpdateSettings(const Settings *settings);
    void needCheckForUpdate();

public slots:
    void showPage(int index);

private slots:
    void showPortInfo(int idx);
    void apply();
    void cancel();
    void buttonBoxClicked(QAbstractButton *button);
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

    void mod_RCP_widget_change(bool state);
    void mod_RTP_widget_change(bool state);
    void chooseFile_Map();
    void chooseFile_RCP();
    void chooseFile_RTP();
    void chooseColor_RCP();
    void chooseColor_RTP();

    void fillPortsInfo();

    void showLanguageSelected(int index);
    void adjustPreviewPic();
    void checkForUpdate();

private:
    void fillPortsParameters();
    void switchLanguage();
    void recoverSettings();
    void updateSettings();

private:
    Ui::SettingsDialog *ui;
    Settings m_settings;
    QIntValidator *m_intValidator = nullptr;
    QTranslator *m_translator;
};

#endif // SETTINGSDIALOG_H
