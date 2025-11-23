#pragma once

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QGraphicsRectItem>
#include <QStringList>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QRandomGenerator>

/*
 * Program: Sorting Algorithm Visualizer
 * Author: Albion Berisha
 *
 * Description:
 * This program provides a visual demonstration of multiple sorting algorithms
 * (e.g., Bubble Sort, Insertion Sort, Quick Sort, Merge Sort, Tim Sort, Radix Sort, Gnome Sort).
 * Each algorithm is animated step-by-step, with bars representing array elements
 * and colors highlighting comparisons, swaps, and sorted sections.
 *
 * Usage:
 * 1. Launch the application.
 * 2. Select a sorting algorithm from the dropdown menu.
 * 3. Press "Start" to begin the visualization.
 * 4. Use the speed slider to adjust animation speed in real time.
 * 5. Observe the legend for color meanings:
 *      - Blue: Default unsorted elements
 *      - Magenta/Cyan/Orange: Active comparisons depending on algorithm
 *      - Green: Sorted section
 * 6. The log panel will display textual updates for each step.
 *
 * Notes:
 * - Each algorithm runs incrementally on a timer, so you can follow the process visually.
 * - The program is designed for educational purposes, helping users understand
 *   how different sorting algorithms operate internally.
 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), i(0), j(0), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    // Core widgets
    algorithmBox = new QComboBox();
    algorithmBox->addItems({"Bubble Sort", "Insertion Sort", "Selection Sort", "Quick Sort", "Merge Sort", "Heap Sort", "Shell Sort", "Tim Sort", "Radix Sort", "Gnome Sort"});

    startButton = new QPushButton("Start Sort");
    resetButton = new QPushButton("Reset to Default");
    randomButton = new QPushButton("Random Input");

    stepByStepCheck = new QCheckBox("Step-by-Step Mode");
    nextStepButton = new QPushButton("Next Step");
    nextStepButton->setEnabled(false);

    delayBox = new QSlider(Qt::Horizontal, this);
    delayBox->setRange(100, 5000);
    delayBox->setValue(1000);
    delayBox->setTickInterval(100);
    delayBox->setSingleStep(100);
    delayBox->setPageStep(100);
    delayBox->setToolTip("Adjust animation speed (ms delay)");

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

    descriptionLabel->setText("Bubble Sort - Simple but slow. Repeatedly swaps adjacent elements until sorted."); //DEFAULT STARTING ALGORITHM

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
    timingControls->addWidget(new QLabel("Speed:"));
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
    connect(delayBox, &QSlider::valueChanged, this, [&](int value) {

        int snapped = (value / 100) * 100;
        delayBox->setValue(snapped);

        stepDelay = snapped;

        if (timer->isActive()) {
            timer->setInterval(stepDelay);
        }
        appendLog(QString("Speed set to %1 ms").arg(stepDelay));
    });


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
        descriptionLabel->setText("Heap Sort – An in-place, comparison-based algorithm that builds a binary heap and repeatedly extracts the maximum for O(n log n) performance.");
    }
    else if (selected == "Quick Sort") {
        legendTitleLabel->setText("Legend — Quick Sort");
        legendLayout->addWidget(makeLegendItem("mediumorchid", "Pivot"));
        legendLayout->addWidget(makeLegendItem("dodgerblue", "Comparing"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted"));
        descriptionLabel->setText("Quick Sort - Efficient divide-and-conquer using a pivot. Fast on average, but worst-case is quadratic.");
    }
    else if (selected == "Merge Sort") {
        legendTitleLabel->setText("Legend — Merge Sort");
        legendLayout->addWidget(makeLegendItem("cyan", "Left Half"));
        legendLayout->addWidget(makeLegendItem("deeppink", "Right Half"));
        legendLayout->addWidget(makeLegendItem("green", "Merged Output"));
        descriptionLabel->setText("Merge Sort – A stable, divide-and-conquer algorithm that recursively splits and merges arrays for guaranteed O(n log n) performance.");
    }
    else if (selected == "Bubble Sort") {
        legendTitleLabel->setText("Legend — Bubble Sort");
        legendLayout->addWidget(makeLegendItem("crimson", "Current Comparison"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Tail"));
        descriptionLabel->setText("Bubble Sort - Simple but slow. Repeatedly swaps adjacent elements until sorted.");
    }
    else if (selected == "Insertion Sort") {
        legendTitleLabel->setText("Legend — Insertion Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element"));
        legendLayout->addWidget(makeLegendItem("orange", "Comparing Position"));
        legendLayout->addWidget(makeLegendItem("green", "Inserted / Sorted"));
        descriptionLabel->setText("Insertion Sort - Builds the sorted array one item at a time. Fast on nearly sorted data.");
    }
    else if (selected == "Selection Sort") {
        legendTitleLabel->setText("Legend — Selection Sort");
        legendLayout->addWidget(makeLegendItem("purple", "Current Minimum"));
        legendLayout->addWidget(makeLegendItem("red", "Comparing"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Prefix"));
        descriptionLabel->setText("Selection Sort - Finds the minimum and places it. Easy to understand, but always O(n²).");
    }
    else if (selected == "Shell Sort") {
        legendTitleLabel->setText("Legend — Shell Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element"));
        legendLayout->addWidget(makeLegendItem("orange", "Gap Comparison"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted / Final"));
        descriptionLabel->setText("Shell Sort – An adaptive, gap-based algorithm that generalizes insertion sort for faster O(n log² n) performance.");
    }
    else if (selected == "Tim Sort") {
        legendTitleLabel->setText("Legend — TimSort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element (Insertion in runs)"));
        legendLayout->addWidget(makeLegendItem("orange", "Comparison (Insertion/Merge)"));
        legendLayout->addWidget(makeLegendItem("green", "Placed / Sorted"));
        descriptionLabel->setText("TimSort – A hybrid, stable algorithm that blends merge sort and insertion sort for adaptive O(n log n) performance.");
    }
    else if (selected == "Radix Sort") {
        legendTitleLabel->setText("Legend — Radix Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Current Digit"));
        legendLayout->addWidget(makeLegendItem("orange", "Bucket Placement"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Output"));
        descriptionLabel->setText("Radix Sort – A non-comparative, stable algorithm that sorts digit by digit using counting sort.");
    }
    else if (selected == "Gnome Sort") {
        legendTitleLabel->setText("Legend — Gnome Sort");
        legendLayout->addWidget(makeLegendItem("magenta", "Current Gnome Position"));
        legendLayout->addWidget(makeLegendItem("cyan", "Neighbor Being Compared"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Section"));
        descriptionLabel->setText("Gnome Sort – A simple algorithm that moves elements back and forth like a garden gnome tidying a line.");
    }
}
//Generate 20 random numbers(1 to 100)
void MainWindow::onRandomClicked(){
    array.clear();
    QStringList numbers;

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

    logView->clear();
    appendLog("Input: " + inputField->text());

    if (selected == "Bubble Sort") {
        currentAlgorithm = SortAlgorithm::Bubble;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        i = 0; j = 0;

        appendLog("Starting Bubble Sort.");
    }
    else if (selected == "Insertion Sort") {
        currentAlgorithm = SortAlgorithm::Insertion;
        i = 1; j = 0; key = 0; inserting = false;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        appendLog("Starting Insertion Sort.");
    }
    else if (selected == "Selection Sort") {
        currentAlgorithm = SortAlgorithm::Selection;
        i = 0; j = 0; minIndex = 0;
        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        pushFrame(array, -1, -1, -1);
        appendLog("Selection Sort starting.");

        if (timer && !timer->isActive())
            timer->start(delayBox->value());

        appendLog("Starting Selection Sort.");
    }
    else if (selected == "Quick Sort") {
        currentAlgorithm = SortAlgorithm::Quick;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        quickStack.clear();
        if (!array.empty()) {
            quickStack.push({0, static_cast<int>(array.size() - 1)});
        }

        quickI = quickJ = quickPivot = -1;
        appendLog("Starting Quick Sort.");
    }
    else if (selected == "Merge Sort"){
        currentAlgorithm = SortAlgorithm::Merge;
        mergeStack = {};
        mergeBuffer = array;
        mergeStack.push({0, static_cast<int>(array.size() - 1), false}); // false = split phase
        merging = false;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        mergeLeft = mergeMid = mergeRight = -1;
        mergeI = mergeJ = mergeK = -1;

        mergeLeftStart = mergeLeftEnd = -1;
        mergeRightStart = mergeRightEnd = -1;
        mergeMergedStart = mergeMergedEnd = -1;

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

        int lastNonLeaf = heapSize / 2 - 1;
        for (int k = 0; k <= lastNonLeaf; ++k) {
            heapStack.push(k);
        }

        pushFrame(array, -1, -1, -1);

        appendLog(QString("Heap Sort starting. heapSize=%1").arg(heapSize));

        if (timer && !timer->isActive()) timer->start(delayBox->value());
    }
    else if (selected == "Shell Sort") {
        currentAlgorithm = SortAlgorithm::Shell;

        gap = array.size() / 2;
        shellI = gap;
        shellInserting = false;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        pushFrame(array, -1, -1, -1);

        appendLog(QString("Shell Sort starting. gap=%1").arg(gap));

        if (timer && !timer->isActive()) timer->start(delayBox->value());
    }
    else if (selected == "TimSort") {
        currentAlgorithm = SortAlgorithm::Tim;

        timRunSize = 32;

        timRuns.clear();
        timInserting = true;
        timMerging = false;

        timStart = 0;
        timEnd   = std::min(timStart + timRunSize, (int)array.size());

        timI   = timStart + 1;
        timJ   = timI;
        timKey = (timI < timEnd) ? array[timI] : 0;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        pushFrame(array, -1, -1, -1);

        appendLog(QString("TimSort starting. runSize=%1").arg(timRunSize));

        if (timer && !timer->isActive())
            timer->start(delayBox->value());
    }
    else if (selected == "Radix Sort") {
        currentAlgorithm = SortAlgorithm::Radix;

        radixInitialized = false;
        digitPlace = 1;
        radixIndex = 0;
        radixPhase = RadixPhase::Count;
        maxValue = *std::max_element(array.begin(), array.end());
        std::fill(count.begin(), count.end(), 0);
        bucket.resize(array.size());

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        pushFrame(array, -1, -1, -1);

        appendLog(QString("Radix Sort starting. maxValue=%1").arg(maxValue));

        if (timer && !timer->isActive())
            timer->start(delayBox->value());
    }
    else if (selected == "Gnome Sort") {
        currentAlgorithm = SortAlgorithm::Gnome;

        sortedIndices.clear();
        history.clear();
        iHistory.clear();
        jHistory.clear();
        pivotHistory.clear();

        gnomeIndex = 0;
        pushFrame(array, -1, -1, -1);

        appendLog("Starting Gnome Sort.");

        if (timer && !timer->isActive())
            timer->start(delayBox->value());
    }

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
        } else {
            timer->stop();
            appendLog("Insertion Sort complete.");
            appendLog("Array is sorted.");

            for (int k = 0; k < array.size(); ++k){
                sortedIndices.insert(k);
            }
            highlightComparison(-1, -1, -1);

            drawArrayFinished(array);
        }//INSERTION ALGORITHM ENDS HERE

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
            for (int k = 0; k < array.size(); ++k) sortedIndices.insert(k);
            highlightComparison(-1, -1, -1);

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

        //HEAP SORT ALGORITHM STARTS HERE

    else if (currentAlgorithm == SortAlgorithm::Heap) {
        if (heapBuilding) {
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
                    heapStack.push(largest);
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

        if (heapSwapping) {
            if (heapStack.empty() && heapSize > 1) {

                std::swap(array[0], array[heapSize - 1]);
                sortedIndices.insert(heapSize - 1);

                heapSize--;
                heapStack.push(0);

                heapI = 0;
                heapJ = heapSize;

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
                    heapStack.push(largest);
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
    //HEAP SORT ALGORITHM ENDS HERE


    //SHELL SORT ALGORITHM STARTS HERE
    else if (currentAlgorithm == SortAlgorithm::Shell) {
        if (gap > 0) {
            if (shellI < static_cast<int>(array.size())) {
                if (!shellInserting) {
                    shellKey = array[shellI];
                    shellJ = shellI;
                    shellInserting = true;

                    appendLog(QString("Taking key = %1 at index %2").arg(shellKey).arg(shellI));

                    highlightComparison(shellI, -1, -1);
                    return;
                }

                if (shellJ >= gap && array[shellJ - gap] > shellKey) {
                    appendLog(QString("Gap %1: shifting %2 right").arg(gap).arg(array[shellJ - gap]));
                    array[shellJ] = array[shellJ - gap];
                    shellJ -= gap;

                    history.push_back(array);
                    currentStep = static_cast<int>(history.size()) - 1;
                    slider->setMaximum(currentStep);
                    slider->setValue(currentStep);

                    highlightComparison(shellJ, shellJ + gap, -1);
                    return;
                } else {
                    array[shellJ] = shellKey;
                    appendLog(QString("Inserted %1 at index %2").arg(shellKey).arg(shellJ));

                    sortedIndices.insert(shellJ);
                    shellInserting = false;


                    highlightComparison(shellJ, -1, -1);

                    shellI++;
                }
            } else {
                gap /= 2;
                shellI = gap;
            }
        } else {
            timer->stop();
            appendLog("Shell Sort complete.");
            for (int k = 0; k < array.size(); ++k) sortedIndices.insert(k);

            shellI = -1;
            shellJ = -1;
            shellInserting = false;

            highlightComparison(-1, -1, -1);
        }
    }
    //SHELL SORT ALGORITHM ENDS HERE



    //TIM SORT ALGORITHM STARTS HERE
    else if (currentAlgorithm == SortAlgorithm::Tim) {
        if (timInserting) {
            if (timStart < (int)array.size()) {
                if (timI < timEnd) {
                    if (timJ > timStart && array[timJ - 1] > timKey) {
                        array[timJ] = array[timJ - 1];
                        timJ--;

                        history.push_back(array);
                        currentStep = (int)history.size() - 1;
                        slider->setMaximum(currentStep);
                        slider->setValue(currentStep);


                        highlightComparison(timJ, timJ + 1, -1);
                        if (stepMode) { timer->stop(); return; }
                        return;
                    } else {

                        array[timJ] = timKey;


                        sortedIndices.insert(timJ);
                        highlightComparison(timJ, -1, -1);


                        timI++;
                        timJ = timI;

                        if (timI < timEnd) {
                            timKey = array[timI];

                            highlightComparison(timI, -1, -1);
                        } else {

                            timRuns.push_back({timStart, timEnd});
                            appendLog(QString("Run sorted: [%1, %2)").arg(timStart).arg(timEnd));


                            timStart = timEnd;
                            timEnd   = std::min(timStart + timRunSize, (int)array.size());

                            if (timStart < (int)array.size()) {

                                timI   = timStart + 1;
                                timJ   = timI;
                                timKey = (timI < timEnd) ? array[timI] : 0;

                                highlightComparison(timI, -1, -1);
                            } else {

                                timInserting = false;
                                timMerging   = true;
                                appendLog("All runs sorted. Starting merge phase.");
                            }
                        }
                    }
                } else {

                    timRuns.push_back({timStart, timEnd});
                    appendLog(QString("Run sorted: [%1, %2)").arg(timStart).arg(timEnd));

                    timStart = timEnd;
                    timEnd   = std::min(timStart + timRunSize, (int)array.size());

                    if (timStart < (int)array.size()) {
                        timI   = timStart + 1;
                        timJ   = timI;
                        timKey = (timI < timEnd) ? array[timI] : 0;
                        highlightComparison(timI, -1, -1);
                    } else {
                        timInserting = false;
                        timMerging   = true;
                        appendLog("All runs sorted. Starting merge phase.");
                    }
                }
            } else {

                timInserting = false;
                timMerging   = true;
            }
        }


        else if (timMerging) {
            if (timRuns.size() > 1) {

                auto rightRun = timRuns.back(); timRuns.pop_back();
                auto leftRun  = timRuns.back(); timRuns.pop_back();

                timLeft  = leftRun.first;
                timMid   = leftRun.second;
                timRight = rightRun.second;

                timMergeBuffer.assign(array.begin() + timLeft, array.begin() + timMid);

                int i = 0;
                int j = timMid;
                int k = timLeft;

                while (i < (int)timMergeBuffer.size() && j < timRight) {
                    if (timMergeBuffer[i] <= array[j]) {
                        array[k++] = timMergeBuffer[i++];
                    } else {
                        array[k++] = array[j++];
                    }

                    history.push_back(array);
                    currentStep = (int)history.size() - 1;
                    slider->setMaximum(currentStep);
                    slider->setValue(currentStep);


                    highlightComparison(i + timLeft, j, -1);
                    if (stepMode) { timer->stop(); return; }
                    return;
                }

                while (i < (int)timMergeBuffer.size()) {
                    array[k++] = timMergeBuffer[i++];
                }

                for (int idx = timLeft; idx < timRight; ++idx)
                    sortedIndices.insert(idx);

                timRuns.push_back({timLeft, timRight});
                appendLog(QString("Merged runs into [%1, %2)").arg(timLeft).arg(timRight));
            } else {
                timer->stop();

                appendLog("TimSort complete.");
                for (int k = 0; k < (int)array.size(); ++k) sortedIndices.insert(k);
                highlightComparison(-1, -1, -1);
                timMerging = false;
            }
        }
    }
    //SHELL SORT ALGORITHM ENDS HERE


    //RADIX SORT ALGORITHM STARTS HERE
    else if (currentAlgorithm == SortAlgorithm::Radix) {
        // Initialization
        if (!radixInitialized) {
            maxValue = *std::max_element(array.begin(), array.end());
            digitPlace = 1;
            radixPhase = RadixPhase::Count;
            radixIndex = 0;
            std::fill(count.begin(), count.end(), 0);
            bucket.resize(array.size(), 0);
            radixInitialized = true;

            appendLog(QString("Radix Sort starting. maxValue=%1").arg(maxValue));
            pushFrame(array, -1, -1, -1);
            return;
        }

        // Counting phase
        if (radixPhase == RadixPhase::Count) {
            if (radixIndex < (int)array.size()) {
                int digit = (array[radixIndex] / digitPlace) % 10;
                count[digit]++;
                highlightComparison(radixIndex, -1, -1);
                pushFrame(array, radixIndex, -1, -1);

                appendLog(QString("Counting digit %1 at index %2")
                              .arg(digit).arg(radixIndex));

                radixIndex++;
                return;
            } else {
                radixPhase = RadixPhase::Accumulate;
                radixIndex = 0;
                appendLog("Switching to Accumulate phase.");
                return;
            }
        }

        // Accumulate phase
        if (radixPhase == RadixPhase::Accumulate) {
            if (radixIndex < 9) {
                count[radixIndex + 1] += count[radixIndex];
                highlightComparison(-1, radixIndex, -1);
                pushFrame(array, -1, radixIndex, -1);

                appendLog(QString("Accumulating bucket %1").arg(radixIndex));

                radixIndex++;
                return;
            } else {
                radixPhase = RadixPhase::Place;
                radixIndex = (int)array.size() - 1;
                appendLog("Switching to Placement phase.");
                return;
            }
        }

        // Placement phase
        if (radixPhase == RadixPhase::Place) {
            if (radixIndex >= 0) {
                int digit = (array[radixIndex] / digitPlace) % 10;
                bucket[count[digit] - 1] = array[radixIndex];
                count[digit]--;
                highlightComparison(radixIndex, -1, -1);
                pushFrame(array, radixIndex, -1, -1);

                appendLog(QString("Placing value %1 (digit %2) into bucket[%3]")
                              .arg(array[radixIndex])
                              .arg(digit)
                              .arg(count[digit]));


                radixIndex--;
                return;
            } else {
                radixPhase = RadixPhase::CopyBack;
                radixIndex = 0;
                appendLog("Switching to CopyBack phase.");
                return;
            }
        }

        // CopyBack phase
        if (radixPhase == RadixPhase::CopyBack) {
            if (radixIndex < (int)array.size()) {
                array[radixIndex] = bucket[radixIndex];
                highlightComparison(-1, -1, radixIndex);
                pushFrame(array, -1, -1, radixIndex);

                appendLog(QString("Copying back value %1 to index %2")
                              .arg(array[radixIndex]).arg(radixIndex));

                radixIndex++;
                return;
            } else {
                digitPlace *= 10;
                if (digitPlace <= maxValue) {
                    radixPhase = RadixPhase::Count;
                    radixIndex = 0;
                    std::fill(count.begin(), count.end(), 0);
                    std::fill(bucket.begin(), bucket.end(), 0);

                    appendLog(QString("Next digit place: %1").arg(digitPlace));
                    return;
                } else {
                    timer->stop();
                    drawArrayFinished(array);
                    appendLog("Radix Sort complete.");
                }
            }
        }
    }
    //RADIX SORT ALGORITHM ENDS HERE


    //GNOME SORT ALGORITHM STARTS HERE
    else if (currentAlgorithm == SortAlgorithm::Gnome) {
        if (gnomeIndex < array.size()) {
            int index1 = gnomeIndex;
            int index2 = gnomeIndex - 1;

            if (gnomeIndex == 0 || array[index1] >= array[index2]) {
                gnomeIndex++;
            } else {
                std::swap(array[index1], array[index2]);
                gnomeIndex--;
            }

            highlightComparison(index1, index2, -1);

            pushFrame(array, index1, index2, -1);

            appendLog(QString("Comparing indices %1 and %2").arg(index1).arg(index2));
            return;
        } else {
            timer->stop();
            drawArrayFinished(array);
            appendLog("Gnome Sort complete.");
        }
    }
    //GNOME SORT ALGORITHM ENDS HERE
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
        QColor color = QColor(200, 200, 200);

        if (currentAlgorithm == SortAlgorithm::Merge) {
            if (k >= mergeLeftStart && k <= mergeLeftEnd)
                color = QColor(0, 255, 255);
            else if (k >= mergeRightStart && k <= mergeRightEnd)
                color = QColor(255, 20, 147);
            else if (k >= mergeMergedStart && k <= mergeMergedEnd)
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Quick) {
            if (k == pivotIndex && pivotIndex >= 0)
                color = QColor(186, 85, 211);
            else if (k == index1 || k == index2)
                color = QColor(30, 144, 255);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Heap) {
            if (k == index1)
                color = QColor(255, 165, 0);
            else if (k == index2)
                color = QColor(255, 0, 0);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Bubble) {
            if (k == index1 || k == index2)
                color = QColor(220, 20, 60);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Insertion) {
            if (k == index1)
                color = QColor(65, 105, 225);
            else if (k == index2)
                color = QColor(255, 165, 0);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Selection) {
            if (k == index1)
                color = QColor(128, 0, 128);
            else if (k == index2)
                color = QColor(255, 0, 0);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }
        if (currentAlgorithm == SortAlgorithm::Shell) {
            if (shellInserting && k == shellI)
                color = QColor(65, 105, 225);
            else if (shellInserting && (k == shellJ || k == shellJ + gap))
                color = QColor(255, 165, 0);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }
        if (currentAlgorithm == SortAlgorithm::Tim) {
            if (timInserting && k == timI)
                color = QColor(65, 105, 225);
            else if ((timInserting && (k == timJ || k == timJ+1)) ||
                     (timMerging && (k == timLeft || k == timMid)))
                color = QColor(255, 165, 0);
            else if (sortedIndices.contains(k))
                color = QColor(0, 255, 0);
        }
        if (currentAlgorithm == SortAlgorithm::Radix) {
            if (radixPhase == RadixPhase::Count && k == radixIndex) {
                color = QColor(65, 105, 225);
            }
            else if ((radixPhase == RadixPhase::Accumulate && k == radixIndex) ||
                     (radixPhase == RadixPhase::Place && k == radixIndex)) {
                color = QColor(255, 165, 0);
            }
            else if (radixPhase == RadixPhase::CopyBack && k == radixIndex) {
                color = QColor(0, 255, 0);
            }  
        }
        if (currentAlgorithm == SortAlgorithm::Gnome) {
            if (k == index1) {
                color = QColor(255, 0, 255);
            }
            else if (k == index2) {
                color = QColor(0, 255, 255);
            }
            else if (sortedIndices.contains(k)) {
                color = QColor(0, 255, 0);
            }
        }

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
