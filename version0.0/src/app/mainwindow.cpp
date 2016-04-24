#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"

#include <QSettings>
#include <assert.h>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :

    QMainWindow(parent),
    m_ui(new Ui::MainWindow) {

    this->setMinimumSize(800, 450);
    QGLFormat qglFormat;
    qglFormat.setVersion(4,0);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    m_ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout();

    // setup widget
    m_widget = new GLWidget(qglFormat, this);
    gridLayout->addWidget(m_widget, 0, 1);
    m_ui->canvas_widget->setLayout(gridLayout);
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_widget;
}
