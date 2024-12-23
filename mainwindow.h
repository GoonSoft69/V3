#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "structs.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onPushButtonStartClicked();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    void readPreferencess();
    void writePreferences();

    void shuffleInfoList();
    void next(QMediaPlayer *player);

    void hideCursor();
    void showCursor();

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
};
#endif // MAINWINDOW_H
