#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

namespace Ui {
class ImageViewer;
}

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

private slots:
    void dropEvent(QDropEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);

    void on_actionOpen_triggered();

    void on_actionPrint_triggered();

    void on_actionExit_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_actionNormalSize_triggered();

    void on_actionFitToWindow_triggered();

    void on_actionAboutQt_triggered();

    void on_actionSaveAs_triggered();

private:
    Ui::ImageViewer *ui;

    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;

    QAction *openAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutQtAct;

    double scaleFactor;

    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void setImage(const QImage &newImage);
    bool saveFile(const QString &fileName);

   #ifndef QT_NO_PRINTER
       QPrinter printer;
   #endif
};

#endif // IMAGEVIEWER_H
