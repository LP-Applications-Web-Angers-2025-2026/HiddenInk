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
#include <vector>
#include <algorithm>
#include "../stenographie/utils/external/stb_image.h"
#include "histogramwidget.h"
#include <QImage>
#include <QTemporaryFile>

// Project headers
#include "../stenographie/PNG/png_hide_text.h"
#include "../stenographie/PNG/png_hide_image.h"
#include "../stenographie/PNG/png_extract.h"
#include "../stenographie/BMP/bmp_convert.h"
#include "../stenographie/BMP/bmp_Recuperation.h"
#include "../stenographie/utils/encrypt/encrypt.h"
#include "../stenographie/utils/histogramme/histogram.h"
#include "../stenographie/utils/analysis/image_analysis.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this); // We will replace central widget dynamically
    setWindowIcon(QIcon(":/icons/logo"));
    initialiserMenus();
    initialiserOnglets();
    appliquerStyle();
    statusBar()->showMessage("Prêt");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initialiserMenus() {
    // Clear existing menu bar actions if any
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
    // Create main tab widget
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    // --- Tab Stéganographie ---
    QWidget *tabSteg = new QWidget(tabs);
    QVBoxLayout *layoutSteg = new QVBoxLayout(tabSteg);

    // Logo
    QLabel *logoLabel = new QLabel(tabSteg); logoLabel->setObjectName("logoLabel");
    logoLabel->setAlignment(Qt::AlignCenter); logoLabel->setMinimumHeight(120);
    QPixmap logoPix(":/icons/logo");
    if(!logoPix.isNull()) logoLabel->setPixmap(logoPix.scaled(260,120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layoutSteg->addWidget(logoLabel);

    // Group: Cachage Texte
    QGroupBox *grpCachageTexte = new QGroupBox("Cacher un texte", tabSteg);
    QFormLayout *formTexte = new QFormLayout(grpCachageTexte);
    auto *editImageSourceTexte = new QLineEdit(grpCachageTexte); editImageSourceTexte->setObjectName("editImageSourceTexte");
    auto *btnChoisirImageSourceTexte = new QPushButton("Choisir...");
    connect(btnChoisirImageSourceTexte, &QPushButton::clicked, this, [this, editImageSourceTexte]() {
        auto f = choisirFichier("Image source (PNG)", "PNG (*.png)"); if(!f.isEmpty()) editImageSourceTexte->setText(f);
    });
    auto *editTexteFichier = new QLineEdit(grpCachageTexte); editTexteFichier->setObjectName("editTexteFichier");
    auto *btnChoisirTexte = new QPushButton("Texte...");
    connect(btnChoisirTexte, &QPushButton::clicked, this, [this, editTexteFichier]() {
        auto f = choisirFichier("Fichier texte", "Texte (*.txt)"); if(!f.isEmpty()) editTexteFichier->setText(f);
    });
    auto *spinBitsTexte = new QSpinBox(grpCachageTexte); spinBitsTexte->setRange(1,4); spinBitsTexte->setValue(1); spinBitsTexte->setObjectName("spinBitsTexte");
    auto *editSortieTexte = new QLineEdit(grpCachageTexte); editSortieTexte->setObjectName("editSortieTexte");
    auto *btnSortieTexte = new QPushButton("Sauvegarde...");
    connect(btnSortieTexte, &QPushButton::clicked, this, [this, editSortieTexte]() {
        auto f = sauverFichier("Image destination", "PNG (*.png)"); if(!f.isEmpty()) editSortieTexte->setText(f);
    });
    auto *btnCacherTexte = new QPushButton("Cacher le texte");
    connect(btnCacherTexte, &QPushButton::clicked, this, &MainWindow::cacherTexte);

    QHBoxLayout *rowImgSrcTexte = new QHBoxLayout; rowImgSrcTexte->addWidget(editImageSourceTexte); rowImgSrcTexte->addWidget(btnChoisirImageSourceTexte);
    QHBoxLayout *rowTxtFile = new QHBoxLayout; rowTxtFile->addWidget(editTexteFichier); rowTxtFile->addWidget(btnChoisirTexte);
    QHBoxLayout *rowSortieTexte = new QHBoxLayout; rowSortieTexte->addWidget(editSortieTexte); rowSortieTexte->addWidget(btnSortieTexte);
    formTexte->addRow("Image source:", rowImgSrcTexte);
    formTexte->addRow("Fichier texte:", rowTxtFile);
    formTexte->addRow("Bits/canal:", spinBitsTexte);
    formTexte->addRow("Image sortie:", rowSortieTexte);
    formTexte->addRow(btnCacherTexte);

    // Group: Cachage Image
    QGroupBox *grpCachageImage = new QGroupBox("Cacher une image", tabSteg);
    QFormLayout *formImage = new QFormLayout(grpCachageImage);
    auto *editImageSourceImage = new QLineEdit(grpCachageImage); editImageSourceImage->setObjectName("editImageSourceImage");
    auto *btnImageSourceImage = new QPushButton("Conteneur...");
    connect(btnImageSourceImage, &QPushButton::clicked, this, [this, editImageSourceImage]() {
        auto f = choisirFichier("Image conteneur", "PNG (*.png)"); if(!f.isEmpty()) editImageSourceImage->setText(f);
    });
    auto *editImageACacher = new QLineEdit(grpCachageImage); editImageACacher->setObjectName("editImageACacher");
    auto *btnImageACacher = new QPushButton("À cacher...");
    connect(btnImageACacher, &QPushButton::clicked, this, [this, editImageACacher]() {
        auto f = choisirFichier("Image à cacher", "PNG (*.png)"); if(!f.isEmpty()) editImageACacher->setText(f);
    });
    auto *spinBitsImage = new QSpinBox(grpCachageImage); spinBitsImage->setRange(1,4); spinBitsImage->setValue(1); spinBitsImage->setObjectName("spinBitsImage");
    auto *editSortieImage = new QLineEdit(grpCachageImage); editSortieImage->setObjectName("editSortieImage");
    auto *btnSortieImage = new QPushButton("Sortie...");
    connect(btnSortieImage, &QPushButton::clicked, this, [this, editSortieImage]() {
        auto f = sauverFichier("Image sortie", "PNG (*.png)"); if(!f.isEmpty()) editSortieImage->setText(f);
    });
    auto *btnCacherImage = new QPushButton("Cacher l'image");
    connect(btnCacherImage, &QPushButton::clicked, this, &MainWindow::cacherImage);

    QHBoxLayout *rowSrcCont = new QHBoxLayout; rowSrcCont->addWidget(editImageSourceImage); rowSrcCont->addWidget(btnImageSourceImage);
    QHBoxLayout *rowImgHide = new QHBoxLayout; rowImgHide->addWidget(editImageACacher); rowImgHide->addWidget(btnImageACacher);
    QHBoxLayout *rowImgOut = new QHBoxLayout; rowImgOut->addWidget(editSortieImage); rowImgOut->addWidget(btnSortieImage);
    formImage->addRow("Conteneur:", rowSrcCont);
    formImage->addRow("Image secrète:", rowImgHide);
    formImage->addRow("Bits/canal:", spinBitsImage);
    formImage->addRow("Sortie:", rowImgOut);
    formImage->addRow(btnCacherImage);

    // Group: Extraction
    QGroupBox *grpExtraction = new QGroupBox("Extraction", tabSteg);
    QFormLayout *formExt = new QFormLayout(grpExtraction);
    auto *editImageStegTexte = new QLineEdit(grpExtraction); editImageStegTexte->setObjectName("editImageStegTexte");
    auto *btnImageStegTexte = new QPushButton("Image texte...");
    connect(btnImageStegTexte, &QPushButton::clicked, this, [this, editImageStegTexte]() {
        auto f = choisirFichier("Image stéganographiée (texte)", "PNG (*.png)"); if(!f.isEmpty()) editImageStegTexte->setText(f);
    });
    auto *editTexteExtrait = new QLineEdit(grpExtraction); editTexteExtrait->setObjectName("editTexteExtrait");
    auto *btnTexteExtrait = new QPushButton("Sauver texte...");
    connect(btnTexteExtrait, &QPushButton::clicked, this, [this, editTexteExtrait]() {
        auto f = sauverFichier("Fichier texte extrait", "Texte (*.txt)"); if(!f.isEmpty()) editTexteExtrait->setText(f);
    });
    auto *btnExtraireTexte = new QPushButton("Extraire texte");
    connect(btnExtraireTexte, &QPushButton::clicked, this, &MainWindow::extraireTexte);

    auto *editImageStegImage = new QLineEdit(grpExtraction); editImageStegImage->setObjectName("editImageStegImage");
    auto *btnImageStegImage = new QPushButton("Image cachée...");
    connect(btnImageStegImage, &QPushButton::clicked, this, [this, editImageStegImage]() {
        auto f = choisirFichier("Image stéganographiée (image)", "PNG (*.png)"); if(!f.isEmpty()) editImageStegImage->setText(f);
    });
    auto *editImageExtraite = new QLineEdit(grpExtraction); editImageExtraite->setObjectName("editImageExtraite");
    auto *btnImageExtraite = new QPushButton("Image extraite...");
    connect(btnImageExtraite, &QPushButton::clicked, this, [this, editImageExtraite]() {
        auto f = sauverFichier("Image extraite", "PNG (*.png)"); if(!f.isEmpty()) editImageExtraite->setText(f);
    });
    auto *btnExtraireImage = new QPushButton("Extraire image");
    connect(btnExtraireImage, &QPushButton::clicked, this, &MainWindow::extraireImage);

    QHBoxLayout *rowStegTxt = new QHBoxLayout; rowStegTxt->addWidget(editImageStegTexte); rowStegTxt->addWidget(btnImageStegTexte);
    QHBoxLayout *rowTxtOut = new QHBoxLayout; rowTxtOut->addWidget(editTexteExtrait); rowTxtOut->addWidget(btnTexteExtrait);
    QHBoxLayout *rowStegImg = new QHBoxLayout; rowStegImg->addWidget(editImageStegImage); rowStegImg->addWidget(btnImageStegImage);
    QHBoxLayout *rowImgExtr = new QHBoxLayout; rowImgExtr->addWidget(editImageExtraite); rowImgExtr->addWidget(btnImageExtraite);
    formExt->addRow("Image (texte):", rowStegTxt);
    formExt->addRow("Fichier texte:", rowTxtOut);
    formExt->addRow(btnExtraireTexte);
    formExt->addRow("Image (image):", rowStegImg);
    formExt->addRow("Image sortie:", rowImgExtr);
    formExt->addRow(btnExtraireImage);

    layoutSteg->addWidget(grpCachageTexte);
    layoutSteg->addWidget(grpCachageImage);
    layoutSteg->addWidget(grpExtraction);
    layoutSteg->addStretch();
    tabs->addTab(tabSteg, "Stéganographie");

    // --- Tab Analyse ---
    QWidget *tabAnalyse = new QWidget(tabs);
    QVBoxLayout *layoutAnalyse = new QVBoxLayout(tabAnalyse);
    QGroupBox *grpComparaison = new QGroupBox("Comparer deux images", tabAnalyse);
    QFormLayout *formComp = new QFormLayout(grpComparaison);
    auto *editImageA = new QLineEdit(grpComparaison); editImageA->setObjectName("editImageA");
    auto *btnImageA = new QPushButton("Image A...");
    connect(btnImageA, &QPushButton::clicked, this, [this, editImageA]() { auto f = choisirFichier("Image A", "PNG (*.png);;BMP (*.bmp)"); if(!f.isEmpty()) editImageA->setText(f); });
    auto *editImageB = new QLineEdit(grpComparaison); editImageB->setObjectName("editImageB");
    auto *btnImageB = new QPushButton("Image B...");
    connect(btnImageB, &QPushButton::clicked, this, [this, editImageB]() { auto f = choisirFichier("Image B", "PNG (*.png);;BMP (*.bmp)"); if(!f.isEmpty()) editImageB->setText(f); });
    auto *btnComparer = new QPushButton("Calculer MSE / PSNR");
    connect(btnComparer, &QPushButton::clicked, this, &MainWindow::comparerImages);
    QHBoxLayout *rowA = new QHBoxLayout; rowA->addWidget(editImageA); rowA->addWidget(btnImageA);
    QHBoxLayout *rowB = new QHBoxLayout; rowB->addWidget(editImageB); rowB->addWidget(btnImageB);
    formComp->addRow("Image A:", rowA);
    formComp->addRow("Image B:", rowB);
    formComp->addRow(btnComparer);

    QGroupBox *grpHistogramme = new QGroupBox("Histogramme", tabAnalyse);
    QFormLayout *formHist = new QFormLayout(grpHistogramme);
    auto *editImageHist = new QLineEdit(grpHistogramme); editImageHist->setObjectName("editImageHist");
    auto *btnImageHist = new QPushButton("Image...");
    connect(btnImageHist, &QPushButton::clicked, this, [this, editImageHist]() { auto f = choisirFichier("Image histogramme", "PNG (*.png);;BMP (*.bmp)"); if(!f.isEmpty()) editImageHist->setText(f); });
    auto *btnGenererHist = new QPushButton("Générer histogramme");
    connect(btnGenererHist, &QPushButton::clicked, this, &MainWindow::genererHistogramme);
    QHBoxLayout *rowHist = new QHBoxLayout; rowHist->addWidget(editImageHist); rowHist->addWidget(btnImageHist);
    formHist->addRow("Image:", rowHist);
    formHist->addRow(btnGenererHist);

    QGroupBox *grpDetection = new QGroupBox("Détection stéganographie", tabAnalyse);
    QFormLayout *formDet = new QFormLayout(grpDetection);
    auto *editImageDetect = new QLineEdit(grpDetection); editImageDetect->setObjectName("editImageDetect");
    auto *btnImageDetect = new QPushButton("Image...");
    connect(btnImageDetect, &QPushButton::clicked, this, [this, editImageDetect]() { auto f = choisirFichier("Image à analyser", "PNG (*.png);;BMP (*.bmp)"); if(!f.isEmpty()) editImageDetect->setText(f); });
    auto *btnDetecter = new QPushButton("Analyser");
    connect(btnDetecter, &QPushButton::clicked, this, &MainWindow::detecterStegano);
    QHBoxLayout *rowDet = new QHBoxLayout; rowDet->addWidget(editImageDetect); rowDet->addWidget(btnImageDetect);
    formDet->addRow("Image:", rowDet);
    formDet->addRow(btnDetecter);

    layoutAnalyse->addWidget(grpComparaison);
    layoutAnalyse->addWidget(grpHistogramme);
    layoutAnalyse->addWidget(grpDetection);
    // Zone résultats + histogramme
    QGroupBox *grpResultats = new QGroupBox("Résultats", tabAnalyse);
    QVBoxLayout *resLayout = new QVBoxLayout(grpResultats);
    auto *analysisOutput = new QPlainTextEdit(grpResultats); analysisOutput->setObjectName("analysisOutput"); analysisOutput->setReadOnly(true);
    auto *histCanvas = new HistogramWidget(grpResultats); histCanvas->setObjectName("histogramWidget"); histCanvas->setMinimumHeight(180);
    resLayout->addWidget(analysisOutput);
    resLayout->addWidget(histCanvas);
    layoutAnalyse->addWidget(grpResultats);
    layoutAnalyse->addStretch();
    tabs->addTab(tabAnalyse, "Analyse");

    // --- Tab Journal ---
    QWidget *tabJournal = new QWidget(tabs);
    QVBoxLayout *layoutJournal = new QVBoxLayout(tabJournal);
    auto *logEdit = new QPlainTextEdit(tabJournal); logEdit->setObjectName("logEdit"); logEdit->setReadOnly(true);
    layoutJournal->addWidget(logEdit);
    tabs->addTab(tabJournal, "Journal");
}

void MainWindow::appliquerStyle() {
    QString style = R"(QMainWindow { background-color: #1e1e1e; }
        QWidget { color: #e0e0e0; font-family: 'Segoe UI'; font-size: 10pt; }
        QGroupBox { border: 1px solid #444; border-radius:4px; margin-top:12px; }
        QGroupBox::title { subcontrol-origin: margin; left:10px; padding:0 4px; }
        QPushButton { background:#2d2d30; border:1px solid #3e3e42; padding:6px 14px; border-radius:4px; }
        QPushButton:hover { background:#3a3a3d; }
        QPushButton:pressed { background:#007acc; color:white; }
        QLineEdit, QSpinBox { background:#252526; border:1px solid #3e3e42; padding:4px; }
        QTabWidget::pane { border:1px solid #3e3e42; }
        QTabBar::tab { background:#2d2d30; padding:6px 12px; }
        QTabBar::tab:selected { background:#007acc; }
        QPlainTextEdit { background:#252526; border:1px solid #3e3e42; }
    )";
    setStyleSheet(style);
}

void MainWindow::logMessage(const QString &msg) {
    auto logEdit = findChild<QPlainTextEdit*>("logEdit");
    if(!logEdit) return;
    logEdit->appendPlainText(QTime::currentTime().toString("HH:mm:ss ") + msg);
}

QString MainWindow::choisirFichier(const QString &titre, const QString &filtre) {
    return QFileDialog::getOpenFileName(this, titre, QString(), filtre);
}

QString MainWindow::sauverFichier(const QString &titre, const QString &filtre) {
    return QFileDialog::getSaveFileName(this, titre, QString(), filtre);
}

bool MainWindow::confirmer(const QString &texte, const QString &titre) {
    return QMessageBox::question(this, titre, texte, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes;
}

// Slots Fichier
void MainWindow::ouvrirImage() {
    auto f = choisirFichier("Ouvrir une image", "Images (*.png *.bmp *.jpg *.jpeg)");
    if(f.isEmpty()) return;
    derniereImageChargee = f;
    logMessage("Image ouverte: " + QFileInfo(f).fileName());
}

void MainWindow::quitter() { close(); }

// Cachage Texte
void MainWindow::cacherTexte() {
    auto src = findChild<QLineEdit*>("editImageSourceTexte");
    auto txtFile = findChild<QLineEdit*>("editTexteFichier");
    auto bitsSpin = findChild<QSpinBox*>("spinBitsTexte");
    auto sortie = findChild<QLineEdit*>("editSortieTexte");
    if(!src || !txtFile || !bitsSpin || !sortie) return;
    if(src->text().isEmpty() || txtFile->text().isEmpty() || sortie->text().isEmpty()) { logMessage("Champs manquants pour cachage texte."); return; }
    QFile f(txtFile->text());
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) { logMessage("Impossible de lire le fichier texte."); return; }
    QByteArray data = f.readAll(); f.close();
    try {
        // Signature: hidePNGText(carrierPath, textOrFilePath, outputPath, bits)
        hidePNGText(src->text().toStdString(), data.toStdString(), sortie->text().toStdString(), bitsSpin->value());
        logMessage("Texte caché avec succès.");
    } catch(const std::exception &e) { logMessage(QString("Erreur cachage texte: ")+e.what()); }
}

// Cachage Image
void MainWindow::cacherImage() {
    auto cont = findChild<QLineEdit*>("editImageSourceImage");
    auto secret = findChild<QLineEdit*>("editImageACacher");
    auto bitsSpin = findChild<QSpinBox*>("spinBitsImage");
    auto sortie = findChild<QLineEdit*>("editSortieImage");
    if(!cont || !secret || !bitsSpin || !sortie) return;
    if(cont->text().isEmpty() || secret->text().isEmpty() || sortie->text().isEmpty()) { logMessage("Champs manquants pour cachage image."); return; }
    try {
        QString info;
        QString secretUtilisee = preparerImageSecreteRedimensionnee(secret->text(), cont->text(), bitsSpin->value(), info);
        if(secretUtilisee.isEmpty()) { logMessage("Préparation de l'image secrète échouée."); return; }
        int result = hidePNGImage(cont->text().toStdString(), secretUtilisee.toStdString(), sortie->text().toStdString(), bitsSpin->value());
        if(result==0) {
            logMessage("Image cachée avec succès. " + info);
        } else {
            logMessage("Échec cachage image (voir console)." + (info.isEmpty()?"":" "+info));
        }
    } catch(const std::exception &e) { logMessage(QString("Erreur cachage image: ")+e.what()); }
}

// Extraction Texte
void MainWindow::extraireTexte() {
    auto img = findChild<QLineEdit*>("editImageStegTexte");
    auto out = findChild<QLineEdit*>("editTexteExtrait");
    if(!img || !out) return;
    if(img->text().isEmpty() || out->text().isEmpty()) { logMessage("Champs manquants extraction texte."); return; }
    try {
        int result = extractPNGText(img->text().toStdString(), out->text().toStdString());
        if(result == 0) {
            logMessage("Texte extrait (voir fichier de sortie).");
        } else {
            logMessage("Erreur extraction texte (voir console). ");
        }
    } catch(const std::exception &e) { logMessage(QString("Erreur extraction texte: ")+e.what()); }
}

// Extraction Image
void MainWindow::extraireImage() {
    auto img = findChild<QLineEdit*>("editImageStegImage");
    auto out = findChild<QLineEdit*>("editImageExtraite");
    if(!img || !out) return;
    if(img->text().isEmpty() || out->text().isEmpty()) { logMessage("Champs manquants extraction image."); return; }
    try {
        extractPNGImage(img->text().toStdString(), out->text().toStdString());
        logMessage("Image extraite.");
    } catch(const std::exception &e) { logMessage(QString("Erreur extraction image: ")+e.what()); }
}

// Analyse Comparaison
void MainWindow::comparerImages() {
    auto imgA = findChild<QLineEdit*>("editImageA");
    auto imgB = findChild<QLineEdit*>("editImageB");
    if(!imgA || !imgB) return; if(imgA->text().isEmpty() || imgB->text().isEmpty()) { logMessage("Images manquantes comparaison."); return; }
    try {
        int w1,h1,c1,w2,h2,c2;
        unsigned char *data1 = stbi_load(imgA->text().toStdString().c_str(), &w1,&h1,&c1,0);
        unsigned char *data2 = stbi_load(imgB->text().toStdString().c_str(), &w2,&h2,&c2,0);
        if(!data1 || !data2) { logMessage("Chargement images échoué."); if(data1) stbi_image_free(data1); if(data2) stbi_image_free(data2); return; }
        if(w1!=w2 || h1!=h2 || c1!=c2) { logMessage("Dimensions différentes."); stbi_image_free(data1); stbi_image_free(data2); return; }
        size_t total = size_t(w1)*h1*c1; double mse=0; size_t changed=0;
        for(size_t i=0;i<total;i++){ int diff=int(data1[i])-int(data2[i]); if(data1[i]!=data2[i]) changed++; mse+=diff*diff; }
        mse/=total; double psnr = (mse==0? std::numeric_limits<double>::infinity() : 10.0*log10((255.0*255.0)/mse));
        stbi_image_free(data1); stbi_image_free(data2);
        auto out = findChild<QPlainTextEdit*>("analysisOutput");
        if(out){ out->appendPlainText(QString("[Comparaison]\nMSE: %1\nPSNR: %2 dB\nPixels modifiés: %3 (%4%)\n")
                                      .arg(mse,0,'f',5)
                                      .arg(psnr==INFINITY?0:psnr,0,'f',2)
                                      .arg(changed)
                                      .arg(changed*100.0/total,0,'f',2)); }
        logMessage("Comparaison OK.");
    } catch(const std::exception &e) { logMessage(QString("Erreur comparaison: ")+e.what()); }
}

// Analyse Histogramme
void MainWindow::genererHistogramme() {
    auto img = findChild<QLineEdit*>("editImageHist");
    if(!img) return; if(img->text().isEmpty()) { logMessage("Image manquante histogramme."); return; }
    try {
        int w,h,c; unsigned char *data = stbi_load(img->text().toStdString().c_str(), &w,&h,&c,0);
        if(!data){ logMessage("Échec chargement image."); return; }
        std::vector<int> hist(256,0);
        // Utilise premier canal pour simplicité
        for(int i=0;i<w*h;i++){ hist[data[i*c]]++; }
        stbi_image_free(data);
        auto canvas = findChild<HistogramWidget*>("histogramWidget"); if(canvas) canvas->setHist(hist);
        auto out = findChild<QPlainTextEdit*>("analysisOutput");
        if(out){ int maxVal=*std::max_element(hist.begin(),hist.end()); out->appendPlainText(QString("[Histogramme]\nImage: %1\nPixels: %2\nCanaux: %3\nPic (fréq max): %4\n")
            .arg(QFileInfo(img->text()).fileName())
            .arg(w*h)
            .arg(c)
            .arg(maxVal)); }
        logMessage("Histogramme généré.");
    } catch(const std::exception &e) { logMessage(QString("Erreur histogramme: ")+e.what()); }
}

// Analyse Détection
void MainWindow::detecterStegano() {
    auto img = findChild<QLineEdit*>("editImageDetect");
    if(!img) return; if(img->text().isEmpty()) { logMessage("Image manquante détection."); return; }
    try {
        int w,h,c; unsigned char *data = stbi_load(img->text().toStdString().c_str(), &w,&h,&c,0);
        if(!data){ logMessage("Chargement image échec."); return; }
        size_t total = size_t(w)*h*c; size_t zeros=0, ones=0;
        for(size_t i=0;i<total;i++){ if((data[i]&1)==0) zeros++; else ones++; }
        stbi_image_free(data);
        double ratio = ones*1.0/(zeros+ones);
        QString interpretation;
        if (std::abs(ratio-0.5) < 0.01) interpretation = "Distribution ~50/50 : équilibrée (potentiellement cachage ou naturel)";
        else if (std::abs(ratio-0.5) < 0.05) interpretation = "Légère asymétrie : possible stéganographie";
        else interpretation = "Asymétrie forte : suspect mais pas concluant";
        auto out = findChild<QPlainTextEdit*>("analysisOutput");
        if(out){ out->appendPlainText(QString("[Détection]\nLSB 0: %1 (%2%)\nLSB 1: %3 (%4%)\nRatio 1: %5\n%6\n")
             .arg(zeros)
             .arg(zeros*100.0/total,0,'f',2)
             .arg(ones)
             .arg(ones*100.0/total,0,'f',2)
             .arg(ratio,0,'f',4)
             .arg(interpretation)); }
        logMessage("Analyse LSB terminée.");
    } catch(const std::exception &e) { logMessage(QString("Erreur détection: ")+e.what()); }
}

// --- Helpers de redimensionnement et capacité ---
int MainWindow::calculerCapaciteMax(const QSize &carrierSize, int bits) {
    // Approximation: capacité en octets = largeur * hauteur * 3 canaux * bits / 8 - overhead
    const int overhead = 1024; // signature + métadonnées
    qint64 rawBits = qint64(carrierSize.width()) * carrierSize.height() * 3 * bits;
    qint64 bytes = rawBits/8 - overhead;
    if(bytes < 0) bytes = 0;
    return int(bytes);
}

int MainWindow::tailleImagePixels(const QSize &secretSize) {
    return secretSize.width() * secretSize.height() * 3; // 3 canaux supposés (RGB)
}

QString MainWindow::preparerImageSecreteRedimensionnee(const QString &secretPath, const QString &carrierPath, int bits, QString &info) {
    info.clear();
    QImage carrier(carrierPath);
    QImage secret(secretPath);
    if(carrier.isNull() || secret.isNull()) {
        info = "(Chargement images échoué)";
        return QString();
    }
    int capaciteOctets = calculerCapaciteMax(carrier.size(), bits);
    int secretOctets = tailleImagePixels(secret.size());
    if(secretOctets <= capaciteOctets) {
        info = QString("[OK capacité] %1 octets <= %2 octets").arg(secretOctets).arg(capaciteOctets);
        return secretPath; // pas besoin redimensionner
    }
    // Calcul facteur de réduction
    double ratio = double(capaciteOctets) / double(secretOctets);
    if(ratio <= 0) { info = "(Capacité nulle)"; return QString(); }
    double scale = std::sqrt(ratio); // uniform scaling on width & height
    int newW = std::max(1, int(secret.width() * scale));
    int newH = std::max(1, int(secret.height() * scale));
    QImage resized = secret.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // Vérifier à nouveau la capacité (arrondis peuvent grossir un peu)
    int resizedOctets = tailleImagePixels(resized.size());
    if(resizedOctets > capaciteOctets) {
        // Ajuster encore en réduisant légèrement
        double adjust = std::sqrt(double(capaciteOctets) / double(resizedOctets));
        newW = std::max(1, int(resized.width() * adjust));
        newH = std::max(1, int(resized.height() * adjust));
        resized = secret.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        resizedOctets = tailleImagePixels(resized.size());
    }
    // Sauvegarde dans fichier temporaire
    QString tempPath = QDir::temp().filePath(QString("hiddenink_resized_%1x%2.png").arg(resized.width()).arg(resized.height()));
    if(!resized.save(tempPath, "PNG")) {
        info = "(Sauvegarde de l'image redimensionnée échouée)";
        return QString();
    }
    info = QString("[Redimensionnée] %1x%2 -> %3x%4, capacité %5, taille finale %6 octets")
            .arg(secret.width()).arg(secret.height())
            .arg(resized.width()).arg(resized.height())
            .arg(capaciteOctets)
            .arg(resizedOctets);
    return tempPath;
}

// Génération clé (XOR)
void MainWindow::genererCle() {
    std::string cle = generate_key(32); // 32 bytes
    cleCouranteHex = QString::fromStdString(to_hex(cle));
    logMessage("Clé générée (hex): " + cleCouranteHex);
}

void MainWindow::effacerJournal() {
    auto logEdit = findChild<QPlainTextEdit*>("logEdit"); if(logEdit) logEdit->clear();
}

void MainWindow::afficherAide() {
    QMessageBox::information(this, "Aide", "HiddenInk GUI\n\nFonctions: cachage / extraction texte & image, analyse MSE/PSNR, histogramme, détection stéganographie.\nUtilisez le menu Outils pour générer une clé de chiffrement (future intégration)." );
}
