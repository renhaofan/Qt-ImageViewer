#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintDialog>

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImageViewer)
{
    ui->setupUi(this);

    this->setAcceptDrops(true);

    openAct = ui->actionOpen;
    saveAsAct = ui->actionSaveAs;
    printAct = ui->actionPrint;
    exitAct = ui->actionExit;
    zoomInAct = ui->actionZoomIn;
    zoomOutAct = ui->actionZoomOut;
    normalSizeAct = ui->actionNormalSize;
    fitToWindowAct = ui->actionFitToWindow;
    aboutQtAct = ui->actionAboutQt;

    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    QFont font("Microsoft YaHei", 10, 75);
    imageLabel->setFont(font);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("Start by dragging images here");

//    scrollArea = new QScrollArea;
    scrollArea = ui->scrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);

    setCentralWidget(scrollArea);

    setWindowTitle(tr("Image Viewer"));
    resize(500, 400);

//    connect(openAct,SIGNAL(triggered()),this,SLOT(open()));
//    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));
//    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
//    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
//    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
//    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));
//    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
//    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(about()));
}

ImageViewer::~ImageViewer()
{
    delete ui;
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

bool ImageViewer::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


void ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->type() != QEvent::DragEnter) return;
    // for linux:event->mimeData()->urls()[0].toLocalFile().right(3).compare("bmp")
    if (!event->mimeData()->urls()[0].toLocalFile().right(3).compare("bmp") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("jpg") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("pbm") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("pgm") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("png") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("ppm") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("xbm") ||
        !event->mimeData()->urls()[0].toLocalFile().right(3).compare("xpm"))
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void ImageViewer::dropEvent(QDropEvent *event) {
    if (event->type() != QEvent::Drop) return;
    QList<QUrl> urls = event->mimeData()->urls();
    QString fileName = urls.first().toLocalFile();
    if (!fileName.isEmpty()) {
        image = QImage(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        imageLabel->setPixmap(QPixmap::fromImage(image));
        scaleFactor = 1.0;

        printAct->setEnabled(true);
        fitToWindowAct->setEnabled(true);
        updateActions();

        if (!fitToWindowAct->isChecked())
            imageLabel->adjustSize();
        on_actionFitToWindow_triggered(); // fit to window
    }
}

void ImageViewer::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                     tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
         image = QImage(fileName);
         if (image.isNull()) {
             QMessageBox::information(this, tr("Image Viewer"),
                                      tr("Cannot load %1.").arg(fileName));
             return;
         }
         imageLabel->setPixmap(QPixmap::fromImage(image));
         scaleFactor = 1.0;

         printAct->setEnabled(true);
         fitToWindowAct->setEnabled(true);
         updateActions();

         if (!fitToWindowAct->isChecked())
             imageLabel->adjustSize();
         on_actionFitToWindow_triggered(); // fit to window
    }
}

void ImageViewer::on_actionPrint_triggered()
{
    Q_ASSERT(imageLabel->pixmap());
#ifndef QT_NO_PRINTER
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}



void ImageViewer::on_actionExit_triggered()
{
    this->close();
}



void ImageViewer::on_actionZoomIn_triggered()
{
    scaleImage(1.25);
}

void ImageViewer::on_actionZoomOut_triggered()
{
    scaleImage(0.8);
}

void ImageViewer::on_actionNormalSize_triggered()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::on_actionFitToWindow_triggered()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        on_actionNormalSize_triggered();
    }
    updateActions();
}

void ImageViewer::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}

void ImageViewer::on_actionSaveAs_triggered()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}
