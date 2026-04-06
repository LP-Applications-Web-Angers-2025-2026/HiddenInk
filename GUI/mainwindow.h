#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSize>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Fichier
    void ouvrirImage();
    void quitter();

    // PNG – Cachage
    void cacherTexte();
    void cacherImage();

    // PNG – Extraction
    void extraireTexte();
    void extraireImage();

    // BMP
    void cacherBMP();
    void extraireBMP();

    // WAV
    void cacherWAV();
    void extraireWAV();

    // Analyse classique
    void comparerImages();
    void genererHistogramme();
    void detecterStegano();

    // Stéganalyse avancée
    void analyserEntropie();
    void detecterPayload();

    // Outils
    void genererCle();
    void copierCle();
    void effacerJournal();
    void afficherAide();

private:
    Ui::MainWindow *ui;
    QString derniereImageChargee;
    QString cleCouranteHex;

    void logMessage(const QString &msg);
    QString choisirFichier(const QString &titre, const QString &filtre);
    QString sauverFichier(const QString &titre, const QString &filtre);
    bool confirmer(const QString &texte, const QString &titre = "Confirmation");
    void appliquerStyle();
    void initialiserMenus();
    void initialiserOnglets();

    // Helpers image secrète PNG
    QString preparerImageSecreteRedimensionnee(const QString &secretPath,
                                               const QString &carrierPath,
                                               int bits, QString &info);
    int  calculerCapaciteMax(const QSize &carrierSize, int bits);
    int  tailleImagePixels(const QSize &secretSize);

    // Auto-suggestion chemin de sortie
    QString suggererCheminSortie(const QString &sourcePath,
                                 const QString &suffixe,
                                 const QString &ext);

    // Détection payload sur un fichier extrait (affiché dans le log + analysisOutput)
    void afficherResultatPayload(const QString &filePath);
};

#endif // MAINWINDOW_H
