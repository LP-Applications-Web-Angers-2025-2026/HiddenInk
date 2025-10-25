#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QScrollArea>
#include <QDesktopServices>
#include <QUrl>
#include <QFont>
#include <QApplication>
#include <QDir>
#include <QDateTime>
#include <QTemporaryFile>
#include <sstream>

// Inclure les fonctions de stéganographie directement
#include "../stenographie/PNG/png_hide_text.h"
#include "../stenographie/PNG/png_hide_image.h"
#include "../stenographie/PNG/png_extract.h"
#include "../stenographie/utils/analysis/image_analysis.hpp"
#include "../stenographie/utils/stegano/stegano_text.hpp"
#include "../stenographie/utils/stegano/stegano_imageinimage.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyStyles();
    
    setWindowTitle("HiddenInk - Professional Steganography Tool");
    setMinimumSize(1000, 750);
    resize(1100, 800);
}

MainWindow::~MainWindow()
{
}

QString MainWindow::getExecutablePath()
{
    // Le chemin de l'exécutable HiddenInk doit être dans le dossier parent
    QDir currentDir = QDir::current();
    QString exePath = currentDir.filePath("HiddenInk.exe");
    
    // Si on est dans le dossier gui, remonter d'un niveau
    if (currentDir.dirName() == "gui" || currentDir.dirName().contains("build")) {
        currentDir.cdUp();
        exePath = currentDir.filePath("HiddenInk.exe");
    }
    
    return exePath;
}

void MainWindow::setupUI()
{
    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create tab widget
    tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    
    // Setup tabs
    setupHideTab();
    setupExtractTab();
    setupAnalysisTab();
    setupAboutTab();
    
    // Add tabs to tab widget
    tabWidget->addTab(hideTab, "Hide Data");
    tabWidget->addTab(extractTab, "Extract Data");
    tabWidget->addTab(analysisTab, "Image Analysis");
    tabWidget->addTab(aboutTab, "About");
    
    mainLayout->addWidget(tabWidget);
    setCentralWidget(centralWidget);
}

void MainWindow::setupHideTab()
{
    hideTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(hideTab);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    QLabel *titleLabel = new QLabel("Hide Secret Data in an Image");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #1e293b; margin-bottom: 10px;");
    layout->addWidget(titleLabel);
    
    // Type selection
    QGroupBox *typeGroup = new QGroupBox("Data Type");
    QHBoxLayout *typeLayout = new QHBoxLayout(typeGroup);
    
    hideTypeGroup = new QButtonGroup(this);
    hideTextRadio = new QRadioButton("Text Message");
    hideImageRadio = new QRadioButton("Image File");
    hideFileRadio = new QRadioButton("Any File");
    
    hideTextRadio->setChecked(true);
    hideTypeGroup->addButton(hideTextRadio, 0);
    hideTypeGroup->addButton(hideImageRadio, 1);
    hideTypeGroup->addButton(hideFileRadio, 2);
    
    typeLayout->addWidget(hideTextRadio);
    typeLayout->addWidget(hideImageRadio);
    typeLayout->addWidget(hideFileRadio);
    typeLayout->addStretch();
    
    layout->addWidget(typeGroup);
    
    // Cover Image Section
    QGroupBox *coverGroup = new QGroupBox("Cover Image");
    QHBoxLayout *coverLayout = new QHBoxLayout(coverGroup);
    coverImageInput = new QLineEdit();
    coverImageInput->setPlaceholderText("Select the image that will hide your secret data");
    browseCoverBtn = new QPushButton("Browse");
    browseCoverBtn->setFixedWidth(100);
    coverLayout->addWidget(coverImageInput);
    coverLayout->addWidget(browseCoverBtn);
    
    layout->addWidget(coverGroup);
    
    // Secret Data Section
    QGroupBox *secretGroup = new QGroupBox("Secret Data");
    QVBoxLayout *secretLayout = new QVBoxLayout(secretGroup);
    
    // Text input
    QLabel *textLabel = new QLabel("Text Message:");
    secretTextInput = new QTextEdit();
    secretTextInput->setPlaceholderText("Enter your secret message here...");
    secretTextInput->setMaximumHeight(100);
    
    // File input
    QLabel *fileLabel = new QLabel("File:");
    QHBoxLayout *fileLayout = new QHBoxLayout();
    secretFileInput = new QLineEdit();
    secretFileInput->setPlaceholderText("Select a file to hide");
    secretFileInput->setEnabled(false);
    browseSecretFileBtn = new QPushButton("Browse");
    browseSecretFileBtn->setFixedWidth(100);
    browseSecretFileBtn->setEnabled(false);
    fileLayout->addWidget(secretFileInput);
    fileLayout->addWidget(browseSecretFileBtn);
    
    secretLayout->addWidget(textLabel);
    secretLayout->addWidget(secretTextInput);
    secretLayout->addWidget(fileLabel);
    secretLayout->addLayout(fileLayout);
    
    layout->addWidget(secretGroup);
    
    // Options Section
    QGroupBox *optionsGroup = new QGroupBox("Steganography Options");
    QGridLayout *optionsLayout = new QGridLayout(optionsGroup);
    optionsLayout->setColumnStretch(1, 1);
    
    QLabel *formatLabel = new QLabel("Output Format:");
    hideFormatCombo = new QComboBox();
    hideFormatCombo->addItems({"PNG (Recommended)", "BMP (Lossless)"});
    
    QLabel *bitLabel = new QLabel("Bit Position:");
    hideBitSpin = new QSpinBox();
    hideBitSpin->setRange(1, 8);
    hideBitSpin->setValue(1);
    hideBitSpin->setPrefix("Bit ");
    hideBitSpin->setSuffix(" (LSB)");
    
    hideEncryptCheck = new QCheckBox("Encrypt data");
    hidePasswordInput = new QLineEdit();
    hidePasswordInput->setPlaceholderText("Encryption key (will be generated if empty)");
    hidePasswordInput->setEchoMode(QLineEdit::Password);
    hidePasswordInput->setEnabled(false);
    
    optionsLayout->addWidget(formatLabel, 0, 0);
    optionsLayout->addWidget(hideFormatCombo, 0, 1);
    optionsLayout->addWidget(bitLabel, 1, 0);
    optionsLayout->addWidget(hideBitSpin, 1, 1);
    optionsLayout->addWidget(hideEncryptCheck, 2, 0, 1, 2);
    optionsLayout->addWidget(hidePasswordInput, 3, 0, 1, 2);
    
    layout->addWidget(optionsGroup);
    
    // Output Section
    QGroupBox *outputGroup = new QGroupBox("Output");
    QHBoxLayout *outputLayout = new QHBoxLayout(outputGroup);
    hideOutputInput = new QLineEdit();
    hideOutputInput->setPlaceholderText("Output image path (auto-generated if empty)");
    browseHideOutputBtn = new QPushButton("Browse");
    browseHideOutputBtn->setFixedWidth(100);
    outputLayout->addWidget(hideOutputInput);
    outputLayout->addWidget(browseHideOutputBtn);
    
    layout->addWidget(outputGroup);
    
    // Action Button
    hideButton = new QPushButton("Hide Data in Image");
    hideButton->setFixedHeight(50);
    hideButton->setCursor(Qt::PointingHandCursor);
    hideButton->setObjectName("primaryButton");
    
    layout->addWidget(hideButton);
    
    // Status Label
    hideStatusLabel = new QLabel("");
    hideStatusLabel->setWordWrap(true);
    hideStatusLabel->setAlignment(Qt::AlignCenter);
    hideStatusLabel->setMinimumHeight(30);
    layout->addWidget(hideStatusLabel);
    
    layout->addStretch();
    
    // Connect signals
    connect(browseCoverBtn, &QPushButton::clicked, this, &MainWindow::onBrowseCoverImage);
    connect(browseSecretFileBtn, &QPushButton::clicked, this, &MainWindow::onBrowseSecretFile);
    connect(browseHideOutputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseHideOutput);
    connect(hideButton, &QPushButton::clicked, this, &MainWindow::onHideData);
    connect(hideTypeGroup, &QButtonGroup::idClicked, this, &MainWindow::onHideTypeChanged);
    connect(hideEncryptCheck, &QCheckBox::toggled, hidePasswordInput, &QLineEdit::setEnabled);
}

void MainWindow::setupExtractTab()
{
    extractTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(extractTab);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    QLabel *titleLabel = new QLabel("Extract Hidden Data from an Image");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #1e293b; margin-bottom: 10px;");
    layout->addWidget(titleLabel);
    
    // Data Type Selection
    QGroupBox *typeGroup = new QGroupBox("What to Extract");
    QHBoxLayout *typeLayout = new QHBoxLayout(typeGroup);
    
    extractTextRadio = new QRadioButton("Extract Text Message");
    extractImageRadio = new QRadioButton("Extract Hidden Image");
    extractTextRadio->setChecked(true);
    
    extractTypeGroup = new QButtonGroup(this);
    extractTypeGroup->addButton(extractTextRadio, 0);
    extractTypeGroup->addButton(extractImageRadio, 1);
    
    typeLayout->addWidget(extractTextRadio);
    typeLayout->addWidget(extractImageRadio);
    typeLayout->addStretch();
    
    layout->addWidget(typeGroup);
    
    // Input Image Section
    QGroupBox *inputGroup = new QGroupBox("Input Image");
    QHBoxLayout *inputLayout = new QHBoxLayout(inputGroup);
    extractImageInput = new QLineEdit();
    extractImageInput->setPlaceholderText("Select the image containing hidden data");
    browseExtractImageBtn = new QPushButton("Browse");
    browseExtractImageBtn->setFixedWidth(100);
    inputLayout->addWidget(extractImageInput);
    inputLayout->addWidget(browseExtractImageBtn);
    
    layout->addWidget(inputGroup);
    
    // Options Section
    QGroupBox *optionsGroup = new QGroupBox("Extraction Options");
    QGridLayout *optionsLayout = new QGridLayout(optionsGroup);
    optionsLayout->setColumnStretch(1, 1);
    
    QLabel *bitLabel = new QLabel("Bit Position:");
    extractBitSpin = new QSpinBox();
    extractBitSpin->setRange(1, 8);
    extractBitSpin->setValue(1);
    extractBitSpin->setPrefix("Bit ");
    extractBitSpin->setSuffix(" (LSB)");
    
    extractDecryptCheck = new QCheckBox("Decrypt data");
    extractPasswordInput = new QLineEdit();
    extractPasswordInput->setPlaceholderText("Decryption key (hex format)");
    extractPasswordInput->setEchoMode(QLineEdit::Password);
    extractPasswordInput->setEnabled(false);
    
    optionsLayout->addWidget(bitLabel, 0, 0);
    optionsLayout->addWidget(extractBitSpin, 0, 1);
    optionsLayout->addWidget(extractDecryptCheck, 1, 0, 1, 2);
    optionsLayout->addWidget(extractPasswordInput, 2, 0, 1, 2);
    
    layout->addWidget(optionsGroup);
    
    // Output Section (optional)
    QGroupBox *outputGroup = new QGroupBox("Output File (Optional)");
    QVBoxLayout *outputMainLayout = new QVBoxLayout(outputGroup);
    QLabel *outputInfo = new QLabel("Leave empty to display text in the window below. Specify a path for binary files.");
    outputInfo->setStyleSheet("color: #64748b; font-size: 12px; font-style: italic;");
    outputInfo->setWordWrap(true);
    
    QHBoxLayout *outputLayout = new QHBoxLayout();
    extractOutputInput = new QLineEdit();
    extractOutputInput->setPlaceholderText("Output file path");
    browseExtractOutputBtn = new QPushButton("Browse");
    browseExtractOutputBtn->setFixedWidth(100);
    outputLayout->addWidget(extractOutputInput);
    outputLayout->addWidget(browseExtractOutputBtn);
    
    outputMainLayout->addWidget(outputInfo);
    outputMainLayout->addLayout(outputLayout);
    
    layout->addWidget(outputGroup);
    
    // Action Button
    extractButton = new QPushButton("Extract Hidden Data");
    extractButton->setFixedHeight(50);
    extractButton->setCursor(Qt::PointingHandCursor);
    extractButton->setObjectName("primaryButton");
    
    layout->addWidget(extractButton);
    
    // Status Label
    extractStatusLabel = new QLabel("");
    extractStatusLabel->setWordWrap(true);
    extractStatusLabel->setAlignment(Qt::AlignCenter);
    extractStatusLabel->setMinimumHeight(30);
    layout->addWidget(extractStatusLabel);
    
    // Extracted Data Display
    QGroupBox *displayGroup = new QGroupBox("Extracted Data");
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    extractedDataDisplay = new QTextEdit();
    extractedDataDisplay->setReadOnly(true);
    extractedDataDisplay->setPlaceholderText("Extracted text message will appear here...\nBinary files will be saved to the specified output path.");
    extractedDataDisplay->setMinimumHeight(150);
    displayLayout->addWidget(extractedDataDisplay);
    
    layout->addWidget(displayGroup);
    layout->addStretch();
    
    // Connect signals
    connect(browseExtractImageBtn, &QPushButton::clicked, this, &MainWindow::onBrowseExtractImage);
    connect(browseExtractOutputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseExtractOutput);
    connect(extractButton, &QPushButton::clicked, this, &MainWindow::onExtractData);
    connect(extractDecryptCheck, &QCheckBox::toggled, extractPasswordInput, &QLineEdit::setEnabled);
}

void MainWindow::setupAnalysisTab()
{
    analysisTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(analysisTab);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    QLabel *titleLabel = new QLabel("Image Analysis Tools");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #1e293b; margin-bottom: 10px;");
    layout->addWidget(titleLabel);
    
    // Compare Images Section
    QGroupBox *compareGroup = new QGroupBox("Compare Two Images");
    QVBoxLayout *compareLayout = new QVBoxLayout(compareGroup);
    
    QLabel *compareDesc = new QLabel("Calculate MSE and PSNR between original and stego images");
    compareDesc->setStyleSheet("color: #64748b; font-size: 12px; font-style: italic; margin-bottom: 8px;");
    compareLayout->addWidget(compareDesc);
    
    QHBoxLayout *img1Layout = new QHBoxLayout();
    QLabel *img1Label = new QLabel("Image 1:");
    img1Label->setFixedWidth(80);
    image1Input = new QLineEdit();
    image1Input->setPlaceholderText("First image (e.g., original)");
    browseImage1Btn = new QPushButton("Browse");
    browseImage1Btn->setFixedWidth(100);
    img1Layout->addWidget(img1Label);
    img1Layout->addWidget(image1Input);
    img1Layout->addWidget(browseImage1Btn);
    
    QHBoxLayout *img2Layout = new QHBoxLayout();
    QLabel *img2Label = new QLabel("Image 2:");
    img2Label->setFixedWidth(80);
    image2Input = new QLineEdit();
    image2Input->setPlaceholderText("Second image (e.g., stego)");
    browseImage2Btn = new QPushButton("Browse");
    browseImage2Btn->setFixedWidth(100);
    img2Layout->addWidget(img2Label);
    img2Layout->addWidget(image2Input);
    img2Layout->addWidget(browseImage2Btn);
    
    compareButton = new QPushButton("Compare Images");
    compareButton->setFixedHeight(40);
    compareButton->setCursor(Qt::PointingHandCursor);
    
    compareLayout->addLayout(img1Layout);
    compareLayout->addLayout(img2Layout);
    compareLayout->addWidget(compareButton);
    
    layout->addWidget(compareGroup);
    
    // Single Image Analysis Section
    QGroupBox *singleGroup = new QGroupBox("Single Image Analysis");
    QVBoxLayout *singleLayout = new QVBoxLayout(singleGroup);
    
    QHBoxLayout *analysisImageLayout = new QHBoxLayout();
    QLabel *analysisLabel = new QLabel("Image:");
    analysisLabel->setFixedWidth(80);
    analysisImageInput = new QLineEdit();
    analysisImageInput->setPlaceholderText("Select image to analyze");
    browseAnalysisBtn = new QPushButton("Browse");
    browseAnalysisBtn->setFixedWidth(100);
    analysisImageLayout->addWidget(analysisLabel);
    analysisImageLayout->addWidget(analysisImageInput);
    analysisImageLayout->addWidget(browseAnalysisBtn);
    
    QHBoxLayout *analysisButtonsLayout = new QHBoxLayout();
    histogramButton = new QPushButton("Generate Histogram");
    histogramButton->setFixedHeight(40);
    histogramButton->setCursor(Qt::PointingHandCursor);
    
    detectButton = new QPushButton("Detect Steganography");
    detectButton->setFixedHeight(40);
    detectButton->setCursor(Qt::PointingHandCursor);
    
    analysisButtonsLayout->addWidget(histogramButton);
    analysisButtonsLayout->addWidget(detectButton);
    
    singleLayout->addLayout(analysisImageLayout);
    singleLayout->addLayout(analysisButtonsLayout);
    
    layout->addWidget(singleGroup);
    
    // Status Label
    analysisStatusLabel = new QLabel("");
    analysisStatusLabel->setWordWrap(true);
    analysisStatusLabel->setAlignment(Qt::AlignCenter);
    analysisStatusLabel->setMinimumHeight(30);
    layout->addWidget(analysisStatusLabel);
    
    // Results Display
    QGroupBox *resultsGroup = new QGroupBox("Analysis Results");
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroup);
    analysisResultDisplay = new QTextEdit();
    analysisResultDisplay->setReadOnly(true);
    analysisResultDisplay->setPlaceholderText("Analysis results will be displayed here...");
    analysisResultDisplay->setMinimumHeight(200);
    analysisResultDisplay->setFont(QFont("Consolas", 9));
    resultsLayout->addWidget(analysisResultDisplay);
    
    layout->addWidget(resultsGroup);
    layout->addStretch();
    
    // Connect signals
    connect(browseImage1Btn, &QPushButton::clicked, this, &MainWindow::onBrowseImage1);
    connect(browseImage2Btn, &QPushButton::clicked, this, &MainWindow::onBrowseImage2);
    connect(browseAnalysisBtn, &QPushButton::clicked, this, &MainWindow::onBrowseAnalysisImage);
    connect(compareButton, &QPushButton::clicked, this, &MainWindow::onCompareImages);
    connect(histogramButton, &QPushButton::clicked, this, &MainWindow::onGenerateHistogram);
    connect(detectButton, &QPushButton::clicked, this, &MainWindow::onDetectSteganography);
}

void MainWindow::setupAboutTab()
{
    aboutTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(aboutTab);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(50, 50, 50, 50);
    
    // Logo / Title
    QLabel *logoLabel = new QLabel("HiddenInk");
    QFont logoFont = logoLabel->font();
    logoFont.setPointSize(32);
    logoFont.setBold(true);
    logoLabel->setFont(logoFont);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("color: #2563eb; margin: 20px;");
    
    // Version
    QLabel *versionLabel = new QLabel("Version 1.0.0");
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #64748b; font-size: 14px; margin-bottom: 10px;");
    
    // Description
    QLabel *descLabel = new QLabel(
        "Professional Steganography Tool\n\n"
        "Hide and extract secret data in images using advanced LSB techniques.\n"
        "Supports text, images, and files with optional encryption."
    );
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #475569; font-size: 13px; line-height: 1.6; margin: 20px;");
    
    // Features
    QLabel *featuresLabel = new QLabel(
        "Features:\n\n"
        "• Hide text messages, images, or any file type\n"
        "• Extract hidden data with decryption support\n"
        "• Image quality analysis (MSE, PSNR)\n"
        "• Histogram generation\n"
        "• Steganography detection\n"
        "• Multiple output formats (PNG, BMP)\n"
        "• Configurable bit position (1-8)"
    );
    featuresLabel->setAlignment(Qt::AlignLeft);
    featuresLabel->setWordWrap(true);
    featuresLabel->setStyleSheet(
        "color: #334155; font-size: 12px; "
        "background-color: #f8fafc; "
        "border: 1px solid #e2e8f0; "
        "border-radius: 8px; "
        "padding: 20px; "
        "line-height: 1.8;"
    );
    
    // GitHub button
    QPushButton *githubBtn = new QPushButton("View on GitHub");
    githubBtn->setFixedHeight(45);
    githubBtn->setFixedWidth(200);
    githubBtn->setCursor(Qt::PointingHandCursor);
    githubBtn->setObjectName("secondaryButton");
    
    // Copyright
    QLabel *copyrightLabel = new QLabel("© 2025 HiddenInk Project");
    copyrightLabel->setAlignment(Qt::AlignCenter);
    copyrightLabel->setStyleSheet("color: #94a3b8; font-size: 11px; margin-top: 30px;");
    
    layout->addWidget(logoLabel);
    layout->addWidget(versionLabel);
    layout->addWidget(descLabel);
    layout->addWidget(featuresLabel);
    layout->addSpacing(20);
    layout->addWidget(githubBtn, 0, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(copyrightLabel);
    
    connect(githubBtn, &QPushButton::clicked, this, &MainWindow::onOpenGithub);
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background-color: #ffffff;
        }
        
        QTabWidget::pane {
            border: none;
            background-color: #ffffff;
        }
        
        QTabBar::tab {
            background-color: #f8fafc;
            color: #64748b;
            padding: 12px 24px;
            margin-right: 4px;
            border: none;
            border-bottom: 3px solid transparent;
            font-weight: 600;
            font-size: 13px;
        }
        
        QTabBar::tab:selected {
            background-color: #ffffff;
            color: #2563eb;
            border-bottom: 3px solid #2563eb;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: #f1f5f9;
            color: #475569;
        }
        
        QGroupBox {
            font-weight: 600;
            font-size: 13px;
            color: #334155;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            margin-top: 16px;
            padding-top: 16px;
            background-color: #ffffff;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px;
            background-color: #ffffff;
            color: #1e293b;
        }
        
        QLineEdit {
            padding: 10px 14px;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            background-color: #ffffff;
            font-size: 13px;
            color: #1e293b;
            selection-background-color: #2563eb;
        }
        
        QLineEdit:focus {
            border: 2px solid #2563eb;
            background-color: #f8fafc;
        }
        
        QLineEdit:disabled {
            background-color: #f1f5f9;
            color: #94a3b8;
        }
        
        QTextEdit {
            padding: 10px 14px;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            background-color: #ffffff;
            font-size: 13px;
            color: #1e293b;
            selection-background-color: #2563eb;
        }
        
        QTextEdit:focus {
            border: 2px solid #2563eb;
            background-color: #f8fafc;
        }
        
        QComboBox {
            padding: 10px 14px;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            background-color: #ffffff;
            font-size: 13px;
            color: #1e293b;
        }
        
        QComboBox:focus {
            border: 2px solid #2563eb;
        }
        
        QComboBox::drop-down {
            border: none;
            padding-right: 12px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #64748b;
            margin-right: 8px;
        }
        
        QComboBox QAbstractItemView {
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            background-color: #ffffff;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
            padding: 4px;
        }
        
        QSpinBox {
            padding: 10px 14px;
            border: 2px solid #e2e8f0;
            border-radius: 6px;
            background-color: #ffffff;
            font-size: 13px;
            color: #1e293b;
        }
        
        QSpinBox:focus {
            border: 2px solid #2563eb;
        }
        
        QSpinBox::up-button, QSpinBox::down-button {
            width: 20px;
            border: none;
            background-color: #f1f5f9;
        }
        
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #e2e8f0;
        }
        
        QPushButton {
            background-color: #f1f5f9;
            color: #334155;
            border: 2px solid #e2e8f0;
            padding: 10px 20px;
            border-radius: 6px;
            font-weight: 600;
            font-size: 13px;
        }
        
        QPushButton:hover {
            background-color: #e2e8f0;
            border-color: #cbd5e1;
        }
        
        QPushButton:pressed {
            background-color: #cbd5e1;
        }
        
        QPushButton#primaryButton {
            background-color: #2563eb;
            color: #ffffff;
            border: none;
            font-size: 14px;
        }
        
        QPushButton#primaryButton:hover {
            background-color: #1d4ed8;
        }
        
        QPushButton#primaryButton:pressed {
            background-color: #1e40af;
        }
        
        QPushButton#secondaryButton {
            background-color: #10b981;
            color: #ffffff;
            border: none;
        }
        
        QPushButton#secondaryButton:hover {
            background-color: #059669;
        }
        
        QPushButton:disabled {
            background-color: #f1f5f9;
            color: #cbd5e1;
            border-color: #e2e8f0;
        }
        
        QRadioButton {
            color: #334155;
            font-size: 13px;
            spacing: 8px;
        }
        
        QRadioButton::indicator {
            width: 18px;
            height: 18px;
            border-radius: 9px;
            border: 2px solid #cbd5e1;
            background-color: #ffffff;
        }
        
        QRadioButton::indicator:checked {
            background-color: #2563eb;
            border-color: #2563eb;
        }
        
        QRadioButton::indicator:checked::after {
            content: "";
            width: 8px;
            height: 8px;
            border-radius: 4px;
            background-color: #ffffff;
        }
        
        QCheckBox {
            color: #334155;
            font-size: 13px;
            spacing: 8px;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #cbd5e1;
            background-color: #ffffff;
        }
        
        QCheckBox::indicator:checked {
            background-color: #2563eb;
            border-color: #2563eb;
        }
        
        QLabel {
            color: #475569;
            font-size: 13px;
        }
        
        QScrollArea {
            border: none;
        }
    )";
    
    setStyleSheet(styleSheet);
}

void MainWindow::showSuccess(const QString& message)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("Success");
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::showError(const QString& message)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Error");
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::showInfo(const QString& message)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("Information");
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

// ==================== HIDE TAB SLOTS ====================

void MainWindow::onHideTypeChanged(int id)
{
    // 0 = Text, 1 = Image, 2 = File
    bool isText = (id == 0);
    bool isFile = (id == 1 || id == 2);
    
    secretTextInput->setEnabled(isText);
    secretFileInput->setEnabled(isFile);
    browseSecretFileBtn->setEnabled(isFile);
}

void MainWindow::onBrowseCoverImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select Cover Image", "",
        "Images (*.bmp *.png *.jpg *.jpeg);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        coverImageInput->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseSecretFile()
{
    QString filter;
    int type = hideTypeGroup->checkedId();
    
    if (type == 1) { // Image
        filter = "Images (*.bmp *.png *.jpg *.jpeg);;All Files (*)";
    } else { // Any file
        filter = "All Files (*)";
    }
    
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select File to Hide", "", filter);
    
    if (!fileName.isEmpty()) {
        secretFileInput->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseHideOutput()
{
    QString filter;
    if (hideFormatCombo->currentIndex() == 0) {
        filter = "PNG Image (*.png)";
    } else {
        filter = "BMP Image (*.bmp)";
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Output Image", "", filter);
    
    if (!fileName.isEmpty()) {
        hideOutputInput->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onHideData()
{
    // Validate cover image
    if (coverImageInput->text().isEmpty()) {
        showError("Please select a cover image.");
        return;
    }
    
    if (!QFile::exists(coverImageInput->text())) {
        showError("Cover image file does not exist.");
        return;
    }
    
    // Validate secret data
    int type = hideTypeGroup->checkedId();
    QString secretData;
    QString tempPath;
    
    if (type == 0) { // Text
        secretData = secretTextInput->toPlainText();
        if (secretData.isEmpty()) {
            showError("Please enter a secret message.");
            return;
        }
        
        // Create temporary file for text
        tempPath = QDir::temp().filePath("hiddenink_temp_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".txt");
        QFile tempFile(tempPath);
        if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&tempFile);
            out << secretData;
            tempFile.close();
            secretData = tempPath;
        } else {
            showError("Failed to create temporary file for text.");
            return;
        }
    } else { // Image file
        secretData = secretFileInput->text();
        if (secretData.isEmpty()) {
            showError("Please select a file to hide.");
            return;
        }
        
        if (!QFile::exists(secretData)) {
            showError("Secret file does not exist.");
            return;
        }
    }
    
    // Prepare output path
    QString outputPath = hideOutputInput->text();
    if (outputPath.isEmpty()) {
        QString ext = (hideFormatCombo->currentIndex() == 0) ? ".png" : ".bmp";
        QDir outDir("out");
        if (!outDir.exists()) {
            outDir.mkpath(".");
        }
        outputPath = "out/hidden_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ext;
    }
    
    hideStatusLabel->setText("Processing...");
    hideStatusLabel->setStyleSheet("color: #2563eb; font-weight: bold;");
    QApplication::processEvents();
    
    // Appeler directement les fonctions C++ de stéganographie
    int result;
    std::string coverPath = coverImageInput->text().toStdString();
    std::string secretPath = secretData.toStdString();
    std::string outPath = outputPath.toStdString();
    int bitsPerChannel = hideBitSpin->value();
    
    try {
        if (type == 0) { // Text
            result = hidePNGText(coverPath, secretPath, outPath, bitsPerChannel);
        } else { // Image
            result = hidePNGImage(coverPath, secretPath, outPath, bitsPerChannel);
        }
    } catch (const std::exception& e) {
        hideStatusLabel->setText("✗ Error: Exception occurred");
        hideStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
        showError(QString("Exception: ") + e.what());
        if (!tempPath.isEmpty()) QFile::remove(tempPath);
        return;
    }
    
    // Clean up temporary file
    if (!tempPath.isEmpty()) {
        QFile::remove(tempPath);
    }
    
    if (result == 0) {
        hideStatusLabel->setText("✓ Success: Data hidden successfully!");
        hideStatusLabel->setStyleSheet("color: #16a34a; font-weight: bold;");
        showSuccess("Data has been successfully hidden in the image!\n\nOutput: " + outputPath);
    } else {
        hideStatusLabel->setText("✗ Error: Failed to hide data");
        hideStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
        showError("Failed to hide data. Please check the console for details.");
    }
}

// ==================== EXTRACT TAB SLOTS ====================

void MainWindow::onBrowseExtractImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select Image with Hidden Data", "",
        "Images (*.bmp *.png *.jpg *.jpeg);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        extractImageInput->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseExtractOutput()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Extracted Data", "",
        "All Files (*)");
    
    if (!fileName.isEmpty()) {
        extractOutputInput->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onExtractData()
{
    // Validate input image
    if (extractImageInput->text().isEmpty()) {
        showError("Please select an image to extract data from.");
        return;
    }
    
    if (!QFile::exists(extractImageInput->text())) {
        showError("Input image file does not exist.");
        return;
    }
    
    extractStatusLabel->setText("Extracting...");
    extractStatusLabel->setStyleSheet("color: #2563eb; font-weight: bold;");
    QApplication::processEvents();
    
    std::string inputPath = extractImageInput->text().toStdString();
    int type = extractTypeGroup->checkedId(); // 0 = text, 1 = image
    
    try {
        if (type == 0) { // Extract Text
            // Charger l'image et extraire directement le texte
            int w, h, c;
            unsigned char* img = stbi_load(inputPath.c_str(), &w, &h, &c, 0);
            
            if (!img) {
                extractStatusLabel->setText("✗ Error: Cannot load image");
                extractStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
                extractedDataDisplay->setPlainText("Failed to load the image.");
                showError("Cannot load image file.");
                return;
            }
            
            // Extraire le texte
            std::string extractedText = extractTextFromImage(img, w, h, c);
            stbi_image_free(img);
            
            if (extractedText.empty()) {
                extractStatusLabel->setText("✗ Error: No hidden text found");
                extractStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
                extractedDataDisplay->setPlainText("No hidden text found in this image.");
                showError("This image does not contain any hidden text.");
            } else {
                // Afficher le texte extrait
                extractedDataDisplay->setPlainText(QString::fromStdString(extractedText));
                extractStatusLabel->setText("✓ Success: Text extracted successfully!");
                extractStatusLabel->setStyleSheet("color: #16a34a; font-weight: bold;");
                
                // Sauvegarder si un chemin de sortie est spécifié
                QString outputPath = extractOutputInput->text();
                if (!outputPath.isEmpty()) {
                    QFile outFile(outputPath);
                    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&outFile);
                        out << QString::fromStdString(extractedText);
                        outFile.close();
                        showSuccess("Text extracted successfully!\n\nSaved to: " + outputPath + "\n\nThe text is also displayed below.");
                    } else {
                        showError("Failed to save file, but text is displayed below.");
                    }
                } else {
                    showSuccess("Text extracted successfully!\n\nThe extracted text is displayed below.");
                }
            }
        } else { // Extract Image
            QString outputPath = extractOutputInput->text();
            if (outputPath.isEmpty()) {
                QDir outDir("out");
                if (!outDir.exists()) {
                    outDir.mkpath(".");
                }
                outputPath = "out/extracted_image_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".png";
            }
            
            int result = extractPNGImage(inputPath, outputPath.toStdString());
            
            if (result == 0) {
                extractStatusLabel->setText("✓ Success: Image extracted successfully!");
                extractStatusLabel->setStyleSheet("color: #16a34a; font-weight: bold;");
                extractedDataDisplay->setPlainText("Image extracted successfully!\n\nSaved to: " + outputPath);
                showSuccess("Image extracted successfully!\n\nOutput: " + outputPath);
            } else {
                extractStatusLabel->setText("✗ Error: No hidden image found");
                extractStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
                extractedDataDisplay->setPlainText("No hidden image found in this file.");
                showError("This image does not contain any hidden image.");
            }
        }
    } catch (const std::exception& e) {
        extractStatusLabel->setText("✗ Error: Exception occurred");
        extractStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
        extractedDataDisplay->setPlainText(QString("Exception: ") + e.what());
        showError(QString("Exception: ") + e.what());
    }
}

// ==================== ANALYSIS TAB SLOTS ====================

void MainWindow::onBrowseImage1()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select First Image", "",
        "Images (*.bmp *.png *.jpg *.jpeg);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        image1Input->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseImage2()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select Second Image", "",
        "Images (*.bmp *.png *.jpg *.jpeg);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        image2Input->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseAnalysisImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select Image to Analyze", "",
        "Images (*.bmp *.png *.jpg *.jpeg);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        analysisImageInput->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onCompareImages()
{
    if (image1Input->text().isEmpty() || image2Input->text().isEmpty()) {
        showError("Please select both images to compare.");
        return;
    }
    
    if (!QFile::exists(image1Input->text()) || !QFile::exists(image2Input->text())) {
        showError("One or both image files do not exist.");
        return;
    }
    
    analysisStatusLabel->setText("Comparing images...");
    analysisStatusLabel->setStyleSheet("color: #2563eb; font-weight: bold;");
    analysisResultDisplay->clear();
    QApplication::processEvents();
    
    try {
        std::string img1 = image1Input->text().toStdString();
        std::string img2 = image2Input->text().toStdString();
        
        // Rediriger la sortie de compareImages dans un buffer
        std::ostringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        compareImages(img1, img2);
        
        std::cout.rdbuf(old);
        
        analysisStatusLabel->setText("✓ Analysis complete");
        analysisStatusLabel->setStyleSheet("color: #16a34a; font-weight: bold;");
        
        analysisResultDisplay->setPlainText(QString::fromStdString(buffer.str()));
        
    } catch (const std::exception& e) {
        analysisStatusLabel->setText("✗ Error during analysis");
        analysisStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
        analysisResultDisplay->setPlainText(QString("Error: ") + e.what());
        showError(QString("Analysis failed: ") + e.what());
    }
}

void MainWindow::onGenerateHistogram()
{
    if (analysisImageInput->text().isEmpty()) {
        showError("Please select an image to analyze.");
        return;
    }
    
    if (!QFile::exists(analysisImageInput->text())) {
        showError("Image file does not exist.");
        return;
    }
    
    analysisStatusLabel->setText("Generating histogram...");
    analysisStatusLabel->setStyleSheet("color: #2563eb; font-weight: bold;");
    QApplication::processEvents();
    
    try {
        std::string imgPath = analysisImageInput->text().toStdString();
        
        // Rediriger la sortie
        std::ostringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        generateHistogram(imgPath);
        
        std::cout.rdbuf(old);
        
        analysisStatusLabel->setText("✓ Histogram generated successfully!");
        analysisStatusLabel->setStyleSheet("color: #16a34a; font-weight: bold;");
        analysisResultDisplay->setPlainText(QString::fromStdString(buffer.str()));
        showSuccess("Histogram analysis completed!");
        
    } catch (const std::exception& e) {
        analysisStatusLabel->setText("✗ Error generating histogram");
        analysisStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
        analysisResultDisplay->setPlainText(QString("Error: ") + e.what());
        showError(QString("Failed to generate histogram: ") + e.what());
    }
}

void MainWindow::onDetectSteganography()
{
    if (analysisImageInput->text().isEmpty()) {
        showError("Please select an image to analyze.");
        return;
    }
    
    if (!QFile::exists(analysisImageInput->text())) {
        showError("Image file does not exist.");
        return;
    }
    
    analysisStatusLabel->setText("Analyzing LSB distribution...");
    analysisStatusLabel->setStyleSheet("color: #2563eb; font-weight: bold;");
    analysisResultDisplay->clear();
    QApplication::processEvents();
    
    try {
        std::string imgPath = analysisImageInput->text().toStdString();
        
        // Rediriger la sortie
        std::ostringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        analyzeImageForSteganography(imgPath);
        
        std::cout.rdbuf(old);
        
        analysisStatusLabel->setText("✓ Analysis complete");
        analysisStatusLabel->setStyleSheet("color: #16a34a; font-weight: bold;");
        analysisResultDisplay->setPlainText(QString::fromStdString(buffer.str()));
        
    } catch (const std::exception& e) {
        analysisStatusLabel->setText("✗ Error during analysis");
        analysisStatusLabel->setStyleSheet("color: #dc2626; font-weight: bold;");
        analysisResultDisplay->setPlainText(QString("Error: ") + e.what());
        showError(QString("Analysis failed: ") + e.what());
    }
}

// ==================== ABOUT TAB SLOTS ====================

void MainWindow::onOpenGithub()
{
    QDesktopServices::openUrl(QUrl("https://github.com/LP-Applications-Web-Angers-2025-2026/HiddenInk"));
}
