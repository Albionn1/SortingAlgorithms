
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QStack>
#include <QPair>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <stack>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    enum class SortAlgorithm { Bubble, Insertion, Selection, Quick, Merge, Heap, Shell, Tim, Radix, Gnome };
private slots:
    void onStartClicked();
    void onTimerTick();
    void onResetClicked();
    void onStepModeToggled(bool checked);
    void onSliderMoved(int value);
    void onRandomClicked();
    void onAlgorithmSelected(const QString& selected);
    void onControlsChanged();

private:



    SortAlgorithm currentAlgorithm;
    //Sorting state
    int i = 0, j = 0, key = 0, minIndex = 0;

    int quickLeft = 0, quickRight = 0;
    QStack<QPair<int, int>> quickStack;
    int quickPivot = -1;
    int quickI = -1, quickJ = -1;
    bool inserting = false;
    int pivotValue = -1;

    int mergeLeftStart = -1, mergeLeftEnd = -1;
    int mergeRightStart = -1, mergeRightEnd = -1;
    int mergeMergedStart = -1, mergeMergedEnd = -1;

    // QStack<QPair<int, int>> mergeStack;
    std::vector<int> mergeBuffer;
    std::stack<std::tuple<int, int, bool>> mergeStack; // bool = isMergePhase

    int mergeLeft = -1, mergeMid = -1, mergeRight = -1;
    int mergeI = -1, mergeJ = -1, mergeK = -1;
    bool merging = false;

    int heapSize = 0;
    bool heapBuilding = false;
    bool heapSwapping = false;
    std::stack<int> heapStack;
    int heapI = -1;
    int heapJ = -1;

    int gap;
    int shellI;
    int shellJ;
    int shellKey;
    bool shellInserting;


    int timRunSize;
    std::vector<std::pair<int, int>> timRuns;
    int timStart, timEnd;
    int timI, timJ, timKey;
    bool timInserting;
    bool timMerging;
    int timLeft, timMid, timRight;
    std::vector<int> timMergeBuffer;

    bool radixInitialized = false;
    int maxValue;
    int digitPlace;
    int radixIndex;
    std::vector<int> count = std::vector<int>(10, 0);
    std::vector<int> bucket;
    enum class RadixPhase { Count, Accumulate, Place, CopyBack };
    RadixPhase radixPhase;

    int gnomeIndex;

    inline void pushFrame(const std::vector<int>& arr, int i, int j, int pivot = -1) {
        history.push_back(arr);
        iHistory.push_back(i);
        jHistory.push_back(j);
        pivotHistory.push_back(pivot);
        // record which indices are currently considered sorted (snapshot)
        sortedIndicesHistory.push_back(sortedIndices);
        // record radix-specific state for this frame
        radixPhaseHistory.push_back(radixPhase);
        radixIndexHistory.push_back(radixIndex);
        // record shell per-frame state
        shellIHistory.push_back(shellI);
        shellJHistory.push_back(shellJ);
        shellInsertingHistory.push_back(shellInserting ? 1 : 0);
        // record tim per-frame state
        timIHistory.push_back(timI);
        timJHistory.push_back(timJ);
        timInsertingHistory.push_back(timInserting ? 1 : 0);
        timMergingHistory.push_back(timMerging ? 1 : 0);
        timLeftHistory.push_back(timLeft);
        timMidHistory.push_back(timMid);
        timRightHistory.push_back(timRight);
        {
            QSignalBlocker block(slider);
            int step = static_cast<int>(history.size()) - 1;
            slider->setMaximum(step);
            slider->setValue(step);
        }
    }


    bool stepMode = false;

    Ui::MainWindow* ui;
    QLineEdit* inputField;
    QPushButton* startButton;
    QGraphicsView* view;
    QGraphicsScene* scene;
    QTimer* timer;
    QLabel* stepLabel;
    QPlainTextEdit* logView;
    QLabel* descriptionLabel;
    QSlider* delayBox;
    QComboBox* algorithmBox;
    QLabel* legendLabel;
    QPushButton* resetButton;
    QCheckBox* stepByStepCheck;
    QPushButton* nextStepButton;
    QHBoxLayout* legendLayout;
    QLabel* colorBox;
    QLabel* textLabel;
    QHBoxLayout* itemLayout;
    QSlider* slider;
    QLabel* sliderLabel;
    QLabel* stepCounterLabel;
    QLabel* stepDescriptionLabel;
    QPushButton* randomButton;
    QSpinBox* sizeSpinBox;
    QComboBox* distributionBox;
    QSlider* nearlySortedSlider;
    QLabel* nearlySortedValueLabel;

    void generateArrayFromControls(bool log = true);
    QGraphicsTextItem* complexityLabel = nullptr;
    QLabel* legendTitleLabel;

    // Pseudocode widget
    QListWidget* pseudocodeView;
    int pseudocodeCurrent = -1;
    void setPseudocode(const QStringList& lines);
    void highlightPseudocodeLine(int index); // index is 0-based

    std::vector<int> array;
    std::vector<std::vector<int>> history;
    std::vector<int> pivotHistory;
    std::vector<int> iHistory;
    std::vector<int> jHistory;
    std::vector<QSet<int>> sortedIndicesHistory;
    std::vector<RadixPhase> radixPhaseHistory;
    std::vector<int> radixIndexHistory;
    // Shell sort per-frame state
    std::vector<int> shellIHistory;
    std::vector<int> shellJHistory;
    std::vector<int> shellInsertingHistory;
    // Tim sort per-frame state
    std::vector<int> timIHistory;
    std::vector<int> timJHistory;
    std::vector<int> timInsertingHistory;
    std::vector<int> timMergingHistory;
    std::vector<int> timLeftHistory;
    std::vector<int> timMidHistory;
    std::vector<int> timRightHistory;
    QSet<int> sortedIndices;
    QSet<int> displayedSortedIndices; // used when scrubbing history to show per-frame sorted state
    std::vector<int> mergeLeftStartHistory;
    std::vector<int> mergeLeftEndHistory;
    std::vector<int> mergeRightStartHistory;
    std::vector<int> mergeRightEndHistory;
    std::vector<int> mergeMergedStartHistory;
    std::vector<int> mergeMergedEndHistory;


    int currentStep = 0;
    int stepDelay;

    void drawArray(const std::vector<int>& arr);
    void highlightComparison(int index1, int index2, int pivotIndex); //If the algorithm doesn't contain any pivot just pass -1 as the third argument so it doesn't throw errors
    void drawArrayFinished(const std::vector<int>& arr);
    void setStep(const QString& msg);
    void appendLog(const QString& msg);
    void updateScene();
};
#endif // MAINWINDOW_H
