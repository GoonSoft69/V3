#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "preferencesdialog.h"

#include <QFileDialog>
#include <QVideoSink>
#include <QAudioOutput>
#include <QKeyEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>
#include <QWindow>

#include <random>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_playerLeft (new QMediaPlayer ()),
    m_audioLeft (new QAudioOutput ()),
    m_playerCenter (new QMediaPlayer ()),
    m_audioCenter (new QAudioOutput ()),
    m_playerRight (new QMediaPlayer ()),
    m_audioRight (new QAudioOutput ()),
    m_preferencesLocation(PreferencesLocation::None)
{
    ui->setupUi(this);
    setWindowTitle("V3");

    m_playerLeft->setVideoOutput(ui->videoLeft);
    ui->videoLeft->hide();
    ui->videoLeft->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    m_playerLeft->setAudioOutput (m_audioLeft);
    connect (m_playerLeft, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    m_audioLeft->setVolume (0.01f);
    m_audioLeft->setMuted (true);

    m_playerCenter->setVideoOutput(ui->videoCenter);
    ui->videoCenter->hide();
    ui->videoCenter->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    m_playerCenter->setAudioOutput (m_audioCenter);
    connect (m_playerCenter, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    m_audioCenter->setVolume (0.01f);

    m_playerRight->setVideoOutput(ui->videoRight);
    ui->videoRight->hide();
    ui->videoRight->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
    m_playerRight->setAudioOutput (m_audioRight);
    connect (m_playerRight, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    m_audioRight->setVolume (0.01f);
    m_audioRight->setMuted (true);

    readPreferencess();

    connect (ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::onPushButtonStartClicked);

    connect(ui->actionPreferences, &QAction::triggered, this, [this]() {
        PreferencesDialog dialog(m_preferences);

        dialog.exec();
    });
}

MainWindow::~MainWindow()
{
    writePreferences();
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key () == Qt::Key::Key_F11)
    {
        if (isFullScreen ())
        {
            showNormal ();
            ui->menuBar->show();
            showCursor();
        }
        else
        {
            showFullScreen ();
            ui->menuBar->hide();

            if (m_playerCenter->playbackState() == QMediaPlayer::PlaybackState::PlayingState)
            {
                hideCursor();
            }
        }
    }
    else if (event->key () == Qt::Key::Key_Escape)
    {
        writePreferences();
        qApp->quit ();
    }
    else if (event->key() == Qt::Key::Key_Control)
    {
        showCursor();
    }
    else if (event->key () == Qt::Key::Key_1)
    {
        m_audioLeft->setMuted (false);
        m_audioCenter->setMuted (true);
        m_audioRight->setMuted (true);
    }
    else if (event->key () == Qt::Key::Key_2)
    {
        m_audioLeft->setMuted (true);
        m_audioCenter->setMuted (false);
        m_audioRight->setMuted (true);
    }
    else if (event->key () == Qt::Key::Key_3)
    {
        m_audioLeft->setMuted (true);
        m_audioCenter->setMuted (true);
        m_audioRight->setMuted (false);
    }

    QMainWindow::keyPressEvent (event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Control)
    {
        if (m_playerCenter->playbackState() == QMediaPlayer::PlaybackState::PlayingState)
        {
            hideCursor();
        }
    }

    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    qDebug() << event->angleDelta();
    auto globalPos = event->globalPosition().toPoint();
    auto localPos = mapFromGlobal(globalPos);
    auto child = childAt(localPos);

    if (child)
    {
        QWidget *container = child;

        while (container && !qobject_cast<QVideoWidget*>(container))
        {
            container = container->parentWidget();
        }

        QVideoWidget *videoWidget = qobject_cast<QVideoWidget*>(container);
        QAudioOutput *audio = nullptr;

        if (videoWidget)
        {
            if (videoWidget == ui->videoLeft)
            {
                audio = m_audioLeft;
            }
            else if (videoWidget == ui->videoCenter)
            {
                audio = m_audioCenter;
            }
            else if (videoWidget == ui->videoRight)
            {
                audio = m_audioRight;
            }

            if (audio)
            {
                if (event->angleDelta().y() > 0)
                {
                    audio->setVolume(audio->volume() + 0.01f);
                }
                else
                {
                    audio->setVolume(audio->volume() - 0.01f);
                }
            }
        }
    }

    QMainWindow::wheelEvent(event);
}

void MainWindow::onPushButtonStartClicked()
{
    QString dirPath;

    if (qApp->keyboardModifiers () == Qt::KeyboardModifier::ControlModifier)
    {
        dirPath = m_preferences.defaultMediaDirectory;

        if (dirPath.isEmpty ())
        {
            QMessageBox::critical(this, "No default directory set", "You haven't set a default media directory yet. "
                                  "Go to File/Preferences to set one.");
            return;
        }
    }
    else
    {
        QStringList videoLocations = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
        dirPath = QFileDialog::getExistingDirectory (this, "Choose video directory",
                                                    videoLocations.isEmpty() ? "" : videoLocations.first());

        if (dirPath.isEmpty ())
        {
            return;
        }
    }    

    QDir dir (dirPath);

    if (!dir.exists())
    {
        QMessageBox::critical(this, "Directory does not exist", QString("The directory '%1' does not exist").arg(dirPath));
        return;
    }

    m_fileInfos = dir.entryInfoList (QDir::Filter::Files | QDir::Filter::NoDotAndDotDot);
    shuffleInfoList ();
    ui->pushButtonStart->hide ();
    ui->videoLeft->show();
    ui->videoCenter->show();
    ui->videoRight->show();
    ui->centralwidget->setStyleSheet("background: black;");
    hideCursor();
    next (m_playerLeft);
    next (m_playerCenter);
    next (m_playerRight);
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::MediaStatus::EndOfMedia)
    {
        next(qobject_cast<QMediaPlayer*>(sender()));
    }
}

void MainWindow::readPreferencess()
{
    QVariant v;
    HKEY key;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\GoonSoft\\V3", 0, KEY_READ, &key) == ERROR_SUCCESS)
    {
        RegCloseKey(key);

        QSettings registry;

        v = registry.value("DefaultMediaDirectory");
        m_preferencesLocation = PreferencesLocation::Registry;
    }
    else
    {
        QFile preferencesIni(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/preferences.ini");

        if (preferencesIni.exists())
        {
            QSettings settings (preferencesIni.fileName(), QSettings::IniFormat);

            v = settings.value("DefaultMediaDirectory");
            m_preferencesLocation = PreferencesLocation::AppData;
        }
    }

    if (v.isValid())
    {
        m_preferences.defaultMediaDirectory = v.toString();
    }
}

void MainWindow::writePreferences()
{
    if (m_preferences.defaultMediaDirectory.isEmpty())
    {
        return;
    }

    if (m_preferencesLocation == None)
    {
        QMessageBox msgb(this);

        msgb.setIcon(QMessageBox::Icon::Question);
        msgb.setWindowTitle("Select preferences save location");
        msgb.setText("Where do you want to save your preferences?");
        QPushButton *buttonRegistry = new QPushButton("Registry");
        QPushButton *buttonAppData = new QPushButton("AppData");
        QPushButton *buttonCancel = new QPushButton("Don't Save");
        msgb.addButton(buttonRegistry, QMessageBox::ButtonRole::AcceptRole);
        msgb.addButton(buttonAppData, QMessageBox::ButtonRole::AcceptRole);
        msgb.addButton(buttonCancel, QMessageBox::ButtonRole::RejectRole);
        msgb.setDefaultButton(buttonRegistry);
        msgb.setEscapeButton(buttonCancel);
        showCursor();
        msgb.exec();
        auto clicked = msgb.clickedButton();

        if (clicked == buttonRegistry)
        {
            m_preferencesLocation = Registry;
        }
        else if (clicked == buttonAppData)
        {
            m_preferencesLocation = AppData;
        }
        else
        {
            return;
        }
    }

    switch(m_preferencesLocation)
    {
    case None:
        break;

    case Registry:
    {
        QSettings registry;

        registry.setValue("DefaultMediaDirectory", m_preferences.defaultMediaDirectory);
        break;
    }
    case AppData:
    {
        QSettings settings (QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/preferences.ini",
                           QSettings::IniFormat);
        settings.setValue("DefaultMediaDirectory", m_preferences.defaultMediaDirectory);
        break;
    }
    }

}

void MainWindow::shuffleInfoList()
{
    std::random_device dev;
    std::mt19937 rng(dev());

    std::shuffle (m_fileInfos.begin (), m_fileInfos.end (), rng);
    m_fileInfoIter = m_fileInfos.constBegin ();
}

void MainWindow::next(QMediaPlayer *player)
{
    if (m_fileInfoIter == m_fileInfos.constEnd ())
    {
        shuffleInfoList ();
    }

    while (m_fileInfoIter->suffix ().toUpper () != "MP4" && m_fileInfoIter->suffix ().toUpper () != "MOV")
    {
        ++m_fileInfoIter;
    }

    player->setSource (m_fileInfoIter->absoluteFilePath ());
    player->play ();
    ++m_fileInfoIter;
}

void MainWindow::hideCursor()
{
    QCursor blank(Qt::BlankCursor);

    setCursor(blank);
}

void MainWindow::showCursor()
{
    unsetCursor();
}
