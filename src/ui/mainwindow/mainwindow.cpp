#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->settings->setLayout(new QFormLayout());

    readSettings();

    connect(this, SIGNAL(proxyReady()), this, SLOT(startLoop()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClient(ThriftClient *tc)
{
    proxy = tc->getProxy();
    emit proxyReady();
}

void MainWindow::on_actionConnectionManager_triggered()
{
    hide();
    deleteLater();

    ConnectionManager *cm;
    cm = new ConnectionManager();
    cm->show();
}

void MainWindow::startLoop()
{
    logOffset = 0;

    QTreeWidgetItem *root = ui->settingsTree->invisibleRootItem();

    QTreeWidgetItem *guiSettings = new QTreeWidgetItem(QStringList("GUI"));
    QTreeWidgetItem *coreSettings = new QTreeWidgetItem(QStringList("Core"));
    QTreeWidgetItem *pluginSettings = new QTreeWidgetItem(QStringList("Plugins"));

    root->addChild(guiSettings);
    root->addChild(coreSettings);
    root->addChild(pluginSettings);

    proxy->getConfig(coresections);
    proxy->getPluginConfig(pluginsections);

    for (uint i = 0; i < coresections.size(); i++)
    {
        ConfigSection section = coresections[i];
        QString desc = QString::fromStdString(section.description);
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(desc));
        item->setData(0, Qt::UserRole, i);
        coreSettings->addChild(item);
    }

    for (uint i = 0; i < pluginsections.size(); i++)
    {
        ConfigSection section = pluginsections[i];
        QString desc = QString::fromStdString(section.description);
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(desc));
        item->setData(0, Qt::UserRole, i);
        pluginSettings->addChild(item);
    }

    ui->settingsTree->expandAll();

    connect(ui->settingsTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(settingsSectionChanged(QTreeWidgetItem*, int)));
}

void MainWindow::settingsSectionChanged(QTreeWidgetItem *item, int column)
{
    QLayoutItem *child;
    while (ui->settings->layout()->count() > 0 && (child = ui->settings->layout()->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    delete ui->settings->layout();
    QFormLayout *layout = new QFormLayout();
    ui->settings->setLayout(layout);

    if (item->parent() != 0)
    {
        ConfigSection section;

        if (item->parent()->data(0, Qt::DisplayRole).toString() == QString("Core"))
        {
            section = coresections[item->data(0, Qt::UserRole).toUInt()];
        }
        else
        {
            section = pluginsections[item->data(0, Qt::UserRole).toUInt()];
        }

        for (uint i = 0; i < section.items.size(); i++)
        {
            ConfigItem confitem = section.items[i];
            QString name = QString::fromStdString(confitem.description);
            if (confitem.type == "int")
            {
                QSpinBox *sp = new QSpinBox(ui->settings);
                sp->setRange(-999999, 999999);
                sp->setValue(QString::fromStdString(confitem.value).toInt());
                layout->addRow(name, sp);
            }
            else if (confitem.type == "bool")
            {
                QComboBox *cb = new QComboBox(ui->settings);
                cb->addItem("Yes", true);
                cb->addItem("No", false);
                if (QString::fromStdString(confitem.value).toLower() == QString("true"))
                {
                    cb->setCurrentIndex(0);
                }
                else
                {
                    cb->setCurrentIndex(1);
                }
                layout->addRow(name, cb);
            }
            else if (QString::fromStdString(confitem.type).indexOf(";") > 0)
            {
                QComboBox *cb = new QComboBox(ui->settings);
                cb->addItems(QString::fromStdString(confitem.type).split(";"));
                cb->setCurrentIndex(cb->findText(QString::fromStdString(confitem.value)));
                layout->addRow(name, cb);
            }
            else
            {
                QLineEdit *edit = new QLineEdit(QString::fromStdString(confitem.value), ui->settings);
                layout->addRow(name, edit);
            }
        }
    }
}

void MainWindow::updateLog()
{
    std::vector<std::string> log;
    proxy->getLog(log, 0);
    for (uint i = 0; i < log.size(); i++)
    {
        QString line = QString::fromStdString(log[i]);
        ui->logBox->appendPlainText(line.left(line.length()-1));
    }
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("window");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settings.setValue("controls/clipboard", ui->actionClipboard->isChecked());
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("window");
    resize(settings.value("size", QSize(660, 500)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    ui->actionClipboard->setChecked(settings.value("controls/clipboard").toBool());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainWindow::on_actionSettings_triggered()
{
    ui->tabWidget->setCurrentIndex(2);
}

