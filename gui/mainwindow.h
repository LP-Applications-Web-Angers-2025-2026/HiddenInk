#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QTabWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QSpinBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void onHideData();
    void onBrowseCoverImage();
    void onBrowseSecretFile();
    void onBrowseHideOutput();
    void onHideTypeChanged(int id);
    
    
    void onExtractData();
    void onBrowseExtractImage();
    void onBrowseExtractOutput();
    
   
    void onCompareImages();
    void onGenerateHistogram();
    void onDetectSteganography();
    void onBrowseImage1();
    void onBrowseImage2();
    void onBrowseAnalysisImage();
    
  
    void onOpenGithub();

private:
    void setupUI();
    void setupHideTab();
    void setupExtractTab();
    void setupAnalysisTab();
    void setupAboutTab();
    void applyStyles();
    void showSuccess(const QString& message);
    void showError(const QString& message);
    void showInfo(const QString& message);
    QString getExecutablePath();
    
   
    QWidget *hideTab;
    QRadioButton *hideTextRadio;
    QRadioButton *hideImageRadio;
    QRadioButton *hideFileRadio;
    QButtonGroup *hideTypeGroup;
    
    QLineEdit *coverImageInput;
    QTextEdit *secretTextInput;
    QLineEdit *secretFileInput;
    QLineEdit *hideOutputInput;
    QComboBox *hideFormatCombo;
    QSpinBox *hideBitSpin;
    QLineEdit *hidePasswordInput;
    QCheckBox *hideEncryptCheck;
    QPushButton *hideButton;
    QPushButton *browseCoverBtn;
    QPushButton *browseSecretFileBtn;
    QPushButton *browseHideOutputBtn;
    QLabel *hideStatusLabel;
    
   
    QWidget *extractTab;
    QRadioButton *extractTextRadio;
    QRadioButton *extractImageRadio;
    QButtonGroup *extractTypeGroup;
    QLineEdit *extractImageInput;
    QLineEdit *extractOutputInput;
    QSpinBox *extractBitSpin;
    QLineEdit *extractPasswordInput;
    QCheckBox *extractDecryptCheck;
    QPushButton *extractButton;
    QPushButton *browseExtractImageBtn;
    QPushButton *browseExtractOutputBtn;
    QLabel *extractStatusLabel;
    QTextEdit *extractedDataDisplay;
    
    // Analysis tab widgets
    QWidget *analysisTab;
    QLineEdit *image1Input;
    QLineEdit *image2Input;
    QLineEdit *analysisImageInput;
    QPushButton *compareButton;
    QPushButton *histogramButton;
    QPushButton *detectButton;
    QPushButton *browseImage1Btn;
    QPushButton *browseImage2Btn;
    QPushButton *browseAnalysisBtn;
    QTextEdit *analysisResultDisplay;
    QLabel *analysisStatusLabel;
    
    // About tab
    QWidget *aboutTab;
    
    QTabWidget *tabWidget;
};

#endif // MAINWINDOW_H
