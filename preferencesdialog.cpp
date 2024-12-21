#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QFileDialog>

PreferencesDialog::PreferencesDialog(Preferences &preferences, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog),
    m_preferences(preferences)
{
    ui->setupUi(this);
    ui->lineEditDirectory->setText(preferences.defaultMediaDirectory);

    auto pushbuttonApply = ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply);

    pushbuttonApply->setDisabled(true);

    connect(ui->pushButtonBrowse, &QPushButton::clicked, this, [this]() {
        QString directory = QFileDialog::getExistingDirectory(this, "Choose default media directory");

        if (directory.isEmpty())
        {
            return;
        }

        ui->lineEditDirectory->setText(directory);
    });

    connect(this, &QDialog::accepted, this, [&preferences, this]() {
        preferences.defaultMediaDirectory = ui->lineEditDirectory->text();
    });

    connect(pushbuttonApply, &QPushButton::clicked, this, [&preferences, this]() {
        preferences.defaultMediaDirectory = ui->lineEditDirectory->text();
        qobject_cast<QPushButton*>(sender())->setDisabled(true);
    });

    connect(ui->lineEditDirectory, &QLineEdit::textChanged, this, [pushbuttonApply]() {
        pushbuttonApply->setEnabled(true);
    });
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}
