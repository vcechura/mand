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
    /* Stop the threads and wait for them to correctly finish */
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

    /* Threads */
    for(int i = 0; i < NUM_OF_CALC_THREADS; i++){
        delete t[i];
    }

}

/* Creates mutex and initializes data struct
 * used in the computation with starting data assigned in initVariables()
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
    width = BASIC_WIDTH; // 500 x 500 pixels
    height = BASIC_HEIGHT;
    xOffset = 0; // No offset. image center is at (0,0)
    yOffset = 0;
    zoom = 1; // No zoom
    numOfThreadsFinished = 0;
    threadsReady = true; // Threads are ready to calculate
}

/* QLabel init, QLabel is used to show the calculated image */
void MainWindow::initImage(){
    image = new QLabel(this);
    if(image == nullptr){
        exit(50);
    }
    image->setAlignment(Qt::AlignCenter); // Put the image in the center
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

    /* Use the created layout */
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
        connect(t[i], &CalcThread::resultReady, this, &MainWindow::handleResults); //When calc. is ready, signal GUI thread
        connect(this, &MainWindow::recalculateImage, t[i], &CalcThread::startRecalculation);// Signal worker threads to recalculate
        connect(t[i], &CalcThread::finished, t[i], &QObject::deleteLater);//Deleting threads
        t[i]->start();
        while(!t[i]->isRunning()){};//wait while thread is starting
    }
}

/* Changes the wdith and height of the window with the fractal
   based on the user input in edit fields. Called by "Set" button.
*/
void MainWindow::setDimensions(){
    int temp = widthEdit->text().toInt(); // get text from line edits and convert to int
    if(temp >= WIDTH_MIN && temp <= WIDTH_MAX){ // check if it is within allowed range
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
        image->setPixmap(currentPix); //Show the image
        numOfThreadsFinished = 0;
        threadsReady = true;// Threads are now ready for next calculation
    }
}

/* Check which key was pressed and apply the correct changes */
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(threadsReady){
        bool recalc = true;
        switch( event->key() )
        {
            case Qt::Key_8://move up
                    m->lock();
                    yStep = data->yStep; //offset is calculated off of step
                    data->yOffset -= 20*yStep;//so it is consistent
                    m->unlock();
                    break;
            case Qt::Key_2://move down
                    m->lock();
                    yStep = data->yStep;
                    data->yOffset += 20*yStep;
                    m->unlock();
                    break;
            case Qt::Key_4://move left
                    m->lock();
                    xStep = data->xStep;
                    data->xOffset += 20*xStep;
                    m->unlock();
                    break;
            case Qt::Key_6://move right
                    m->lock();
                    xStep = data->xStep;
                    data->xOffset -= 20*xStep;
                    m->unlock();
                    break;
            case Qt::Key_Plus://zoom in
                    zoom = zoom*0.8; //zoom value
                    m->lock();
                    data->zoom = zoom; // update calc. data
                    m->unlock();
                    break;
            case Qt::Key_Minus://zoom out
                    zoom = zoom*1.2;
                    m->lock();
                    data->zoom = zoom; // update calc. data
                    m->unlock();
                    break;
            default: // Don't recalculate if some garbage key was pressed
                    recalc = false;
                    break;
        }
        if(recalc){
            threadsReady = false;
            emit recalculateImage(); // recalculate to show the changes
        }
    }
}
