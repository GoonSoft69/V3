#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "structs.h"

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(Preferences &preferences, QWidget *parent = nullptr);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog *ui;
    Preferences &m_preferences;
};

#endif // PREFERENCESDIALOG_H
