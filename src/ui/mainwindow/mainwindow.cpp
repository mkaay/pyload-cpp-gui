#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../eventloop.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->settings->setLayout(new QFormLayout());
    ui->statusBar->addWidget(&downloadStatus);

    addMenu.addAction("Package");
    addMenu.addAction("Links");
    addMenu.addAction("Container", this, SLOT(addContainer()));

    readSettings();

    QClipboard *clipboard = QApplication::clipboard();

    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
    connect(this, SIGNAL(proxyReady()), this, SLOT(startLoop()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClient(ThriftClient *client)
{
    proxy = client->getProxy();
    emit proxyReady();
}

void MainWindow::on_actionConnectionManager_triggered()
{
    hide();

    eventloop->exit();

    emit connectionManager();
}

void MainWindow::updateTrigger()
{
    updateServerStatus();
    updateLog();

    QTimer::singleShot(1000, this, SLOT(updateTrigger()));
}

void MainWindow::startLoop()
{
    logOffset = 0;
    ui->downloads->setModel(&model);

    eventloop = QSharedPointer<EventLoop>(new EventLoop);

    qRegisterMetaType<Pyload::FileData>("Pyload::FileData&");
    qRegisterMetaType<Pyload::PackageData>("Pyload::PackageData&");
    qRegisterMetaType<Pyload::PackageInfo>("Pyload::PackageInfo&");
    qRegisterMetaType<Pyload::DownloadInfo>("Pyload::DownloadInfo&");

    connect(eventloop.data(), SIGNAL(addFile(Pyload::FileData&)), &model, SLOT(addFile(Pyload::FileData&)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(addPackage(Pyload::PackageData&)), &model, SLOT(addPackage(Pyload::PackageData&)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(addPackage(Pyload::PackageInfo&)), &model, SLOT(addPackage(Pyload::PackageInfo&)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(removeFile(int)), &model, SLOT(removeFile(int)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(removePackage(int)), &model, SLOT(removePackage(int)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(updateFile(Pyload::FileData&)), &model, SLOT(updateFile(Pyload::FileData&)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(updatePackage(Pyload::PackageData&)), &model, SLOT(updatePackage(Pyload::PackageData&)), Qt::QueuedConnection);
    connect(eventloop.data(), SIGNAL(updateDownloadStatus(Pyload::DownloadInfo&)), &model, SLOT(updateDownloadStatus(Pyload::DownloadInfo&)), Qt::QueuedConnection);

    eventloop->start();
    eventloop->init();

    ProgressDelegate *delegate = new ProgressDelegate();
    ui->downloads->setItemDelegateForColumn(6, delegate);

    ui->downloads->setColumnWidth(0, 300);
    ui->downloads->setColumnWidth(1, 100);
    ui->downloads->setColumnWidth(2, 140);
    ui->downloads->setColumnWidth(3, 50);
    ui->downloads->setColumnWidth(4, 130);
    ui->downloads->setColumnWidth(5, 70);

    QTimer::singleShot(0, this, SLOT(updateTrigger()));

    //return;

    QTreeWidgetItem *root = ui->settingsTree->invisibleRootItem();

    QTreeWidgetItem *guiSettings = new QTreeWidgetItem(QStringList("GUI"));
    QTreeWidgetItem *coreSettings = new QTreeWidgetItem(QStringList("Core"));
    QTreeWidgetItem *pluginSettings = new QTreeWidgetItem(QStringList("Plugins"));

    root->addChild(guiSettings);
    root->addChild(coreSettings);
    root->addChild(pluginSettings);

    try {
        proxy->getConfig(coresections);
        proxy->getPluginConfig(pluginsections);
    } catch (TApplicationException) {
        qDebug() << "config error";
        return;
    }

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

void MainWindow::settingsSectionChanged(QTreeWidgetItem *item, int)
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
    proxy->getLog(log, logOffset);
    ui->logBox->clear();

    std::string line;
    foreach (line, log) {
        ui->logBox->appendPlainText(QString::fromStdString(line).trimmed());
        logOffset++;
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
    QCoreApplication::quit();
}

void MainWindow::on_actionSettings_triggered()
{

}

void MainWindow::on_actionWiki_triggered()
{
    QDesktopServices::openUrl(QUrl("http://pyload.org/wiki"));
}

void MainWindow::on_actionForum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forum.pyload.org/"));
}

void MainWindow::on_actionPause_triggered()
{
    proxy->togglePause();
    updateServerStatus();
}

void MainWindow::updateServerStatus()
{
    ServerStatus status;
    proxy->statusServer(status);

    if (status.pause) {
        ui->actionPause->setIcon(QIcon(":/images/images/toolbar_start.png"));
    } else {
        ui->actionPause->setIcon(QIcon(":/images/images/toolbar_pause.png"));
    }

    downloadStatus.setText(QString("Speed: %1").arg(QString::number(status.speed/1024.0, 'f', 2)));
}

void MainWindow::clipboardChanged()
{
    if (ui->actionClipboard->isChecked()) {
        QRegExp linkmatch("(http|https|ftp)://[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(([0-9]{1,5})?/.*)?", Qt::CaseInsensitive);

        QString clip = QApplication::clipboard()->text();
        LinkList links;
        int pos = 0;

        while ((pos = linkmatch.indexIn(clip, pos)) != -1) {
            links.resize(links.size()+1);
            links[links.size()-1] = linkmatch.cap().toStdString();
            pos += linkmatch.matchedLength();
        }

        proxy->addPackage("Clipboard", links, Destination::Queue);
    }
}

void MainWindow::on_actionAdd_triggered()
{
    addMenu.exec(QCursor::pos());
}

void MainWindow::addContainer()
{
    QStringList namefilter;
    namefilter << "All Container Types (*.dlc *.ccf *.rsdf *.txt)";
    namefilter << "DLC (*.dlc)";
    namefilter << "CCF (*.ccf)";
    namefilter << "RSDF (*.rsdf)";
    namefilter << "Text Files (*.txt)";

    QSettings settings;

    QFileDialog dialog(this, "Open container");
    dialog.setDirectory(settings.value("browser/container-path", "").toString());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilters(namefilter);

    if (dialog.exec() == QDialog::Accepted) {
        QString file;
        foreach (file, dialog.selectedFiles()) {
            settings.setValue("browser/container-path", file);

            QFile fh(file);
            QFileInfo info(file);
            if (fh.open(QIODevice::ReadOnly)) {
                proxy->uploadContainer(info.fileName().toStdString(), QString(fh.readAll()).toStdString());
                qDebug() << "uplaoded" << info.fileName();
            } else {
                qDebug() << "can't open file" << info.fileName();
            }
        }
    }
}
