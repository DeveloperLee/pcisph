#pragma once

#include <QMainWindow>
#include <QList>

class GLWidget;
class DataBinding;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    GLWidget *m_widget;
};

