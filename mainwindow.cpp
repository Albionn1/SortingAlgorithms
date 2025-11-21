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
    algorithmBox->addItems({"Bubble Sort", "Insertion Sort", "Selection Sort", "Quick Sort", "Merge Sort", "Heap Sort"});

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


    QVBoxLayout* legendContainer = new QVBoxLayout();
    legendTitleLabel = new QLabel("Legend");
    legendTitleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    legendLayout = new QHBoxLayout();

    legendContainer->addWidget(legendTitleLabel);
    legendContainer->addLayout(legendLayout);

    QWidget* legendWidget = new QWidget();
    legendWidget->setLayout(legendContainer);
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
    connect(algorithmBox, &QComboBox::currentTextChanged, this, &MainWindow::onAlgorithmSelected);

}

void MainWindow::onAlgorithmSelected(const QString& selected) {
    // Clear existing legend items
    QLayoutItem* child;
    while ((child = legendLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

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

    if (selected == "Heap Sort") {
        legendTitleLabel->setText("Legend — Heap Sort");
        legendLayout->addWidget(makeLegendItem("orange", "Heapify Comparison"));
        legendLayout->addWidget(makeLegendItem("red", "Extraction Swap"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Element"));
    }
    else if (selected == "Quick Sort") {
        legendTitleLabel->setText("Legend — Quick Sort");
        legendLayout->addWidget(makeLegendItem("mediumorchid", "Pivot"));
        legendLayout->addWidget(makeLegendItem("dodgerblue", "Comparing"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted"));
    }
    else if (selected == "Merge Sort") {
        legendTitleLabel->setText("Legend — Merge Sort");
        legendLayout->addWidget(makeLegendItem("cyan", "Left Half"));
        legendLayout->addWidget(makeLegendItem("deeppink", "Right Half"));
        legendLayout->addWidget(makeLegendItem("green", "Merged Output"));
    }
    else if (selected == "Bubble Sort") {
        legendTitleLabel->setText("Legend — Bubble Sort");
        legendLayout->addWidget(makeLegendItem("crimson", "Current Comparison"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Tail"));
    }
    else if (selected == "Insertion Sort") {
        legendTitleLabel->setText("Legend — Insertion Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element"));
        legendLayout->addWidget(makeLegendItem("orange", "Comparing Position"));
        legendLayout->addWidget(makeLegendItem("green", "Inserted / Sorted"));
    }
    else if (selected == "Selection Sort") {
        legendTitleLabel->setText("Legend — Selection Sort");
        legendLayout->addWidget(makeLegendItem("purple", "Current Minimum"));
        legendLayout->addWidget(makeLegendItem("red", "Comparing"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Prefix"));
    }
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

        }
    }
    if (currentAlgorithm == SortAlgorithm::Heap) {
        if (value < static_cast<int>(iHistory.size()) &&
            value < static_cast<int>(jHistory.size()) &&
            value < static_cast<int>(pivotHistory.size())) {
            highlightComparison(iHistory[value], jHistory[value], pivotHistory[value]);
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
        descriptionLabel->setText("Bubble Sort - Simple but slow. Repeatedly swaps adjacent elements until sorted.");
        appendLog("Starting Bubble Sort.");
    }
    else if (selected == "Insertion Sort") {
        currentAlgorithm = SortAlgorithm::Insertion;
        i = 1; j = 0; key = 0; inserting = false;
        descriptionLabel->setText("Insertion Sort - Builds the sorted array one item at a time. Fast on nearly sorted data.");
        appendLog("Starting Insertion Sort.");
    }
    else if (selected == "Selection Sort") {
        currentAlgorithm = SortAlgorithm::Selection;
        i = 0; j = 0; minIndex = 0;
        descriptionLabel->setText("Selection Sort - Finds the minimum and places it. Easy to understand, but always O(n²).");
        appendLog("Starting Selection Sort.");
    }
    else if (selected == "Quick Sort") {
        currentAlgorithm = SortAlgorithm::Quick;
        quickStack.clear();
        if (!array.empty()) {
            quickStack.push({0, static_cast<int>(array.size() - 1)});
        }
        quickI = quickJ = quickPivot = -1;
        descriptionLabel->setText("Quick Sort - Efficient divide-and-conquer using a pivot. Fast on average, but worst-case is quadratic.");
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
        descriptionLabel->setText("Merge Sort – A stable, divide-and-conquer algorithm that recursively splits and merges arrays for guaranteed O(n log n) performance.");
        appendLog("Starting Merge Sort...");
    }
    else if (selected == "Heap Sort") {
        currentAlgorithm = SortAlgorithm::Heap;

        // Reset heap state
        heapStack = std::stack<int>();
        heapBuilding = true;
        heapSwapping = false;
        heapI = heapJ = -1;
        heapSize = static_cast<int>(array.size());

        // Clear visualization state
        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        // Push all non-leaf nodes for initial heapify (bottom-up).
        // Push in increasing order so the stack pops the highest index first
        // (i.e. we process the deepest non-leaf nodes before the root).
        int lastNonLeaf = heapSize / 2 - 1;
        for (int k = 0; k <= lastNonLeaf; ++k) {
            heapStack.push(k);
        }

        // Initial frame
        pushFrame(array, -1, -1, -1);

        appendLog(QString("Heap Sort starting. heapSize=%1").arg(heapSize));

        // Start timer using the configured delay
        if (timer && !timer->isActive()) timer->start(delayBox->value());
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
                    // drawArray(array);
                }else{
                    appendLog(QString("No swap: %1 <= %2.").arg(array[j]).arg(array[j+1]));
                }
                j++;
                if(stepMode){
                    timer->stop();
                    return;
                }
            }else{

                int settledIndex = array.size() - i - 1;
                sortedIndices.insert(settledIndex);

                appendLog(QString("Pass %1 complete. Largest element settled at position %2.").arg(i+1).arg(array.size() - i - 1));

                highlightComparison(settledIndex, -1, -1);

                j = 0;
                i++;
            }
        }else{
            timer->stop();
            appendLog("Sorting complete.");
            appendLog("Array is sorted.");

            for (int k = 0; k < array.size(); ++k) {
                sortedIndices.insert(k);
            }
            highlightComparison(-1, -1, -1);

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

                highlightComparison(i, -1, -1);
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

                    highlightComparison(j, j + 1, -1);

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

                    sortedIndices.insert(j + 1);
                    highlightComparison(j + 1, -1, -1);

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
    }    //MERGE SORT ALGORITHM ENDS HERE
    else if (currentAlgorithm == SortAlgorithm::Heap) {
        // Phase 1: Build max-heap incrementally (bottom-up)
        if (heapBuilding) {
            if (!heapStack.empty()) {
                heapI = heapStack.top(); heapStack.pop();

                int largest = heapI;
                int left = 2 * heapI + 1;
                int right = 2 * heapI + 2;

                if (left  < heapSize && array[left]  > array[largest]) largest = left;
                if (right < heapSize && array[right] > array[largest]) largest = right;

                heapJ = largest;

                // If a swap is needed, perform and continue heapifying this subtree later
                if (largest != heapI) {
                    std::swap(array[heapI], array[largest]);
                    heapStack.push(largest); // continue down this subtree
                    appendLog(QString("Heapify swap at %1 with %2").arg(heapI).arg(largest));
                } else {
                    appendLog(QString("Heapify compare at %1 (no swap)").arg(heapI));
                }

                // Live repaint and record frame
                highlightComparison(heapI, heapJ, -1);

                pushFrame(array, heapI, heapJ, -1);

                if (stepMode) { timer->stop(); return; }
                return;
            } else {
                // Transition to extraction
                heapBuilding = false;
                heapSwapping = true;
                appendLog("Max-heap built. Starting extraction.");
                return;
            }
        }

        // Phase 2: Extract max and re-heapify
        if (heapSwapping) {
            // Extraction step: place current max at the end of the heap
            if (heapStack.empty() && heapSize > 1) {
                // Swap root with last element of active heap
                std::swap(array[0], array[heapSize - 1]);
                sortedIndices.insert(heapSize - 1);

                // Shrink heap and prepare re-heapify from root
                heapSize--;
                heapStack.push(0);

                heapI = 0;
                heapJ = heapSize; // visual anchor to the shrinking boundary

                // Visualize extraction
                highlightComparison(0, heapSize, -1);

                appendLog(QString("Extracted max to index %1; heapSize=%2").arg(heapSize).arg(heapSize));

                // Record this step
                pushFrame(array, heapI, heapJ, -1);

                if (stepMode) { timer->stop(); return; }
                return;
            }

            // Re-heapify one node per tick
            if (!heapStack.empty()) {
                heapI = heapStack.top(); heapStack.pop();

                int largest = heapI;
                int left = 2 * heapI + 1;
                int right = 2 * heapI + 2;

                if (left  < heapSize && array[left]  > array[largest]) largest = left;
                if (right < heapSize && array[right] > array[largest]) largest = right;

                heapJ = largest;

                if (largest != heapI) {
                    std::swap(array[heapI], array[largest]);
                    heapStack.push(largest); // continue down the subtree
                    appendLog(QString("Re-heapify swap at %1 with %2").arg(heapI).arg(largest));
                } else {
                    appendLog(QString("Re-heapify compare at %1 (no swap)").arg(heapI));
                }
                highlightComparison(heapI, largest, -1);

                pushFrame(array, heapI, heapJ, -1);

                if (stepMode) { timer->stop(); return; }
                return;
            }

            // Finalization when heap reduced to 0 or 1
            if (heapSize <= 1 && heapStack.empty()) {
                if (heapSize == 1) {
                    sortedIndices.insert(0);
                    heapSize = 0;
                }

                appendLog("Heap Sort complete.");
                pushFrame(array, -1, -1, -1);

                highlightComparison(-1, -1, -1);
                drawArrayFinished(array);
                if (timer) timer->stop();
                return;
            }
        }
    }
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

void MainWindow::highlightComparison(int index1, int index2, int pivotIndex /* = -1 */) {
    scene->clear();

    if (array.empty()) return;

    int maxVal = *std::max_element(array.begin(), array.end());
    int maxBarHeight = 150;
    int x = 10;

    for (int k = 0; k < array.size(); ++k) {
        int barHeight = (maxVal > 0) ? (array[k] * maxBarHeight / maxVal) : 0;

        // Neutral base color for all bars
        QColor color = QColor(200, 200, 200); // light gray

        // --- Merge Sort ---
        if (currentAlgorithm == SortAlgorithm::Merge) {
            if (k >= mergeLeftStart && k <= mergeLeftEnd)
                color = QColor(0, 255, 255);   // Cyan – Left Half
            else if (k >= mergeRightStart && k <= mergeRightEnd)
                color = QColor(255, 20, 147);  // DeepPink – Right Half
            else if (k >= mergeMergedStart && k <= mergeMergedEnd)
                color = QColor(0, 255, 0);     // Lime – Merged Output
        }

        // --- Quick Sort ---
        if (currentAlgorithm == SortAlgorithm::Quick) {
            if (k == pivotIndex && pivotIndex >= 0)
                color = QColor(186, 85, 211);  // MediumOrchid – Pivot
            else if (k == index1 || k == index2)
                color = QColor(30, 144, 255);  // DodgerBlue – Comparing
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);   // Gold – Sorted
        }

        // --- Heap Sort ---
        if (currentAlgorithm == SortAlgorithm::Heap) {
            if (k == index1)
                color = QColor(255, 165, 0);   // Orange – Heapify Comparison
            else if (k == index2)
                color = QColor(255, 0, 0);     // Red – Extraction Swap
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);     // Green – Sorted Element
        }

        // --- Bubble Sort ---
        if (currentAlgorithm == SortAlgorithm::Bubble) {
            if (k == index1 || k == index2)
                color = QColor(220, 20, 60);   // Crimson – Current Comparison
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);   // ForestGreen – Sorted Tail
        }

        // --- Insertion Sort ---
        if (currentAlgorithm == SortAlgorithm::Insertion) {
            if (k == index1)
                color = QColor(65, 105, 225);  // RoyalBlue – Key Element
            else if (k == index2)
                color = QColor(255, 165, 0);   // Orange – Comparing Position
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);     // Green – Inserted/Sorted
        }

        // --- Selection Sort ---
        if (currentAlgorithm == SortAlgorithm::Selection) {
            if (k == index1)
                color = QColor(128, 0, 128);   // Purple – Current Minimum
            else if (k == index2)
                color = QColor(255, 0, 0);     // Red – Comparing
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);     // Green – Sorted Prefix
        }

        // Draw bar
        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(color));

        // Draw value text
        QGraphicsTextItem* text = scene->addText(QString::number(array[k]));
        text->setPos(x, 200 + 5);
        x += 30;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
