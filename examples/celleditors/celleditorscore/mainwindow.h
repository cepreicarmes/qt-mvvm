// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef CELLEDITORSCORE_MAINWINDOW_H
#define CELLEDITORSCORE_MAINWINDOW_H

#include <QMainWindow>
#include <memory>

namespace CellEditors {

class SampleModel;

//! A main window. Contain two sample models and tabs with two model editors.

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void writeSettings();
    void initApplication();
    void populateModel();

    std::unique_ptr<SampleModel> m_model;
};

} // namespace CellEditors

#endif // CELLEDITORSCORE_MAINWINDOW_H
