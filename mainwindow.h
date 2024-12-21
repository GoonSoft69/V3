#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "structs.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QVideoFrame>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QAudioOutput;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent (QKeyEvent *event) override;

private slots:
    void onPushButtonStartClicked ();
    void onMediaStatusChanged (QMediaPlayer::MediaStatus status);

private:
    void readPreferencess();
    void writePreferences();

    void shuffleInfoList ();
    void next (QMediaPlayer *player);

private:
    Ui::MainWindow *ui;

    QMediaPlayer *m_playerLeft;
    QAudioOutput *m_audioLeft;
    QMediaPlayer *m_playerCenter;
    QAudioOutput *m_audioCenter;
    QMediaPlayer *m_playerRight;
    QAudioOutput *m_audioRight;

    Preferences m_preferences;
    QFileInfoList m_fileInfos;
    QFileInfoList::const_iterator m_fileInfoIter;

    enum PreferencesLocation
    {
        None,
        Registry,
        AppData
    }
    m_preferencesLocation;
};
#endif // MAINWINDOW_H
