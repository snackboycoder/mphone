#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QPointer>
#include <QMessageBox>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QListWidget>
#include <QTimer>
#include <QMap>
#include <QSet>


#include "adbprocess.h"
#include "../MPhoneCore/include/MPhoneCore.h"
#include "audio/audiooutput.h"

namespace Ui
{
    class Widget;
}

class QYUVOpenGLWidget;
class QGridLayout;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QScrollArea;
class QTabWidget;
class VideoForm;
class Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void outLog(const QString &log, bool newLine = true);
    bool filterLog(const QString &log);
    void getIPbyIp();

private slots:
    void onDeviceConnected(bool success, const QString& serial, const QString& deviceName, const QSize& size);
    void onDeviceDisconnected(QString serial);

    void on_updateDevice_clicked();
    void on_startServerBtn_clicked();
    void on_stopServerBtn_clicked();
    void on_wirelessConnectBtn_clicked();
    void on_startAdbdBtn_clicked();
    void on_getIPBtn_clicked();
    void on_wirelessDisConnectBtn_clicked();
    void on_selectRecordPathBtn_clicked();
    void on_recordPathEdt_textChanged(const QString &arg1);
    void on_adbCommandBtn_clicked();
    void on_stopAdbBtn_clicked();
    void on_clearOut_clicked();
    void on_stopAllServerBtn_clicked();
    void on_recordScreenCheck_clicked(bool checked);
    void on_usbConnectBtn_clicked();
    void on_wifiConnectBtn_clicked();
    void on_connectedPhoneList_itemDoubleClicked(QListWidgetItem *item);
    void on_updateNameBtn_clicked();
    void on_useSingleModeCheck_clicked();
    void on_serialBox_currentIndexChanged(const QString &arg1);

    void on_startAudioBtn_clicked();

    void on_stopAudioBtn_clicked();

    void on_installSndcpyBtn_clicked();

    void on_autoUpdatecheckBox_toggled(bool checked);

    void showIpEditMenu(const QPoint &pos);

private:
    bool checkAdbRun();
    void initUI();
    void updateBootConfig(bool toView = true);
    void execAdbCmd();
    void delayMs(int ms);
    void slotActivated(QSystemTrayIcon::ActivationReason reason);
    int findDeviceFromeSerialBox(bool wifi);
    quint32 getBitRate();
    const QString &getServerPath();
    void loadIpHistory();
    void saveIpHistory(const QString &ip);
    void loadPortHistory();
    void savePortHistory(const QString &port);

    void showPortEditMenu(const QPoint &pos);
    void initMPhoneView();
    void applyMPhoneStyle();
    QString i18n(const QString &key) const;
    void applyMPhoneLanguage();
    void localizeMPhoneUi();
    void rebuildMPhoneConfigPanel();
    void relayoutMPhoneScreens();
    void setMPhoneCard(const QString &serial, QWidget *card);
    void clearMPhoneGrid();
    QWidget *makeStatusCard(const QString &serial, const QString &status, const QString &details = QString());
    QWidget *makeVideoCard(const QString &serial, VideoForm *videoForm);
    QPushButton *makeDeviceActionButton(const QString &label, const QString &tooltip, QWidget *parent);
    void runDeviceAction(const QString &serial, const QString &action);
    void runDeviceActionOnTargets(const QString &sourceSerial, const QString &action);
    void setDeviceSelected(const QString &serial, bool selected);
    void selectAllDeviceCards();
    void clearDeviceSelection();
    void invertDeviceSelection();
    void applyDevicePoolFilter();
    QStringList visibleDeviceSerials() const;
    QStringList selectedDeviceSerials() const;
    void updateFleetSelectionUi();
    void updateFleetMetrics();
    void mirrorMouseInput(const QString &sourceSerial, QEvent::Type type, const QPointF &ratioPos,
                          Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    void mirrorWheelInput(const QString &sourceSerial, const QPointF &ratioPos, const QPoint &angleDelta,
                          Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    void mirrorKeyInput(const QString &sourceSerial, QEvent::Type type, int key, Qt::KeyboardModifiers modifiers,
                        const QString &text, bool autoRepeat, ushort count);
    void showDeviceContextMenu(const QString &serial, VideoForm *videoForm, const QPoint &globalPos);
    void showExpandedDevice(const QString &serial, VideoForm *videoForm);
    void startDeviceServer(const QString &serial);
    void startAllDeviceScreens();
    void pumpDeviceStartQueue();
    void toggleLeftPanel();
    void toggleRightPanel();
    void initBatchScriptPanel();
    QString batchScriptDir() const;
    void loadBatchScriptList();
    void loadBatchScript(const QString &name);
    void saveCurrentBatchScript();
    void deleteCurrentBatchScript();
    void runCurrentBatchScript();
    void runScriptLineForDevice(const QString &serial, const QString &line);
    QStringList selectedBatchScriptDevices() const;
    bool runAdbCommand(const QString &serial, const QStringList &adbArgs, int timeoutMs = 30000);
    bool runAdbShell(const QString &serial, const QStringList &shellArgs);
    bool hasAndroidService(const QString &serial, const QString &serviceName);
    QString mphoneAgentApkPath() const;
    void ensureMPhoneAgent(const QString &serial);
    bool installMPhoneAgent(const QString &serial);
    bool startMPhoneAgent(const QString &serial);
    void applyMPhoneAgentHardening(const QString &serial);
    void showMPhoneAgentStatus(const QString &serial);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::Widget *ui;
    qsc::AdbProcess m_adb;
    QSystemTrayIcon *m_hideIcon;
    QMenu *m_menu;
    QAction *m_showWindow;
    QAction *m_quit;
    AudioOutput m_audioOutput;
    QTimer m_autoUpdatetimer;
    QWidget *m_mphoneContainer = nullptr;
    QScrollArea *m_mphoneScroll = nullptr;
    QGridLayout *m_mphoneGrid = nullptr;
    QWidget *m_devicePoolPanel = nullptr;
    QWidget *m_operationsPanel = nullptr;
    QPushButton *m_toggleLeftButton = nullptr;
    QPushButton *m_toggleRightButton = nullptr;
    QScrollArea *m_rightConfigScroll = nullptr;
    QComboBox *m_languageBox = nullptr;
    QTabWidget *m_operationsTabs = nullptr;
    QLineEdit *m_deviceSearchEdit = nullptr;
    QComboBox *m_deviceFilterBox = nullptr;
    QComboBox *m_scriptTargetBox = nullptr;
    QMap<QString, QWidget*> m_mphoneCards;
    QSet<QString> m_selectedDevices;
    QPushButton *m_selectAllButton = nullptr;
    QPushButton *m_selectNoneButton = nullptr;
    QPushButton *m_invertSelectionButton = nullptr;
    QPushButton *m_syncControlButton = nullptr;
    QLabel *m_fleetStatusLabel = nullptr;
    QLabel *m_totalDevicesMetric = nullptr;
    QLabel *m_runningDevicesMetric = nullptr;
    QLabel *m_failedDevicesMetric = nullptr;
    QLabel *m_selectedDevicesMetric = nullptr;
    QLabel *m_poolSummaryLabel = nullptr;
    QLabel *m_adbStatusLabel = nullptr;
    QLabel *m_queueStatusLabel = nullptr;
    QString m_languageCode = "en";
    bool m_syncControlEnabled = false;
    QStringList m_startQueue;
    QMap<QString, int> m_startAttempts;
    QMap<QString, int> m_recoverAttempts;
    QSet<QString> m_agentCheckedDevices;
    QPointer<QWidget> m_expandedOverlay;
    int m_activeStarts = 0;
    int m_maxConcurrentStarts = 4;
    QGroupBox *m_batchScriptGroup = nullptr;
    QComboBox *m_batchScriptSelector = nullptr;
    QPlainTextEdit *m_batchScriptEditor = nullptr;
    QPushButton *m_newScriptButton = nullptr;
    QPushButton *m_saveScriptButton = nullptr;
    QPushButton *m_deleteScriptButton = nullptr;
    QPushButton *m_runScriptButton = nullptr;
};

#endif // DIALOG_H
