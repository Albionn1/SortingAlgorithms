#pragma once

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QGraphicsRectItem>
#include <QStringList>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QListWidget>
#include <QBrush>
#include <QColor>
#include <QString>
#include <QStyleFactory>


/*
 * Program: Sorting Algorithm Visualizer
 * Author: Albion Berisha
 *
 *
 */

    MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), i(0), j(0), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette = QPalette();
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(233,233,233));
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(240,240,240));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(76,163,224));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    qApp->setPalette(palette);

    qApp->setStyleSheet(
        "QCheckBox { color: black; } "
        "QCheckBox::indicator { width: 10px; height: 10px; } "
        "QCheckBox::indicator:unchecked { border: 1px solid black; background: transparent; } "
        "QCheckBox::indicator:checked { border: 1px solid black; background: #4CA3E0; }"
        );

    // Core widgets
    algorithmBox = new QComboBox();
    algorithmBox->addItems({ "Bubble Sort", "Insertion Sort", "Selection Sort", "Quick Sort", "Merge Sort", "Heap Sort", "Shell Sort", "Tim Sort", "Radix Sort", "Gnome Sort" });

    startButton = new QPushButton("Start Sort");
    resetButton = new QPushButton("Reset to Default");
    randomButton = new QPushButton("Random Input");

    // Size and distribution controls
    sizeSpinBox = new QSpinBox();
    sizeSpinBox->setRange(2, 200);
    sizeSpinBox->setValue(20);
    sizeSpinBox->setToolTip("Number of elements to generate for the array");

    distributionBox = new QComboBox();
    distributionBox->addItems({ "Random", "Sorted", "Reversed", "Nearly Sorted" });
    distributionBox->setToolTip("Choose data distribution for generated array");

    nearlySortedSlider = new QSlider(Qt::Horizontal, this);
    nearlySortedSlider->setRange(0, 100);
    nearlySortedSlider->setSingleStep(5);
    nearlySortedSlider->setPageStep(5);
    nearlySortedSlider->setTickInterval(5);
    nearlySortedSlider->setTickPosition(QSlider::TicksBelow);
    nearlySortedSlider->setValue(10);
    nearlySortedSlider->setToolTip("Percent of elements to randomly swap in a nearly-sorted array");

    nearlySortedSlider->setFixedWidth(150); // make the perturb slider smaller (~half)
    nearlySortedValueLabel = new QLabel("10 %");
    nearlySortedValueLabel->setFixedWidth(36);

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

    bigoDescriptionLabel = new QLabel();
    bigoDescriptionLabel->setText("Best Case: O(n)\nAverage Case: O(n^2)\nWorst Case: O(n^2)");

    descriptionLabel->setText("Bubble Sort - Simple but slow. Repeatedly swaps adjacent elements until sorted."); //DEFAULT STARTING ALGORITHM

    slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setMaximum(0);
    slider->setToolTip("Drag to scrub through sorting steps");

    // Pseudocode view
    pseudocodeView = new QListWidget();
    pseudocodeView->setSelectionMode(QAbstractItemView::SingleSelection);
    pseudocodeView->setFocusPolicy(Qt::NoFocus);
    pseudocodeView->setFixedWidth(420);
    pseudocodeView->setWordWrap(true);

    // Dark mode toggle
    darkModeToggle = new QCheckBox("Dark Mode", this);
    layout->addWidget(darkModeToggle);

    // Layout: Algorithm + Start + Reset
    QHBoxLayout* sortControls = new QHBoxLayout();
    sortControls->addWidget(new QLabel("Algorithm:"));
    sortControls->addWidget(algorithmBox);
    sortControls->addWidget(startButton);
    sortControls->addWidget(randomButton);
    sortControls->addWidget(new QLabel("Size:"));
    sortControls->addWidget(sizeSpinBox);
    sortControls->addWidget(new QLabel("Distribution:"));
    sortControls->addWidget(distributionBox);
    sortControls->addWidget(new QLabel("Perturb:"));
    sortControls->addWidget(nearlySortedSlider);
    sortControls->addWidget(nearlySortedValueLabel);
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

    // Main visual + pseudocode + log (side-by-side)
    QHBoxLayout* mainRow = new QHBoxLayout();
    // left: graphics view
    mainRow->addWidget(view, /*stretch=*/3);

    // right: pseudocode above log + description
    QVBoxLayout* rightCol = new QVBoxLayout();
    QLabel* pcLabel = new QLabel("Pseudocode");
    pcLabel->setStyleSheet("font-weight:bold;");
    rightCol->addWidget(pcLabel);
    rightCol->addWidget(pseudocodeView);
    rightCol->addWidget(new QLabel("Log"));
    rightCol->addWidget(logView, /*stretch=*/2);
    rightCol->addWidget(descriptionLabel);
    rightCol->addWidget(bigoDescriptionLabel);

    QWidget* rightWidget = new QWidget();
    rightWidget->setLayout(rightCol);
    mainRow->addWidget(rightWidget, /*stretch=*/2);

    layout->addLayout(mainRow);

    // Finalize
    setCentralWidget(central);
    resize(1400, 900);
    setWindowTitle("Sorting Algorithms Visualizer");

    // Connections
    timer = new QTimer(this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(stepByStepCheck, &QCheckBox::toggled, this, &MainWindow::onStepModeToggled);
    connect(nextStepButton, &QPushButton::clicked, this, &MainWindow::onTimerTick);
    connect(darkModeToggle, &QCheckBox::toggled, this, &MainWindow::setDarkMode);
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
    connect(distributionBox, &QComboBox::currentTextChanged, this, &MainWindow::onControlsChanged);
    connect(sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onControlsChanged);
    connect(nearlySortedSlider, &QSlider::valueChanged, this, [&](int v){
        nearlySortedValueLabel->setText(QString::number(v) + " %");
        // Only regenerate when Nearly Sorted is selected; otherwise just update label
        if (distributionBox->currentText() == "Nearly Sorted")
            onControlsChanged();
    });

    // Initialize visibility/state of the perturb controls based on current distribution
    bool isNearly = (distributionBox->currentText() == "Nearly Sorted");
    nearlySortedSlider->setVisible(isNearly);
    nearlySortedValueLabel->setVisible(isNearly);
    nearlySortedSlider->setEnabled(isNearly);
    nearlySortedValueLabel->setEnabled(isNearly);
    connect(algorithmBox, &QComboBox::currentTextChanged, this, &MainWindow::onAlgorithmSelected);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::onSliderMoved);

}

void MainWindow::setPseudocode(const QStringList& lines) {
    pseudocodeView->clear();
    for (const QString& ln : lines) {
        QListWidgetItem* it = new QListWidgetItem(ln);
        it->setFlags(it->flags() & ~Qt::ItemIsSelectable); // selection used only for highlight
        pseudocodeView->addItem(it);
    }
    pseudocodeCurrent = -1;
    highlightPseudocodeLine(-1);
}

void MainWindow::highlightPseudocodeLine(int index) {
    if (!pseudocodeView) return;

    // Reset old styles
    for (int r = 0; r < pseudocodeView->count(); ++r) {
        QListWidgetItem* it = pseudocodeView->item(r);
        QFont f = it->font();
        f.setBold(false);
        it->setFont(f);
        it->setForeground(QBrush(Qt::black));
        it->setBackground(QBrush(Qt::white));
    }

    // If invalid index, clear selection
    if (index < 0 || index >= pseudocodeView->count()) {
        pseudocodeView->clearSelection();
        pseudocodeView->setCurrentRow(-1);
        pseudocodeCurrent = -1;
        return;
    }

    // Style the active line
    QListWidgetItem* active = pseudocodeView->item(index);
    QFont af = active->font();
    af.setBold(true);
    active->setFont(af);
    active->setForeground(QBrush(QColor(30, 144, 255))); // DodgerBlue
    active->setBackground(QBrush(QColor(230, 245, 255)));
    pseudocodeView->setCurrentRow(index);
    pseudocodeCurrent = index;
}

static QString complexityText(MainWindow::SortAlgorithm alg) {
    switch (alg) {
        case MainWindow::SortAlgorithm::Bubble:
            return "Best Case: O(n)\nAverage Case: O(n^2)\nWorst Case: O(n^2)";
        case MainWindow::SortAlgorithm::Insertion:
            return "Best Case: O(n)\nAverage Case: O(n^2)\nWorst Case: O(n^2)";
        case MainWindow::SortAlgorithm::Selection:
            return "Best Case: O(n^2)\nAverage Case: O(n^2)\nWorst Case: O(n^2)";
        case MainWindow::SortAlgorithm::Merge:
            return "Best Case: O(n log n)\nAverage Case: O(n log n)\nWorst Case: O(n log n)";
        case MainWindow::SortAlgorithm::Quick:
            return "Best Case: O(n log n)\nAverage Case: O(n log n)\nWorst Case: O(n^2)";
        case MainWindow::SortAlgorithm::Heap:
            return "Best Case: O(n log n)\nAverage Case: O(n log n)\nWorst Case: O(n log n)";
        case MainWindow::SortAlgorithm::Shell:
            return "Best Case: O(n log n)\nAverage Case: O(n (log n)^2)\nWorst Case: O(n^2)";
        case MainWindow::SortAlgorithm::Tim:
            return "Best Case: O(n)\nAverage Case: O(n log n)\nWorst Case: O(n log n)";
        case MainWindow::SortAlgorithm::Radix:
            return "Best Case: O(nk)\nAverage Case: O(nk)\nWorst Case: O(nk)";
        case MainWindow::SortAlgorithm::Gnome:
            return "Best Case: O(n)\nAverage Case: O(n^2)\nWorst Case: O(n^2)";
        default:
            return "";
    }
}

void MainWindow::setDarkMode(bool enabled) {
    darkModeEnabled = enabled;

    QPalette palette;
    if (enabled) {
        // Fusion Dark Palette
        palette.setColor(QPalette::Window, QColor(53,53,53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(42,42,42));
        palette.setColor(QPalette::AlternateBase, QColor(66,66,66));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53,53,53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
        palette.setColor(QPalette::HighlightedText, Qt::black);

        qApp->setPalette(palette);
        qApp->setStyleSheet(
            "QCheckBox { color: white; } "
            "QCheckBox::indicator { width: 10px; height: 10px; } "
            "QCheckBox::indicator:unchecked { border: 1px solid white; background: transparent; } "
            "QCheckBox::indicator:checked { border: 1px solid white; background: #8E2DC5; }"
            );
    } else {
        // Fusion Light Palette
        palette.setColor(QPalette::Window, Qt::white);
        palette.setColor(QPalette::WindowText, Qt::black);
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::AlternateBase, QColor(233,233,233));
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Button, QColor(240,240,240));
        palette.setColor(QPalette::ButtonText, Qt::black);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Highlight, QColor(76,163,224));
        palette.setColor(QPalette::HighlightedText, Qt::white);

        qApp->setPalette(palette);
        qApp->setStyleSheet(
            "QCheckBox { color: black; } "
            "QCheckBox::indicator { width: 10px; height: 10px; } "
            "QCheckBox::indicator:unchecked { border: 1px solid black; background: transparent; } "
            "QCheckBox::indicator:checked { border: 1px solid black; background: #4CA3E0; }"
            );
    }

    // redraw bars with new default color
    drawArray(array);
    scene->update();
}


// Note: onAlgorithmSelected also sets pseudocode per algorithm.
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
        descriptionLabel->setText("Heap Sort – An in-place, comparison-based algorithm that builds a binary heap and repeatedly extracts the maximum.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Heap));
        setPseudocode({
            "1. Build max-heap from array",
            "2. For each node: heapify (compare and swap children)",
            "3. Swap root with last element (extract max)",
            "4. Repeat until sorted"
            });
    }
    else if (selected == "Quick Sort") {
        legendTitleLabel->setText("Legend — Quick Sort");
        legendLayout->addWidget(makeLegendItem("mediumorchid", "Pivot"));
        legendLayout->addWidget(makeLegendItem("dodgerblue", "Comparing"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted"));
        descriptionLabel->setText("Quick Sort - Efficient divide-and-conquer using a pivot. Fast on average, but worst-case is quadratic.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Quick));
        setPseudocode({
            "1. Choose pivot (rightmost)",
            "2. Partition: for j=left..right-1 compare with pivot; if less swap",
            "3. Place pivot by swapping A[i+1] and A[right]",
            "4. Push left/right partitions to stack and repeat",
            "5. Finished"
            });
    }
    else if (selected == "Merge Sort") {
        legendTitleLabel->setText("Legend — Merge Sort");
        legendLayout->addWidget(makeLegendItem("cyan", "Left Half"));
        legendLayout->addWidget(makeLegendItem("deeppink", "Right Half"));
        legendLayout->addWidget(makeLegendItem("green", "Merged Output"));
        descriptionLabel->setText("Merge Sort – A stable, divide-and-conquer algorithm that recursively splits and merges arrays for guaranteed O(n log n) performance.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Merge));
        setPseudocode({
            "1. If left >= right return",
            "2. mid = (left+right)/2",
            "3. sort(left, mid)",
            "4. sort(mid+1, right)",
            "5. merge two halves into temporary buffer and copy back",
            "6. Finished"
            });
    }
    else if (selected == "Bubble Sort") {
        legendTitleLabel->setText("Legend — Bubble Sort");
        legendLayout->addWidget(makeLegendItem("crimson", "Current Comparison"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Tail"));
        descriptionLabel->setText("Bubble Sort - Simple but slow. Repeatedly swaps adjacent elements until sorted.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Bubble));
        setPseudocode({
            "1. for i = 0 to n-1",
            "2.   for j = 0 to n-i-2",
            "3.     compare A[j] and A[j+1]",
            "4.     if A[j] > A[j+1] swap",
            "5.   mark position n-i-1 as settled",
            "6. finished"
            });
    }
    else if (selected == "Insertion Sort") {
        legendTitleLabel->setText("Legend — Insertion Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element"));
        legendLayout->addWidget(makeLegendItem("orange", "Comparing Position"));
        legendLayout->addWidget(makeLegendItem("green", "Inserted / Sorted"));
        descriptionLabel->setText("Insertion Sort - Builds the sorted array one item at a time. Fast on nearly sorted data.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Insertion));
        setPseudocode({
            "1. for i = 1 to n-1",
            "2.   key = A[i]; j = i-1",
            "3.   while j >= 0 and A[j] > key: A[j+1] = A[j]; j--",
            "4.   A[j+1] = key",
            "5. finished"
            });
    }
    else if (selected == "Selection Sort") {
        legendTitleLabel->setText("Legend — Selection Sort");
        legendLayout->addWidget(makeLegendItem("purple", "Current Minimum"));
        legendLayout->addWidget(makeLegendItem("red", "Comparing"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Prefix"));
        descriptionLabel->setText("Selection Sort - Finds the minimum and places it. Easy to understand, but always O(n^2).");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Selection));
        setPseudocode({
            "1. for i = 0 to n-2",
            "2.   min = i",
            "3.   for j = i+1 to n-1 if A[j] < A[min] min = j",
            "4.   swap A[i], A[min]",
            "5. finished"
            });
    }
    else if (selected == "Shell Sort") {
        legendTitleLabel->setText("Legend — Shell Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element"));
        legendLayout->addWidget(makeLegendItem("orange", "Gap Comparison"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted / Final"));
        descriptionLabel->setText("Shell Sort – An adaptive, gap-based algorithm that generalizes insertion sort for faster practical performance.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Shell));
        setPseudocode({
            "1. gap = n/2",
            "2. while gap > 0: do gapped insertion sort for gap",
            "3. gap /= 2",
            "4. finished"
            });
    }
    else if (selected == "Tim Sort") {
        legendTitleLabel->setText("Legend — TimSort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Key Element (Insertion in runs)"));
        legendLayout->addWidget(makeLegendItem("orange", "Comparison (Insertion/Merge)"));
        legendLayout->addWidget(makeLegendItem("green", "Placed / Sorted"));
        descriptionLabel->setText("TimSort – A hybrid, stable algorithm that blends merge sort and insertion sort for adaptive performance.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Tim));
        setPseudocode({
            "1. Break array into runs (e.g. 32)",
            "2. Insertion-sort each run",
            "3. Merge runs pairwise until one run covers array",
            "4. finished"
            });
    }
    else if (selected == "Radix Sort") {
        legendTitleLabel->setText("Legend — Radix Sort");
        legendLayout->addWidget(makeLegendItem("royalblue", "Current Digit"));
        legendLayout->addWidget(makeLegendItem("orange", "Bucket Placement"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Output"));
        descriptionLabel->setText("Radix Sort – A non-comparative, stable algorithm that sorts digit by digit using counting sort.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Radix));
        setPseudocode({
            "1. For digitPlace = 1; digitPlace <= max; digitPlace *= 10:",
            "2.   count digits at digitPlace",
            "3.   accumulate counts",
            "4.   place items into bucket by digit",
            "5.   copy buckets back to array",
            "6. finished"
            });
    }
    else if (selected == "Gnome Sort") {
        legendTitleLabel->setText("Legend — Gnome Sort");
        legendLayout->addWidget(makeLegendItem("magenta", "Current Gnome Position"));
        legendLayout->addWidget(makeLegendItem("cyan", "Neighbor Being Compared"));
        legendLayout->addWidget(makeLegendItem("green", "Sorted Section"));
        descriptionLabel->setText("Gnome Sort – A simple algorithm that moves elements back and forth like a garden gnome tidying a line.");
        bigoDescriptionLabel->setText(complexityText(SortAlgorithm::Gnome));
        setPseudocode({
            "1. index = 0",
            "2. while index < n:",
            "3.   if index == 0 or A[index] >= A[index-1]: index++",
            "4.   else swap A[index], A[index-1]; index--",
            "5. finished"
            });
    }
}

void MainWindow::onRandomClicked() {
    generateArrayFromControls(true);
}

void MainWindow::onControlsChanged() {
    bool isNearly = (distributionBox && distributionBox->currentText() == "Nearly Sorted");
    if (nearlySortedSlider) {
        nearlySortedSlider->setVisible(isNearly);
        nearlySortedSlider->setEnabled(isNearly);
    }
    if (nearlySortedValueLabel) {
        nearlySortedValueLabel->setVisible(isNearly);
        nearlySortedValueLabel->setEnabled(isNearly);
    }

    generateArrayFromControls(false);
}

void MainWindow::generateArrayFromControls(bool log) {
    array.clear();
    displayedSortedIndices.clear();
    QStringList numbers;

    int sz = 20;
    if (sizeSpinBox) sz = sizeSpinBox->value();
    QString dist = "Random";
    if (distributionBox) dist = distributionBox->currentText();

    // First generate a base random array
    array.reserve(sz);
    for (int k = 0; k < sz; ++k) {
        int v = QRandomGenerator::global()->bounded(1, 101);
        array.push_back(v);
    }

    // Apply distribution
    if (dist == "Sorted") {
        std::sort(array.begin(), array.end());
    }
    else if (dist == "Reversed") {
        std::sort(array.begin(), array.end(), std::greater<int>());
    }
    else if (dist == "Nearly Sorted") {
        std::sort(array.begin(), array.end());
        int percent = 10;
        if (nearlySortedSlider) percent = nearlySortedSlider->value();
        // number of swaps to perform = max(1, floor(sz * percent / 100))
        int swaps = std::max(1, (sz * percent) / 100);
        for (int s = 0; s < swaps; ++s) {
            int a = QRandomGenerator::global()->bounded(0, sz);
            int b = QRandomGenerator::global()->bounded(0, sz);
            if (a == b) continue;
            std::swap(array[a], array[b]);
        }
    }

    for (int v : array) numbers << QString::number(v);
    inputField->setText(numbers.join(" "));
    scene->clear();
    drawArray(array);

    if (log) appendLog(QString("Generated %1 input (%2): %3").arg(sz).arg(dist).arg(inputField->text()));
}

void MainWindow::onSliderMoved(int value) {
    if (value < 0 || value >= static_cast<int>(history.size())) return;

    array = history[value];
    drawArray(array);
    currentStep = value;

    displayedSortedIndices.clear();
    if (value < static_cast<int>(sortedIndicesHistory.size())) {
        displayedSortedIndices = sortedIndicesHistory[value];
    }
    else if (!history.empty()) {
        const std::vector<int>& finalArr = history.back();
        if (value < static_cast<int>(history.size())) {
            for (int idx = 0; idx < static_cast<int>(array.size()) && idx < static_cast<int>(finalArr.size()); ++idx) {
                if (array[idx] == finalArr[idx]) displayedSortedIndices.insert(idx);
            }
        }
    }

    if (value < static_cast<int>(radixPhaseHistory.size())) radixPhase = radixPhaseHistory[value];
    if (value < static_cast<int>(radixIndexHistory.size())) radixIndex = radixIndexHistory[value];
    // Restore shell per-frame state
    if (value < static_cast<int>(shellIHistory.size())) shellI = shellIHistory[value];
    if (value < static_cast<int>(shellJHistory.size())) shellJ = shellJHistory[value];
    if (value < static_cast<int>(shellInsertingHistory.size())) shellInserting = (shellInsertingHistory[value] != 0);
    // Restore tim per-frame state
    if (value < static_cast<int>(timIHistory.size())) timI = timIHistory[value];
    if (value < static_cast<int>(timJHistory.size())) timJ = timJHistory[value];
    if (value < static_cast<int>(timInsertingHistory.size())) timInserting = (timInsertingHistory[value] != 0);
    if (value < static_cast<int>(timMergingHistory.size())) timMerging = (timMergingHistory[value] != 0);
    if (value < static_cast<int>(timLeftHistory.size())) timLeft = timLeftHistory[value];
    if (value < static_cast<int>(timMidHistory.size())) timMid = timMidHistory[value];
    if (value < static_cast<int>(timRightHistory.size())) timRight = timRightHistory[value];

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

    if (currentAlgorithm == SortAlgorithm::Merge) {
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

    // General fallback: if specific algorithm handlers didn't highlight, use stored histories
    if (!(currentAlgorithm == SortAlgorithm::Quick) && !(currentAlgorithm == SortAlgorithm::Merge) && !(currentAlgorithm == SortAlgorithm::Heap)) {
        int ii = -1, jj = -1, pv = -1;
        if (value < static_cast<int>(iHistory.size())) ii = iHistory[value];
        if (value < static_cast<int>(jHistory.size())) jj = jHistory[value];
        if (value < static_cast<int>(pivotHistory.size())) pv = pivotHistory[value];
        if (ii != -1 || jj != -1 || pv != -1) {
            highlightComparison(ii, jj, pv);
        }
    }

    stepLabel->setText(QString("Step %1 / %2").arg(value).arg(history.size() - 1));
}

void MainWindow::onStepModeToggled(bool checked) {
    stepMode = checked;
    nextStepButton->setEnabled(checked);

    if (checked) {
        timer->stop();
        descriptionLabel->setText("Step-by-step mode enabled. Click Next step to proceed.");
    }
    else {
        timer->start(delayBox->value());
        descriptionLabel->setText("Step-by-step mode disabled. Sorting will proceed automatically.");
    }
}

void MainWindow::onResetClicked() {

    timer->stop();

    scene->clear();
    stepLabel->clear();
    logView->clear();

    history.clear();
    pivotHistory.clear();
    iHistory.clear();
    jHistory.clear();

    sortedIndicesHistory.clear();
    radixPhaseHistory.clear();
    radixIndexHistory.clear();
    shellIHistory.clear();
    shellJHistory.clear();
    shellInsertingHistory.clear();
    timIHistory.clear();
    timJHistory.clear();
    timInsertingHistory.clear();
    timMergingHistory.clear();
    timLeftHistory.clear();
    timMidHistory.clear();
    timRightHistory.clear();
    mergeLeftStartHistory.clear();
    mergeLeftEndHistory.clear();
    mergeRightStartHistory.clear();
    mergeRightEndHistory.clear();
    mergeMergedStartHistory.clear();
    mergeMergedEndHistory.clear();

    displayedSortedIndices.clear();
    sortedIndices.clear();

    slider->setValue(0);
    slider->setMaximum(0);
    currentStep = 0;

    i = j = -1;
    quickStack.clear(); quickI = quickJ = quickPivot = -1; pivotValue = -1;
    mergeStack = {};
    mergeBuffer.clear(); merging = false;
    mergeLeft = mergeMid = mergeRight = -1;
    mergeI = mergeJ = mergeK = -1;
    mergeLeftStart = mergeLeftEnd = mergeRightStart = mergeRightEnd = mergeMergedStart = mergeMergedEnd = -1;

    heapStack = std::stack<int>(); heapBuilding = false; heapSwapping = false; heapI = heapJ = -1; heapSize = 0;

    gap = (sizeSpinBox) ? (sizeSpinBox->value() / 2) : 0;
    shellI = shellJ = -1; shellInserting = false;

    timRuns.clear(); timInserting = false; timMerging = false; timStart = timEnd = timI = timJ = -1;
    timLeft = timMid = timRight = -1; timMergeBuffer.clear();

    radixInitialized = false; digitPlace = 1; radixIndex = 0; radixPhase = RadixPhase::Count; std::fill(count.begin(), count.end(), 0); bucket.clear();

    gnomeIndex = 0;

    array.clear();
    QStringList numberStrings = inputField->text().split(" ", Qt::SkipEmptyParts);
    for (const QString& numStr : numberStrings) {
        bool ok;
        int num = numStr.toInt(&ok);
        if (ok) array.push_back(num);
    }

    if (array.empty()) {
        generateArrayFromControls(false);
    } else {
        drawArray(array);
    }

    appendLog("Reset state (preserved algorithm & input).");
    highlightPseudocodeLine(-1);
}

void MainWindow::onStartClicked() {
    QString selected = algorithmBox->currentText();


    // Reset histories
    history.clear();
    pivotHistory.clear();
    iHistory.clear();
    jHistory.clear();
    // Clear any per-frame recorded histories
    sortedIndicesHistory.clear();
    radixPhaseHistory.clear();
    radixIndexHistory.clear();
    mergeLeftStartHistory.clear();
    mergeLeftEndHistory.clear();
    mergeRightStartHistory.clear();
    mergeRightEndHistory.clear();
    mergeMergedStartHistory.clear();
    mergeMergedEndHistory.clear();
    slider->setValue(0);
    slider->setMaximum(0);
    currentStep = 0;

    // Clear any scrubbing overlay so live sort starts with actual state
    displayedSortedIndices.clear();

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
        // show pseudocode (already set in onAlgorithmSelected) and highlight first line
        highlightPseudocodeLine(0);
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
        highlightPseudocodeLine(0);
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
        highlightPseudocodeLine(0);
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
            quickStack.push({ 0, static_cast<int>(array.size() - 1) });
        }

        quickI = quickJ = quickPivot = -1;
        appendLog("Starting Quick Sort.");
        highlightPseudocodeLine(0);
    }
    else if (selected == "Merge Sort") {
        currentAlgorithm = SortAlgorithm::Merge;
        mergeStack = {};
        mergeBuffer = array;
        mergeStack.push({ 0, static_cast<int>(array.size() - 1), false }); // false = split phase
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
        highlightPseudocodeLine(0);
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
        highlightPseudocodeLine(0);
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
        highlightPseudocodeLine(0);
    }
    else if (selected == "Tim Sort") {
        currentAlgorithm = SortAlgorithm::Tim;

        timRunSize = 32;

        timRuns.clear();
        timInserting = true;
        timMerging = false;

        timStart = 0;
        timEnd = std::min(timStart + timRunSize, (int)array.size());

        timI = timStart + 1;
        timJ = timI;
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
        highlightPseudocodeLine(0);
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
        highlightPseudocodeLine(0);
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
        highlightPseudocodeLine(0);
    }

    drawArray(array);

    // Timer logic
    stepMode = stepByStepCheck->isChecked();
    if (stepMode) {
        timer->stop();
        nextStepButton->setEnabled(true);
        descriptionLabel->setText("Step-by-step mode: click 'Next Step' to begin.");
    }
    else {
        timer->start(delayBox->value());
    }
}

void MainWindow::onTimerTick() {
    // Clear any scrubbing overlay so live state controls highlighting
    displayedSortedIndices.clear();

    // Bubble sort step
    if (currentAlgorithm == SortAlgorithm::Bubble) {
        if (i < static_cast<int>(array.size())) {
            if (j < static_cast<int>(array.size()) - i - 1) {



                appendLog(QString("Comparing positions %1 and %2 (%3 vs %4).").arg(j).arg(j + 1).arg(array[j]).arg(array[j + 1]));

                // Record this comparison frame with indices so scrubbing can show highlights
                pushFrame(array, j, j + 1, -1);

                appendLog(stepLabel->text());

                // pseudocode line: compare (0-based index 2 => step 3 in displayed list)
                highlightPseudocodeLine(2);
                highlightComparison(j, j + 1, -1);
                if (array[j] > array[j + 1]) {
                    appendLog(QString("Swap: %1 > %2 -> swapping.").arg(array[j]).arg(array[j + 1]));

                    std::swap(array[j], array[j + 1]);
                    // drawArray(array);

                    // show the swap pseudocode line
                    highlightPseudocodeLine(3);
                }
                else {
                    appendLog(QString("No swap: %1 <= %2.").arg(array[j]).arg(array[j + 1]));
                }
                j++;
                if (stepMode) {
                    timer->stop();
                    return;
                }
            }
            else {

                int settledIndex = array.size() - i - 1;
                sortedIndices.insert(settledIndex);

                appendLog(QString("Pass %1 complete. Largest element settled at position %2.").arg(i + 1).arg(array.size() - i - 1));

                // mark settled pseudocode line
                highlightPseudocodeLine(4);
                highlightComparison(settledIndex, -1, -1);

                j = 0;
                i++;
            }
        }
        else {
            timer->stop();
            appendLog("Sorting complete.");
            appendLog("Array is sorted.");
            highlightPseudocodeLine(5);

            for (int k = 0; k < array.size(); ++k) {
                sortedIndices.insert(k);
            }
            highlightComparison(-1, -1, -1);

            drawArrayFinished(array);
        }

    }
    // End bubble sort

    // Insertion sort step
    else if (currentAlgorithm == SortAlgorithm::Insertion) {
        if (i < static_cast<int>(array.size())) {
            if (!inserting) {
                key = array[i];
                j = i - 1;
                inserting = true;

                pushFrame(array, i, -1, -1);

                appendLog(QString("Taking key = %1 at index %2").arg(key).arg(i));
                appendLog(stepLabel->text());

                // pseudocode: taking key (line 1)
                highlightPseudocodeLine(1);
                highlightComparison(i, -1, -1);
                return;
            }
            else {
                if (j >= 0 && array[j] > key) {
                    appendLog(QString("Shifting %1 right (index %2)").arg(array[j]).arg(j));

                    pushFrame(array, j, j + 1, -1);

                    appendLog(stepLabel->text());
                    array[j + 1] = array[j];

                    // pseudocode: shifting (line 2)
                    highlightPseudocodeLine(2);
                    highlightComparison(j, j + 1, -1);

                    j--;
                    return;
                }
                else {
                    pushFrame(array, j + 1, -1, -1);

                    array[j + 1] = key;
                    appendLog(QString("Inserting key %1 at index %2").arg(key).arg(j + 1));
                    appendLog(stepLabel->text());

                    // pseudocode: insert (line 3)
                    highlightPseudocodeLine(3);
                    highlightComparison(j + 1, -1, -1);

                    i++;
                    inserting = false;
                    return;
                }
                if (stepMode) {
                    timer->stop();
                    return;
                }
            }
        }
        else {
            timer->stop();
            appendLog("Insertion Sort complete.");
            appendLog("Array is sorted.");
            highlightPseudocodeLine(4);

            for (int k = 0; k < array.size(); ++k) {
                sortedIndices.insert(k);
            }
            highlightComparison(-1, -1, -1);

            drawArrayFinished(array);
        }// End insertion sort

    }


    // Selection sort step
    else if (currentAlgorithm == SortAlgorithm::Selection) {
        if (i < static_cast<int>(array.size()) - 1) {
            if (j < static_cast<int>(array.size())) {

                pushFrame(array, j, minIndex, -1);


                appendLog(QString("Comparing index %1 (%2) with current min index %3 (%4)")
                    .arg(j).arg(array[j]).arg(minIndex).arg(array[minIndex]));
                appendLog(stepLabel->text());

                // show "for j" comparison line (line 2)
                highlightPseudocodeLine(2);
                highlightComparison(j, minIndex, -1);

                if (array[j] < array[minIndex]) {
                    minIndex = j;
                    appendLog(QString("New minimum found at index %1 (%2)").arg(minIndex).arg(array[minIndex]));
                }
                j++;
                return;
            }
            else {
                if (minIndex != i) {
                    std::swap(array[i], array[minIndex]);
                    appendLog(QString("Swapping index %1 (%2) with min index %3 (%4)")
                        .arg(i).arg(array[minIndex]).arg(minIndex).arg(array[i]));

                    highlightPseudocodeLine(3);
                }
                else {
                    appendLog(QString("No swap needed for index %1").arg(i));
                }
                i++;
                if (stepMode) {
                    timer->stop();
                    return;
                }
                j = i;
                minIndex = i;
                return;
            }
        }
        else {
            timer->stop();
            appendLog("Selection Sort complete.");
            appendLog("Array is sorted.");
            drawArrayFinished(array);
            highlightPseudocodeLine(4);
        }

    }

    // End selection sort

    // Quick sort step

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

                // pseudocode: choose pivot / initialize partition (line 0 or 1)
                highlightPseudocodeLine(0);
                highlightComparison(-1, -1, quickRight);

                pushFrame(array, quickI, quickJ, quickRight);
                return;
            }
        }

        if (quickJ >= 0 && quickJ < quickRight) {
            appendLog(QString("Comparing %1 with pivot %2").arg(array[quickJ]).arg(pivotValue));
            appendLog(stepLabel->text());
            // pseudocode: comparing line
            highlightPseudocodeLine(1);
            highlightComparison(quickJ, quickRight, quickRight);
            pushFrame(array, quickI, quickJ, quickRight);

            if (array[quickJ] < pivotValue) {
                quickI++;
                std::swap(array[quickI], array[quickJ]);
                drawArray(array);
                appendLog(QString("Swapped %1 and %2").arg(array[quickI]).arg(array[quickJ]));

                // pseudocode: swap occurred (line 2)
                highlightPseudocodeLine(2);

                pushFrame(array, quickI, quickJ, quickRight);
            }
            quickJ++;
            if (stepMode) {
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
            quickStack.push({ quickLeft, pivotIndex - 1 });
            quickStack.push({ pivotIndex + 1, quickRight });

            quickI = quickJ = quickPivot = -1;
            pivotValue = -1;

            pushFrame(array, quickI, quickJ, pivotIndex);

            // pseudocode: placing pivot (line 3)
            highlightPseudocodeLine(3);

            return;
        }

        if (quickStack.isEmpty() && quickI == -1 && quickJ == -1) {
            timer->stop();
            appendLog("Quick Sort complete.");
            appendLog("Array is sorted.");
            for (int k = 0; k < array.size(); ++k) sortedIndices.insert(k);
            highlightComparison(-1, -1, -1);

            drawArrayFinished(array);
            highlightPseudocodeLine(4);
        }
    }
    // End quick sort


    // Merge sort
    else if (currentAlgorithm == SortAlgorithm::Merge) {
        if (!mergeStack.empty()) {
            appendLog(QString("mergeStack size: %1").arg(mergeStack.size()));

            if (!merging) {
                auto [left, right, isMerge] = mergeStack.top();
                mergeStack.pop();

                if (!isMerge) {
                    if (left < right) {
                        int mid = (left + right) / 2;
                        mergeStack.push({ left, right, true });       // merge phase
                        mergeStack.push({ mid + 1, right, false });   // right half
                        mergeStack.push({ left, mid, false });        // left half

                        mergeLeftStartHistory.push_back(mergeLeftStart);
                        mergeLeftEndHistory.push_back(mergeLeftEnd);
                        mergeRightStartHistory.push_back(mergeRightStart);
                        mergeRightEndHistory.push_back(mergeRightEnd);
                        mergeMergedStartHistory.push_back(mergeMergedStart);
                        mergeMergedEndHistory.push_back(mergeMergedEnd);

                        pushFrame(array, -1, -1, -1);
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
            // highlight "merge" pseudocode line (line 4 in displayed list)
            highlightPseudocodeLine(4);

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

                    pushFrame(array, -1, -1, -1);

                }
                else {
                    mergeBuffer[mergeK++] = array[mergeJ++];

                    mergeLeftStartHistory.push_back(mergeLeftStart);
                    mergeLeftEndHistory.push_back(mergeLeftEnd);
                    mergeRightStartHistory.push_back(mergeRightStart);
                    mergeRightEndHistory.push_back(mergeRightEnd);
                    mergeMergedStartHistory.push_back(mergeMergedStart);
                    mergeMergedEndHistory.push_back(mergeMergedEnd);
                    pushFrame(array, -1, -1, -1);
                }
            }
            else if (mergeI <= mergeMid) {
                highlightComparison(mergeI, -1, -1);
                mergeBuffer[mergeK++] = array[mergeI++];

                mergeLeftStartHistory.push_back(mergeLeftStart);
                mergeLeftEndHistory.push_back(mergeLeftEnd);
                mergeRightStartHistory.push_back(mergeRightStart);
                mergeRightEndHistory.push_back(mergeRightEnd);
                mergeMergedStartHistory.push_back(mergeMergedStart);
                mergeMergedEndHistory.push_back(mergeMergedEnd);

                pushFrame(array, -1, -1, -1);
            }
else if (mergeJ <= mergeRight) {
                highlightComparison(-1, mergeJ, -1);
                mergeBuffer[mergeK++] = array[mergeJ++];

                mergeLeftStartHistory.push_back(mergeLeftStart);
                mergeLeftEndHistory.push_back(mergeLeftEnd);
                mergeRightStartHistory.push_back(mergeRightStart);
                mergeRightEndHistory.push_back(mergeRightEnd);
                mergeMergedStartHistory.push_back(mergeMergedStart);
                mergeMergedEndHistory.push_back(mergeMergedEnd);

                pushFrame(array, -1, -1, -1);
            }
            else {
                for (int i = mergeLeft; i <= mergeRight; ++i)
                    array[i] = mergeBuffer[i];

                highlightComparison(-1, -1, mergeK - 1);

                mergeLeftStartHistory.push_back(mergeLeftStart);
                mergeLeftEndHistory.push_back(mergeLeftEnd);
                mergeRightStartHistory.push_back(mergeRightStart);
                mergeRightEndHistory.push_back(mergeRightEnd);
                mergeMergedStartHistory.push_back(mergeMergedStart);
                mergeMergedEndHistory.push_back(mergeMergedEnd);

                pushFrame(array, -1, -1, -1);

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
                    highlightPseudocodeLine(5); // finished
                }
            }
        }
    }    // End merge sort

        // Heap sort

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

                // pseudocode: heapify step
                highlightPseudocodeLine(1);

                if (largest != heapI) {
                    std::swap(array[heapI], array[largest]);
                    heapStack.push(largest);
                    appendLog(QString("Heapify swap at %1 with %2").arg(heapI).arg(largest));
                }
                else {
                    appendLog(QString("Heapify compare at %1 (no swap)").arg(heapI));
                }

                // Live repaint and record frame
                highlightComparison(heapI, heapJ, -1);

                pushFrame(array, heapI, heapJ, -1);

                if (stepMode) { timer->stop(); return; }
                return;
            }
            else {
                // Transition to extraction
                heapBuilding = false;
                heapSwapping = true;
                appendLog("Max-heap built. Starting extraction.");
                // pseudocode: ready to extract
                highlightPseudocodeLine(2);
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

                // pseudocode: extract max swap
                highlightPseudocodeLine(2);

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

                // pseudocode: heapify step
                highlightPseudocodeLine(1);

                if (largest != heapI) {
                    std::swap(array[heapI], array[largest]);
                    heapStack.push(largest);
                    appendLog(QString("Re-heapify swap at %1 with %2").arg(heapI).arg(largest));
                }
                else {
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

                // pseudocode: finished
                highlightPseudocodeLine(3);
                return;
            }
        }
    }
    // End heap sort


    // Shell sort
    else if (currentAlgorithm == SortAlgorithm::Shell) {
        if (gap > 0) {
            if (shellI < static_cast<int>(array.size())) {
                if (!shellInserting) {
                    shellKey = array[shellI];
                    shellJ = shellI;
                    shellInserting = true;

                    appendLog(QString("Taking key = %1 at index %2").arg(shellKey).arg(shellI));

                    // pseudocode: gapped insertion start
                    highlightPseudocodeLine(1);

                    highlightComparison(shellI, -1, -1);
                    return;
                }

                if (shellJ >= gap && array[shellJ - gap] > shellKey) {
                    appendLog(QString("Gap %1: shifting %2 right").arg(gap).arg(array[shellJ - gap]));
                    array[shellJ] = array[shellJ - gap];
                    shellJ -= gap;

                    pushFrame(array, shellJ, shellJ + gap, -1);

                    // pseudocode: shifting in gapped insertion
                    highlightPseudocodeLine(2);

                    highlightComparison(shellJ, shellJ + gap, -1);
                    return;
                }
                else {
                    array[shellJ] = shellKey;
                    appendLog(QString("Inserted %1 at index %2").arg(shellKey).arg(shellJ));

                    sortedIndices.insert(shellJ);
                    shellInserting = false;

                    // pseudocode: insertion done (mark)
                    highlightPseudocodeLine(3);

                    highlightComparison(shellJ, -1, -1);

                    shellI++;
                }
            }
            else {
                gap /= 2;
                shellI = gap;

                // pseudocode: gap reduction
                highlightPseudocodeLine(2);
            }
        }
        else {
            timer->stop();
            appendLog("Shell Sort complete.");
            for (int k = 0; k < array.size(); ++k) sortedIndices.insert(k);

            shellI = -1;
            shellJ = -1;
            shellInserting = false;

            highlightComparison(-1, -1, -1);

            // pseudocode finished
            highlightPseudocodeLine(3);
        }
    }
    // End shell sort


    // TimSort (insertion then merge)
    else if (currentAlgorithm == SortAlgorithm::Tim) {
        if (timInserting) {
            if (timStart < (int)array.size()) {
                if (timI < timEnd) {
                    // When we start working on a new key, ensure we log it.
                    if (timJ == timI) {
                        appendLog(QString("Run [%1, %2): taking key at index %3 (value %4)")
                            .arg(timStart).arg(timEnd).arg(timI).arg(array[timI]));
                    }

                    if (timJ > timStart && array[timJ - 1] > timKey) {
                        appendLog(QString("Shifting index %1 (value %2) right to %3").arg(timJ - 1).arg(array[timJ - 1]).arg(timJ));
                        array[timJ] = array[timJ - 1];
                        timJ--;

                        pushFrame(array, timJ, timJ + 1, -1);

                        // pseudocode: insertion in run
                        highlightPseudocodeLine(1);

                        highlightComparison(timJ, timJ + 1, -1);
                        if (stepMode) { timer->stop(); return; }
                        return;
                    }
                    else {
                        // Insert key into its place and log it.
                        array[timJ] = timKey;
                        appendLog(QString("Inserted key %1 at index %2 (run [%3,%4))").arg(timKey).arg(timJ).arg(timStart).arg(timEnd));

                        sortedIndices.insert(timJ);
                        highlightComparison(timJ, -1, -1);

                        timI++;
                        timJ = timI;

                        if (timI < timEnd) {
                            timKey = array[timI];
                            highlightComparison(timI, -1, -1);
                        }
                        else {
                            // finished current run
                            timRuns.push_back({ timStart, timEnd });
                            appendLog(QString("Run sorted: [%1, %2)").arg(timStart).arg(timEnd));

                            timStart = timEnd;
                            timEnd = std::min(timStart + timRunSize, (int)array.size());

                            // prepare next run's indices (if any)
                            timI = timStart + 1;
                            timJ = timI;
                            timKey = (timI < timEnd) ? array[timI] : 0;

                            // If we still have more runs to process, log it
                            if (timStart < (int)array.size()) {
                                appendLog(QString("Starting next run: [%1, %2)").arg(timStart).arg(timEnd));
                                highlightComparison(timI, -1, -1);
                            }
                            else {
                                // No more runs to insert-sort; switch to merging phase
                                timInserting = false;
                                timMerging = true;
                                appendLog("All runs sorted. Starting merge phase.");
                                highlightPseudocodeLine(2);
                            }
                        }
                    }
                }
                else {
                    // timI >= timEnd: finalize current run and prepare next
                    timRuns.push_back({ timStart, timEnd });
                    appendLog(QString("Run sorted: [%1, %2)").arg(timStart).arg(timEnd));

                    timStart = timEnd;
                    timEnd = std::min(timStart + timRunSize, (int)array.size());

                    if (timStart < (int)array.size()) {
                        timI = timStart + 1;
                        timJ = timI;
                        timKey = (timI < timEnd) ? array[timI] : 0;
                        appendLog(QString("Starting next run: [%1, %2)").arg(timStart).arg(timEnd));
                        highlightComparison(timI, -1, -1);
                    }
                    else {
                        timInserting = false;
                        timMerging = true;
                        appendLog("All runs sorted. Starting merge phase.");
                        highlightPseudocodeLine(2);
                    }
                }
            }
            else {
                // no input left: switch to merging
                timInserting = false;
                timMerging = true;
            }
        }


        else if (timMerging) {
            if (timRuns.size() > 1) {

                auto rightRun = timRuns.back(); timRuns.pop_back();
                auto leftRun = timRuns.back(); timRuns.pop_back();

                timLeft = leftRun.first;
                timMid = leftRun.second;
                timRight = rightRun.second;

                appendLog(QString("Merging runs [%1,%2) and [%3,%4)").arg(timLeft).arg(timMid).arg(timMid).arg(timRight));

                timMergeBuffer.assign(array.begin() + timLeft, array.begin() + timMid);

                int i = 0;
                int j = timMid;
                int k = timLeft;

                while (i < (int)timMergeBuffer.size() && j < timRight) {
                    if (timMergeBuffer[i] <= array[j]) {
                        array[k++] = timMergeBuffer[i++];
                    }
                    else {
                        array[k++] = array[j++];
                    }

                    pushFrame(array, i + timLeft, j, -1);

                    highlightComparison(i + timLeft, j, -1);
                    // pseudocode: merging runs
                    highlightPseudocodeLine(2);
                    if (stepMode) { timer->stop(); return; }
                    return;
                }

                while (i < (int)timMergeBuffer.size()) {
                    array[k++] = timMergeBuffer[i++];
                }

                for (int idx = timLeft; idx < timRight; ++idx)
                    sortedIndices.insert(idx);

                timRuns.push_back({ timLeft, timRight });
                appendLog(QString("Merged runs into [%1, %2)").arg(timLeft).arg(timRight));
            }
            else {
                timer->stop();

                appendLog("TimSort complete.");
                for (int k = 0; k < (int)array.size(); ++k) sortedIndices.insert(k);
                highlightComparison(-1, -1, -1);
                timMerging = false;

                highlightPseudocodeLine(3);
            }
        }
        }
    // End TimSort


    // Radix sort
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

            // pseudocode: outer loop init
            highlightPseudocodeLine(0);
            return;
        }

        // Counting phase
        if (radixPhase == RadixPhase::Count) {
            if (radixIndex < (int)array.size()) {
                int digit = (array[radixIndex] / digitPlace) % 10;
                count[digit]++;
                // pseudocode: counting digits
                highlightPseudocodeLine(1);

                highlightComparison(radixIndex, -1, -1);
                pushFrame(array, radixIndex, -1, -1);

                appendLog(QString("Counting digit %1 at index %2")
                    .arg(digit).arg(radixIndex));

                radixIndex++;
                return;
            }
            else {
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
                // pseudocode: accumulate
                highlightPseudocodeLine(2);

                highlightComparison(-1, radixIndex, -1);
                pushFrame(array, -1, radixIndex, -1);

                appendLog(QString("Accumulating bucket %1").arg(radixIndex));

                radixIndex++;
                return;
            }
            else {
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
                // pseudocode: place into bucket
                highlightPseudocodeLine(3);

                highlightComparison(radixIndex, -1, -1);
                pushFrame(array, radixIndex, -1, -1);

                appendLog(QString("Placing value %1 (digit %2) into bucket[%3]")
                    .arg(array[radixIndex])
                    .arg(digit)
                    .arg(count[digit]));


                radixIndex--;
                return;
            }
            else {
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
                // pseudocode: copy back
                highlightPseudocodeLine(4);

                highlightComparison(-1, -1, radixIndex);
                pushFrame(array, -1, -1, radixIndex);

                appendLog(QString("Copying back value %1 to index %2")
                    .arg(array[radixIndex]).arg(radixIndex));

                radixIndex++;
                return;
            }
            else {
                digitPlace *= 10;
                if (digitPlace <= maxValue) {
                    radixPhase = RadixPhase::Count;
                    radixIndex = 0;
                    std::fill(count.begin(), count.end(), 0);
                    std::fill(bucket.begin(), bucket.end(), 0);

                    appendLog(QString("Next digit place: %1").arg(digitPlace));
                    return;
                }
                else {
                    timer->stop();
                    drawArrayFinished(array);
                    appendLog("Radix Sort complete.");
                    // pseudocode finished
                    highlightPseudocodeLine(5);
                }
            }
        }
    }
    // End radix sort


    // Gnome sort
    else if (currentAlgorithm == SortAlgorithm::Gnome) {
        if (gnomeIndex < array.size()) {
            int index1 = gnomeIndex;
            int index2 = gnomeIndex - 1;

            if (gnomeIndex == 0 || array[index1] >= array[index2]) {
                gnomeIndex++;
                // pseudocode: advancing
                highlightPseudocodeLine(2);
            }
            else {
                std::swap(array[index1], array[index2]);
                gnomeIndex--;
                // pseudocode: swap
                highlightPseudocodeLine(3);
            }

            highlightComparison(index1, index2, -1);

            pushFrame(array, index1, index2, -1);

            appendLog(QString("Comparing indices %1 and %2").arg(index1).arg(index2));
            return;
        }
        else {
            timer->stop();
            drawArrayFinished(array);
            appendLog("Gnome Sort complete.");
            highlightPseudocodeLine(4);
        }
    }
    // End gnome sort
}

void MainWindow::updateScene() {
    scene->clear();

    if (array.empty()) return;

    int maxVal = *std::max_element(array.begin(), array.end());
    int maxBarHeight = 150;
    int x = 10;

    const QSet<int>* activeSorted = displayedSortedIndices.isEmpty() ? &sortedIndices : &displayedSortedIndices;


    for (size_t index = 0; index < array.size(); ++index) {
        int val = array[index];
        int barHeight = (maxVal > 0) ? (val * maxBarHeight / maxVal) : 0;

        QColor color = Qt::blue;

        // Optional: highlight logic (minimal)
        if (currentAlgorithm == SortAlgorithm::Bubble && (index == i || index == j)) {
            color = Qt::red;
        }
        else if (currentAlgorithm == SortAlgorithm::Quick && index == quickPivot) {
            color = Qt::yellow;
        }
        else if (activeSorted->contains(static_cast<int>(index))) {
            color = Qt::green;
        }

        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(color));

        QGraphicsTextItem* text = scene->addText(QString::number(val));
        text->setPos(x, 200 + 5);

        x += 30;
    }
}

void MainWindow::setStep(const QString& msg) {
    stepLabel->setText(msg);
}

void MainWindow::appendLog(const QString& msg) {
    logView->appendPlainText(msg);
}

void MainWindow::drawArray(const std::vector<int>& arr) {
    scene->clear();

    if (arr.empty()) return;

    int maxVal = *std::max_element(arr.begin(), arr.end());
    int maxBarHeight = 150;

    int x = 10;
    for (int val : arr) {
        int barHeight = (maxVal > 0) ? (val * maxBarHeight / maxVal) : 0;

        QColor barColor = darkModeEnabled
                              ? QColor(30, 144, 255)   // light blue for dark mode
                              : QColor(65, 105, 225);   // royal blue for light mode

        scene->addRect(x, 200 - barHeight, 20, barHeight, QPen(Qt::black), QBrush(barColor));

        QGraphicsTextItem* text = scene->addText(QString::number(val));
        text->setPos(x, 200 + 5);

        x += 30;

    }
    scene->update();
}

void MainWindow::drawArrayFinished(const std::vector<int>& arr) {

    scene->clear();

    if (arr.empty()) return;

    int maxVal = *std::max_element(array.begin(), array.end());
    int maxBarHeight = 150;

    int x = 10;
    for (int val : arr) {
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

    const QSet<int>* activeSorted = displayedSortedIndices.isEmpty() ? &sortedIndices : &displayedSortedIndices;

    // Build a concise step description to display above the bars.
    QString stepMsg;
    // Radix has distinct phases
    if (currentAlgorithm == SortAlgorithm::Radix) {
        switch (radixPhase) {
        case RadixPhase::Count:
            if (index1 >= 0 && index1 < (int)array.size())
                stepMsg = QString("Counting digit at index %1 (value %2)").arg(index1).arg(array[index1]);
            else
                stepMsg = QString("Counting digits...");
            break;
        case RadixPhase::Accumulate:
            if (index2 >= 0)
                stepMsg = QString("Accumulating bucket %1").arg(index2);
            else
                stepMsg = QString("Accumulating buckets...");
            break;
        case RadixPhase::Place:
            if (index1 >= 0 && index1 < (int)array.size())
                stepMsg = QString("Placing value %1 (index %2)").arg(array[index1]).arg(index1);
            else
                stepMsg = QString("Placing values into buckets...");
            break;
        case RadixPhase::CopyBack:
            if (pivotIndex >= 0)
                stepMsg = QString("Copying back value to index %1").arg(pivotIndex);
            else
                stepMsg = QString("Copying back from buckets...");
            break;
        }
    }
    // Quick sort pivot/info
    else if (currentAlgorithm == SortAlgorithm::Quick) {
        if (pivotIndex >= 0 && pivotIndex < (int)array.size() && (index1 < 0 && index2 < 0))
            stepMsg = QString("Pivot at index %1 (value %2)").arg(pivotIndex).arg(array[pivotIndex]);
        else if (index1 >= 0 && pivotIndex >= 0)
            stepMsg = QString("Comparing index %1 (%2) with pivot index %3 (%4)").arg(index1).arg(array[index1]).arg(pivotIndex).arg(array[pivotIndex]);
        else if (index1 >= 0 && index2 >= 0)
            stepMsg = QString("Comparing index %1 (%2) and %3 (%4)").arg(index1).arg(array[index1]).arg(index2).arg(array[index2]);
    }
    // Merge: show merging range if available
    else if (currentAlgorithm == SortAlgorithm::Merge) {
        if (mergeLeftStart >= 0 && mergeMergedEnd >= 0)
            stepMsg = QString("Merging range [%1, %2]").arg(mergeLeftStart).arg(mergeMergedEnd);
        else if (index1 >= 0 && index2 >= 0)
            stepMsg = QString("Comparing left %1 (%2) and right %3 (%4)").arg(index1).arg(array[index1]).arg(index2).arg(array[index2]);
        else if (index1 >= 0)
            stepMsg = QString("Taking from left index %1 (%2)").arg(index1).arg(array[index1]);
        else if (index2 >= 0)
            stepMsg = QString("Taking from right index %1 (%2)").arg(index2).arg(array[index2]);
    }
    // Generic messages for other algorithms
    else {
        if (index1 >= 0 && index2 >= 0 && index1 < (int)array.size() && index2 < (int)array.size()) {
            stepMsg = QString("Comparing index %1 (%2) and %3 (%4)").arg(index1).arg(array[index1]).arg(index2).arg(array[index2]);
        }
        else if (index1 >= 0 && index1 < (int)array.size()) {
            stepMsg = QString("Active index %1 (value %2)").arg(index1).arg(array[index1]);
        }
        else if (index2 >= 0 && index2 < (int)array.size()) {
            stepMsg = QString("Active index %1 (value %2)").arg(index2).arg(array[index2]);
        }
        else if (pivotIndex >= 0 && pivotIndex < (int)array.size()) {
            stepMsg = QString("Pivot at index %1 (value %2)").arg(pivotIndex).arg(array[pivotIndex]);
        }
        else {
            stepMsg.clear();
        }
    }

    // Display the step description above the array (as a scene text item)
    if (!stepMsg.isEmpty()) {
        QGraphicsTextItem* header = scene->addText(stepMsg);
        QFont f = header->font();
        f.setBold(true);
        header->setFont(f);
        header->setDefaultTextColor(Qt::black);
        // Position header above the bars; bars start at y = 200 - maxBarHeight
        header->setPos(10, 200 - maxBarHeight - 24);
    }

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
            else if (activeSorted->contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Heap) {
            if (k == index1)
                color = QColor(255, 165, 0);
            else if (k == index2)
                color = QColor(255, 0, 0);
            else if (activeSorted->contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Bubble) {
            if (k == index1 || k == index2)
                color = QColor(220, 20, 60);
            else if (activeSorted->contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Insertion) {
            if (k == index1)
                color = QColor(65, 105, 225);
            else if (k == index2)
                color = QColor(255, 165, 0);
            else if (activeSorted->contains(k))
                color = QColor(0, 255, 0);
        }

        if (currentAlgorithm == SortAlgorithm::Selection) {
            if (k == index1)
                color = QColor(128, 0, 128);
            else if (k == index2)
                color = QColor(255, 0, 0);
            else if (activeSorted->contains(k))
                color = QColor(0, 255, 0);
        }
        if (currentAlgorithm == SortAlgorithm::Shell) {
            if (shellInserting && k == shellI)
                color = QColor(65, 105, 225);
            else if (shellInserting && (k == shellJ || k == shellJ + gap))
                color = QColor(255, 165, 0);
            else if (activeSorted->contains(k))
                color = QColor(0, 255, 0);
        }
        if (currentAlgorithm == SortAlgorithm::Tim) {
            if (timInserting && k == timI)
                color = QColor(65, 105, 225);
            else if ((timInserting && (k == timJ || k == timJ + 1)) ||
                (timMerging && (k == timLeft || k == timMid)))
                color = QColor(255, 165, 0);
            else if (activeSorted->contains(k))
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
            else if (activeSorted->contains(k)) {
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
