#include "mainwindow.h"
#include "worker.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    /* Set window size */
    this->setMaximumSize(QSize(1920,1280));
    this->setWindowState(Qt::WindowMaximized);

    initVariables();

    initcompData();

    startWorkInAThread();

    initImage();

    initEditFields();

    initButtons();

    initLayout();

    threadsReady = false;
    emit recalculateImage();
}

/* Delete all allocated memory */
MainWindow::~MainWindow()
{
    for(int i = 0; i < NUM_OF_CALC_THREADS; i++){
        t[i]->requestInterruption();
        t[i]->wait();
    }

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
    for(int i = 0; i < NUM_OF_CALC_THREADS; i++){
        delete t[i];
    }

}

/* Creates mutex and initializes data struct
 * used in the computation.
 */
void MainWindow::initcompData(){
    data = new compData;
    m = new QMutex;
    if(data == nullptr || m == nullptr){ // check allocation
        exit(50);
    }
    data->height = height;
    data->width = width;
    data->p = p;
    data->xOffset = xOffset;
    data->yOffset = yOffset;
    data->zoom = zoom;
    data->im = QImage(width, height, QImage::Format_RGB888);
}

/* Initializes class variables to thier starting values*/
void MainWindow::initVariables(){
    width = BASIC_WIDTH;
    height = BASIC_HEIGHT;
    xOffset = 0;
    yOffset = 0;
    zoom = 0;
    numOfThreadsFinished = 0;
    threadsReady = true;
}

/* QLabel init, QLabel is used to show the calculated image */
void MainWindow::initImage(){
    image = new QLabel(this);
    if(image == nullptr){
        exit(50);
    }
    image->setAlignment(Qt::AlignCenter);
    image->setText("Calculating image, please wait");
}

/* Places the used widgets into layout on the screen */
void MainWindow::initLayout(){
    layout = new QGridLayout(this);
    if(layout == nullptr){
        exit(50);
    }
    /* Place the image */
    layout->addWidget(image, 1, 0, 20, 20);

    /* Place edits for width and height */
    layout->addWidget(widthEdit, 0,3);
    layout->addWidget(heightEdit,0,4);

    /* Place buttons */
    layout->addWidget(buttons[4],0,5);
    layout->addWidget(buttons[0],0,0);
    layout->addWidget(buttons[1],0,1);
    layout->addWidget(buttons[2],0,2);
    layout->addWidget(buttons[3],0,6);

    setLayout(layout);
}

/* Create buttons, add text and thier signal connections */
void MainWindow::initButtons(){
    QString buttonNames[NUM_OF_BUTTONS] = {"BlackRed", "BWRGB", "Sepia", "Save", "Set"};
    for(int i = 0; i < NUM_OF_BUTTONS; i++){
        buttons[i] = new QPushButton(buttonNames[i]);
        if(buttons[i] == nullptr){ // check allocation
            exit(50);
        }
        buttons[i]->setMinimumSize(QSize(70,50));
    }
    connect(buttons[0], &QPushButton::clicked, this, &MainWindow::changePalette);//add signals to each button
    connect(buttons[1], &QPushButton::clicked, this, &MainWindow::changePalette);
    connect(buttons[2], &QPushButton::clicked, this, &MainWindow::changePalette);
    connect(buttons[3], &QPushButton::clicked, this, &MainWindow::savePicture);
    connect(buttons[4], &QPushButton::clicked, this, &MainWindow::setDimensions);
}

/* Create edit fields where the user can input width and height */
void MainWindow::initEditFields(){
    widthEdit = new QLineEdit(this);
    widthV = new QIntValidator(WIDTH_MIN, WIDTH_MAX, this);
    if(widthEdit == nullptr || widthV == nullptr){ // check allocation
        exit(50);
    }
    widthEdit->setAlignment(Qt::AlignCenter); // set line edit properties - text, size, validator
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

/* Starts the threads that run the fractal calculation */
void MainWindow::startWorkInAThread(){
    for(int i = 0; i < NUM_OF_CALC_THREADS; i++){
        t[i] = new CalcThread(data, m, i);
        if(t[i] == nullptr){ // check if memory allocation was successful
            exit(50);
        }
        connect(t[i], &CalcThread::resultReady, this, &MainWindow::handleResults); // add signal connections
        connect(this, &MainWindow::recalculateImage, t[i], &CalcThread::startRecalculation);
        connect(t[i], &CalcThread::finished, t[i], &QObject::deleteLater);
        t[i]->start();
        while(!t[i]->isRunning()){};//wait while thread is starting
    }
}

/* Changes the wdith and height of the window with the fractal
   based on the user input in edit fields.
*/
void MainWindow::setDimensions(){
    int temp = widthEdit->text().toInt(); // get text from line edits and convert to int
    if(temp >= WIDTH_MIN && temp <= WIDTH_MAX){ // check if the within allowed window
        width = temp;
    }
    temp = heightEdit->text().toInt();
    if(temp >= HEIGHT_MIN && temp <= HEIGHT_MAX){
        height = temp;
    }
    m->lock();//update comp. data
    data->width = width;
    data->height = height;
    data->im = QImage(width, height, QImage::Format_RGB888); // create new image of size width x height
    m->unlock();
    if(threadsReady){//recalculate
        threadsReady = false;
        emit recalculateImage();
    }
}

/* Changes the currently used palette. This function is called by button press. */
void MainWindow::changePalette(){
    QPushButton *button = (QPushButton *)sender();
    QString text = button->text();
    if(text == "BlackRed"){ // change palette according to the button text
        p.currentPalette = p.BLACKRED;
    }else if(text == "BWRGB"){
        p.currentPalette = p.BWRGB;
    }else {
        p.currentPalette = p.SEPIA;
    }
    m->lock();// update calculation data struct
    data->p = p;
    m->unlock();
    if(threadsReady){//recalculate
        threadsReady = false;
        emit recalculateImage();
    }
}

/* Saves picture of the currently shown image into "fractal.png" */
void MainWindow::savePicture(){
    QFile file("fractal.png");
    file.open(QIODevice::WriteOnly);
    currentPix.save(&file, "PNG");
    file.close();
}

/* Handles the results from the calculation threads */
void MainWindow::handleResults(){
    numOfThreadsFinished++;
    if(numOfThreadsFinished == NUM_OF_CALC_THREADS){ //when all theads finish calculating update the image
        QImage im;
        m->lock();
        im = data->im;
        m->unlock();
        currentPix = QPixmap::fromImage(im);
        image->setPixmap(currentPix);
        numOfThreadsFinished = 0;
        threadsReady = true;
    }
}

/* If one of the following keys is pressed: 8,2,4,6, move the origin
    of the calulation in the correct direction.
*/
void MainWindow::changeOrigin(int key){
    m->lock();
    yStep = data->yStep;
    xStep = data->xStep;
    m->unlock();
    if(threadsReady){
        if(key == Qt::Key_2){
            yOffset += 10*yStep; // move the origin by a number of pixels
        }else if(key == Qt::Key_8){//(*yStep) so the jumps are aways scaled
            yOffset -= 10*yStep;
        }else if(key == Qt::Key_6){
            xOffset -= 10*xStep;
        }else{
            xOffset += 10*xStep;
        }
        m->lock();
        data->xOffset = xOffset;
        data->yOffset = yOffset;
        m->unlock();
        threadsReady = false;
        emit recalculateImage();// recalculate to show the changes
    }
}

/* If +/- is pressed, zoom in/out on the image */
void MainWindow::changeZoom(int key){
    if(threadsReady){
        if(key == Qt::Key_Plus){
            zoom -= 0.05; //zoom value
            if(zoom < -1.5){// 1.5 is the width of the used complex field
                zoom = -1.499;//so the zoom must be smaller
            }
        }else{
            zoom += 0.05;
        }
        m->lock();
        data->zoom = zoom; // update calc. data
        m->unlock();
        threadsReady = false;
        emit recalculateImage(); // recalculate to show the changes
    }
}

/* Check which key was pressed and apply the correct function */
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_8
        || event->key() == Qt::Key_2
        || event->key() == Qt::Key_4
        || event->key() == Qt::Key_6){
            changeOrigin(event->key()); //8,2,4,6 pressed -> change origin
    }else if(event->key() == Qt::Key_Plus
        || event->key() == Qt::Key_Minus){
            changeZoom(event->key());// +,- -> change zoom
    }
}
