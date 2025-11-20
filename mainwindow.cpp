#pragma once

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QGraphicsRectItem>
#include <QStringList>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), i(0), j(0), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);


    // Core widgets
    algorithmBox = new QComboBox();
    algorithmBox->addItems({"Bubble Sort", "Insertion Sort", "Selection Sort", "Quick Sort", "Merge Sort"});

    startButton = new QPushButton("Start Sort");
    resetButton = new QPushButton("Reset to Default");
    randomButton = new QPushButton("Random Input");

    stepByStepCheck = new QCheckBox("Step-by-Step Mode");
    nextStepButton = new QPushButton("Next Step");
    nextStepButton->setEnabled(false);

    delayBox = new QSpinBox();
    delayBox->setRange(50, 5000);
    delayBox->setValue(500);
    delayBox->setSuffix(" ms");

    inputField = new QLineEdit();
    inputField->setPlaceholderText("Enter numbers separated by spaces");
    inputField->setText("58 12 91 7 34 76 25 63 89 3 47 68 20 99 14 55 81 39 6 72");
    inputField->setToolTip("Enter numbers separated by spaces. You can replace the default list.");

    view = new QGraphicsView();
    scene = new QGraphicsScene(this);
    view->setScene(scene);

    stepLabel = new QLabel("Ready");
    logView = new QPlainTextEdit();
    logView->setPlaceholderText("All the steps will be shown here in detail");
    logView->setReadOnly(true);

    descriptionLabel = new QLabel();
    descriptionLabel->setWordWrap(true);

    slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setMaximum(0);
    slider->setToolTip("Drag to scrub through sorting steps");

    // Layout: Algorithm + Start + Reset
    QHBoxLayout* sortControls = new QHBoxLayout();
    sortControls->addWidget(new QLabel("Algorithm:"));
    sortControls->addWidget(algorithmBox);
    sortControls->addWidget(startButton);
    sortControls->addWidget(randomButton);
    sortControls->addWidget(resetButton);
    layout->addLayout(sortControls);

    // Layout: Input field
    QHBoxLayout* inputRow = new QHBoxLayout();
    inputRow->addWidget(new QLabel("Input:"));
    inputRow->addWidget(inputField);
    layout->addLayout(inputRow);

    // Layout: Delay + Step-by-step + Next
    QHBoxLayout* timingControls = new QHBoxLayout();
    timingControls->addWidget(new QLabel("Delay:"));
    timingControls->addWidget(delayBox);
    timingControls->addWidget(stepByStepCheck);
    timingControls->addWidget(nextStepButton);
    layout->addLayout(timingControls);

    // Layout: Slider + Step label
    QHBoxLayout* sliderRow = new QHBoxLayout();
    sliderRow->addWidget(new QLabel("Scrub:"));
    sliderRow->addWidget(slider);
    sliderRow->addWidget(stepLabel);
    layout->addLayout(sliderRow);

    // Legend
    legendLayout = new QHBoxLayout();
    auto makeLegendItem = [](const QString& colorName, const QString& labelText) {
        QLabel* colorBox = new QLabel();
        colorBox->setFixedSize(20, 20);
        colorBox->setStyleSheet("background-color:" + colorName + "; border:1px solid #444;");
        QLabel* textLabel = new QLabel(labelText);
        QHBoxLayout* itemLayout = new QHBoxLayout();
        itemLayout->addWidget(colorBox);
        itemLayout->addWidget(textLabel);
        QWidget* itemWidget = new QWidget();
        itemWidget->setLayout(itemLayout);
        return itemWidget;
    };
    legendLayout->addWidget(makeLegendItem("orange", "Comparing"));
    legendLayout->addWidget(makeLegendItem("yellow", "Sorted"));
    legendLayout->addWidget(makeLegendItem("mediumorchid", "Pivot"));
    legendLayout->addWidget(makeLegendItem("cyan", "Left Half"));
    legendLayout->addWidget(makeLegendItem("deeppink", "Right Half"));
    legendLayout->addWidget(makeLegendItem("lime", "Merged Output"));

    QWidget* legendWidget = new QWidget();
    legendWidget->setLayout(legendLayout);
    layout->addWidget(legendWidget);

    // Main visual + log
    layout->addWidget(view);
    layout->addWidget(logView);
    layout->addWidget(descriptionLabel);

    // Finalize
    setCentralWidget(central);
    resize(1000, 800);
    setWindowTitle("Sorting Algorithms Visualizer");

    // Connections
    timer = new QTimer(this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(stepByStepCheck, &QCheckBox::toggled, this, &MainWindow::onStepModeToggled);
    connect(nextStepButton, &QPushButton::clicked, this, &MainWindow::onTimerTick);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::onSliderMoved);
    connect(randomButton, &QPushButton::clicked, this, &MainWindow::onRandomClicked);
}

void MainWindow::onRandomClicked(){
    array.clear();
    QStringList numbers;

    //Generate 20 random numbers(1 to 100)
    for(int i = 0; i < 20; i++){
        int num = QRandomGenerator::global()->bounded(1, 101);
        array.push_back(num);
        numbers << QString::number(num);
    }
    inputField->setText(numbers.join(" "));
    scene->clear();
    drawArray(array);

    appendLog("Generated random input: " + inputField->text());
}

void MainWindow::onSliderMoved(int value) {
    if (value < 0 || value >= static_cast<int>(history.size())) return;


    array = history[value];
    drawArray(array);
    currentStep = value;

    // Restore Quick Sort metadata safely
    if (currentAlgorithm == SortAlgorithm::Quick) {
        if (value < pivotHistory.size() &&
            value < iHistory.size() &&
            value < jHistory.size()) {

            quickPivot = pivotHistory[value];
            quickI = iHistory[value];
            quickJ = jHistory[value];

            highlightComparison(jHistory[value], quickPivot, quickPivot);
        }
    }

    if(currentAlgorithm == SortAlgorithm::Merge){
        if (value >= 0 && value < history.size()) {
            array = history[value];

            // Restore merge zone history if you're scrubbing Merge Sort
            if (currentAlgorithm == SortAlgorithm::Merge && value < mergeLeftStartHistory.size()) {
                mergeLeftStart = mergeLeftStartHistory[value];
                mergeLeftEnd = mergeLeftEndHistory[value];
                mergeRightStart = mergeRightStartHistory[value];
                mergeRightEnd = mergeRightEndHistory[value];
                mergeMergedStart = mergeMergedStartHistory[value];
                mergeMergedEnd = mergeMergedEndHistory[value];
            }
            int i = iHistory[value];
            int j = jHistory[value];
            int pivot = pivotHistory[value];

            highlightComparison(i, j, pivot);

            appendLog(QString("Scrub %1: L[%2-%3] R[%4-%5] M[%6-%7]")
                          .arg(value)
                          .arg(mergeLeftStart).arg(mergeLeftEnd)
                          .arg(mergeRightStart).arg(mergeRightEnd)
                          .arg(mergeMergedStart).arg(mergeMergedEnd));

        }


    }


    // Optional: update step label
     stepLabel->setText(QString("Step %1 / %2").arg(value).arg(history.size() - 1));
}

void MainWindow::onStepModeToggled(bool checked){
    stepMode = checked;
    nextStepButton->setEnabled(checked);

    if(checked){
        timer->stop();
        descriptionLabel->setText("Step-by-step mode enabled. Click Next step to proceed.");
    }else{
        timer->start(delayBox->value());
        descriptionLabel->setText("Step-by-step mode disabled. Sorting will proceed automatically.");
    }
}

void MainWindow::onResetClicked(){

    inputField->setText("58 12 91 7 34 76 25 63 89 3 47 68 20 99 14 55 81 39 6 72");
    array.clear();
    scene->clear();
    stepLabel->clear();
    logView->clear();
    history.clear();
    pivotHistory.clear();
    iHistory.clear();
    jHistory.clear();
    slider->setValue(0);
    slider->setMaximum(0);
    currentStep = 0;

    timer->stop();
    drawArray(array);
    appendLog("Reset to default.");


}

void MainWindow::onStartClicked() {
    QString selected = algorithmBox->currentText();

    // Reset histories
    history.clear();
    pivotHistory.clear();
    iHistory.clear();
    jHistory.clear();
    slider->setValue(0);
    slider->setMaximum(0);
    currentStep = 0;

    // Parse input
    array.clear();
    QStringList numberStrings = inputField->text().split(" ", Qt::SkipEmptyParts);
    for (const QString& numStr : numberStrings) {
        bool ok;
        int num = numStr.toInt(&ok);
        if (ok) array.push_back(num);
    }

    if (array.empty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid list of numbers.");
        return;
    }
    if (array.size() < 2) {
        QMessageBox::information(this, "Info", "Array is too small to sort.");
        return;
    }

    // Reset log
    logView->clear();
    appendLog("Input: " + inputField->text());

    // Algorithm selection
    if (selected == "Bubble Sort") {
        currentAlgorithm = SortAlgorithm::Bubble;
        i = 0; j = 0;
        descriptionLabel->setText("Bubble Sort: Simple but slow. Repeatedly swaps adjacent elements until sorted.");
        appendLog("Starting Bubble Sort.");
    }
    else if (selected == "Insertion Sort") {
        currentAlgorithm = SortAlgorithm::Insertion;
        i = 1; j = 0; key = 0; inserting = false;
        descriptionLabel->setText("Insertion Sort: Builds the sorted array one item at a time. Fast on nearly sorted data.");
        appendLog("Starting Insertion Sort.");
    }
    else if (selected == "Selection Sort") {
        currentAlgorithm = SortAlgorithm::Selection;
        i = 0; j = 0; minIndex = 0;
        descriptionLabel->setText("Selection Sort: Finds the minimum and places it. Easy to understand, but always O(n²).");
        appendLog("Starting Selection Sort.");
    }
    else if (selected == "Quick Sort") {
        currentAlgorithm = SortAlgorithm::Quick;
        quickStack.clear();
        if (!array.empty()) {
            quickStack.push({0, static_cast<int>(array.size() - 1)});
        }
        quickI = quickJ = quickPivot = -1;
        descriptionLabel->setText("Quick Sort: Efficient divide-and-conquer using a pivot. Fast on average, but worst-case is quadratic.");
        appendLog("Starting Quick Sort.");
    }
    else if (selected == "Merge Sort"){
        currentAlgorithm = SortAlgorithm::Merge;
        mergeStack = {};
        mergeBuffer = array;
        mergeStack.push({0, static_cast<int>(array.size() - 1), false}); // false = split phase
        merging = false;

        mergeLeft = mergeMid = mergeRight = -1;
        mergeI = mergeJ = mergeK = -1;

        mergeLeftStart = mergeLeftEnd = -1;
        mergeRightStart = mergeRightEnd = -1;
        mergeMergedStart = mergeMergedEnd = -1;

        appendLog("Starting Merge Sort...");


    }

    // Draw initial array
    drawArray(array);

    // Timer logic
    stepMode = stepByStepCheck->isChecked();
    if (stepMode) {
        timer->stop();
        nextStepButton->setEnabled(true);
        descriptionLabel->setText("Step-by-step mode: click 'Next Step' to begin.");
    } else {
        timer->start(delayBox->value());
    }
}

void MainWindow::onTimerTick(){
    //BUBBLE ALGORITHM STARTS HERE
    if(currentAlgorithm == SortAlgorithm::Bubble){
        if(i < static_cast<int>(array.size())){
            if(j < static_cast<int>(array.size()) - i - 1){



                appendLog(QString("Comparing positions %1 and %2 (%3 vs %4).").arg(j).arg(j+1).arg(array[j]).arg(array[j+1]));

                history.push_back(array);
                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);

                appendLog(stepLabel->text());

                highlightComparison(j, j+1, -1);
                if(array[j] > array[j+1]){
                    appendLog(QString("Swap: %1 > %2 -> swapping.").arg(array[j]).arg(array[j+1]));

                    std::swap(array[j], array[j+1]);
                    drawArray(array);
                }else{
                    appendLog(QString("No swap: %1 <= %2.").arg(array[j]).arg(array[j+1]));
                }
                j++;
                if(stepMode){
                    timer->stop();
                    return;
                }
            }else{
                appendLog(QString("Pass %1 complete. Largest element settled at position %2.").arg(i+1).arg(array.size() - i - 1));
                j = 0;
                i++;
            }
        }else{
            timer->stop();
            appendLog("Sorting complete.");
            appendLog("Array is sorted.");
            drawArrayFinished(array);

        }

    }
    //BUBBLE ALGORITHMS ENDS HERE

    //INSERTION ALGORITHM STARTS HERE
    else if(currentAlgorithm == SortAlgorithm::Insertion){
        if (i < static_cast<int>(array.size())) {
            if (!inserting) {



                key = array[i];
                j = i - 1;
                inserting = true;

                history.push_back(array);
                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);

                appendLog(QString("Taking key = %1 at index %2").arg(key).arg(i));
                appendLog(stepLabel->text());
                return;
            } else {
                if (j >= 0 && array[j] > key) {
                    appendLog(QString("Shifting %1 right (index %2)").arg(array[j]).arg(j));

                    history.push_back(array);
                    currentStep = static_cast<int>(history.size()) - 1;
                    slider->setMaximum(currentStep);
                    slider->setValue(currentStep);

                    appendLog(stepLabel->text());
                    array[j + 1] = array[j];
                    drawArray(array);
                    j--;
                    return;
                } else {
                    history.push_back(array);
                    currentStep = static_cast<int>(history.size()) - 1;
                    slider->setMaximum(currentStep);
                    slider->setValue(currentStep);

                    array[j + 1] = key;
                    appendLog(QString("Inserting key %1 at index %2").arg(key).arg(j + 1));
                    appendLog(stepLabel->text());
                    drawArray(array);
                    i++;
                    inserting = false;
                    return;
                }
                if(stepMode){
                    timer->stop();
                    return;
                }
            }

            //INSERTION ALGORITHM ENDS HERE


        } else {
            timer->stop();
            appendLog("Insertion Sort complete.");
            appendLog("Array is sorted.");
            drawArrayFinished(array);
        }

    }


    //SELECTION ALGORITHM STARTS HERE
    else if(currentAlgorithm == SortAlgorithm::Selection) {
        if (i < static_cast<int>(array.size()) - 1) {
            if (j < static_cast<int>(array.size())) {

                history.push_back(array);
                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);


                appendLog(QString("Comparing index %1 (%2) with current min index %3 (%4)")
                            .arg(j).arg(array[j]).arg(minIndex).arg(array[minIndex]));
                appendLog(stepLabel->text());

                highlightComparison(j, minIndex, -1);

                if (array[j] < array[minIndex]) {
                    minIndex = j;
                    appendLog(QString("New minimum found at index %1 (%2)").arg(minIndex).arg(array[minIndex]));
                }
                j++;
                return;
            } else {
                if (minIndex != i) {
                    std::swap(array[i], array[minIndex]);
                    appendLog(QString("Swapping index %1 (%2) with min index %3 (%4)")
                                  .arg(i).arg(array[minIndex]).arg(minIndex).arg(array[i]));
                    drawArray(array);
                } else {
                    appendLog(QString("No swap needed for index %1").arg(i));
                }
                i++;
                if(stepMode){
                    timer->stop();
                    return;
                }
                j = i;
                minIndex = i;
                return;
            }
        } else {
            timer->stop();
            appendLog("Selection Sort complete.");
            appendLog("Array is sorted.");
            drawArrayFinished(array);
        }

    }

    //SELECTION ALGORITHM ENDS HERE

    //QUICKSORT ALGORITHM STARTS HERE

    else if (currentAlgorithm == SortAlgorithm::Quick) {
        while (!quickStack.isEmpty() && quickI == -1 && quickJ == -1) {
            auto [left, right] = quickStack.top();
            quickStack.pop();
            if (left < right) {
                quickLeft = left;
                quickRight = right;
                pivotValue = array[right];
                quickPivot = quickRight;
                quickI = left - 1;
                quickJ = left;
                appendLog(QString("Partitioning from %1 to %2 with pivot %3").arg(left).arg(right).arg(pivotValue));
                appendLog(stepLabel->text());

                highlightComparison(-1, -1, quickRight);

                history.push_back(array);
                pivotHistory.push_back(quickRight);
                iHistory.push_back(quickI);
                jHistory.push_back(quickJ);
                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);
                return;
            }
        }

        if (quickJ >= 0 && quickJ < quickRight) {
            appendLog(QString("Comparing %1 with pivot %2").arg(array[quickJ]).arg(pivotValue));
            appendLog(stepLabel->text());
            highlightComparison(quickJ, quickRight, quickRight);


            history.push_back(array);
            pivotHistory.push_back(quickRight);
            iHistory.push_back(quickI);
            jHistory.push_back(quickJ);
            currentStep = static_cast<int>(history.size()) - 1;
            slider->setMaximum(currentStep);
            slider->setValue(currentStep);

            if (array[quickJ] < pivotValue) {
                quickI++;
                std::swap(array[quickI], array[quickJ]);
                drawArray(array);
                appendLog(QString("Swapped %1 and %2").arg(array[quickI]).arg(array[quickJ]));

                history.push_back(array);
                pivotHistory.push_back(quickRight);
                iHistory.push_back(quickI);
                jHistory.push_back(quickJ);
                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);
            }
            quickJ++;
            if(stepMode){
                timer->stop();
                return;
            }
            return;
        }

        if (quickJ == quickRight) {            
            std::swap(array[quickI + 1], array[quickRight]);
            drawArray(array);


            highlightComparison(-1, -1, quickI + 1);

            int pivotIndex = quickI + 1;
            appendLog(QString("Placed pivot %1 at index %2").arg(array[pivotIndex]).arg(pivotIndex));
            quickStack.push({quickLeft, pivotIndex - 1});
            quickStack.push({pivotIndex + 1, quickRight});

            quickI = quickJ = quickPivot = -1;
            pivotValue = -1;

            history.push_back(array);
            pivotHistory.push_back(pivotIndex);
            iHistory.push_back(quickI);
            jHistory.push_back(quickJ);
            currentStep = static_cast<int>(history.size()) - 1;
            slider->setMaximum(currentStep);
            slider->setValue(currentStep);

            return;
        }

        if (quickStack.isEmpty() && quickI == -1 && quickJ == -1) {
            timer->stop();
            appendLog("Quick Sort complete.");
            appendLog("Array is sorted.");
            drawArrayFinished(array);
        }
    }
    //QUICKSORT ALGORITHM ENDS HERE


    //MERGE SORT ALGORITHM STARTS HERE
    else if (currentAlgorithm == SortAlgorithm::Merge) {
        if (!mergeStack.empty()) {
            appendLog(QString("mergeStack size: %1").arg(mergeStack.size()));

            if (!merging) {
                auto [left, right, isMerge] = mergeStack.top();
                mergeStack.pop();

                if (!isMerge) {
                    if (left < right) {
                        int mid = (left + right) / 2;
                        mergeStack.push({left, right, true});       // merge phase
                        mergeStack.push({mid + 1, right, false});   // right half
                        mergeStack.push({left, mid, false});        // left half

                        mergeLeftStartHistory.push_back(mergeLeftStart);
                        mergeLeftEndHistory.push_back(mergeLeftEnd);
                        mergeRightStartHistory.push_back(mergeRightStart);
                        mergeRightEndHistory.push_back(mergeRightEnd);
                        mergeMergedStartHistory.push_back(mergeMergedStart);
                        mergeMergedEndHistory.push_back(mergeMergedEnd);

                        history.push_back(array);
                        iHistory.push_back(-1);
                        jHistory.push_back(-1);
                        pivotHistory.push_back(-1);

                        currentStep = static_cast<int>(history.size()) - 1;
                        slider->setMaximum(currentStep);
                        slider->setValue(currentStep);
                    }
                    return;
                }

                // Initialize merge phase
                if (left >= right) {
                    // Skip invalid merge
                    return;
                }

                mergeLeft = left;
                mergeRight = right;
                mergeMid = (left + right) / 2;
                mergeI = mergeLeft;
                mergeJ = mergeMid + 1;
                mergeK = mergeLeft;
                merging = true;

                // Set merge zone ranges
                mergeLeftStart = mergeLeft;
                mergeLeftEnd = mergeMid;
                mergeRightStart = mergeMid + 1;
                mergeRightEnd = mergeRight;
                mergeMergedStart = mergeLeft;
                mergeMergedEnd = mergeRight;
            }
        }

        // Merge step-by-step
        if (merging) {
            if (mergeI <= mergeMid && mergeJ <= mergeRight) {
                highlightComparison(mergeI, mergeJ, -1);
                if (array[mergeI] <= array[mergeJ]) {
                    mergeBuffer[mergeK++] = array[mergeI++];

                    mergeLeftStartHistory.push_back(mergeLeftStart);
                    mergeLeftEndHistory.push_back(mergeLeftEnd);
                    mergeRightStartHistory.push_back(mergeRightStart);
                    mergeRightEndHistory.push_back(mergeRightEnd);
                    mergeMergedStartHistory.push_back(mergeMergedStart);
                    mergeMergedEndHistory.push_back(mergeMergedEnd);

                    history.push_back(array);
                    iHistory.push_back(-1);
                    jHistory.push_back(-1);
                    pivotHistory.push_back(-1);

                } else {
                    mergeBuffer[mergeK++] = array[mergeJ++];

                    mergeLeftStartHistory.push_back(mergeLeftStart);
                    mergeLeftEndHistory.push_back(mergeLeftEnd);
                    mergeRightStartHistory.push_back(mergeRightStart);
                    mergeRightEndHistory.push_back(mergeRightEnd);
                    mergeMergedStartHistory.push_back(mergeMergedStart);
                    mergeMergedEndHistory.push_back(mergeMergedEnd);

                    history.push_back(array);
                    iHistory.push_back(-1);
                    jHistory.push_back(-1);
                    pivotHistory.push_back(-1);


                }
            } else if (mergeI <= mergeMid) {
                highlightComparison(mergeI, -1, -1);
                mergeBuffer[mergeK++] = array[mergeI++];

                mergeLeftStartHistory.push_back(mergeLeftStart);
                mergeLeftEndHistory.push_back(mergeLeftEnd);
                mergeRightStartHistory.push_back(mergeRightStart);
                mergeRightEndHistory.push_back(mergeRightEnd);
                mergeMergedStartHistory.push_back(mergeMergedStart);
                mergeMergedEndHistory.push_back(mergeMergedEnd);

                history.push_back(array);
                iHistory.push_back(-1);
                jHistory.push_back(-1);
                pivotHistory.push_back(-1);

                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);
            } else if (mergeJ <= mergeRight) {
                highlightComparison(-1, mergeJ, -1);
                mergeBuffer[mergeK++] = array[mergeJ++];

                mergeLeftStartHistory.push_back(mergeLeftStart);
                mergeLeftEndHistory.push_back(mergeLeftEnd);
                mergeRightStartHistory.push_back(mergeRightStart);
                mergeRightEndHistory.push_back(mergeRightEnd);
                mergeMergedStartHistory.push_back(mergeMergedStart);
                mergeMergedEndHistory.push_back(mergeMergedEnd);

                history.push_back(array);
                iHistory.push_back(-1);
                jHistory.push_back(-1);
                pivotHistory.push_back(-1);

                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);
            } else {
                for (int i = mergeLeft; i <= mergeRight; ++i)
                    array[i] = mergeBuffer[i];

                highlightComparison(-1, -1, mergeK - 1);

                mergeLeftStartHistory.push_back(mergeLeftStart);
                mergeLeftEndHistory.push_back(mergeLeftEnd);
                mergeRightStartHistory.push_back(mergeRightStart);
                mergeRightEndHistory.push_back(mergeRightEnd);
                mergeMergedStartHistory.push_back(mergeMergedStart);
                mergeMergedEndHistory.push_back(mergeMergedEnd);

                history.push_back(array);
                iHistory.push_back(-1);
                jHistory.push_back(-1);
                pivotHistory.push_back(-1);

                currentStep = static_cast<int>(history.size()) - 1;
                slider->setMaximum(currentStep);
                slider->setValue(currentStep);

                drawArray(array);
                appendLog(QString("Merged [%1, %2]").arg(mergeLeft).arg(mergeRight));

                // Reset merge state
                mergeLeft = mergeMid = mergeRight = -1;
                mergeI = mergeJ = mergeK = -1;
                merging = false;

                mergeLeftStart = mergeLeftEnd = -1;
                mergeRightStart = mergeRightEnd = -1;
                mergeMergedStart = mergeMergedEnd = -1;

                if (mergeStack.empty()) {
                    timer->stop();
                    drawArrayFinished(array);
                    appendLog("Merge Sort complete.");
                }
            }
        }
    }
    //MERGE SORT ALGORITHM ENDS HERE
}

void MainWindow::updateScene() {
    scene->clear();

    if (array.empty()) return;

    int maxVal = *std::max_element(array.begin(), array.end());
    int maxBarHeight = 150;
    int x = 10;

    for (size_t index = 0; index < array.size(); ++index) {
        int val = array[index];
        int barHeight = (maxVal > 0) ? (val * maxBarHeight / maxVal) : 0;

        QColor color = Qt::blue;

        // Optional: highlight logic (minimal)
        if (currentAlgorithm == SortAlgorithm::Bubble && (index == i || index == j)) {
            color = Qt::red;
        } else if (currentAlgorithm == SortAlgorithm::Quick && index == quickPivot) {
            color = Qt::yellow;
        } else if (sortedIndices.contains(static_cast<int>(index))) {
            color = Qt::green;
        }

        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(color));

        QGraphicsTextItem* text = scene->addText(QString::number(val));
        text->setPos(x, 200 + 5);

        x += 30;
    }
}

void MainWindow::setStep(const QString& msg){
    stepLabel->setText(msg);
}

void MainWindow::appendLog(const QString& msg){
    logView->appendPlainText(msg);
}

void MainWindow::drawArray(const std::vector<int>& arr){
    scene->clear();

    if(arr.empty()) return;

    int maxVal = *std::max_element(arr.begin(), arr.end());
    int maxBarHeight = 150;

    int x = 10;
    for(int val : arr){
        int barHeight = (maxVal > 0) ? (val * maxBarHeight / maxVal) : 0;

        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(Qt::blue));

        QGraphicsTextItem* text = scene->addText(QString::number(val));
        text->setPos(x, 200 + 5);

        x += 30;
    }
}

void MainWindow::drawArrayFinished(const std::vector<int>& arr){
    scene->clear();

    if(arr.empty()) return;

    int maxVal = *std::max_element(array.begin(), array.end());
    int maxBarHeight = 150;

    int x = 10;
    for(int val : arr){
        int barHeight = (maxVal > 0) ? (val * maxBarHeight / maxVal) : 0;
        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(Qt::green));

        QGraphicsTextItem* text = scene->addText(QString::number(val));
        text->setPos(x, 200 + 5);

        x += 30;
    }
}

void MainWindow::highlightComparison(int index1, int index2, int pivotIndex =  -1){
    scene->clear();

    if(array.empty()) return;

    int maxVal = *std::max_element(array.begin(), array.end());
    int maxBarHeight = 150;

    int x = 10;

    for(int k = 0; k < array.size(); ++k){
        int barHeight = (maxVal > 0) ? (array[k] * maxBarHeight / maxVal) : 0;
        QColor color = Qt::blue;

        if (currentAlgorithm == SortAlgorithm::Merge) {
            if (k >= mergeLeftStart && k <= mergeLeftEnd)
                color = QColor(0, 255, 255); // Neon Blue – Left Half
            else if (k >= mergeRightStart && k <= mergeRightEnd)
                color = QColor(255, 20, 147); // Neon Pink – Right Half
            else if (k >= mergeMergedStart && k <= mergeMergedEnd)
                color = QColor(0, 255, 0); // Neon Green – Merged Output
        }

        if (k == pivotIndex && pivotIndex >= 0) {
            color = QColor(186, 85, 211); // Bright Purple – Pivot
        } else if ((k == index1 || k == index2)) {
            color = QColor(255, 165, 0); // Bright Orange – Comparison
        } else if (sortedIndices.contains(k)) {
            color = QColor(255, 255, 0); // Bright Yellow – Sorted
        }

        // qDebug() << "Highlighting: index1=" << index1
        //          << " index2=" << index2
        //          << " pivotIndex=" << pivotIndex;

        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(color));

        QGraphicsTextItem* text = scene->addText(QString::number(array[k]));
        text->setPos(x, 200 + 5);
        x += 30;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
