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
#include <QSpinBox>
#include <QComboBox>
#include <QStack>
#include <QPair>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QWidget>
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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onTimerTick();
    void onResetClicked();
    void onStepModeToggled(bool checked);
    void onSliderMoved(int value);
    void onRandomClicked();
    void onAlgorithmSelected(const QString& selected);

private:

    enum class SortAlgorithm{Bubble, Insertion, Selection, Quick, Merge, Heap, Shell, Tim, Radix, Gnome};
    SortAlgorithm currentAlgorithm;

    //Sorting state
    int i = 0, j = 0, key = 0, minIndex =0;

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
    std::vector<std::pair<int,int>> timRuns;
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
        {
            QSignalBlocker block(slider);
            int step = static_cast<int>(history.size()) - 1;
            slider->setMaximum(step);
            slider->setValue(step);
        }
    }


    bool stepMode = false;

    Ui::MainWindow *ui;
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
    QGraphicsTextItem* complexityLabel = nullptr;
    QLabel* legendTitleLabel;


    std::vector<int> array;
    std::vector<std::vector<int>> history;
    std::vector<int> pivotHistory;
    std::vector<int> iHistory;
    std::vector<int> jHistory;
    QSet<int> sortedIndices;
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
