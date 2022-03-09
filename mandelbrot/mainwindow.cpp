#include "mainwindow.h"
#include "worker.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    /* Set window size */
    this->setMaximumSize(QSize(1920,1280));
    this->setWindowState(Qt::WindowMaximized);
    width = 800;
    height = 800;
    xOrigin = 1.5;
    yOrigin = 1.5;
    /* Init mutex and computation data */
    data = new compData;
    m = new QMutex;
    if(data == nullptr || m == nullptr){
        exit(50);
    }
    data->height = height;
    data->width = width;
    data->p = p;
    data->xOrigin = xOrigin;
    data->yOrigin = yOrigin;
    startWorkInAThread();

    initImage();

    initEditFields();

    initButtons();

    initLayout();

    emit recalculateImage();
}

MainWindow::~MainWindow()
{
    t->requestInterruption();
    t->wait();

    delete image;

    for(int i = 0; i < NUM_OF_BUTTONS; i++){
        delete buttons[i];
    }

    /* QLineEdits */
    delete widthV;
    delete heightV;
    delete widthEdit;
    delete heightEdit;

    delete layout;

    /* Shared computation data */
    delete data;
    delete m;

    /* Thread */
    delete t;

}

void MainWindow::initImage(){
    image = new QLabel(this);
    if(image == nullptr){
        exit(50);
    }
    image->setAlignment(Qt::AlignCenter);
    image->setText("Calculating image, please wait");
}

void MainWindow::initLayout(){
    layout = new QGridLayout(this);
    if(layout == nullptr){
        exit(50);
    }
    /* Place the image */
    layout->addWidget(image, 1, 0, 20, 20);

    /* Place edits for width and height */
    layout->addWidget(widthEdit, 0,0);
    layout->addWidget(heightEdit,0,1);

    /* Place buttons */
    layout->addWidget(buttons[4],0,2);
    layout->addWidget(buttons[0],0,3);
    layout->addWidget(buttons[1],0,4);
    layout->addWidget(buttons[2],0,5);
    layout->addWidget(buttons[3],0,6);

    setLayout(layout);
}

void MainWindow::initButtons(){
    QString buttonNames[NUM_OF_BUTTONS] = {"BlackRed", "BlueRed", "Sepia", "Save", "Set"};
    for(int i = 0; i < NUM_OF_BUTTONS; i++){
        buttons[i] = new QPushButton(buttonNames[i]);
        if(buttons[i] == nullptr){
            exit(50);
        }
        buttons[i]->setMinimumSize(QSize(70,50));
    }
    connect(buttons[0], &QPushButton::clicked, this, &MainWindow::changePalette);
    connect(buttons[1], &QPushButton::clicked, this, &MainWindow::changePalette);
    connect(buttons[2], &QPushButton::clicked, this, &MainWindow::changePalette);
    connect(buttons[3], &QPushButton::clicked, this, &MainWindow::savePicture);
    connect(buttons[4], &QPushButton::clicked, this, &MainWindow::setDimensions);
}

void MainWindow::initEditFields(){
    widthEdit = new QLineEdit(this);
    widthV = new QIntValidator(WIDTH_MIN, WIDTH_MAX, this);
    if(widthEdit == nullptr || widthV == nullptr){
        exit(50);
    }
    widthEdit->setAlignment(Qt::AlignCenter);
    widthEdit->setPlaceholderText("Width");
    widthEdit->setMinimumSize(QSize(70,50));
    widthEdit->setMaximumSize(QSize(70,50));
    widthEdit->setValidator(widthV);

    heightEdit = new QLineEdit(this);
    heightV = new QIntValidator(HEIGHT_MIN, HEIGHT_MAX, this);
    if(heightEdit == nullptr || heightV == nullptr){
        exit(50);
    }
    heightEdit->setAlignment(Qt::AlignCenter);
    heightEdit->setPlaceholderText("Height");
    heightEdit->setMinimumSize(QSize(70,50));
    heightEdit->setMaximumSize(QSize(70,50));
    heightEdit->setValidator(heightV);
}

void MainWindow::startWorkInAThread(){
    t = new CalcThread(data, m);
    if(t == nullptr){
        exit(50);
    }
    connect(t, &CalcThread::resultReady, this, &MainWindow::handleResults);
    connect(this, &MainWindow::recalculateImage, t, &CalcThread::startRecalculation);
    connect(t, &CalcThread::finished, t, &QObject::deleteLater);
    t->start();
    while(!t->isRunning()){};
}

void MainWindow::setDimensions(){
    int temp = widthEdit->text().toInt();
    if(temp >= WIDTH_MIN && temp <= WIDTH_MAX){
        width = temp;
    }
    temp = heightEdit->text().toInt();
    if(temp >= HEIGHT_MIN && temp <= HEIGHT_MAX){
        height = temp;
    }
    m->lock();
    data->width = width;
    data->height = height;
    m->unlock();
    emit recalculateImage();
}

void MainWindow::changePalette(){
    QPushButton *button = (QPushButton *)sender();
    QString text = button->text();
    if(text == "BlackRed"){
        p.currentPalette = p.BLACKRED;
    }else if(text == "BlueRed"){
        p.currentPalette = p.BLUERED;
    }else {
        p.currentPalette = p.SEPIA;
    }
    m->lock();
    data->p = p;
    m->unlock();
    emit recalculateImage();
}

void MainWindow::savePicture(){
    QFile file("fractal.png");
    file.open(QIODevice::WriteOnly);
    currentPix.save(&file, "PNG");
    file.close();
}

void MainWindow::handleResults(){
    QImage im;
    m->lock();
    im = data->im;
    m->unlock();
    currentPix = QPixmap::fromImage(im);
    image->setPixmap(currentPix);
}

void MainWindow::changeOrigin(int key){
    if(key == Qt::Key_8){
        yOrigin -=0.1;
    }else if(key == Qt::Key_2){
        yOrigin += 0.1;
    }else if(key == Qt::Key_4){
        xOrigin -= 0.1;
    }else{
        xOrigin += 0.1;
    }
    m->lock();
    data->xOrigin = xOrigin;
    data->yOrigin = yOrigin;
    m->unlock();
    emit recalculateImage();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_8
        || event->key() == Qt::Key_2
        || event->key() == Qt::Key_4
        || event->key() == Qt::Key_6){
            changeOrigin(event->key());
    }
}
