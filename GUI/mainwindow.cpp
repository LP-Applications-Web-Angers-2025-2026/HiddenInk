#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QImageReader>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QStatusBar>
#include <QScrollBar>
#include <QTabWidget>
#include <QMenuBar>
#include <QSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QApplication>
#include <QTime>
#include <QPainter>
#include <QIcon>
#include <QClipboard>
#include <QScrollArea>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include "../stenographie/utils/external/stb_image.h"
#include "histogramwidget.h"
#include <QImage>
#include <QTemporaryFile>
#include <QRegularExpression>

// Project headers
#include "../stenographie/PNG/png_hide_text.h"
#include "../stenographie/PNG/png_hide_image.h"
#include "../stenographie/PNG/png_extract.h"
#include "../stenographie/BMP/bmp_convert.h"
#include "../stenographie/BMP/bmp_Recuperation.h"
#include "../stenographie/WAV/wav_hide.h"
#include "../stenographie/utils/encrypt/encrypt.h"
#include "../stenographie/utils/histogramme/histogram.h"
#include "../stenographie/utils/analysis/image_analysis.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/logo"));
    initialiserMenus();
    initialiserOnglets();
    appliquerStyle();
    statusBar()->showMessage("Prêt");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initialiserMenus() {
    menuBar()->clear();

    QMenu *menuFichier = menuBar()->addMenu("&Fichier");
    QAction *actOuvrir = menuFichier->addAction("Ouvrir une image...");
    QAction *actQuitter = menuFichier->addAction("Quitter");
    connect(actOuvrir, &QAction::triggered, this, &MainWindow::ouvrirImage);
    connect(actQuitter, &QAction::triggered, this, &MainWindow::quitter);

    QMenu *menuOutils = menuBar()->addMenu("&Outils");
    QAction *actCle = menuOutils->addAction("Générer une clé");
    QAction *actEffacer = menuOutils->addAction("Effacer le journal");
    connect(actCle, &QAction::triggered, this, &MainWindow::genererCle);
    connect(actEffacer, &QAction::triggered, this, &MainWindow::effacerJournal);

    QMenu *menuAide = menuBar()->addMenu("&Aide");
    QAction *actAide = menuAide->addAction("À propos / Aide");
    connect(actAide, &QAction::triggered, this, &MainWindow::afficherAide);
}

void MainWindow::initialiserOnglets() {
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // =========================================================
    // Tabs Injection / Extraction (avec scroll pour éviter le débordement)
    // =========================================================
    auto *scrollInjection = new QScrollArea(tabs);
    scrollInjection->setWidgetResizable(true);
    QWidget *tabInjection = new QWidget();
    scrollInjection->setWidget(tabInjection);
    QVBoxLayout *layoutInjection = new QVBoxLayout(tabInjection);
    layoutInjection->setSpacing(8);

    auto *scrollExtraction = new QScrollArea(tabs);
    scrollExtraction->setWidgetResizable(true);
    QWidget *tabExtraction = new QWidget();
    scrollExtraction->setWidget(tabExtraction);
    QVBoxLayout *layoutExtraction = new QVBoxLayout(tabExtraction);
    layoutExtraction->setSpacing(8);

    // Logo
    QLabel *logoLabel = new QLabel(tabInjection);
    logoLabel->setObjectName("logoLabel");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setMinimumHeight(100);
    QPixmap logoPix(":/icons/logo");
    if (!logoPix.isNull())
        logoLabel->setPixmap(logoPix.scaled(220, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layoutInjection->addWidget(logoLabel);

    // ---- Groupe : Injection texte (PNG) ----
    auto *grpCachageTexte = new QGroupBox("Injection de texte dans une image PNG", tabInjection);
    auto *formTexte = new QFormLayout(grpCachageTexte);
    auto *editImageSourceTexte = new QLineEdit(); editImageSourceTexte->setObjectName("editImageSourceTexte"); editImageSourceTexte->setPlaceholderText("Image porteuse .png");
    auto *btnChoisirImageSourceTexte = new QPushButton("Choisir...");
    auto *editTexteFichier = new QLineEdit(); editTexteFichier->setObjectName("editTexteFichier"); editTexteFichier->setPlaceholderText("Fichier .txt à injecter");
    auto *btnChoisirTexte = new QPushButton("Texte...");
    auto *spinBitsTexte = new QSpinBox(); spinBitsTexte->setRange(1,4); spinBitsTexte->setValue(1); spinBitsTexte->setObjectName("spinBitsTexte"); spinBitsTexte->setToolTip("1 = imperceptible, 4 = capacité max");
    auto *editSortieTexte = new QLineEdit(); editSortieTexte->setObjectName("editSortieTexte"); editSortieTexte->setPlaceholderText("Image de sortie .png");
    auto *btnSortieTexte = new QPushButton("Sauvegarde...");
    auto *btnCacherTexte = new QPushButton("Injecter le texte");
    btnCacherTexte->setObjectName("btnPrimary");

    connect(btnChoisirImageSourceTexte, &QPushButton::clicked, this, [this, editImageSourceTexte, editSortieTexte]() {
        auto f = choisirFichier("Image porteuse (PNG)", "PNG (*.png)");
        if (!f.isEmpty()) {
            editImageSourceTexte->setText(f);
            if (editSortieTexte->text().isEmpty())
                editSortieTexte->setText(suggererCheminSortie(f, "_steg", ".png"));
        }
    });
    connect(btnChoisirTexte, &QPushButton::clicked, this, [this, editTexteFichier]() {
        auto f = choisirFichier("Fichier texte à injecter", "Texte (*.txt)"); if (!f.isEmpty()) editTexteFichier->setText(f);
    });
    connect(btnSortieTexte, &QPushButton::clicked, this, [this, editSortieTexte]() {
        auto f = sauverFichier("Image destination", "PNG (*.png)"); if (!f.isEmpty()) editSortieTexte->setText(f);
    });
    connect(btnCacherTexte, &QPushButton::clicked, this, &MainWindow::cacherTexte);

    auto *rowImgSrcTexte = new QHBoxLayout; rowImgSrcTexte->addWidget(editImageSourceTexte); rowImgSrcTexte->addWidget(btnChoisirImageSourceTexte);
    auto *rowTxtFile = new QHBoxLayout; rowTxtFile->addWidget(editTexteFichier); rowTxtFile->addWidget(btnChoisirTexte);
    auto *rowSortieTexte = new QHBoxLayout; rowSortieTexte->addWidget(editSortieTexte); rowSortieTexte->addWidget(btnSortieTexte);
    formTexte->addRow("Image source:", rowImgSrcTexte);
    formTexte->addRow("Fichier texte à injecter:", rowTxtFile);
    formTexte->addRow("Bits/canal:", spinBitsTexte);
    formTexte->addRow("Image sortie:", rowSortieTexte);
    formTexte->addRow(btnCacherTexte);

    // ---- Groupe : Injection image (PNG) ----
    auto *grpCachageImage = new QGroupBox("Injection d'image dans une image PNG", tabInjection);
    auto *formImage = new QFormLayout(grpCachageImage);
    auto *editImageSourceImage = new QLineEdit(); editImageSourceImage->setObjectName("editImageSourceImage"); editImageSourceImage->setPlaceholderText("Image porteuse .png");
    auto *btnImageSourceImage = new QPushButton("Conteneur...");
    auto *editImageACacher = new QLineEdit(); editImageACacher->setObjectName("editImageACacher"); editImageACacher->setPlaceholderText("Image secrète .png");
    auto *btnImageACacher = new QPushButton("À injecter...");
    auto *spinBitsImage = new QSpinBox(); spinBitsImage->setRange(1,4); spinBitsImage->setValue(1); spinBitsImage->setObjectName("spinBitsImage"); spinBitsImage->setToolTip("1 = imperceptible, 4 = capacité max");
    auto *editSortieImage = new QLineEdit(); editSortieImage->setObjectName("editSortieImage"); editSortieImage->setPlaceholderText("Image de sortie .png");
    auto *btnSortieImage = new QPushButton("Sortie...");
    auto *btnCacherImage = new QPushButton("Injecter l'image");
    btnCacherImage->setObjectName("btnPrimary");

    connect(btnImageSourceImage, &QPushButton::clicked, this, [this, editImageSourceImage, editSortieImage]() {
        auto f = choisirFichier("Image porteuse", "PNG (*.png)");
        if (!f.isEmpty()) {
            editImageSourceImage->setText(f);
            if (editSortieImage->text().isEmpty())
                editSortieImage->setText(suggererCheminSortie(f, "_steg", ".png"));
        }
    });
    connect(btnImageACacher, &QPushButton::clicked, this, [this, editImageACacher]() {
        auto f = choisirFichier("Image à injecter", "PNG (*.png)"); if (!f.isEmpty()) editImageACacher->setText(f);
    });
    connect(btnSortieImage, &QPushButton::clicked, this, [this, editSortieImage]() {
        auto f = sauverFichier("Image sortie", "PNG (*.png)"); if (!f.isEmpty()) editSortieImage->setText(f);
    });
    connect(btnCacherImage, &QPushButton::clicked, this, &MainWindow::cacherImage);

    auto *rowSrcCont = new QHBoxLayout; rowSrcCont->addWidget(editImageSourceImage); rowSrcCont->addWidget(btnImageSourceImage);
    auto *rowImgHide = new QHBoxLayout; rowImgHide->addWidget(editImageACacher); rowImgHide->addWidget(btnImageACacher);
    auto *rowImgOut = new QHBoxLayout; rowImgOut->addWidget(editSortieImage); rowImgOut->addWidget(btnSortieImage);
    formImage->addRow("Conteneur:", rowSrcCont);
    formImage->addRow("Image à injecter:", rowImgHide);
    formImage->addRow("Bits/canal:", spinBitsImage);
    formImage->addRow("Sortie:", rowImgOut);
    formImage->addRow(btnCacherImage);

    // ---- Groupe : Extraction PNG ----
    auto *grpExtraction = new QGroupBox("Extraction PNG", tabExtraction);
    auto *formExt = new QFormLayout(grpExtraction);
    auto *editImageStegTexte = new QLineEdit(); editImageStegTexte->setObjectName("editImageStegTexte"); editImageStegTexte->setPlaceholderText("Image PNG contenant le texte");
    auto *btnImageStegTexte = new QPushButton("Image...");
    auto *editTexteExtrait = new QLineEdit(); editTexteExtrait->setObjectName("editTexteExtrait"); editTexteExtrait->setPlaceholderText("Fichier .txt de sortie");
    auto *btnTexteExtrait = new QPushButton("Sauver...");
    auto *btnExtraireTexte = new QPushButton("Extraire texte");
    btnExtraireTexte->setObjectName("btnPrimary");

    auto *editImageStegImage = new QLineEdit(); editImageStegImage->setObjectName("editImageStegImage"); editImageStegImage->setPlaceholderText("Image PNG contenant l'image");
    auto *btnImageStegImage = new QPushButton("Image...");
    auto *editImageExtraite = new QLineEdit(); editImageExtraite->setObjectName("editImageExtraite"); editImageExtraite->setPlaceholderText("Image .png extraite");
    auto *btnImageExtraite = new QPushButton("Sortie...");
    auto *btnExtraireImage = new QPushButton("Extraire image");
    btnExtraireImage->setObjectName("btnPrimary");

    connect(btnImageStegTexte, &QPushButton::clicked, this, [this, editImageStegTexte, editTexteExtrait]() {
        auto f = choisirFichier("Image (texte caché)", "PNG (*.png)");
        if (!f.isEmpty()) {
            editImageStegTexte->setText(f);
            if (editTexteExtrait->text().isEmpty())
                editTexteExtrait->setText(suggererCheminSortie(f, "_extrait", ".txt"));
        }
    });
    connect(btnTexteExtrait, &QPushButton::clicked, this, [this, editTexteExtrait]() {
        auto f = sauverFichier("Fichier texte extrait", "Texte (*.txt)"); if (!f.isEmpty()) editTexteExtrait->setText(f);
    });
    connect(btnExtraireTexte, &QPushButton::clicked, this, &MainWindow::extraireTexte);
    connect(btnImageStegImage, &QPushButton::clicked, this, [this, editImageStegImage, editImageExtraite]() {
        auto f = choisirFichier("Image (image cachée)", "PNG (*.png)");
        if (!f.isEmpty()) {
            editImageStegImage->setText(f);
            if (editImageExtraite->text().isEmpty())
                editImageExtraite->setText(suggererCheminSortie(f, "_extrait", ".png"));
        }
    });
    connect(btnImageExtraite, &QPushButton::clicked, this, [this, editImageExtraite]() {
        auto f = sauverFichier("Image extraite", "PNG (*.png)"); if (!f.isEmpty()) editImageExtraite->setText(f);
    });
    connect(btnExtraireImage, &QPushButton::clicked, this, &MainWindow::extraireImage);

    auto *rowStegTxt = new QHBoxLayout; rowStegTxt->addWidget(editImageStegTexte); rowStegTxt->addWidget(btnImageStegTexte);
    auto *rowTxtOut = new QHBoxLayout; rowTxtOut->addWidget(editTexteExtrait); rowTxtOut->addWidget(btnTexteExtrait);
    auto *rowStegImg = new QHBoxLayout; rowStegImg->addWidget(editImageStegImage); rowStegImg->addWidget(btnImageStegImage);
    auto *rowImgExtr = new QHBoxLayout; rowImgExtr->addWidget(editImageExtraite); rowImgExtr->addWidget(btnImageExtraite);
    formExt->addRow("Image (texte):", rowStegTxt);
    formExt->addRow("Sortie .txt:", rowTxtOut);
    formExt->addRow(btnExtraireTexte);
    formExt->addRow("Image (image):", rowStegImg);
    formExt->addRow("Sortie image:", rowImgExtr);
    formExt->addRow(btnExtraireImage);

    // ---- Groupe : BMP ----
    // BMP – Injection
    auto *grpBMPCacher = new QGroupBox("Injection BMP", tabInjection);
    auto *formBMPCacher = new QFormLayout(grpBMPCacher);
    auto *editBMPSrc = new QLineEdit(); editBMPSrc->setObjectName("editBMPSrc"); editBMPSrc->setPlaceholderText("Image porteuse .bmp");
    auto *btnBMPSrc = new QPushButton("Choisir...");
    auto *editBMPFichier = new QLineEdit(); editBMPFichier->setObjectName("editBMPFichier"); editBMPFichier->setPlaceholderText("Fichier à injecter (.txt / .bmp / .png)");
    auto *btnBMPFichier = new QPushButton("Fichier...");
    auto *spinBMPBit = new QSpinBox(); spinBMPBit->setRange(0,7); spinBMPBit->setValue(0); spinBMPBit->setObjectName("spinBMPBit"); spinBMPBit->setToolTip("Position du bit (0=LSB, qualité maximale)");
    auto *editBMPCle = new QLineEdit(); editBMPCle->setObjectName("editBMPCle"); editBMPCle->setPlaceholderText("Clé hex (optionnel — laisser vide)");
    auto *editBMPSortie = new QLineEdit(); editBMPSortie->setObjectName("editBMPSortie"); editBMPSortie->setPlaceholderText("Image BMP de sortie");
    auto *btnBMPSortie = new QPushButton("Sortie...");
    auto *btnCacherBMP = new QPushButton("Injecter dans BMP");
    btnCacherBMP->setObjectName("btnPrimary");

    connect(btnBMPSrc, &QPushButton::clicked, this, [this, editBMPSrc, editBMPSortie]() {
        auto f = choisirFichier("Image BMP porteuse", "BMP (*.bmp)");
        if (!f.isEmpty()) {
            editBMPSrc->setText(f);
            if (editBMPSortie->text().isEmpty())
                editBMPSortie->setText(suggererCheminSortie(f, "_steg", ".bmp"));
        }
    });
    connect(btnBMPFichier, &QPushButton::clicked, this, [this, editBMPFichier]() {
        auto f = choisirFichier("Fichier à injecter", "Fichiers supportés (*.txt *.bmp *.png);;Texte (*.txt);;BMP (*.bmp);;PNG (*.png)");
        if (!f.isEmpty()) editBMPFichier->setText(f);
    });
    connect(btnBMPSortie, &QPushButton::clicked, this, [this, editBMPSortie]() {
        auto f = sauverFichier("Image BMP de sortie", "BMP (*.bmp)"); if (!f.isEmpty()) editBMPSortie->setText(f);
    });
    connect(btnCacherBMP, &QPushButton::clicked, this, &MainWindow::cacherBMP);

    auto *rowBMPSrc = new QHBoxLayout; rowBMPSrc->addWidget(editBMPSrc); rowBMPSrc->addWidget(btnBMPSrc);
    auto *rowBMPFichier = new QHBoxLayout; rowBMPFichier->addWidget(editBMPFichier); rowBMPFichier->addWidget(btnBMPFichier);
    auto *rowBMPSortie = new QHBoxLayout; rowBMPSortie->addWidget(editBMPSortie); rowBMPSortie->addWidget(btnBMPSortie);
    formBMPCacher->addRow("Image BMP:", rowBMPSrc);
    formBMPCacher->addRow("Fichier à injecter:", rowBMPFichier);
    formBMPCacher->addRow("Position bit:", spinBMPBit);
    formBMPCacher->addRow("Clé (hex):", editBMPCle);
    formBMPCacher->addRow("Sortie BMP:", rowBMPSortie);
    formBMPCacher->addRow(btnCacherBMP);

    // BMP – Extraction
    auto *grpBMPExtraire = new QGroupBox("Extraction BMP", tabExtraction);
    auto *formBMPExtr = new QFormLayout(grpBMPExtraire);
    auto *editBMPExtrSrc = new QLineEdit(); editBMPExtrSrc->setObjectName("editBMPExtrSrc"); editBMPExtrSrc->setPlaceholderText("Image BMP source");
    auto *btnBMPExtrSrc = new QPushButton("Choisir...");
    auto *spinBMPExtrBit = new QSpinBox(); spinBMPExtrBit->setRange(0,7); spinBMPExtrBit->setValue(0); spinBMPExtrBit->setObjectName("spinBMPExtrBit");
    auto *editBMPExtrCle = new QLineEdit(); editBMPExtrCle->setObjectName("editBMPExtrCle"); editBMPExtrCle->setPlaceholderText("Clé hex (optionnel)");
    auto *editBMPExtrSortie = new QLineEdit(); editBMPExtrSortie->setObjectName("editBMPExtrSortie"); editBMPExtrSortie->setPlaceholderText("Fichier extrait (auto si vide)");
    auto *btnBMPExtrSortie = new QPushButton("Sortie...");
    auto *btnExtraireBMP = new QPushButton("Extraire du BMP");
    btnExtraireBMP->setObjectName("btnPrimary");

    connect(btnBMPExtrSrc, &QPushButton::clicked, this, [this, editBMPExtrSrc, editBMPExtrSortie]() {
        auto f = choisirFichier("Image BMP stéganographiée", "BMP (*.bmp)");
        if (!f.isEmpty()) {
            editBMPExtrSrc->setText(f);
            if (editBMPExtrSortie->text().isEmpty())
                editBMPExtrSortie->setText(suggererCheminSortie(f, "_extrait", ""));
        }
    });
    connect(btnBMPExtrSortie, &QPushButton::clicked, this, [this, editBMPExtrSortie]() {
        auto f = sauverFichier("Fichier extrait", "Tous les fichiers (*);;Texte (*.txt);;BMP (*.bmp);;PNG (*.png)");
        if (!f.isEmpty()) editBMPExtrSortie->setText(f);
    });
    connect(btnExtraireBMP, &QPushButton::clicked, this, &MainWindow::extraireBMP);

    auto *rowBMPExtrSrc = new QHBoxLayout; rowBMPExtrSrc->addWidget(editBMPExtrSrc); rowBMPExtrSrc->addWidget(btnBMPExtrSrc);
    auto *rowBMPExtrSortie = new QHBoxLayout; rowBMPExtrSortie->addWidget(editBMPExtrSortie); rowBMPExtrSortie->addWidget(btnBMPExtrSortie);
    formBMPExtr->addRow("Image BMP:", rowBMPExtrSrc);
    formBMPExtr->addRow("Position bit:", spinBMPExtrBit);
    formBMPExtr->addRow("Clé (hex):", editBMPExtrCle);
    formBMPExtr->addRow("Sortie:", rowBMPExtrSortie);
    formBMPExtr->addRow(btnExtraireBMP);

    // ---- WAV ----
    // WAV – Injection
    auto *grpWAVCacher = new QGroupBox("Injection WAV", tabInjection);
    auto *formWAVCacher = new QFormLayout(grpWAVCacher);
    auto *editWAVSrc = new QLineEdit(); editWAVSrc->setObjectName("editWAVSrc"); editWAVSrc->setPlaceholderText("Audio porteur .wav");
    auto *btnWAVSrc = new QPushButton("Choisir...");
    auto *editWAVFichier = new QLineEdit(); editWAVFichier->setObjectName("editWAVFichier"); editWAVFichier->setPlaceholderText("Fichier à injecter");
    auto *btnWAVFichier = new QPushButton("Fichier...");
    auto *spinWAVBit = new QSpinBox(); spinWAVBit->setRange(0, 7); spinWAVBit->setValue(0); spinWAVBit->setObjectName("spinWAVBit");
    auto *editWAVCle = new QLineEdit(); editWAVCle->setObjectName("editWAVCle"); editWAVCle->setPlaceholderText("Clé hex (optionnel)");
    auto *editWAVSortie = new QLineEdit(); editWAVSortie->setObjectName("editWAVSortie"); editWAVSortie->setPlaceholderText("Audio WAV de sortie");
    auto *btnWAVSortie = new QPushButton("Sortie...");
    auto *btnCacherWAV = new QPushButton("Injecter dans WAV");
    btnCacherWAV->setObjectName("btnPrimary");

    connect(btnWAVSrc, &QPushButton::clicked, this, [this, editWAVSrc, editWAVSortie]() {
        auto f = choisirFichier("Audio WAV porteur", "WAV (*.wav)");
        if (!f.isEmpty()) {
            editWAVSrc->setText(f);
            if (editWAVSortie->text().isEmpty())
                editWAVSortie->setText(suggererCheminSortie(f, "_steg", ".wav"));
        }
    });
    connect(btnWAVFichier, &QPushButton::clicked, this, [this, editWAVFichier]() {
        auto f = choisirFichier("Fichier à injecter", "Tous les fichiers (*)");
        if (!f.isEmpty()) editWAVFichier->setText(f);
    });
    connect(btnWAVSortie, &QPushButton::clicked, this, [this, editWAVSortie]() {
        auto f = sauverFichier("Audio WAV de sortie", "WAV (*.wav)");
        if (!f.isEmpty()) editWAVSortie->setText(f);
    });
    connect(btnCacherWAV, &QPushButton::clicked, this, &MainWindow::cacherWAV);

    auto *rowWAVSrc = new QHBoxLayout; rowWAVSrc->addWidget(editWAVSrc); rowWAVSrc->addWidget(btnWAVSrc);
    auto *rowWAVFichier = new QHBoxLayout; rowWAVFichier->addWidget(editWAVFichier); rowWAVFichier->addWidget(btnWAVFichier);
    auto *rowWAVSortie = new QHBoxLayout; rowWAVSortie->addWidget(editWAVSortie); rowWAVSortie->addWidget(btnWAVSortie);
    formWAVCacher->addRow("Audio WAV:", rowWAVSrc);
    formWAVCacher->addRow("Fichier à injecter:", rowWAVFichier);
    formWAVCacher->addRow("Position bit:", spinWAVBit);
    formWAVCacher->addRow("Clé (hex):", editWAVCle);
    formWAVCacher->addRow("Sortie WAV:", rowWAVSortie);
    formWAVCacher->addRow(btnCacherWAV);

    // WAV – Extraction
    auto *grpWAVExtraire = new QGroupBox("Extraction WAV", tabExtraction);
    auto *formWAVExtr = new QFormLayout(grpWAVExtraire);
    auto *editWAVExtrSrc = new QLineEdit(); editWAVExtrSrc->setObjectName("editWAVExtrSrc"); editWAVExtrSrc->setPlaceholderText("Audio WAV source");
    auto *btnWAVExtrSrc = new QPushButton("Choisir...");
    auto *spinWAVExtrBit = new QSpinBox(); spinWAVExtrBit->setRange(0, 7); spinWAVExtrBit->setValue(0); spinWAVExtrBit->setObjectName("spinWAVExtrBit");
    auto *editWAVExtrCle = new QLineEdit(); editWAVExtrCle->setObjectName("editWAVExtrCle"); editWAVExtrCle->setPlaceholderText("Clé hex (optionnel)");
    auto *editWAVExtrSortie = new QLineEdit(); editWAVExtrSortie->setObjectName("editWAVExtrSortie"); editWAVExtrSortie->setPlaceholderText("Fichier extrait (optionnel)");
    auto *btnWAVExtrSortie = new QPushButton("Sortie...");
    auto *btnExtraireWAV = new QPushButton("Extraire du WAV");
    btnExtraireWAV->setObjectName("btnPrimary");

    connect(btnWAVExtrSrc, &QPushButton::clicked, this, [this, editWAVExtrSrc, editWAVExtrSortie]() {
        auto f = choisirFichier("Audio WAV stéganographié", "WAV (*.wav)");
        if (!f.isEmpty()) {
            editWAVExtrSrc->setText(f);
            if (editWAVExtrSortie->text().isEmpty())
                editWAVExtrSortie->setText(suggererCheminSortie(f, "_extrait", ""));
        }
    });
    connect(btnWAVExtrSortie, &QPushButton::clicked, this, [this, editWAVExtrSortie]() {
        auto f = sauverFichier("Fichier extrait", "Tous les fichiers (*)");
        if (!f.isEmpty()) editWAVExtrSortie->setText(f);
    });
    connect(btnExtraireWAV, &QPushButton::clicked, this, &MainWindow::extraireWAV);

    auto *rowWAVExtrSrc = new QHBoxLayout; rowWAVExtrSrc->addWidget(editWAVExtrSrc); rowWAVExtrSrc->addWidget(btnWAVExtrSrc);
    auto *rowWAVExtrSortie = new QHBoxLayout; rowWAVExtrSortie->addWidget(editWAVExtrSortie); rowWAVExtrSortie->addWidget(btnWAVExtrSortie);
    formWAVExtr->addRow("Audio WAV:", rowWAVExtrSrc);
    formWAVExtr->addRow("Position bit:", spinWAVExtrBit);
    formWAVExtr->addRow("Clé (hex):", editWAVExtrCle);
    formWAVExtr->addRow("Sortie:", rowWAVExtrSortie);
    formWAVExtr->addRow(btnExtraireWAV);

    layoutInjection->addWidget(grpCachageTexte);
    layoutInjection->addWidget(grpCachageImage);
    layoutInjection->addWidget(grpBMPCacher);
    layoutInjection->addWidget(grpWAVCacher);
    layoutInjection->addStretch();

    layoutExtraction->addWidget(grpExtraction);
    layoutExtraction->addWidget(grpBMPExtraire);
    layoutExtraction->addWidget(grpWAVExtraire);
    layoutExtraction->addStretch();

    tabs->addTab(scrollInjection, "Injection");
    tabs->addTab(scrollExtraction, "Extraction");

    // =========================================================
    // Tab Analyse
    // =========================================================
    QWidget *tabAnalyse = new QWidget(tabs);
    QVBoxLayout *layoutAnalyse = new QVBoxLayout(tabAnalyse);

    auto *grpComparaison = new QGroupBox("Comparer deux images (MSE / PSNR)", tabAnalyse);
    auto *formComp = new QFormLayout(grpComparaison);
    auto *editImageA = new QLineEdit(); editImageA->setObjectName("editImageA"); editImageA->setPlaceholderText("Image originale");
    auto *btnImageA = new QPushButton("Image A...");
    auto *editImageB = new QLineEdit(); editImageB->setObjectName("editImageB"); editImageB->setPlaceholderText("Image modifiée");
    auto *btnImageB = new QPushButton("Image B...");
    auto *btnComparer = new QPushButton("Calculer MSE / PSNR");
    connect(btnImageA, &QPushButton::clicked, this, [this, editImageA]() { auto f = choisirFichier("Image A", "Images (*.png *.bmp)"); if (!f.isEmpty()) editImageA->setText(f); });
    connect(btnImageB, &QPushButton::clicked, this, [this, editImageB]() { auto f = choisirFichier("Image B", "Images (*.png *.bmp)"); if (!f.isEmpty()) editImageB->setText(f); });
    connect(btnComparer, &QPushButton::clicked, this, &MainWindow::comparerImages);
    auto *rowA = new QHBoxLayout; rowA->addWidget(editImageA); rowA->addWidget(btnImageA);
    auto *rowB = new QHBoxLayout; rowB->addWidget(editImageB); rowB->addWidget(btnImageB);
    formComp->addRow("Image A:", rowA);
    formComp->addRow("Image B:", rowB);
    formComp->addRow(btnComparer);

    auto *grpDetection = new QGroupBox("Détection stéganographie (LSB)", tabAnalyse);
    auto *formDet = new QFormLayout(grpDetection);
    auto *editImageDetect = new QLineEdit(); editImageDetect->setObjectName("editImageDetect"); editImageDetect->setPlaceholderText("Image à analyser");
    auto *btnImageDetect = new QPushButton("Image...");
    auto *btnDetecter = new QPushButton("Analyser");
    connect(btnImageDetect, &QPushButton::clicked, this, [this, editImageDetect]() { auto f = choisirFichier("Image à analyser", "Images (*.png *.bmp)"); if (!f.isEmpty()) editImageDetect->setText(f); });
    connect(btnDetecter, &QPushButton::clicked, this, &MainWindow::detecterStegano);
    auto *rowDet = new QHBoxLayout; rowDet->addWidget(editImageDetect); rowDet->addWidget(btnImageDetect);
    formDet->addRow("Image:", rowDet);
    formDet->addRow(btnDetecter);

    auto *grpStegaAvancee = new QGroupBox("Stéganalyse avancée", tabAnalyse);
    auto *formAv = new QFormLayout(grpStegaAvancee);
    auto *editImageEntropie = new QLineEdit(); editImageEntropie->setObjectName("editImageEntropie"); editImageEntropie->setPlaceholderText("Image/Fichier pour analyse d'entropie");
    auto *btnImageEntropie = new QPushButton("Choisir...");
    auto *btnAnalyserEntropie = new QPushButton("Analyser entropie");
    auto *editPayloadFile = new QLineEdit(); editPayloadFile->setObjectName("editPayloadFile"); editPayloadFile->setPlaceholderText("Fichier à inspecter (magic bytes)");
    auto *btnPayloadFile = new QPushButton("Choisir...");
    auto *btnDetecterPayload = new QPushButton("Détecter payload");

    connect(btnImageEntropie, &QPushButton::clicked, this, [this, editImageEntropie]() {
        auto f = choisirFichier("Fichier à analyser", "Tous les fichiers (*)");
        if (!f.isEmpty()) editImageEntropie->setText(f);
    });
    connect(btnAnalyserEntropie, &QPushButton::clicked, this, &MainWindow::analyserEntropie);
    connect(btnPayloadFile, &QPushButton::clicked, this, [this, editPayloadFile]() {
        auto f = choisirFichier("Fichier à inspecter", "Tous les fichiers (*)");
        if (!f.isEmpty()) editPayloadFile->setText(f);
    });
    connect(btnDetecterPayload, &QPushButton::clicked, this, &MainWindow::detecterPayload);

    auto *rowEnt = new QHBoxLayout; rowEnt->addWidget(editImageEntropie); rowEnt->addWidget(btnImageEntropie);
    auto *rowPayload = new QHBoxLayout; rowPayload->addWidget(editPayloadFile); rowPayload->addWidget(btnPayloadFile);
    formAv->addRow("Fichier:", rowEnt);
    formAv->addRow(btnAnalyserEntropie);
    formAv->addRow("Payload:", rowPayload);
    formAv->addRow(btnDetecterPayload);

    auto *grpResultats = new QGroupBox("Résultats", tabAnalyse);
    auto *resLayout = new QVBoxLayout(grpResultats);
    auto *analysisOutput = new QPlainTextEdit(); analysisOutput->setObjectName("analysisOutput"); analysisOutput->setReadOnly(true); analysisOutput->setMinimumHeight(100);
    auto *btnEffacerAnalyse = new QPushButton("Effacer");
    connect(btnEffacerAnalyse, &QPushButton::clicked, this, [analysisOutput](){ analysisOutput->clear(); });
    resLayout->addWidget(analysisOutput);
    resLayout->addWidget(btnEffacerAnalyse);

    layoutAnalyse->addWidget(grpComparaison);
    layoutAnalyse->addWidget(grpDetection);
    layoutAnalyse->addWidget(grpStegaAvancee);
    layoutAnalyse->addWidget(grpResultats);
    layoutAnalyse->addStretch();
    tabs->addTab(tabAnalyse, "Analyse");

    // =========================================================
    // Tab Histogramme (page dédiée)
    // =========================================================
    auto *tabHistogramme = new QWidget(tabs);
    auto *layoutHistogramme = new QVBoxLayout(tabHistogramme);

    auto *grpHistogramme = new QGroupBox("Histogramme", tabHistogramme);
    auto *formHist = new QFormLayout(grpHistogramme);
    auto *editImageHist = new QLineEdit(); editImageHist->setObjectName("editImageHist"); editImageHist->setPlaceholderText("Image à analyser");
    auto *btnImageHist = new QPushButton("Image...");
    auto *btnGenererHist = new QPushButton("Générer histogramme");
    connect(btnImageHist, &QPushButton::clicked, this, [this, editImageHist]() { auto f = choisirFichier("Image", "Images (*.png *.bmp)"); if (!f.isEmpty()) editImageHist->setText(f); });
    connect(btnGenererHist, &QPushButton::clicked, this, &MainWindow::genererHistogramme);
    auto *rowHist = new QHBoxLayout; rowHist->addWidget(editImageHist); rowHist->addWidget(btnImageHist);
    formHist->addRow("Image:", rowHist);
    formHist->addRow(btnGenererHist);

    auto *histCanvas = new HistogramWidget(); histCanvas->setObjectName("histogramWidget"); histCanvas->setMinimumHeight(280);

    layoutHistogramme->addWidget(grpHistogramme);
    layoutHistogramme->addWidget(histCanvas);
    layoutHistogramme->addStretch();
    tabs->addTab(tabHistogramme, "Histogramme");

    // =========================================================
    // Tab Outils
    // =========================================================
    auto *tabOutils = new QWidget(tabs);
    auto *layoutOutils = new QVBoxLayout(tabOutils);

    // Génération de clé
    auto *grpCle = new QGroupBox("Clé de chiffrement XOR", tabOutils);
    auto *formCle = new QFormLayout(grpCle);
    auto *spinKeyLen = new QSpinBox(); spinKeyLen->setRange(8, 64); spinKeyLen->setValue(32); spinKeyLen->setSuffix(" octets"); spinKeyLen->setObjectName("spinKeyLen");
    auto *editCleHex = new QLineEdit(); editCleHex->setObjectName("editCleHex"); editCleHex->setReadOnly(true); editCleHex->setPlaceholderText("Clé hex apparaîtra ici après génération");
    editCleHex->setFont(QFont("Courier New", 9));
    auto *btnGenCle = new QPushButton("Générer une clé");
    auto *btnCopierCle = new QPushButton("Copier");
    btnCopierCle->setToolTip("Copier la clé dans le presse-papier");
    connect(btnGenCle, &QPushButton::clicked, this, &MainWindow::genererCle);
    connect(btnCopierCle, &QPushButton::clicked, this, &MainWindow::copierCle);
    auto *rowCleBtn = new QHBoxLayout; rowCleBtn->addWidget(btnGenCle); rowCleBtn->addWidget(btnCopierCle);
    formCle->addRow("Longueur:", spinKeyLen);
    formCle->addRow("Clé (hex):", editCleHex);
    formCle->addRow(rowCleBtn);
    auto *noteLabel = new QLabel("<i>Copiez la clé dans le champ \"Clé (hex)\" de la section BMP pour chiffrer/déchiffrer.</i>");
    noteLabel->setWordWrap(true);
    noteLabel->setStyleSheet("color: #aaaaaa; font-size: 9pt;");
    formCle->addRow(noteLabel);

    layoutOutils->addWidget(grpCle);
    layoutOutils->addStretch();
    tabs->addTab(tabOutils, "Outils");

    // =========================================================
    // Tab Journal
    // =========================================================
    auto *tabJournal = new QWidget(tabs);
    auto *layoutJournal = new QVBoxLayout(tabJournal);
    auto *logEdit = new QPlainTextEdit(); logEdit->setObjectName("logEdit"); logEdit->setReadOnly(true);
    logEdit->setFont(QFont("Courier New", 9));
    auto *btnEffacerLog = new QPushButton("Effacer le journal");
    connect(btnEffacerLog, &QPushButton::clicked, this, &MainWindow::effacerJournal);
    layoutJournal->addWidget(logEdit);
    layoutJournal->addWidget(btnEffacerLog);
    tabs->addTab(tabJournal, "Journal");
}

void MainWindow::appliquerStyle() {
    QString style = R"(
        QMainWindow { background-color: #1e1e1e; }
        QWidget { color: #e0e0e0; font-family: 'Segoe UI'; font-size: 10pt; background-color: #1e1e1e; }
        QScrollArea { border: none; }
        QGroupBox { border: 1px solid #444; border-radius: 4px; margin-top: 12px; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; color: #c0c0c0; }
        QPushButton { background: #2d2d30; border: 1px solid #3e3e42; padding: 6px 14px; border-radius: 4px; color: #e0e0e0; }
        QPushButton:hover { background: #3a3a3d; border-color: #007acc; }
        QPushButton:pressed { background: #005f9e; color: white; }
        QPushButton#btnPrimary { background: #0e639c; border-color: #007acc; font-weight: bold; }
        QPushButton#btnPrimary:hover { background: #1177bb; }
        QPushButton#btnPrimary:pressed { background: #005f9e; }
        QLineEdit, QSpinBox { background: #252526; border: 1px solid #3e3e42; padding: 4px; border-radius: 3px; color: #e0e0e0; }
        QLineEdit:focus, QSpinBox:focus { border-color: #007acc; }
        QLineEdit[readOnly="true"] { color: #aaaaaa; }
        QTabWidget::pane { border: 1px solid #3e3e42; background: #1e1e1e; }
        QTabBar::tab { background: #2d2d30; padding: 7px 14px; color: #c0c0c0; border-top-left-radius: 4px; border-top-right-radius: 4px; }
        QTabBar::tab:selected { background: #007acc; color: white; }
        QTabBar::tab:hover:!selected { background: #3a3a3d; }
        QPlainTextEdit { background: #1a1a1a; border: 1px solid #3e3e42; color: #d4d4d4; }
        QLabel { background: transparent; }
        QScrollBar:vertical { background: #252526; width: 10px; }
        QScrollBar::handle:vertical { background: #444; border-radius: 5px; min-height: 20px; }
    )";
    setStyleSheet(style);
}

void MainWindow::logMessage(const QString &msg) {
    auto logEdit = findChild<QPlainTextEdit*>("logEdit");
    if (!logEdit) return;
    logEdit->appendPlainText(QTime::currentTime().toString("[HH:mm:ss] ") + msg);
    auto sb = logEdit->verticalScrollBar();
    if (sb) sb->setValue(sb->maximum());
    statusBar()->showMessage(msg, 4000);
}

QString MainWindow::choisirFichier(const QString &titre, const QString &filtre) {
    return QFileDialog::getOpenFileName(this, titre, QString(), filtre);
}

QString MainWindow::sauverFichier(const QString &titre, const QString &filtre) {
    return QFileDialog::getSaveFileName(this, titre, QString(), filtre);
}

bool MainWindow::confirmer(const QString &texte, const QString &titre) {
    return QMessageBox::question(this, titre, texte, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

QString MainWindow::suggererCheminSortie(const QString &sourcePath, const QString &suffixe, const QString &ext) {
    QFileInfo fi(sourcePath);
    QString dir = fi.absolutePath();
    QString base = fi.completeBaseName();
    QString finalExt = ext.isEmpty() ? ("." + fi.suffix()) : ext;
    return dir + "/" + base + suffixe + finalExt;
}

// --- Fichier ---
void MainWindow::ouvrirImage() {
    auto f = choisirFichier("Ouvrir une image", "Images (*.png *.bmp *.jpg *.jpeg)");
    if (f.isEmpty()) return;
    derniereImageChargee = f;
    logMessage("Image ouverte : " + QFileInfo(f).fileName());
}

void MainWindow::quitter() { close(); }

// --- Cachage Texte (PNG) ---
void MainWindow::cacherTexte() {
    auto src = findChild<QLineEdit*>("editImageSourceTexte");
    auto txtFile = findChild<QLineEdit*>("editTexteFichier");
    auto bitsSpin = findChild<QSpinBox*>("spinBitsTexte");
    auto sortie = findChild<QLineEdit*>("editSortieTexte");
    if (!src || !txtFile || !bitsSpin || !sortie) return;
    if (src->text().isEmpty() || txtFile->text().isEmpty() || sortie->text().isEmpty()) {
        logMessage("Erreur : champs manquants pour le cachage texte.");
        return;
    }
    QFile f(txtFile->text());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) { logMessage("Erreur : impossible de lire le fichier texte."); return; }
    QByteArray data = f.readAll(); f.close();
    try {
        int result = hidePNGText(src->text().toStdString(), data.toStdString(), sortie->text().toStdString(), bitsSpin->value());
        if (result == 0) {
            logMessage("Succès : texte caché dans " + QFileInfo(sortie->text()).fileName());
            statusBar()->showMessage("Texte caché avec succès.", 5000);
        } else {
            logMessage("Erreur : échec du cachage texte.");
        }
    } catch (const std::exception &e) { logMessage(QString("Exception cachage texte : ") + e.what()); }
}

// --- Cachage Image (PNG) ---
void MainWindow::cacherImage() {
    auto cont = findChild<QLineEdit*>("editImageSourceImage");
    auto secret = findChild<QLineEdit*>("editImageACacher");
    auto bitsSpin = findChild<QSpinBox*>("spinBitsImage");
    auto sortie = findChild<QLineEdit*>("editSortieImage");
    if (!cont || !secret || !bitsSpin || !sortie) return;
    if (cont->text().isEmpty() || secret->text().isEmpty() || sortie->text().isEmpty()) {
        logMessage("Erreur : champs manquants pour le cachage image.");
        return;
    }
    try {
        QString info;
        QString secretUtilisee = preparerImageSecreteRedimensionnee(secret->text(), cont->text(), bitsSpin->value(), info);
        if (secretUtilisee.isEmpty()) { logMessage("Erreur : préparation de l'image secrète échouée. " + info); return; }
        int result = hidePNGImage(cont->text().toStdString(), secretUtilisee.toStdString(), sortie->text().toStdString(), bitsSpin->value());
        if (result == 0) {
            logMessage("Succès : image cachée dans " + QFileInfo(sortie->text()).fileName() + ". " + info);
        } else {
            logMessage("Erreur : échec cachage image." + (info.isEmpty() ? "" : " " + info));
        }
    } catch (const std::exception &e) { logMessage(QString("Exception cachage image : ") + e.what()); }
}

// --- Extraction Texte (PNG) ---
void MainWindow::extraireTexte() {
    auto img = findChild<QLineEdit*>("editImageStegTexte");
    auto out = findChild<QLineEdit*>("editTexteExtrait");
    if (!img || !out) return;
    if (img->text().isEmpty() || out->text().isEmpty()) { logMessage("Erreur : champs manquants extraction texte."); return; }
    try {
        int result = extractPNGText(img->text().toStdString(), out->text().toStdString());
        if (result == 0) {
            logMessage("Succès : texte extrait dans " + QFileInfo(out->text()).fileName());
            // Afficher le contenu si le fichier n'est pas trop grand
            QFile f(out->text());
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray content = f.read(2000);
                f.close();
                auto analysisOut = findChild<QPlainTextEdit*>("analysisOutput");
                if (analysisOut)
                    analysisOut->appendPlainText("[Texte extrait]\n" + QString::fromUtf8(content) + (content.size() == 2000 ? "\n[...tronqué]" : "") + "\n");
            }
        } else {
            logMessage("Erreur : extraction texte échouée (aucun texte trouvé ou format invalide).");
        }
    } catch (const std::exception &e) { logMessage(QString("Exception extraction texte : ") + e.what()); }
}

// --- Extraction Image (PNG) ---
void MainWindow::extraireImage() {
    auto img = findChild<QLineEdit*>("editImageStegImage");
    auto out = findChild<QLineEdit*>("editImageExtraite");
    if (!img || !out) return;
    if (img->text().isEmpty() || out->text().isEmpty()) { logMessage("Erreur : champs manquants extraction image."); return; }
    try {
        int result = extractPNGImage(img->text().toStdString(), out->text().toStdString());
        if (result == 0) {
            logMessage("Succès : image extraite dans " + QFileInfo(out->text()).fileName());
            afficherResultatPayload(out->text());
        } else {
            logMessage("Erreur : extraction image échouée (aucune image trouvée ou format invalide).");
        }
    } catch (const std::exception &e) { logMessage(QString("Exception extraction image : ") + e.what()); }
}

// --- Cachage BMP ---
void MainWindow::cacherBMP() {
    auto src = findChild<QLineEdit*>("editBMPSrc");
    auto fichier = findChild<QLineEdit*>("editBMPFichier");
    auto spin = findChild<QSpinBox*>("spinBMPBit");
    auto cle = findChild<QLineEdit*>("editBMPCle");
    auto sortie = findChild<QLineEdit*>("editBMPSortie");
    if (!src || !fichier || !spin || !cle || !sortie) return;
    if (src->text().isEmpty() || fichier->text().isEmpty() || sortie->text().isEmpty()) {
        logMessage("Erreur : champs manquants pour le cachage BMP.");
        return;
    }
    try {
        bmpConvert(src->text().toStdString(), fichier->text().toStdString(),
                   sortie->text().toStdString(), spin->value(), cle->text().toStdString());
        logMessage("Succès : fichier caché dans " + QFileInfo(sortie->text()).fileName());
        if (!cle->text().isEmpty())
            logMessage("Clé utilisée : " + cle->text() + " (conservez-la pour l'extraction !)");
    } catch (const std::exception &e) { logMessage(QString("Exception cachage BMP : ") + e.what()); }
}

// --- Extraction BMP ---
void MainWindow::extraireBMP() {
    auto src = findChild<QLineEdit*>("editBMPExtrSrc");
    auto spin = findChild<QSpinBox*>("spinBMPExtrBit");
    auto cle = findChild<QLineEdit*>("editBMPExtrCle");
    auto sortie = findChild<QLineEdit*>("editBMPExtrSortie");
    if (!src || !spin || !cle || !sortie) return;
    if (src->text().isEmpty()) { logMessage("Erreur : image BMP source manquante."); return; }
    try {
        std::string result = bmpRecup(src->text().toStdString(), spin->value(),
                                      cle->text().toStdString(), sortie->text().toStdString());
        logMessage(QString::fromStdString(result));
        QString pathDetect = sortie->text();
        if (pathDetect.isEmpty()) {
            const QString resultQt = QString::fromStdString(result);
            QRegularExpression re("Extrait dans\\s+(.+)$");
            auto m = re.match(resultQt);
            if (m.hasMatch()) pathDetect = m.captured(1).trimmed();
        }
        if (!pathDetect.isEmpty()) afficherResultatPayload(pathDetect);
        // Si c'était un texte et qu'on a un chemin de sortie, montrer le contenu
        if (!sortie->text().isEmpty() && sortie->text().endsWith(".txt", Qt::CaseInsensitive)) {
            QFile f(sortie->text());
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray content = f.read(2000);
                f.close();
                auto analysisOut = findChild<QPlainTextEdit*>("analysisOutput");
                if (analysisOut)
                    analysisOut->appendPlainText("[Texte extrait BMP]\n" + QString::fromUtf8(content) + "\n");
            }
        }
    } catch (const std::exception &e) { logMessage(QString("Exception extraction BMP : ") + e.what()); }
}

// --- Cachage WAV ---
void MainWindow::cacherWAV() {
    auto src = findChild<QLineEdit*>("editWAVSrc");
    auto fichier = findChild<QLineEdit*>("editWAVFichier");
    auto spin = findChild<QSpinBox*>("spinWAVBit");
    auto cle = findChild<QLineEdit*>("editWAVCle");
    auto sortie = findChild<QLineEdit*>("editWAVSortie");
    if (!src || !fichier || !spin || !cle || !sortie) return;
    if (src->text().isEmpty() || fichier->text().isEmpty() || sortie->text().isEmpty()) {
        logMessage("Erreur : champs manquants pour le cachage WAV.");
        return;
    }
    try {
        bool ok = hideInWAV(src->text().toStdString(), fichier->text().toStdString(),
                            sortie->text().toStdString(), spin->value(), cle->text().toStdString());
        if (ok) {
            logMessage("Succès : fichier caché dans " + QFileInfo(sortie->text()).fileName());
            if (!cle->text().isEmpty())
                logMessage("Clé utilisée : " + cle->text() + " (conservez-la pour l'extraction).");
        } else {
            logMessage("Erreur : échec du cachage WAV.");
        }
    } catch (const std::exception &e) {
        logMessage(QString("Exception cachage WAV : ") + e.what());
    }
}

// --- Extraction WAV ---
void MainWindow::extraireWAV() {
    auto src = findChild<QLineEdit*>("editWAVExtrSrc");
    auto spin = findChild<QSpinBox*>("spinWAVExtrBit");
    auto cle = findChild<QLineEdit*>("editWAVExtrCle");
    auto sortie = findChild<QLineEdit*>("editWAVExtrSortie");
    if (!src || !spin || !cle || !sortie) return;
    if (src->text().isEmpty()) {
        logMessage("Erreur : audio WAV source manquant.");
        return;
    }

    try {
        std::string result = extractFromWAV(src->text().toStdString(), spin->value(),
                                            cle->text().toStdString(), sortie->text().toStdString());
        QString resultQt = QString::fromStdString(result);
        if (resultQt == "1") {
            logMessage("Erreur WAV : fichier invalide ou inaccessible.");
            return;
        }
        if (resultQt == "2") {
            logMessage("Erreur WAV : signature HiddenInk absente.");
            return;
        }
        if (resultQt == "3") {
            logMessage("Erreur WAV : balises de contenu introuvables.");
            return;
        }
        if (resultQt == "4") {
            logMessage("Erreur WAV : impossible d'écrire le fichier extrait.");
            return;
        }

        logMessage(resultQt);
        QRegularExpression re("Extrait dans\\s+(.+)$");
        auto m = re.match(resultQt);
        if (m.hasMatch()) {
            QString outPath = m.captured(1).trimmed();
            afficherResultatPayload(outPath);
            if (outPath.endsWith(".txt", Qt::CaseInsensitive)) {
                QFile f(outPath);
                if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QByteArray content = f.read(2000);
                    f.close();
                    auto analysisOut = findChild<QPlainTextEdit*>("analysisOutput");
                    if (analysisOut)
                        analysisOut->appendPlainText("[Texte extrait WAV]\n" + QString::fromUtf8(content) + "\n");
                }
            }
        }
    } catch (const std::exception &e) {
        logMessage(QString("Exception extraction WAV : ") + e.what());
    }
}

// --- Analyse Comparaison ---
void MainWindow::comparerImages() {
    auto imgA = findChild<QLineEdit*>("editImageA");
    auto imgB = findChild<QLineEdit*>("editImageB");
    if (!imgA || !imgB) return;
    if (imgA->text().isEmpty() || imgB->text().isEmpty()) { logMessage("Erreur : images manquantes pour la comparaison."); return; }
    try {
        int w1,h1,c1,w2,h2,c2;
        unsigned char *data1 = stbi_load(imgA->text().toStdString().c_str(), &w1,&h1,&c1,0);
        unsigned char *data2 = stbi_load(imgB->text().toStdString().c_str(), &w2,&h2,&c2,0);
        if (!data1 || !data2) {
            logMessage("Erreur : chargement des images échoué.");
            if (data1) stbi_image_free(data1);
            if (data2) stbi_image_free(data2);
            return;
        }
        if (w1 != w2 || h1 != h2 || c1 != c2) {
            logMessage(QString("Erreur : dimensions différentes (%1x%2x%3 vs %4x%5x%6).")
                       .arg(w1).arg(h1).arg(c1).arg(w2).arg(h2).arg(c2));
            stbi_image_free(data1); stbi_image_free(data2); return;
        }
        size_t total = size_t(w1)*size_t(h1)*size_t(c1);
        double mse = 0; size_t changed = 0;
        for (size_t i = 0; i < total; i++) {
            int diff = int(data1[i]) - int(data2[i]);
            if (data1[i] != data2[i]) changed++;
            mse += diff * diff;
        }
        mse /= total;
        double psnr = (mse == 0 ? std::numeric_limits<double>::infinity() : 10.0 * std::log10(255.0*255.0/mse));
        stbi_image_free(data1); stbi_image_free(data2);
        QString psnrStr = std::isinf(psnr) ? "∞ (images identiques)" : QString::number(psnr,'f',2) + " dB";
        QString qualite;
        if (std::isinf(psnr)) qualite = "Images identiques";
        else if (psnr > 50) qualite = "Imperceptible";
        else if (psnr > 40) qualite = "Excellente qualité";
        else if (psnr > 30) qualite = "Bonne qualité";
        else qualite = "Dégradation visible";
        auto out = findChild<QPlainTextEdit*>("analysisOutput");
        if (out) {
            out->appendPlainText(QString("[Comparaison] %1 vs %2\nMSE      : %3\nPSNR     : %4\nQualité  : %5\nPixels modifiés : %6 / %7 (%8%)\n")
                .arg(QFileInfo(imgA->text()).fileName())
                .arg(QFileInfo(imgB->text()).fileName())
                .arg(mse, 0, 'f', 5)
                .arg(psnrStr)
                .arg(qualite)
                .arg(changed)
                .arg(total)
                .arg(changed * 100.0 / total, 0, 'f', 2));
        }
        logMessage("Comparaison terminée. PSNR : " + psnrStr);
    } catch (const std::exception &e) { logMessage(QString("Exception comparaison : ") + e.what()); }
}

// --- Analyse Histogramme ---
void MainWindow::genererHistogramme() {
    auto img = findChild<QLineEdit*>("editImageHist");
    if (!img || img->text().isEmpty()) { logMessage("Erreur : image manquante pour l'histogramme."); return; }
    try {
        int w, h, c;
        unsigned char *data = stbi_load(img->text().toStdString().c_str(), &w, &h, &c, 0);
        if (!data) { logMessage("Erreur : chargement image échoué."); return; }
        std::vector<int> hist(256, 0);
        size_t npix = size_t(w) * size_t(h);
        for (size_t i = 0; i < npix; i++) hist[data[i * size_t(c)]]++;
        stbi_image_free(data);
        auto canvas = findChild<HistogramWidget*>("histogramWidget");
        if (canvas) canvas->setHist(hist);
        int maxVal = *std::max_element(hist.begin(), hist.end());
        auto out = findChild<QPlainTextEdit*>("analysisOutput");
        if (out) {
            out->appendPlainText(QString("[Histogramme] %1\nPixels : %2  Canaux : %3  Pic (fréq max) : %4\n")
                .arg(QFileInfo(img->text()).fileName())
                .arg(qint64(w) * h)
                .arg(c)
                .arg(maxVal));
        }
        logMessage("Histogramme généré pour " + QFileInfo(img->text()).fileName());
    } catch (const std::exception &e) { logMessage(QString("Exception histogramme : ") + e.what()); }
}

// --- Analyse Détection ---
void MainWindow::detecterStegano() {
    auto img = findChild<QLineEdit*>("editImageDetect");
    if (!img || img->text().isEmpty()) { logMessage("Erreur : image manquante pour la détection."); return; }
    try {
        int w, h, c;
        unsigned char *data = stbi_load(img->text().toStdString().c_str(), &w, &h, &c, 0);
        if (!data) { logMessage("Erreur : chargement image échoué."); return; }
        size_t total = size_t(w) * size_t(h) * size_t(c);
        size_t zeros = 0, ones = 0;
        for (size_t i = 0; i < total; i++) {
            if ((data[i] & 1) == 0) zeros++; else ones++;
        }
        stbi_image_free(data);
        double ratio = ones * 1.0 / (zeros + ones);
        QString interpretation;
        if (std::abs(ratio - 0.5) < 0.01)
            interpretation = "Distribution ~50/50 : équilibrée (typique d'un LSB modifié ou naturel)";
        else if (std::abs(ratio - 0.5) < 0.05)
            interpretation = "Légère asymétrie : stéganographie possible";
        else
            interpretation = "Asymétrie forte : suspect, mais non concluant";
        auto out = findChild<QPlainTextEdit*>("analysisOutput");
        if (out) {
            out->appendPlainText(QString("[Détection LSB] %1\nLSB=0 : %2 (%3%)\nLSB=1 : %4 (%5%)\nRatio 1 : %6\n%7\n")
                .arg(QFileInfo(img->text()).fileName())
                .arg(zeros).arg(zeros * 100.0 / total, 0, 'f', 2)
                .arg(ones).arg(ones * 100.0 / total, 0, 'f', 2)
                .arg(ratio, 0, 'f', 4)
                .arg(interpretation));
        }
        logMessage("Analyse LSB terminée : " + interpretation);
    } catch (const std::exception &e) { logMessage(QString("Exception détection : ") + e.what()); }
}

void MainWindow::analyserEntropie() {
    auto edit = findChild<QLineEdit*>("editImageEntropie");
    if (!edit || edit->text().isEmpty()) {
        logMessage("Erreur : fichier manquant pour l'analyse d'entropie.");
        return;
    }

    QFile f(edit->text());
    if (!f.open(QIODevice::ReadOnly)) {
        logMessage("Erreur : impossible de lire le fichier pour l'entropie.");
        return;
    }
    QByteArray data = f.readAll();
    f.close();
    if (data.isEmpty()) {
        logMessage("Erreur : fichier vide.");
        return;
    }

    std::array<qint64, 256> hist{};
    for (unsigned char c : data) hist[c]++;
    const double n = static_cast<double>(data.size());
    double entropy = 0.0;
    for (qint64 count : hist) {
        if (count == 0) continue;
        const double p = count / n;
        entropy -= p * std::log2(p);
    }

    QString interpretation;
    if (entropy < 5.5) interpretation = "Faible entropie (texte/structure marquée).";
    else if (entropy < 7.2) interpretation = "Entropie moyenne (fichier compressé possible).";
    else interpretation = "Entropie élevée (données chiffrées/compressées probables).";

    auto out = findChild<QPlainTextEdit*>("analysisOutput");
    if (out) {
        out->appendPlainText(QString("[Entropie]\nFichier : %1\nTaille  : %2 octets\nShannon : %3 bits/octet\n%4\n")
            .arg(QFileInfo(edit->text()).fileName())
            .arg(data.size())
            .arg(entropy, 0, 'f', 4)
            .arg(interpretation));
    }
    logMessage(QString("Entropie analysée : %1 bits/octet").arg(entropy, 0, 'f', 4));
}

void MainWindow::detecterPayload() {
    auto edit = findChild<QLineEdit*>("editPayloadFile");
    if (!edit || edit->text().isEmpty()) {
        logMessage("Erreur : fichier payload manquant.");
        return;
    }
    afficherResultatPayload(edit->text());
}

void MainWindow::afficherResultatPayload(const QString &filePath) {
    QFile f(filePath);
    if (!f.exists()) {
        logMessage("Payload : fichier introuvable -> " + filePath);
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        logMessage("Payload : impossible d'ouvrir " + filePath);
        return;
    }

    QByteArray head = f.read(32);
    QByteArray all = f.readAll();
    all.prepend(head);
    f.close();

    auto startsWith = [&head](const QByteArray &sig) {
        return head.size() >= sig.size() && head.left(sig.size()) == sig;
    };

    QString type = "Binaire inconnu";
    QString details = "Aucune signature connue détectée.";
    if (startsWith("\x89PNG\r\n\x1a\n")) {
        type = "Image PNG";
        details = "Signature PNG valide.";
    } else if (startsWith("BM")) {
        type = "Image BMP";
        details = "Header BMP détecté.";
    } else if (startsWith("RIFF") && head.mid(8, 4) == "WAVE") {
        type = "Audio WAV";
        details = "Header RIFF/WAVE détecté.";
    } else if (startsWith("PK\x03\x04")) {
        type = "Archive ZIP";
        details = "Header ZIP détecté.";
    } else if (startsWith("%PDF")) {
        type = "Document PDF";
        details = "Header PDF détecté.";
    } else if (startsWith("\x7F" "ELF")) {
        type = "Exécutable ELF";
        details = "Header ELF détecté.";
    } else if (startsWith("MZ")) {
        type = "Exécutable PE";
        details = "Header MZ détecté.";
    }

    int printable = 0;
    const int sample = std::min<int>(all.size(), 2048);
    for (int i = 0; i < sample; ++i) {
        unsigned char c = static_cast<unsigned char>(all[i]);
        if ((c >= 32 && c < 127) || c == '\n' || c == '\r' || c == '\t') printable++;
    }
    double printableRatio = sample > 0 ? (printable * 100.0 / sample) : 0.0;
    if (type == "Binaire inconnu" && printableRatio > 85.0) {
        type = "Texte probable";
        details = "Contenu majoritairement imprimable.";
    }

    QString headHex = head.left(16).toHex(' ').toUpper();
    auto out = findChild<QPlainTextEdit*>("analysisOutput");
    if (out) {
        out->appendPlainText(QString("[Payload]\nFichier : %1\nType    : %2\nTaille  : %3 octets\nASCII   : %4%\nHeader  : %5\n%6\n")
            .arg(QFileInfo(filePath).fileName())
            .arg(type)
            .arg(QFileInfo(filePath).size())
            .arg(printableRatio, 0, 'f', 2)
            .arg(headHex)
            .arg(details));
    }
    logMessage(QString("Payload détecté : %1 (%2)").arg(type, QFileInfo(filePath).fileName()));
}

// --- Helpers redimensionnement image secrète ---
int MainWindow::calculerCapaciteMax(const QSize &carrierSize, int bits) {
    const int overhead = 1024;
    qint64 rawBits = qint64(carrierSize.width()) * carrierSize.height() * 3 * bits;
    qint64 bytes = rawBits / 8 - overhead;
    return int(qMax(qint64(0), bytes));
}

int MainWindow::tailleImagePixels(const QSize &secretSize) {
    return secretSize.width() * secretSize.height() * 3;
}

QString MainWindow::preparerImageSecreteRedimensionnee(const QString &secretPath, const QString &carrierPath, int bits, QString &info) {
    info.clear();
    QImage carrier(carrierPath);
    QImage secret(secretPath);
    if (carrier.isNull() || secret.isNull()) { info = "(Chargement images échoué)"; return {}; }
    int capaciteOctets = calculerCapaciteMax(carrier.size(), bits);
    int secretOctets = tailleImagePixels(secret.size());
    if (secretOctets <= capaciteOctets) {
        info = QString("[OK] %1 octets <= capacité %2 octets").arg(secretOctets).arg(capaciteOctets);
        return secretPath;
    }
    double ratio = double(capaciteOctets) / double(secretOctets);
    if (ratio <= 0) { info = "(Capacité nulle)"; return {}; }
    double scale = std::sqrt(ratio);
    int newW = std::max(1, int(secret.width() * scale));
    int newH = std::max(1, int(secret.height() * scale));
    QImage resized = secret.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int resizedOctets = tailleImagePixels(resized.size());
    if (resizedOctets > capaciteOctets) {
        double adjust = std::sqrt(double(capaciteOctets) / double(resizedOctets));
        newW = std::max(1, int(resized.width() * adjust));
        newH = std::max(1, int(resized.height() * adjust));
        resized = secret.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        resizedOctets = tailleImagePixels(resized.size());
    }
    QString tempPath = QDir::temp().filePath(
        QString("hiddenink_resized_%1x%2.png").arg(resized.width()).arg(resized.height()));
    if (!resized.save(tempPath, "PNG")) { info = "(Sauvegarde image redimensionnée échouée)"; return {}; }
    info = QString("[Redimensionnée] %1x%2 → %3x%4 (%5 octets)")
        .arg(secret.width()).arg(secret.height())
        .arg(resized.width()).arg(resized.height())
        .arg(resizedOctets);
    return tempPath;
}

// --- Génération / copie clé ---
void MainWindow::genererCle() {
    auto spin = findChild<QSpinBox*>("spinKeyLen");
    int len = spin ? spin->value() : 32;
    std::string cle = generate_key(len);
    cleCouranteHex = QString::fromStdString(to_hex(cle));
    auto editCle = findChild<QLineEdit*>("editCleHex");
    if (editCle) editCle->setText(cleCouranteHex);
    logMessage(QString("Clé générée (%1 octets) : %2").arg(len).arg(cleCouranteHex));
}

void MainWindow::copierCle() {
    if (cleCouranteHex.isEmpty()) { logMessage("Aucune clé à copier. Générez d'abord une clé."); return; }
    QApplication::clipboard()->setText(cleCouranteHex);
    logMessage("Clé copiée dans le presse-papier.");
    statusBar()->showMessage("Clé copiée !", 3000);
}

void MainWindow::effacerJournal() {
    auto logEdit = findChild<QPlainTextEdit*>("logEdit");
    if (logEdit) logEdit->clear();
}

void MainWindow::afficherAide() {
    QMessageBox::information(this, "Aide – HiddenInk",
        "HiddenInk GUI\n\n"
        "Stéganographie :\n"
        "  • Cacher/extraire un texte dans un PNG (LSB)\n"
        "  • Cacher/extraire une image dans un PNG (LSB)\n"
        "  • Cacher/extraire un fichier dans un BMP (bit spécifique)\n\n"
        "Analyse :\n"
        "  • Comparaison MSE/PSNR entre deux images\n"
        "  • Histogramme du canal rouge\n"
        "  • Détection LSB (analyse des bits de poids faible)\n\n"
        "Outils :\n"
        "  • Génération de clé XOR (pour BMP)\n"
        "  • Copiez la clé dans le champ 'Clé (hex)' de la section BMP\n\n"
        "Formats supportés : PNG (stéganographie), BMP (stéganographie + histogramme)\n\n"
        "Note : le chiffrement XOR est une obfuscation basique, pas du chiffrement fort."
    );
}
